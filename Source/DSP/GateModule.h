#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_dsp/juce_dsp.h>
namespace DSP
{
    class GateModule final
    {
    public:
        void prepare(const juce::dsp::ProcessSpec& spec);
        void reset();
        void process(const juce::dsp::ProcessContextReplacing<float>& context);

        void setThreshold(float v);
        void setAttack(float ms);
        void setRelease(float ms);
        void setBypassed(bool b);

    private:
        juce::dsp::NoiseGate<float> gate_;

        std::atomic<bool> bypassed_ = false;
    };
}