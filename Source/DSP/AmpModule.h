#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_dsp/juce_dsp.h>
#include <RTNeural/RTNeural.h>

class AmpModule final
{
public:
    AmpModule();

    void prepare(const juce::dsp::ProcessSpec& spec);
    void reset();
    void process(const juce::dsp::ProcessContextReplacing<float>& context);

    // загрузка ИИ модели
    bool loadModel(const juce::String& path);
    void clearModel();

    bool loadModelFromMemory(const void* data, size_t size);

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

    juce::dsp::ProcessorChain<
        Duplicator,
        Duplicator,
        Duplicator,
        Duplicator,
        juce::dsp::Gain<float>
    > toneStack_;

    // Нейросетевая модель усилителя, архитектура фиксирована на этапе компиляции.
    // Фиксированная архитектура в разы быстрее динамической.
    // Вход: [аудиосэмпл, нормализованное положение gain [0..1]]
    // Выход: обработанный сэмпл
    // Архитектура: GRU(2→32) → Dense(32→1)
    // Массив [2] — отдельный экземпляр на каждый канал (L/R)
    RTNeural::ModelT<float, 2, 1,
        RTNeural::DenseT<float, 2, 16>,
        RTNeural::TanhActivationT<float, 16>,
        RTNeural::Conv1DT<float, 16, 16, 3, 1>,
        RTNeural::TanhActivationT<float, 16>,
        RTNeural::GRULayerT<float, 16, 48>,
        RTNeural::DenseT<float, 48, 1>
    > neuralNetT[2];

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