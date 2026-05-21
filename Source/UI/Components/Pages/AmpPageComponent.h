#pragma once

#include "../AmpComponent.h"
#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_gui_basics/juce_gui_basics.h>

class AmpPageComponent : public juce::Component
{
public:
    AmpPageComponent(juce::AudioProcessorValueTreeState& apvts)
        : ampComponent_(apvts)
    {
        addAndMakeVisible(ampComponent_);
    }

    void resized() override
    {
        // constexpr float aspect = 1902.0f / 677.0f;

        // auto area = getLocalBounds().reduced(10);

        // int w = area.getWidth();
        // int h = juce::jmin((int)(w / aspect), area.getHeight());

        // int x = (area.getWidth() - w) / 2;
        // int y = (area.getHeight() - h) / 2;

        // ampComponent_.setBounds(x, y, w, h);
        ampComponent_.setBounds(getLocalBounds());
    }

private:
    GUI::AmpComponent ampComponent_;
};