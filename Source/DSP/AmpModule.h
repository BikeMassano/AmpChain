#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_dsp/juce_dsp.h>

class AmpModule final
{
public:
    AmpModule();

    void prepare(const juce::dsp::ProcessSpec& spec);
    void reset();
    void process(const juce::dsp::ProcessContextReplacing<float>& context);

    // загрузка ИИ модели
    bool loadModel(const juce::String& path);
    void clearModel();

    void setGain (float db);
    void setBass (float db);
    void setMid (float db);
    void setTreble (float db);
    void setPresence (float db);
    void setLevel (float db);
    void setBypassed (bool v);

    //bool hasModel() const { return model_ != nullptr; }
    double getModelSampleRate() const;

private:
    enum preChainIndex {
        inputGainIndex,     // [0]
        bassFilterIndex,    // [1]
        midFilterIndex,     // [2]
        trebleFilterIndex,  // [3]
    };

    enum postChainIndex {
        presenceFilterIndex, // [0]
        outputGainIndex      // [1]
    };

    using Filter = juce::dsp::IIR::Filter<float>;
    using FilterCoefs = juce::dsp::IIR::Coefficients<float>;
    using Duplicator = juce::dsp::ProcessorDuplicator<Filter, FilterCoefs>;

    // Pre-EQ + input gain
    juce::dsp::ProcessorChain<
        juce::dsp::Gain<float>,
        Duplicator,
        Duplicator,
        Duplicator
    > preChain_;

    juce::dsp::ProcessorChain<
        Duplicator,
        juce::dsp::Gain<float>
    > postChain_;

    std::vector<float>  namInputBuf_;
    std::vector<float>  namOutputBuf_;

    double sampleRate_ = 48000.;
    int maxBlockSize_ = 512;
    std::atomic<bool> bypassed_ = false;

    float bassVal_= 0.5f;
    float midVal_= 0.5f;
    float trebleVal_= 0.5f;
    float presenceVal_ = 0.5f;

    void updateFilters_();
};