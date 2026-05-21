#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_dsp/juce_dsp.h>


namespace DSP
{
    /**
     * @class DSPModule
     * @brief Базовый класс DSP модулей приложения
     *
     * Содержит базовые методы обработки сигнала JUCE
     */
    class DSPModule
    {
    public:
        virtual ~DSPModule() = default;

        virtual void prepare(const juce::dsp::ProcessSpec& spec) = 0;
        virtual void reset() = 0;
        virtual void process(const juce::dsp::ProcessContextReplacing<float>& context) = 0;
        virtual void setBypassed(const bool b) = 0;
    };
}