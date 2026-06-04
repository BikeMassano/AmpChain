#pragma once

#include "../CabComponent.h"
#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_gui_basics/juce_gui_basics.h>

class CabPageComponent : public juce::Component
{
public:
    CabPageComponent(juce::AudioProcessorValueTreeState& apvts)
        : cabComponent_(apvts) 
    {
        cabComponent_.onIRLoad = [this](const juce::File& f)
        {
            if (onIRLoad) onIRLoad(f);
        };

        addAndMakeVisible(cabComponent_);
    }

    void resized() override
    {
        cabComponent_.setBounds(getLocalBounds());
    }

    std::function<void(const juce::File&)> onIRLoad;

private:
    GUI::CabComponent cabComponent_;
};