#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_dsp/juce_dsp.h>

namespace DSP
{
    class MonoModule final
    {
    public:
        enum MonoMode
        {
            Left,
            Right
        };

        void process(const juce::dsp::ProcessContextReplacing<float>& context);

        void setEnabled(bool e);
        bool getEnabled();
        void setMode(MonoMode m);
    private:
        std::atomic<bool> enabled_ = false;
        MonoMode mode_ = Left;
    };
}