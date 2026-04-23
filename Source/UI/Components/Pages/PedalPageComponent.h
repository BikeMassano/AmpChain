#pragma once

#include "../GateComponent.h"
#include "../CompressorComponent.h"
#include "../DistortionComponent.h"
#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_gui_basics/juce_gui_basics.h>

class PedalPageComponent : public juce::Component
{
public:
    PedalPageComponent(juce::AudioProcessorValueTreeState& apvts)
        : gateComponent(apvts), compressorComponent(apvts), distComponent(apvts)
    {
        addAndMakeVisible(gateComponent);
        addAndMakeVisible(compressorComponent);
        addAndMakeVisible(distComponent);
    }

    void resized() override
    {
        auto area = getLocalBounds().reduced(10);

        juce::FlexBox mainRow;
        mainRow.flexDirection = juce::FlexBox::Direction::row;

        mainRow.items.add(juce::FlexItem(gateComponent).withFlex(1).withMargin(10));
        mainRow.items.add(juce::FlexItem(compressorComponent).withFlex(1).withMargin(10));
        mainRow.items.add(juce::FlexItem(distComponent).withFlex(1).withMargin(10));

        mainRow.performLayout(area);
    }

private:
    GateComponent gateComponent;
    CompressorComponent compressorComponent;
    DistortionComponent distComponent;
};