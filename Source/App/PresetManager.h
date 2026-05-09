#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_dsp/juce_dsp.h>

class PresetManager final : juce::ValueTree::Listener
{
public:
    PresetManager(juce::AudioProcessorValueTreeState&);
    ~PresetManager();

    static const juce::File defaultDirectory;
    static const juce::String extension;
    static const juce::String presetNameKey;

    void savePreset(const juce::String& presetName);
    void loadPreset(const juce::String& presetName);
    void deletePreset(const juce::String& presetName);
    int loadNextPreset();
    int loadPreviousPreset();
    juce::StringArray getAllPresets() const;
    juce::String getCurrentPreset() const;

private:
    void valueTreeRedirected(juce::ValueTree& treeWhichHasBeenChanged) override;

    juce::AudioProcessorValueTreeState& valueTreeState_;
    juce::Value currentPreset_;
};