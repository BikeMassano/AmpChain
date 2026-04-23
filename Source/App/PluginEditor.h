#pragma once

#include "PluginProcessor.h"
#include "../UI/Components/LevelMeterComponent.h"
#include "../UI/Components/Pages/PedalPageComponent.h"
#include "../UI/Components/Pages/AmpPageComponent.h"
#include "../UI/Components/Pages/CabPageComponent.h"


//==============================================================================
class AudioPluginAudioProcessorEditor final : public juce::AudioProcessorEditor,
                                            public juce::Timer
{
public:
    explicit AudioPluginAudioProcessorEditor (AudioPluginAudioProcessor&);
    ~AudioPluginAudioProcessorEditor() override;

    //==============================================================================
    void timerCallback() override;
    void paint (juce::Graphics&) override;
    void resized() override;
    //==============================================================================
private:
    enum class Page
    {
        Amp,
        Pedal,
        Cab
    };

    Page currentPage = Page::Amp;

    AudioPluginAudioProcessor& processorRef;

    juce::Component topBar, bottomBar;

    LevelMeterComponent leftInMeter, rightInMeter, leftOutMeter, rightOutMeter;

    juce::TextButton ampButton, pedalButton, cabButton, monoButton;

    PedalPageComponent pedalPage;
    AmpPageComponent ampPage;
    CabPageComponent cabPage;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AudioPluginAudioProcessorEditor)
};
