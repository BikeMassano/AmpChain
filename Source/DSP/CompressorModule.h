#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_dsp/juce_dsp.h>
#include "DSPModule.h"

namespace DSP
{
    /**
     * @class CompressorModule
     * @brief Модуль динамической компрессии с makeup gain.
     *
     * Уменьшает динамический диапазон сигнала, подавляя пики
     * выше заданного порога.
     */
    class CompressorModule final : public DSPModule
    {
    public:
        void prepare(const juce::dsp::ProcessSpec& spec) override;

        void reset() override;

        void process(const juce::dsp::ProcessContextReplacing<float>& context) override;

        /**
         * @brief Устанавливает порог срабатывания компрессора.
         * @param newThresholdDb Порог в децибелах.
         * Диапазон: -60 ... 0 dB.
         */
        void setThreshold(const float newThresholdDb);

        /**
         * @brief Устанавливает степень компрессии.
         *
         * Определяет соотношение между превышением порога и
         * величиной подавления.
         * @param newRatio Коэффициент компрессии.
         * Диапазон: 1 ... 60.
         */
        void setRatio(const float newRatio);

        /**
         * @brief Устанавливает время атаки.
         *
         * Определяет, как быстро компрессор реагирует на превышение порога.
         * @param newAttackMs Время атаки в миллисекундах.
         * Диапазон: 0.1 ... 100 мс.
         */
        void setAttack(const float newAttackMs);

        /**
         * @brief Устанавливает время релиза.
         *
         * Определяет, как быстро компрессор прекращает подавление
         * после того, как сигнал опускается ниже порога.
         *
         * @param newReleaseMs Время релиза в миллисекундах.
         * Диапазон: 10 ... 1000 мс.
         */
        void setRelease(const float newReleaseMs);

        /**
         * @brief Устанавливает уровень makeup gain.
         *
         * Компенсирует потерю громкости после компрессии.
         *
         * @param newMakeupDb Усиление в децибелах.
         * Диапазон: 0 ... 24 dB.
         */
        void setMakeup(const float newMakeupDb);

        /**
         * @brief Включает или отключает bypass.
         * @param b true — сигнал проходит без обработки, false — обработка активна.
         */
        void setBypassed(const bool b) override;

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