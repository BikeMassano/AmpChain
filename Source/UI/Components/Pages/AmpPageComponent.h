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

        ampComponent_.onModelLoad = [this](const juce::File& f)
        {
            if (onModelLoad) onModelLoad(f);
        };
    }

    void resized() override
    {
        ampComponent_.setBounds(getLocalBounds());
    }

    std::function<void(const juce::File&)> onModelLoad;

private:
    GUI::AmpComponent ampComponent_;
};