#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_dsp/juce_dsp.h>
#include "DSPModule.h"

namespace DSP
{
    /**
     * @class FlangerModule
     * @brief Модуль flanger-эффекта
     */
    class FlangerModule final : public DSPModule
    {
    public:
        void prepare(const juce::dsp::ProcessSpec& spec) override;
        void reset() override;
        void process(const juce::dsp::ProcessContextReplacing<float>& context) override;
    
        /**
         * @brief Устанавливает скорость модуляции LFO.
         * @param newRateHz Частота модуляции в герцах (Hz).
         * Диапазон: 0 ... 100 Hz.
         */
        void setRate(const float newRateHz);

        /**
         * @brief Устанавливает центральную задержку модуляции.
         * @param newRangeMs Глубина/диапазон задержки в миллисекундах.
         * Диапазон: 1 ... 5 мс.
         */
        void setRange(const float newRangeMs);
        
        /**
         * @brief Устанавливает уровень обратной связи.
         * @param newFeedback Коэффициент feedback.
         * Диапазон: -1 ... 1.
         */
        void setFeedback(const float newFeedback);

        /**
         * @brief Включает или отключает bypass.
         * @param b true — сигнал проходит без обработки, false — обработка активна.
         */
        void setBypassed(const bool b) override;
    private:
        void update();

        juce::dsp::DelayLine<float, juce::dsp::DelayLineInterpolationTypes::Linear> delay_;
        juce::dsp::Oscillator<float> osc_;
        juce::LinearSmoothedValue<float> oscVolume_;

        std::vector<juce::LinearSmoothedValue<float>> feedbackVolume_ { 2 };
        std::vector<float> lastOutput_ { 2 };

        juce::AudioBuffer<float> delayTimes_;

        juce::dsp::DryWetMixer<float> dryWet_;

        double sampleRate_ = 44100.;

        float rate_     = 0.5f;
        float range_    = 7.f;
        float feedback_ = 0.5f;

        std::atomic<bool> bypassed_ = false;

        static constexpr float  maxDepth               = 1.f,
                                maxCentreDelayMs       = 15.f,
                                oscVolumeMultiplier    = 0.5f,
                                maximumDelayModulation = 5.f;

    };
}