#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_dsp/juce_dsp.h>

class DistortionModule final
{
public:
    void prepare(const juce::dsp::ProcessSpec& spec);
    void reset();
    void process(const juce::dsp::ProcessContextReplacing<float>& context);

    void setTone(float cutoff);
    void setLevel(float db);
    void setDist(float db);

    void setBypassed(bool b);

private:
    enum {
        preGainIndex, // [0]
        biasIndex, // [1]
        waveshaperIndex, // [2]
        dcFilterIndex, // [3]
        filterIndex, // [4]
        postGainIndex // [5]
    };

    using Filter = juce::dsp::IIR::Filter<float>;
    using FilterCoefs = juce::dsp::IIR::Coefficients<float>;
    using Duplicator = juce::dsp::ProcessorDuplicator<Filter, FilterCoefs>;

    std::unique_ptr<juce::dsp::Oversampling<float>> oversampling_;

    juce::dsp::ProcessorChain<
        juce::dsp::Gain<float>,
        juce::dsp::Bias<float>,
        juce::dsp::WaveShaper<float>,
        Duplicator,
        Duplicator,
        juce::dsp::Gain<float>>
        chain_;
        
    double sampleRate_ = 44100.;
    std::atomic<bool> bypassed_ = false;
};