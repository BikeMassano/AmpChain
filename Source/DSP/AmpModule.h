#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_dsp/juce_dsp.h>
#include <NeuralAmpModelerCore/NAM/dsp.h>
#include <NeuralAmpModelerCore/NAM/get_dsp.h>
#include "DSPModule.h"

namespace DSP
{
    /**
     * @class AmpModule
     * @brief Модуль усилителя на основе нейросетевой модели (NAM).
     * Реализует эмуляцию гитарного усилителя с использованием
     * нейросетевой модели (NAM DSP) и пост-обработкой tone stack.
     * 
     * Модуль поддерживает загрузку внешних NAM моделей
     */
    class AmpModule final : public DSPModule
    {
    public:
        AmpModule();

        void prepare(const juce::dsp::ProcessSpec& spec) override;
        void reset() override;
        void process(const juce::dsp::ProcessContextReplacing<float>& context) override;

        /**
         * @brief Загружает обученную нейросетевую модель усилителя (NAM).
         *
         * @param file Путь к NAM модели
         * @return true если модель успешно загружена
         */
        bool loadModel(const juce::File& file);

        void setGain (const float newGainVal);
        void setBass (const float newBassDb);
        void setMid (const float newMidDb);
        void setTreble (const float newTrebleDb);
        void setPresence (const float newPresenceDb);
        void setLevel (const float newLevelDb);

        /**
         * @brief Включает или отключает bypass.
         * @param b true — сигнал проходит без обработки, false — обработка активна.
         */
        void setBypassed (const bool v) override;

        double getModelSampleRate() const;

    private:
        static constexpr int kMaxModelChannels = 2;

        enum toneStackIndex {
            bassFilterIndex,    // [0]
            midFilterIndex,     // [1]
            trebleFilterIndex,  // [2]
            presenceFilterIndex, // [3]
            outputGainIndex      // [4]
        };

        using Filter = juce::dsp::IIR::Filter<float>;
        using FilterCoefs = juce::dsp::IIR::Coefficients<float>;
        using Duplicator = juce::dsp::ProcessorDuplicator<Filter, FilterCoefs>;

        // Все модели nam имеют внутреннюю память
        // Поэтому используется массив моделей
        std::array<std::unique_ptr<nam::DSP>, 2> model_;
        
        std::vector<NAM_SAMPLE*> inputPtrs_;
        std::vector<NAM_SAMPLE*> outputPtrs_;

        juce::dsp::ProcessorChain<
            Duplicator,
            Duplicator,
            Duplicator,
            Duplicator,
            juce::dsp::Gain<float>
        > toneStack_;

        double sampleRate_ = 48000.;
        int maxBlockSize_ = 512;
        std::atomic<bool> bypassed_ = false;

        float gainNorm_ = 0.5f;
        float bassVal_= 0.5f;
        float midVal_= 0.5f;
        float trebleVal_= 0.5f;
        float presenceVal_ = 0.5f;

        void updateFilters_();
    };
}