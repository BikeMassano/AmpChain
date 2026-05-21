#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_gui_basics/juce_gui_basics.h>
#include "../LookAndFeel/KnobLookAndFeel.h"

namespace GUI
{
    namespace FlangerLayout
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

        constexpr ControlDesc range  {0.24f, 0.17f, knobSize};
        constexpr ControlDesc rate   {0.76f, 0.17f, knobSize};
        constexpr ControlDesc color  {0.5f,  0.33f, knobSize};
        constexpr ControlDesc button {0.5f,  0.79f, buttonSize};
        constexpr ControlDesc lamp   {0.5f,  0.69f, lampSize};
    }

    class FlangerComponent final : public juce::Component,
                    public juce::AudioProcessorValueTreeState::Listener
    {
    public:
        FlangerComponent(juce::AudioProcessorValueTreeState& apvts);
        ~FlangerComponent() override;

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

        // Потенциометры
        struct Knobs 
        {
            juce::Slider rate, range, color;
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
            std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> rate;
            std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> range;
            std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> color;
            std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> regen;
            std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> bypass;
        } attachments_;

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(FlangerComponent)
    };
}