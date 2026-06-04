#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_gui_basics/juce_gui_basics.h>
#include "../LookAndFeel/KnobLookAndFeel.h"

namespace GUI
{
    namespace AmpLayout
    {
        constexpr float ampBodyAspect = 1680.0f / 693.0f;  // соотношение сторон усилителя
        constexpr float cabBodyAspect = 1863.0f / 306.0f;  // соотношение сторон кабинета

        constexpr float ampSize     = 0.9f;
        constexpr float cabSize     = 1.f;    // размер кабинета
        constexpr float knobSize    = 0.045f; // диаметр ручки потенциометра
        constexpr float buttonSize  = 0.04f;  // размер кнопки
        constexpr float lampSize    = 0.02f;  // размер лампы

        constexpr float cabOverlap  = 0.005f; // коэффициент наложения изображений друг на друга

        constexpr float knobsHeight = 0.775f;

        // положение элементов в нормализованных координатах усилителя
        struct ControlDesc
        {
            float x, y;
            float size;
        };

        constexpr ControlDesc gain       {0.215f,    knobsHeight,   knobSize};
        constexpr ControlDesc bass       {0.41f,     knobsHeight,   knobSize};
        constexpr ControlDesc mid        {0.5f,      knobsHeight,   knobSize};
        constexpr ControlDesc treble     {0.585f,    knobsHeight,   knobSize};
        constexpr ControlDesc presence   {0.715f,    knobsHeight,   knobSize};
        constexpr ControlDesc level      {0.82f,     knobsHeight,   knobSize};
        constexpr ControlDesc lamp       {0.87f,     0.78f,         lampSize};
        constexpr ControlDesc button     {0.92f,     0.78f,         buttonSize};
    }

    class AmpComponent final : public juce::Component,
                    public juce::AudioProcessorValueTreeState::Listener,
                    private juce::ValueTree::Listener
    {
    public:
        AmpComponent(juce::AudioProcessorValueTreeState& apvts);
        ~AmpComponent() override;

        std::function<void(const juce::File&)> onModelLoad;

        void paint(juce::Graphics& g) override;
        void resized() override;
        void parameterChanged(const juce::String&, float) override;

    private:
        void loadImages();
        void initButtons();
        void initSliders();
        void initAttachments();

        void setupKnob(juce::Slider& s);

        juce::Rectangle<int> getAmpRect() const;
        juce::Rectangle<int> getCabRect() const;

        void updateModelLabel_();
        void valueTreeRedirected(juce::ValueTree&) override { updateModelLabel_(); }
        void valueTreePropertyChanged(juce::ValueTree&, const juce::Identifier&) override { updateModelLabel_(); }

        juce::TextButton loadModelButton_ { "LOAD MODEL" };
        juce::Label modelNameLabel_;

        std::unique_ptr<juce::FileChooser> fileChooser_;

        juce::AudioProcessorValueTreeState& apvtsRef;

        // UI элементы

        // LookAndFeel
        std::unique_ptr<KnobLookAndFeel> knobLnf_;

        // потенциометры
        struct Knobs 
        {
            juce::Slider bass, mid, treble, level, gain, presence;
        } knobs_;

        juce::ImageButton powerButton_;

        // Изображения
        struct Images
        {
            juce::Image amp;
            juce::Image cab;
            juce::Image lampOff, lampOn;
            juce::Image buttonOff, buttonOn;
            juce::Image knob;
        } images_;

        juce::Rectangle<int> cabBounds;
        juce::Rectangle<int> ampBounds;

        struct Attachments
        {
            std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> bass;
            std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> mid;
            std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> treble;
            std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> level;
            std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> gain;
            std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> presence;
            std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> bypass;
        } attachments_;

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AmpComponent)
    };
}