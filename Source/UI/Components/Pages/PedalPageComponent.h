#pragma once

#include "../GateComponent.h"
#include "../CompressorComponent.h"
#include "../DistortionComponent.h"
#include "../FlangerComponent.h"
#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_gui_basics/juce_gui_basics.h>

class PedalPageComponent : public juce::Component
{
public:
    PedalPageComponent(juce::AudioProcessorValueTreeState& apvts)
        : gateComponent_(apvts), compressorComponent_(apvts), flangerComponent_(apvts), distComponent_(apvts)
    {
        addAndMakeVisible(gateComponent_);
        addAndMakeVisible(compressorComponent_);
        addAndMakeVisible(flangerComponent_);
        addAndMakeVisible(distComponent_);
    }

    void resized() override
    {
        constexpr float aspect = 1080.0f / 1917.0f; // pedal aspect

        auto area = getLocalBounds().reduced(10);
        const int count = 4;
        const int margin = 10;
        const int totalMargins = margin * (count + 1);
        const int pedalW = (area.getWidth() - totalMargins) / count;
        const int pedalH = juce::jmin((int)(pedalW / aspect), area.getHeight());

        const int offsetY = (area.getHeight() - pedalH) / 2;

        juce::FlexBox mainRow;
        mainRow.flexDirection = juce::FlexBox::Direction::row;
        mainRow.justifyContent = juce::FlexBox::JustifyContent::center;
        mainRow.alignItems = juce::FlexBox::AlignItems::center;

        mainRow.items.add(juce::FlexItem(gateComponent_)        .withWidth(pedalW).withHeight(pedalH).withMargin(margin));
        mainRow.items.add(juce::FlexItem(compressorComponent_)  .withWidth(pedalW).withHeight(pedalH).withMargin(margin));
        mainRow.items.add(juce::FlexItem(flangerComponent_)     .withWidth(pedalW).withHeight(pedalH).withMargin(margin));
        mainRow.items.add(juce::FlexItem(distComponent_)        .withWidth(pedalW).withHeight(pedalH).withMargin(margin));

        mainRow.performLayout(area);
    }

private:
    GUI::GateComponent          gateComponent_;
    GUI::CompressorComponent    compressorComponent_;
    GUI::FlangerComponent       flangerComponent_;
    GUI::DistortionComponent    distComponent_;
};