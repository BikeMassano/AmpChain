#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_gui_basics/juce_gui_basics.h>

namespace GUI
{
    class CabComponent final : public juce::Component,
                    public juce::AudioProcessorValueTreeState::Listener,
                    private juce::ValueTree::Listener
    {
    public:
        CabComponent(juce::AudioProcessorValueTreeState& apvts);
        ~CabComponent() override;

        void paint(juce::Graphics& g) override;
        void resized() override;
        void parameterChanged(const juce::String&, float) override;

        std::function<void(const juce::File&)> onIRLoad;

    private:
        void openIRFilePicker();
        void clearIR();

        void updateIRLabel_();
        void valueTreeRedirected(juce::ValueTree&) override { updateIRLabel_(); }
        void valueTreePropertyChanged(juce::ValueTree&, const juce::Identifier&) override { updateIRLabel_(); }

        juce::AudioProcessorValueTreeState& apvtsRef_;

        juce::Image cabImage_;

        juce::Label         irNameLabel_;
        juce::TextButton    irLoadButton_;
        juce::TextButton    irClearButton_;

        juce::TextButton    powerButton_;
        
        std::unique_ptr<juce::FileChooser>  fileChooser_;

        std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> bypass_;

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(CabComponent)
    };
}