#include "AmpModule.h"
#include <fstream>
#include <BinaryData.h>
#include <NeuralAmpModelerCore/NAM/wavenet/model.h>
#include <NeuralAmpModelerCore/NAM/model_config.h>

namespace DSP
{
    AmpModule::AmpModule() 
    {
        nam::activations::Activation::enable_fast_tanh();
        if (!nam::ConfigParserRegistry::instance().has("WaveNet"))
            nam::ConfigParserRegistry::instance().registerParser("WaveNet", nam::wavenet::create_config);
    }

    void AmpModule::prepare(const juce::dsp::ProcessSpec& spec)
    {
        // Oversampling
        oversampling_ = std::make_unique<juce::dsp::Oversampling<float>>
        (
            spec.numChannels,
            1,
            juce::dsp::Oversampling<float>::filterHalfBandPolyphaseIIR
        );

        oversampling_->initProcessing(spec.maximumBlockSize);

        juce::dsp::ProcessSpec osSpec = spec;
        osSpec.sampleRate *= oversampling_->getOversamplingFactor();
        osSpec.maximumBlockSize *= oversampling_->getOversamplingFactor();

        sampleRate_ = osSpec.sampleRate;
        maxBlockSize_ = (int)osSpec.maximumBlockSize;

        inputPtrs_.resize(kMaxModelChannels);
        outputPtrs_.resize(kMaxModelChannels);

        toneStack_.prepare(spec);
        updateFilters_();
    }


    void AmpModule::reset()
    {
        toneStack_.reset();
    }

    void AmpModule::process(const juce::dsp::ProcessContextReplacing<float>& context)
    {
        if (bypassed_) 
            return;

        std::unique_lock<std::mutex> lock(modelMutex_, std::try_to_lock);
        if (!lock.owns_lock()) return; // пропустить блок если идёт загрузка

        auto& audioBlock = context.getOutputBlock();

        juce::dsp::AudioBlock<float> upBlock = oversampling_->processSamplesUp(audioBlock);

        const int numSamples = (int) upBlock.getNumSamples();
        const int numChannels = (int) upBlock.getNumChannels();

        if (model_[0] == nullptr || model_[1] == nullptr)
            return;
        
        const int modelInputs  = model_[0]->NumInputChannels();
        const int modelOutputs = model_[0]->NumOutputChannels();
        
        if (modelInputs == 1)
        {
            // Моно модель — каждый канал через свой независимый экземпляр
            for (size_t ch = 0; ch < numChannels && ch < 2; ch++)
            {
                if (model_[ch] == nullptr) continue;

                auto* data = upBlock.getChannelPointer(ch);
                NAM_SAMPLE* in[1]  = { data };
                NAM_SAMPLE* out[1] = { data };
                model_[ch]->process(in, out, numSamples);
            }
        }
        else
        {
            // Мультиканальная модель — передаём все каналы сразу
            for (size_t ch = 0; ch < modelInputs && ch < numChannels; ch++)
            {
                inputPtrs_[ch]  = upBlock.getChannelPointer(ch);
                outputPtrs_[ch] = upBlock.getChannelPointer(ch);
            }

            model_[0]->process(inputPtrs_.data(), outputPtrs_.data(), numSamples);
        }
        

        oversampling_->processSamplesDown(audioBlock);

        toneStack_.process(context);
    }

    bool AmpModule::loadModel(const juce::File& file)
    {
        if (!file.existsAsFile())
            return false;

        std::lock_guard<std::mutex> lock(modelMutex_);

        for (auto& m : model_) m.reset();

        try
        {
            auto path = std::filesystem::path(file.getFullPathName().toStdString());

           for (auto& m : model_)
            {
                m = nam::get_dsp(path);
                if (m != nullptr)
                    m->ResetAndPrewarm(sampleRate_, maxBlockSize_);
            }

            setGain(gainNorm_);

            return model_[0] != nullptr;
        }
        catch (const std::exception& e)
        {
            DBG("NAM load failed: " << e.what());
            for (auto& m : model_) m.reset();
            return false;
        }
        catch (...)
        {
            DBG("NAM load failed: unknown exception");
            for (auto& m : model_) m.reset();
            return false;
        }
    }

    double AmpModule::getModelSampleRate() const
    {
        if (model_[0] != nullptr)
            return model_[0]->GetExpectedSampleRate();
        return NAM_UNKNOWN_EXPECTED_SAMPLE_RATE;
    }

    void AmpModule::setGain(const float val)
    {
        gainNorm_ = val;
        float db = (val - 5.f) * (24.f / 5.f);

        for (auto& m : model_)
            if (m != nullptr)
                m->SetInputLevel(db);
    }

    void AmpModule::setLevel(const float val)
    {
        float db = (val - 5.f) * (24.f / 5.f); // диапазон ±24 dB
        toneStack_.get<outputGainIndex>().setGainDecibels(db);
    }

    void AmpModule::setBypassed(const bool v) { bypassed_ = v; }

    void AmpModule::setBass(const float val) 
    { 
        bassVal_ = (val - 5.f) * (12.f / 5.f);
        updateFilters_();
    }
    void AmpModule::setMid(const float val)
    {
        midVal_ = (val - 5.f) * (10.f / 5.f);
        updateFilters_();
    }
    void AmpModule::setTreble(const float val)
    {
        trebleVal_ = (val - 5.f) * (12.f / 5.f);
        updateFilters_();
    }
    void AmpModule::setPresence(const float val)
    {
        presenceVal_ = (val - 5.f) * (8.f / 5.f);
        updateFilters_();
    }

    void AmpModule::updateFilters_()
    {
        if (sampleRate_ <= 0.0) return;
        const float sr = (float)sampleRate_;

        // Bass: low-shelf ±12 дБ вокруг 250 Гц
        {
            const float freq  = 250.f;
            const float gainDB = bassVal_;
            *toneStack_.get<bassFilterIndex>().state =
                *FilterCoefs::makeLowShelf(sr, freq, 0.707f, juce::Decibels::decibelsToGain(gainDB));
        }
        // Mid: peak ±10 дБ на 800 Гц
        {
            const float freq  = 800.f;
            const float gainDB = midVal_;
            *toneStack_.get<midFilterIndex>().state =
                *FilterCoefs::makePeakFilter(sr, freq, 0.9f, juce::Decibels::decibelsToGain(gainDB));
        }
        // Treble: high-shelf ±12 дБ от 3.5 кГц
        {
            const float freq  = 3500.f;
            const float gainDB = trebleVal_;
            *toneStack_.get<trebleFilterIndex>().state =
                *FilterCoefs::makeHighShelf(sr, freq, 0.707f, juce::Decibels::decibelsToGain(gainDB));
        }
        // Presence: high-shelf ±8 дБ от 3 кГц (post)
        {
            const float freq  = 3000.f;
            const float gainDB = presenceVal_;
            *toneStack_.get<presenceFilterIndex>().state =
                *FilterCoefs::makeHighShelf(sr, freq, 0.707f, juce::Decibels::decibelsToGain(gainDB));
        }
    }
}