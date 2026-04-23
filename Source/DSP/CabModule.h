#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_dsp/juce_dsp.h>

class CabModule final
{
public:
    void prepare(const juce::dsp::ProcessSpec& spec);
    void reset();
    void process(const juce::dsp::ProcessContextReplacing<float>& context);

    void loadIR(juce::File path);
    void setBypassed(bool b);

private:
    std::atomic<bool> bypassed_ = false;
    double sampleRate_ = 44100.;
    juce::dsp::Convolution convolution_;
};