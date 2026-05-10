#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_gui_basics/juce_gui_basics.h>

namespace GUI
{
    class CabComponent final : public juce::Component,
                    public juce::AudioProcessorValueTreeState::Listener
    {
    public:
        CabComponent(juce::AudioProcessorValueTreeState& apvts,
                    std::function<void(const juce::File&)> onIRLoaded);
        ~CabComponent() override;

        void paint(juce::Graphics& g) override;
        void resized() override;
        void parameterChanged(const juce::String&, float) override;

    private:
        void openIRFilePicker();
        juce::AudioProcessorValueTreeState& apvtsRef_;

        juce::Image cabImage_;
        juce::TextButton irLoaderButton_;
        juce::TextButton powerButton_;
        std::unique_ptr<juce::FileChooser>  fileChooser_;
        std::function<void(const juce::File&)> onIRLoaded_;

        struct Attachments
        {
            std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> bypass;
        } attachments;

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(CabComponent)
    };
}