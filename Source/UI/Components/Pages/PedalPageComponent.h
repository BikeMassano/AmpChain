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
        : gateComponent_(apvts), compressorComponent_(apvts), distComponent_(apvts)
    {
        addAndMakeVisible(gateComponent_);
        addAndMakeVisible(compressorComponent_);
        addAndMakeVisible(distComponent_);
    }

    void resized() override
    {
        auto area = getLocalBounds().reduced(10);

        juce::FlexBox mainRow;
        mainRow.flexDirection = juce::FlexBox::Direction::row;

        mainRow.items.add(juce::FlexItem(gateComponent_).withFlex(1).withMargin(10));
        mainRow.items.add(juce::FlexItem(compressorComponent_).withFlex(1).withMargin(10));
        mainRow.items.add(juce::FlexItem(distComponent_).withFlex(1).withMargin(10));

        mainRow.performLayout(area);
    }

private:
    GUI::GateComponent gateComponent_;
    GUI::CompressorComponent compressorComponent_;
    GUI::DistortionComponent distComponent_;
};