#pragma once

#include "PluginProcessor.h"
#include "../UI/Components/LevelMeterComponent.h"
#include "../UI/Components/Pages/PedalPageComponent.h"
#include "../UI/Components/Pages/AmpPageComponent.h"
#include "../UI/Components/Pages/CabPageComponent.h"

namespace EditorLayout
{
    
    constexpr float topBarHeight          = 0.062f;
    constexpr float bottomBarHeight       = 0.062f;

    constexpr float rightBlockWidth      = 0.25f;

    constexpr float pedalBtnWidth   = 0.075f;

    constexpr float presetArrowWidth    = 0.025f;
    constexpr float presetActionWidth   = 0.04f;
    constexpr float presetBoxWidth      = 0.18f;

    constexpr float presetHeight    = 0.037f;

    constexpr int presetGap         = 4;
    constexpr int panelPadding      = 10;

    constexpr int meterWidth        = 150;
    constexpr int meterHeight       = 8;
    constexpr int meterGap          = 5;

    constexpr int monoButtonWidth   = 80;
    }


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

    juce::ComboBox presetBox;

    juce::TextButton savePresetButton, deletePresetButton;
    juce::TextButton prevPresetButton, nextPresetButton;

    GUI::LevelMeterComponent leftInMeter, rightInMeter, leftOutMeter, rightOutMeter;

    juce::TextButton ampButton, pedalButton, cabButton, monoButton;

    PedalPageComponent pedalPage;
    AmpPageComponent ampPage;
    CabPageComponent cabPage;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AudioPluginAudioProcessorEditor)
};
