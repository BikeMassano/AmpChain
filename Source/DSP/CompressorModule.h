#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_dsp/juce_dsp.h>

namespace DSP
{
    class CompressorModule final
    {
    public:
        void prepare(const juce::dsp::ProcessSpec& spec);

        void reset();

        void process(const juce::dsp::ProcessContextReplacing<float>& context);

        void setThreshold(float v);
        void setRatio(float v);
        void setAttack(float v);
        void setRelease(float v);
        void setMakeup(float v);
        void setBypassed(bool b);

    private:
        enum {
            compressorIndex, // [0]
            gainIndex // [1]
        };

        juce::dsp::ProcessorChain<
            juce::dsp::Compressor<float>,
            juce::dsp::Gain<float>
        > chain_;

        std::atomic<bool> bypassed_ = false;
    };
}