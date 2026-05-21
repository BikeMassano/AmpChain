#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_dsp/juce_dsp.h>
#include "DSPModule.h"

namespace DSP
{
    /**
     * @class DistortionModule
     * @brief Модуль эффекта distortion.
     *
     * Реализует цепочку обработки для создания нелинейного
     * искажения сигнала:
     * 
     * - pre gain;
     * 
     * - bias;
     * 
     * - waveshaper;
     * 
     * - DC-block filter;
     * 
     * - tone filter;
     * 
     * - output gain.
     *
     * Для уменьшения aliasing используется oversampling.
     */
    class DistortionModule final : public DSPModule
    {
    public:
        void prepare(const juce::dsp::ProcessSpec& spec) override;
        void reset() override;
        void process(const juce::dsp::ProcessContextReplacing<float>& context) override;

        /**
         * @brief Устанавливает частоту среза тонального фильтра.
         * @param newCutoffHz Частота среза фильтра в герцах (Hz).
         * Диапазон: 1600 ... 12000 Hz.
         */
        void setTone(const float newCutoffHz);

        /**
         * @brief Устанавливает выходную громкость.
         * @param newLevelDb Усиление в децибелах (dB).
         * Диапазон: -24 ... +24 dB.
         */
        void setLevel(const float newLevelDb);

        /**
         * @brief Устанавливает степень искажения сигнала.
         * @param newDistDb Уровень drive/dist в децибелах (dB).
         * Диапазон: 0 ... 50 dB.
         */
        void setDist(const float newDistDb);

        /**
         * @brief Включает или отключает bypass.
         * @param b true — сигнал проходит без обработки, false — обработка активна.
         */
        void setBypassed(const bool b) override;

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
}