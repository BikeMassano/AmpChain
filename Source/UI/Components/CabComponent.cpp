#include "CabComponent.h"
#include "BinaryData.h"
#include "../../App/ParamIDs.h"


CabComponent::CabComponent(juce::AudioProcessorValueTreeState& apvts,
                        std::function<void(const juce::File&)> onIRLoaded) 
                        : apvtsRef_(apvts), onIRLoaded_(std::move(onIRLoaded))
{
    cabImage_ = juce::ImageCache::getFromMemory(
        BinaryData::cab_png,
        BinaryData::cab_pngSize
    );

    apvtsRef_.addParameterListener(ParamIDs::cabBypass, this);

    irLoaderButton_.setButtonText("Load IR");
    irLoaderButton_.onClick = [this] 
    { 
        openIRFilePicker(); 
    };

    powerButton_.setClickingTogglesState(true);
    powerButton_.onStateChange = [this]
    {
        if (powerButton_.getToggleState())
        {
            powerButton_.setButtonText("ON");
        }
        else
        {
            powerButton_.setButtonText("OFF");
        }
    };

    attachments.bypass = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(
        apvtsRef_, ParamIDs::cabBypass, powerButton_);

    addAndMakeVisible(irLoaderButton_);
    addAndMakeVisible(powerButton_);
}

CabComponent::~CabComponent()
{
    apvtsRef_.removeParameterListener(ParamIDs::cabBypass, this);
}

void CabComponent::paint(juce::Graphics& g)
{
    auto area = getLocalBounds().toFloat();
    if (cabImage_.isValid())
    {
        float UI_SCALE = 1.1f;

        float imgW = (float)cabImage_.getWidth();
        float imgH = (float)cabImage_.getHeight();

        float scale = area.getWidth() * UI_SCALE / imgW;

        float drawW = imgW * scale;
        float drawH = imgH * scale;

        float x = area.getCentreX() - drawW * 0.5f;
        float y = area.getBottom() - drawH;

        g.drawImage(cabImage_,
                    juce::Rectangle<float>(x, y, drawW, drawH));
    }
}

void CabComponent::resized()
{
    auto area = getLocalBounds().reduced(8);
    
    irLoaderButton_.setBounds(area.removeFromBottom(28));
    area.removeFromBottom(4); // gap
    powerButton_.setBounds(area.removeFromBottom(28));
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
            irLoaderButton_.setButtonText(result.getFileNameWithoutExtension());
            if (onIRLoaded_)
                onIRLoaded_(result);
        }
    });
}
