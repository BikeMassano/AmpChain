#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_gui_basics/juce_gui_basics.h>
#include "../LookAndFeel/KnobLookAndFeel.h"

class DistortionComponent final : public juce::Component,
                public juce::AudioProcessorValueTreeState::Listener
{
public:
    DistortionComponent(juce::AudioProcessorValueTreeState& apvts);
    ~DistortionComponent() override;

    void paint(juce::Graphics& g) override;
    void resized() override;
    void parameterChanged(const juce::String&, float) override;

private:
    void loadImages();
    void initButtons();
    void initSliders();
    void initLabels();
    void initAttachments();

    void setupKnob(juce::Slider& s, double min, double max);
    void setupLabel(juce::Label& l, const juce::String& text);

    juce::AudioProcessorValueTreeState& apvtsRef;

    // UI элементы
    juce::Slider toneSlider, levelSlider, distSlider;
    juce::Label toneLabel, levelLabel, distLabel;
    juce::ImageButton powerButton;

    // Изображения
    juce::Image bodyImage;
    juce::Image lampOffImage;
    juce::Image lampOnImage;
    juce::Image butOffImage;
    juce::Image butOnImage;
    juce::Image knobImage;
    juce::Image knobShadowImage;

    // LookAndFeel
    std::unique_ptr<KnobLookAndFeel> knobLnf;

    struct Attachments
    {
        std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> tone;
        std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> level;
        std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> dist;
        std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> bypass;
    } attachments;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DistortionComponent)
};