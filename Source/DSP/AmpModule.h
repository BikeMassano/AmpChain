#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_dsp/juce_dsp.h>
#include <NeuralAmpModelerCore/NAM/dsp.h>
#include <NeuralAmpModelerCore/NAM/get_dsp.h>


namespace DSP
{
    class AmpModule final
    {
    public:
        AmpModule();

        void prepare(const juce::dsp::ProcessSpec& spec);
        void reset();
        void process(const juce::dsp::ProcessContextReplacing<float>& context);

        // загрузка ИИ модели
        bool loadModel(const juce::File& file);

        void setGain (float val);
        void setBass (float db);
        void setMid (float db);
        void setTreble (float db);
        void setPresence (float db);
        void setLevel (float db);
        void setBypassed (bool v);

        //bool hasModel() const { return model_ != nullptr; }
        double getModelSampleRate() const;

    private:
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
        std::unique_ptr<nam::DSP> model_[2];

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