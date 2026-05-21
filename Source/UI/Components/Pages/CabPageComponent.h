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
        cabComponent_.setBounds(getLocalBounds());
    }

private:
    GUI::CabComponent cabComponent_;
};