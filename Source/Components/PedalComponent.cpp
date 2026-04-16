#include "PedalComponent.h"

PedalComponent::PedalComponent(juce::AudioProcessorValueTreeState& state,
                               const juce::String& bypassParamID)
    : apvts(state), bypassID(bypassParamID)
{
    apvts.addParameterListener(bypassID, this);

    powerButton.setClickingTogglesState(true);
    addAndMakeVisible(powerButton);
}

PedalComponent::~PedalComponent()
{
    apvts.removeParameterListener(bypassID, this);
}

void PedalComponent::setBodyImage(const juce::Image& img)
{
    bodyImage = img;
}

void PedalComponent::setLampImages(const juce::Image& on,
                                  const juce::Image& off)
{
    lampOnImage = on;
    lampOffImage = off;
}

void PedalComponent::setupPowerButton(juce::Image onImg,
                                      juce::Image offImg)
{
    powerButton.setImages(true, true, true,
                          offImg, 1.0f, juce::Colours::transparentBlack,
                          offImg, 1.0f, juce::Colours::transparentBlack,
                          onImg,  1.0f, juce::Colours::transparentBlack);
}

void PedalComponent::paint(juce::Graphics& g)
{
    paintBackground(g);

    auto area = getLocalBounds().toFloat();

    if (bodyImage.isValid())
    {
        g.drawImageWithin(bodyImage,
                          area.getX(), area.getY(),
                          area.getWidth(), area.getHeight(),
                          juce::RectanglePlacement::stretchToFit);
    }

    paintLamp(g, isOn.load());
}

void PedalComponent::paintBackground(juce::Graphics&)
{
    // override if needed
}

void PedalComponent::paintLamp(juce::Graphics& g, bool on)
{
    auto area = getLocalBounds().toFloat();

    auto size = area.getWidth() * 0.16f;

    juce::Rectangle<float> lampArea(
        area.getCentreX() - size * 0.5f,
        area.getCentreY() + area.getHeight() * 0.08f,
        size, size
    );

    auto& img = on ? lampOnImage : lampOffImage;

    if (img.isValid())
    {
        g.drawImageWithin(img,
                          (int)lampArea.getX(),
                          (int)lampArea.getY(),
                          (int)lampArea.getWidth(),
                          (int)lampArea.getHeight(),
                          juce::RectanglePlacement::centred);
    }
}

void PedalComponent::resized()
{
    auto area = getLocalBounds().reduced(12);

    auto size = area.getWidth() * 0.18f;

    powerButton.setBounds(
        (int)(area.getCentreX() - size * 0.5f),
        (int)(area.getCentreY() - size * 0.5f),
        (int)size,
        (int)size
    );
}

void PedalComponent::parameterChanged(const juce::String& id, float value)
{
    if (id == bypassID)
        updateLampStateAsync();
}

void PedalComponent::updateLampStateAsync()
{
    juce::MessageManager::callAsync([this]
    {
        auto* param = apvts.getRawParameterValue(bypassID);
        isOn.store(param != nullptr && param->load() > 0.5f);
        repaint();
    });
}