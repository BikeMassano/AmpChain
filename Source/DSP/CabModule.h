#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_dsp/juce_dsp.h>
#include "DSPModule.h"

namespace DSP
{
    /**
     * @class CabModule
     * @brief Кабинетная симуляция на основе convolution (IR loader).
     *
     * Реализует эмуляцию гитарного кабинета с использованием
     * импульсных характеристик (Impulse Response, IR).
     */
    class CabModule final : public DSPModule
    {
    public:
        void prepare(const juce::dsp::ProcessSpec& spec) override;
        void reset() override;
        void process(const juce::dsp::ProcessContextReplacing<float>& context) override;

        /**
         * @brief Загружает импульсную характеристику (IR).
         *
         * IR используется для convolution-симуляции акустического
         * отклика гитарного кабинета.
         *
         * @param path Путь к WAV/AIF/AIFF файлу с IR.
         */
        void loadIR(const juce::File path);

        /**
         * @brief Включает или отключает bypass.
         * @param b true — сигнал проходит без обработки, false — обработка активна.
         */
        void setBypassed(const bool b) override;

    private:
        std::atomic<bool> bypassed_ = false;
        double sampleRate_ = 44100.;
        juce::dsp::Convolution convolution_;
    };
}