#pragma once

#include "../CabComponent.h"
#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_gui_basics/juce_gui_basics.h>

class CabPageComponent : public juce::Component
{
public:
    CabPageComponent(juce::AudioProcessorValueTreeState& apvts,
                    std::function<void(const juce::File&)> onIRLoaded)
        : cabComponent_(apvts, std::move(onIRLoaded)) 
    {
        addAndMakeVisible(cabComponent_);
    }

    void resized() override
    {
        auto area = getLocalBounds().reduced(10);

        juce::FlexBox mainRow;
        mainRow.flexDirection = juce::FlexBox::Direction::row;

        mainRow.items.add(juce::FlexItem(cabComponent_).withFlex(1).withMargin(10));
        mainRow.performLayout(area);
    }

private:
    CabComponent cabComponent_;
};