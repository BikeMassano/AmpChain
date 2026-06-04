#include "CabComponent.h"
#include "BinaryData.h"
#include "../../App/ParamIDs.h"

namespace GUI
{
    CabComponent::CabComponent(juce::AudioProcessorValueTreeState& apvts)
        : apvtsRef_(apvts)
    {
        cabImage_ = juce::ImageCache::getFromMemory(
            BinaryData::cab_png,
            BinaryData::cab_pngSize);

        apvtsRef_.addParameterListener(ParamIDs::Cabinet::Bypass, this);

        irNameLabel_.setText("No IR loaded", juce::dontSendNotification);
        irNameLabel_.setFont(juce::Font(13.0f));
        irNameLabel_.setColour(juce::Label::textColourId,
                               juce::Colours::grey);
        irNameLabel_.setJustificationType(juce::Justification::centredLeft);
        irNameLabel_.setMinimumHorizontalScale(1.0f); // allow ellipsis on overflow
        addAndMakeVisible(irNameLabel_);

        irLoadButton_.setButtonText(juce::CharPointer_UTF8("Load IR"));
        irLoadButton_.setTooltip("Load impulse response");
        irLoadButton_.onClick = [this] { openIRFilePicker(); };
        addAndMakeVisible(irLoadButton_);

        apvtsRef_.state.addListener(this);
        updateIRLabel_();

        irClearButton_.setButtonText(juce::CharPointer_UTF8("\xe2\x9c\x95"));
        irClearButton_.setTooltip("Remove impulse response");
        irClearButton_.onClick = [this] { clearIR(); };
        irClearButton_.setEnabled(false);
        addAndMakeVisible(irClearButton_);

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
        apvtsRef_.state.removeListener(this);
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
        auto area = getLocalBounds();
        {
            const int btnH   = 32;
            const int btnW   = 100;
            const int clearW = 32;
            const int stripY = 6;
            const int startX = 6;

            irLoadButton_ .setBounds(startX,                    stripY, btnW,   btnH);
            irClearButton_.setBounds(startX + btnW + 4,         stripY, clearW, btnH);
            powerButton_  .setBounds(startX, stripY + btnH, btnW, btnH);
            irNameLabel_  .setBounds(startX + btnW + clearW + 12, stripY, area.getWidth() - btnW - clearW - 18, btnH);
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

                if (onIRLoad)
                    onIRLoad(result);
            }
        });
    }

    void CabComponent::clearIR()
    {
        irNameLabel_.setText("No IR loaded", juce::dontSendNotification);
        irNameLabel_.setColour(juce::Label::textColourId,
                               juce::Colours::grey);
        irClearButton_.setEnabled(false);

        if (onIRLoad)
            onIRLoad(juce::File{});
    }

    void CabComponent::updateIRLabel_()
    {
        auto path = apvtsRef_.state.getProperty("irPath").toString();
        auto name = path.isNotEmpty()
            ? juce::File(path).getFileNameWithoutExtension()
            : "No IR loaded";
        auto colour = path.isNotEmpty() ? juce::Colours::white : juce::Colours::grey;
        bool hasIR = path.isNotEmpty();

        juce::MessageManager::callAsync([this, name, colour, hasIR]
        {
            irNameLabel_.setText(name, juce::dontSendNotification);
            irNameLabel_.setColour(juce::Label::textColourId, colour);
            irClearButton_.setEnabled(hasIR);
        });
    }
}