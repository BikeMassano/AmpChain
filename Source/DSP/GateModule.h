#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_dsp/juce_dsp.h>
#include "DSPModule.h"

namespace DSP
{
    /**
     * @class GateModule
     * @brief Обёртка над juce::dsp::NoiseGate.
     *
     * Реализует Noise Gate для подавления сигналов,
     * уровень которых ниже заданного порога.
     */
    class GateModule final : public DSPModule
    {
    public:
        void prepare(const juce::dsp::ProcessSpec& spec) override;
        void reset() override;
        void process(const juce::dsp::ProcessContextReplacing<float>& context) override;

        /**
         * @brief Устанавливает порог срабатывания гейта.
         * Сигнал ниже порога будет подавляться.
         * @param newThresholdDb Порог в децибелах (dB). 
         * Диапазон: -100 ... 0 dB.
         */
        void setThreshold(const float newThresholdDb);

        /**
         * @brief Устанавливает время открытия гейта.
         * Определяет скорость открытия после превышения порога.
         * @param ms Время атаки в миллисекундах.
         * Диапазон: 0.1 ... 50 ms.
         */
        void setAttack(const float newAttackMs);

        /**
         * @brief Устанавливает время закрытия гейта.
         * Определяет скорость затухания после падения сигнала ниже порога.
         * @param newReleaseMs Время восстановления (release) в миллисекундах.
         * Диапазон: 10 ... 2000 ms.
         */
        void setRelease(const float newReleaseMs);

        /**
         * @brief Включает или отключает bypass.
         * @param b true — сигнал проходит без обработки, false — обработка активна.
         */
        void setBypassed(const bool b) override;

    private:
        juce::dsp::NoiseGate<float> gate_;

        std::atomic<bool> bypassed_ = false;
    };
}