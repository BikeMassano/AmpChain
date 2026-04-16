#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_audio_processors/juce_audio_processors.h>

class PedalComponent : public juce::Component,
                       public juce::AudioProcessorValueTreeState::Listener
{
public:
    PedalComponent(juce::AudioProcessorValueTreeState& apvts,
                   const juce::String& bypassParamID);

    ~PedalComponent() override;

    void paint(juce::Graphics& g) override;
    void resized() override;

    void parameterChanged(const juce::String& parameterID, float newValue) override;

protected:
    virtual void paintBackground(juce::Graphics& g);
    virtual void paintLamp(juce::Graphics& g, bool isOn);

    void setBodyImage(const juce::Image& img);
    void setLampImages(const juce::Image& on, const juce::Image& off);

    void setupPowerButton(juce::Image onImg, juce::Image offImg);

    juce::AudioProcessorValueTreeState& apvts;
    juce::String bypassID;

    juce::Image bodyImage;
    juce::Image lampOnImage;
    juce::Image lampOffImage;

    juce::ImageButton powerButton;

private:
    void updateLampStateAsync();

    std::atomic<bool> isOn { false };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PedalComponent)
};