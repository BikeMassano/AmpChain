#include "AmpModule.h"
#include <fstream>

AmpModule::AmpModule()
{
    
}

void AmpModule::prepare(const juce::dsp::ProcessSpec& spec)
{
    sampleRate_   = spec.sampleRate;
    maxBlockSize_ = (int)spec.maximumBlockSize;

    toneStack_.prepare(spec);

    for (auto& net : neuralNetT)
        net.reset();

    updateFilters_();
}


void AmpModule::reset()
{
    toneStack_.reset();
    for (auto& net : neuralNetT)
        net.reset();
}

void AmpModule::process(const juce::dsp::ProcessContextReplacing<float>& context)
{
    if (bypassed_) 
        return;

    auto& block = context.getOutputBlock();
    const int numChannels = (int)block.getNumChannels();
    const int numSamples  = (int)block.getNumSamples();

    for (int ch = 0; ch < juce::jmin(numChannels, 2); ++ch)
    {
        auto* data = block.getChannelPointer(ch);
        for (int i = 0; i < numSamples; ++i)
        {
            float inData[2] = { data[i], gainNorm_ };
            float out = neuralNetT[ch].forward(inData);

            data[i] = out;
        }
    }

    toneStack_.process(context);
}

bool AmpModule::loadModel(const juce::String& path)
{
    auto file = juce::File(path);
    if (!file.existsAsFile()) return false;

    std::ifstream jsonStream(file.getFullPathName().toStdString());
    nlohmann::json modelJson;
    jsonStream >> modelJson;

    for (int ch = 0; ch < 2; ++ch)
    {
        neuralNetT[ch].parseJson(modelJson);
        neuralNetT[ch].reset();
    }
    return true;
}

bool AmpModule::loadModelFromMemory(const void* data, size_t size)
    {
        try
        {
            auto json = nlohmann::json::parse(
                static_cast<const char*>(data),
                static_cast<const char*>(data) + size
            );

            for (int ch = 0; ch < 2; ++ch)
            {
                neuralNetT[ch].parseJson(json);;
                neuralNetT[ch].reset();
            }
            DBG("model loaded!");
            return true;
        }
        catch (const std::exception& e)
        {
            DBG("model not loaded!");
            DBG(e.what());
            return false;
        }
    }

void AmpModule::clearModel()
{
    for (auto& net : neuralNetT)
        net.reset();
}

double AmpModule::getModelSampleRate() const
{
    return sampleRate_;
}

void AmpModule::setGain(float val)
{
    // если gain идёт от 0 до 10 dB → нормируем
    gainNorm_ = juce::jlimit(0.f, 1.f, val / 10.f);
}

void AmpModule::setLevel(float db)   
{
    toneStack_.get<outputGainIndex>().setGainDecibels(db);
}

void AmpModule::setBypassed(bool v) { bypassed_ = v; }

void AmpModule::setBass(float db) { bassVal_ = db; updateFilters_(); }
void AmpModule::setMid(float db) { midVal_ = db; updateFilters_(); }
void AmpModule::setTreble(float db) { trebleVal_ = db; updateFilters_(); }
void AmpModule::setPresence(float db) { presenceVal_ = db; updateFilters_(); }

void AmpModule::updateFilters_()
{
    if (sampleRate_ <= 0.0) return;
    const float sr = (float)sampleRate_;

    // Bass: low-shelf ±12 дБ вокруг 250 Гц
    {
        const float freq  = 250.f;
        const float gainDB = juce::jmap(bassVal_, 0.f, 1.f, -12.f, 12.f);
        *toneStack_.get<bassFilterIndex>().state =
            *FilterCoefs::makeLowShelf(sr, freq, 0.707f, juce::Decibels::decibelsToGain(gainDB));
    }
    // Mid: peak ±10 дБ на 800 Гц
    {
        const float freq  = 800.f;
        const float gainDB = juce::jmap(midVal_, 0.f, 1.f, -10.f, 10.f);
        *toneStack_.get<midFilterIndex>().state =
            *FilterCoefs::makePeakFilter(sr, freq, 0.9f, juce::Decibels::decibelsToGain(gainDB));
    }
    // Treble: high-shelf ±12 дБ от 3.5 кГц
    {
        const float freq  = 3500.f;
        const float gainDB = juce::jmap(trebleVal_, 0.f, 1.f, -12.f, 12.f);
        *toneStack_.get<trebleFilterIndex>().state =
            *FilterCoefs::makeHighShelf(sr, freq, 0.707f, juce::Decibels::decibelsToGain(gainDB));
    }
    // Presence: high-shelf ±8 дБ от 3 кГц (post)
    {
        const float freq  = 3000.f;
        const float gainDB = juce::jmap(presenceVal_, 0.f, 1.f, -8.f, 8.f);
        *toneStack_.get<presenceFilterIndex>().state =
            *FilterCoefs::makeHighShelf(sr, freq, 0.707f, juce::Decibels::decibelsToGain(gainDB));
    }
}