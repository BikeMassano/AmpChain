#include "CabComponent.h"
#include "BinaryData.h"
#include "../../App/ParamIDs.h"

namespace GUI
{
    CabComponent::CabComponent(juce::AudioProcessorValueTreeState& apvts,
                               std::function<void(const juce::File&)> onIRLoaded)
        : apvtsRef_(apvts), onIRLoaded_(std::move(onIRLoaded))
    {
        cabImage_ = juce::ImageCache::getFromMemory(
            BinaryData::cab_png,
            BinaryData::cab_pngSize);

        apvtsRef_.addParameterListener(ParamIDs::Cabinet::Bypass, this);

        // --- IR name label ---
        irNameLabel_.setText("No IR loaded", juce::dontSendNotification);
        irNameLabel_.setFont(juce::Font(13.0f));
        irNameLabel_.setColour(juce::Label::textColourId,
                               juce::Colours::grey);
        irNameLabel_.setJustificationType(juce::Justification::centredLeft);
        irNameLabel_.setMinimumHorizontalScale(1.0f); // allow ellipsis on overflow
        addAndMakeVisible(irNameLabel_);

        // --- Load IR button (↑) ---
        irLoadButton_.setButtonText(juce::CharPointer_UTF8("\xe2\x86\x91")); // ↑
        irLoadButton_.setTooltip("Load impulse response");
        irLoadButton_.onClick = [this] { openIRFilePicker(); };
        addAndMakeVisible(irLoadButton_);

        // --- Clear IR button (✕) ---
        irClearButton_.setButtonText(juce::CharPointer_UTF8("\xe2\x9c\x95")); // ✕
        irClearButton_.setTooltip("Remove impulse response");
        irClearButton_.onClick = [this] { clearIR(); };
        irClearButton_.setEnabled(false); // disabled until an IR is loaded
        addAndMakeVisible(irClearButton_);

        // --- "Cab simulation" label ---
        cabLabel_.setText("Cab simulation", juce::dontSendNotification);
        cabLabel_.setFont(juce::Font(13.0f));
        cabLabel_.setColour(juce::Label::textColourId,
                            juce::Colours::lightgrey);
        cabLabel_.setJustificationType(juce::Justification::centredLeft);
        addAndMakeVisible(cabLabel_);

        // --- Power toggle ---
        powerButton_.setClickingTogglesState(true);
        powerButton_.setButtonText("ON");
        powerButton_.onStateChange = [this]
        {
            powerButton_.setButtonText(
                powerButton_.getToggleState() ? "ON" : "OFF");
        };
        bypass_ = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(
            apvtsRef_, ParamIDs::Cabinet::Bypass, powerButton_);
        addAndMakeVisible(powerButton_);
    }

    CabComponent::~CabComponent()
    {
        apvtsRef_.removeParameterListener(ParamIDs::Cabinet::Bypass, this);
    }

    void CabComponent::paint(juce::Graphics& g)
    {
        auto area = getLocalBounds().toFloat();

        if (cabImage_.isValid())
        {
            constexpr float UI_SCALE = 1.1f;
            const float imgW  = (float)cabImage_.getWidth();
            const float imgH  = (float)cabImage_.getHeight();
            const float scale = area.getWidth() * UI_SCALE / imgW;
            const float drawW = imgW * scale;
            const float drawH = imgH * scale;
            const float x     = area.getCentreX() - drawW * 0.5f;
            const float y     = area.getBottom()  - drawH;

            g.drawImage(cabImage_,
                        juce::Rectangle<float>(x, y, drawW, drawH));
        }
    }

    void CabComponent::resized()
    {
        auto area = getLocalBounds().reduced(8);

        // ── IR row (bottom) ──────────────────────────────────────────────
        // [ irNameLabel_ (flex) | irLoadButton_ 28px | irClearButton_ 28px ]
        {
            auto row = area.removeFromBottom(28);
            area.removeFromBottom(4); // gap

            irClearButton_.setBounds(row.removeFromRight(28));
            row.removeFromRight(4);
            irLoadButton_.setBounds(row.removeFromRight(28));
            row.removeFromRight(6);
            irNameLabel_.setBounds(row);
        }

        // ── Power toggle row (above IR row) ─────────────────────────────
        // [ cabLabel_ (flex) | powerButton_ 56px ]
        {
            auto row = area.removeFromBottom(28);
            area.removeFromBottom(4); // gap

            powerButton_.setBounds(row.removeFromRight(56));
            row.removeFromRight(8);
            cabLabel_.setBounds(row);
        }
    }

    void CabComponent::parameterChanged(const juce::String&, float)
    {
    }

    void CabComponent::openIRFilePicker()
    {
        fileChooser_ = std::make_unique<juce::FileChooser>(
            "Select Impulse Response",
            juce::File::getSpecialLocation(juce::File::userHomeDirectory),
            "*.wav;*.aif;*.aiff");

        constexpr auto flags =
            juce::FileBrowserComponent::openMode |
            juce::FileBrowserComponent::canSelectFiles;

        fileChooser_->launchAsync(flags, [this](const juce::FileChooser& fc)
        {
            const auto result = fc.getResult();
            if (result.existsAsFile())
            {
                irNameLabel_.setText(result.getFileNameWithoutExtension(),
                                     juce::dontSendNotification);
                irNameLabel_.setColour(juce::Label::textColourId,
                                       juce::Colours::white);
                irClearButton_.setEnabled(true);

                if (onIRLoaded_)
                    onIRLoaded_(result);
            }
        });
    }

    void CabComponent::clearIR()
    {
        irNameLabel_.setText("No IR loaded", juce::dontSendNotification);
        irNameLabel_.setColour(juce::Label::textColourId,
                               juce::Colours::grey);
        irClearButton_.setEnabled(false);

        // Notify the processor that IR was cleared (pass an invalid File)
        if (onIRLoaded_)
            onIRLoaded_(juce::File{});
    }
}