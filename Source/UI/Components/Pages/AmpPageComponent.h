#pragma once

#include "../AmpComponent.h"
#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_gui_basics/juce_gui_basics.h>

class AmpPageComponent : public juce::Component
{
public:
    AmpPageComponent(juce::AudioProcessorValueTreeState& apvts)
        : ampComponent(apvts)
    {
        addAndMakeVisible(ampComponent);
    }

    void resized() override
    {
        auto area = getLocalBounds();

        juce::FlexBox mainRow;
        mainRow.flexDirection = juce::FlexBox::Direction::row;

        mainRow.items.add(juce::FlexItem(ampComponent).withFlex(1).withMargin(0));

        mainRow.performLayout(area);
    }

private:
    AmpComponent ampComponent;
};