#include "AmpModule.h"
#include <fstream>
#include <BinaryData.h>

namespace DSP
{
    AmpModule::AmpModule() {}

    void AmpModule::prepare(const juce::dsp::ProcessSpec& spec)
    {
        sampleRate_ = spec.sampleRate;
        maxBlockSize_ = (int)spec.maximumBlockSize;

        toneStack_.prepare(spec);
        updateFilters_();

        neuralNetT[0].reset();
        neuralNetT[1].reset();
    }


    void AmpModule::reset()
    {
        neuralNetT[0].reset();
        neuralNetT[1].reset();
        toneStack_.reset();
    }

    void AmpModule::process(const juce::dsp::ProcessContextReplacing<float>& context)
    {
        if (bypassed_) 
            return;

        auto& block = context.getOutputBlock();

        for (int ch = 0; ch < (int)block.getNumChannels(); ++ch)
        {
            auto* x = block.getChannelPointer(ch);
            for (int n = 0; n < (int)block.getNumSamples(); ++n)
            {
                float input[] = { x[n], gainNorm_ };
                x[n] = neuralNetT[ch].forward(input);
            }
        }

        toneStack_.process(context);

    }

    bool AmpModule::loadModel()
    {
        try
        {
            juce::MemoryInputStream jsonStream(
                BinaryData::model_json,
                BinaryData::model_jsonSize,
                false
            );

            auto jsonString = jsonStream.readEntireStreamAsString();
            auto jsonInput = nlohmann::json::parse(jsonString.toStdString());

            neuralNetT[0].parseJson(jsonInput);
            neuralNetT[1].parseJson(jsonInput);
            DBG("model loaded!");
            return true;
        }
        catch (...) 
        {
            DBG("model not loaded!");
            return false; 
        }
    }

    double AmpModule::getModelSampleRate() const
    {
        return sampleRate_;
    }

    void AmpModule::setGain(float val)
    {
        // если gain идёт от 0 до 10 dB → нормируем
        gainNorm_ = val;
    }

    void AmpModule::setLevel(float val)
    {
        float db = (val - 5.f) * (24.f / 5.f); // диапазон ±24 dB
        toneStack_.get<outputGainIndex>().setGainDecibels(db);
    }

    void AmpModule::setBypassed(bool v) { bypassed_ = v; }

    void AmpModule::setBass(float val) 
    { 
        bassVal_ = (val - 5.f) * (12.f / 5.f);
        updateFilters_();
    }
    void AmpModule::setMid(float val)
    {
        midVal_ = (val - 5.f) * (10.f / 5.f);
        updateFilters_();
    }
    void AmpModule::setTreble(float val)
    {
        trebleVal_ = (val - 5.f) * (12.f / 5.f);
        updateFilters_();
    }
    void AmpModule::setPresence(float val)
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