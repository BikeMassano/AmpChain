#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_gui_basics/juce_gui_basics.h>
#include "../LookAndFeel/KnobLookAndFeel.h"

namespace GUI
{
    namespace CompressorLayout
    {
        constexpr float bodyAspect = 1080.0f / 1917.0f;  // соотношение сторон педали

        constexpr float knobSize    = 0.22f;  // диаметр ручки потенциометра
        constexpr float buttonSize  = 0.16f;  // размер кнопки в проценте от ширины педали
        constexpr float lampSize    = 0.08f;  // размер лампы в проценте от ширины педали

        // положение элементов в нормализованных координатах педали
        struct ControlDesc
        {
            float x, y;
            float size;
        };

        constexpr ControlDesc threshold  {0.24f, 0.17f, knobSize};
        constexpr ControlDesc ratio      {0.76f, 0.17f, knobSize};
        constexpr ControlDesc release    {0.24f, 0.36f, knobSize};
        constexpr ControlDesc attack     {0.76f, 0.36f, knobSize};
        constexpr ControlDesc makeup     {0.5f,  0.5f,  knobSize};
        constexpr ControlDesc button     {0.5f,  0.79f, buttonSize};
        constexpr ControlDesc lamp       {0.5f,  0.69f, lampSize};
    }

    class CompressorComponent final : public juce::Component,
                    public juce::AudioProcessorValueTreeState::Listener
    {
    public:
        CompressorComponent(juce::AudioProcessorValueTreeState& apvts);
        ~CompressorComponent() override;

        void paint(juce::Graphics& g) override;
        void resized() override;
        void parameterChanged(const juce::String&, float) override;

    private:
        void loadImages();
        void initButtons();
        void initSliders();
        void initAttachments();

        juce::Rectangle<int> getBodyRect() const;

        void setupKnob(juce::Slider& s);

        juce::AudioProcessorValueTreeState& apvtsRef_;

        // UI элементы

        // LookAndFeel
        std::unique_ptr<KnobLookAndFeel> knobLnf_;

        struct Knobs 
        {
            juce::Slider threshold, ratio, attack, release, makeup;
        } knobs_;

        // Кнопка
        juce::ImageButton powerButton_;

        // Изображения
        struct Images 
        {
            juce::Image body;
            juce::Image lampOff, lampOn;
            juce::Image buttonOff, buttonOn;
            juce::Image knob;
        } images_;

        struct Attachments
        {
            std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> threshold;
            std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> ratio;
            std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> attack;
            std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> release;
            std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> makeupGain;
            std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> bypass;
        } attachments_;

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(CompressorComponent)
    };
}