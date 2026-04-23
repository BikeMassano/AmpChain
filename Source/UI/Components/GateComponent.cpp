#include "GateComponent.h"
#include "BinaryData.h"
#include "../../App/ParamIDs.h"

GateComponent::GateComponent(juce::AudioProcessorValueTreeState& apvts)
    : apvtsRef(apvts)
{
    loadImages();
    initButtons();
    initSliders();
    initLabels();
    initAttachments();

    apvtsRef.addParameterListener(ParamIDs::gateBypass, this);
}

//------------------------------------------------------
void GateComponent::setupKnob(juce::Slider& s, double min, double max)
{
    s.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    s.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    s.setNumDecimalPlacesToDisplay(0);
    s.setRange(min, max);
    s.setPopupDisplayEnabled(true, true, this);
}

void GateComponent::setupLabel(juce::Label& l, const juce::String& text)
{
    l.setText(text, juce::dontSendNotification);
    l.setJustificationType(juce::Justification::centred);
    l.setFont(juce::Font(20.0f));
    l.setColour(juce::Label::textColourId, juce::Colours::black.withAlpha(0.7f));
}

void GateComponent::loadImages()
{
    bodyImage = juce::ImageCache::getFromMemory(
        BinaryData::body_png,
        BinaryData::body_pngSize
    );

    lampOffImage = juce::ImageCache::getFromMemory(
        BinaryData::lampOff_png,
        BinaryData::lampOff_pngSize
    );

    lampOnImage = juce::ImageCache::getFromMemory(
        BinaryData::lampOn_png,
        BinaryData::lampOn_pngSize
    );

    butOffImage = juce::ImageCache::getFromMemory(
        BinaryData::butOff_png,
        BinaryData::butOff_pngSize
    );

    butOnImage = juce::ImageCache::getFromMemory(
        BinaryData::butOn_png,
        BinaryData::butOn_pngSize
    );

    knobImage = juce::ImageCache::getFromMemory(
        BinaryData::knob_png,
        BinaryData::knob_pngSize
    );

    knobShadowImage = juce::ImageCache::getFromMemory(
        BinaryData::knob_shadow_png,
        BinaryData::knob_shadow_pngSize
    );
}

void GateComponent::initButtons()
{
    powerButton.setClickingTogglesState(true);

    powerButton.setImages(
        true,
        true,
        true,

        butOffImage, 1.0f, juce::Colours::transparentBlack,
        butOffImage, 1.0f, juce::Colours::transparentBlack,
        butOnImage, 1.0f, juce::Colours::transparentBlack
    );
    addAndMakeVisible(powerButton);
}

void GateComponent::initSliders()
{
    setupKnob(thresholdSlider, -100.0, 0.0);
    setupKnob(attackSlider, 0.1, 200.0);
    setupKnob(releaseSlider, 10.0, 1000.0);

    knobLnf = std::make_unique<KnobLookAndFeel>(knobImage, knobShadowImage);

    thresholdSlider.setLookAndFeel(knobLnf.get());
    attackSlider.setLookAndFeel(knobLnf.get());
    releaseSlider.setLookAndFeel(knobLnf.get());

    addAndMakeVisible(thresholdSlider);
    addAndMakeVisible(releaseSlider);
    addAndMakeVisible(attackSlider);
}

void GateComponent::initLabels()
{
    setupLabel(thresholdLabel, "Threshold (db.)");
    setupLabel(attackLabel, "Attack (ms.)");
    setupLabel(releaseLabel, "Release (ms.)");

    addAndMakeVisible(thresholdLabel);
    addAndMakeVisible(attackLabel);
    addAndMakeVisible(releaseLabel);
}

void GateComponent::initAttachments()
{
    attachments.threshold = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        apvtsRef, ParamIDs::gateThreshold, thresholdSlider);

    attachments.attack = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        apvtsRef, ParamIDs::gateAttack, attackSlider);

    attachments.release = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        apvtsRef, ParamIDs::gateRelease, releaseSlider);

    attachments.bypass = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(
        apvtsRef, ParamIDs::gateBypass, powerButton);
}

//------------------------------------------------------
void GateComponent::parameterChanged(const juce::String&, float)
{
    juce::MessageManager::callAsync([this]
    {
        repaint();
    });
}
//------------------------------------------------------
void GateComponent::paint(juce::Graphics& g)
{
    auto area = getLocalBounds().toFloat();
    
    // body
    g.drawImageWithin(
        bodyImage,
        (int)area.getX(),
        (int)area.getY(),
        (int)area.getWidth(),
        (int)area.getHeight(),
        juce::RectanglePlacement::stretchToFit
    );
    
    // power lamp
    auto size = area.getWidth() * 0.16f;

    auto lampArea = juce::Rectangle<float>(
        area.getCentreX() - size * 0.5f,
        area.getCentreY() + area.getHeight() * 0.08f,
        size,
        size
    );

    auto* bypass = apvtsRef.getRawParameterValue(ParamIDs::gateBypass);
    auto isOn = bypass != nullptr && bypass->load() > 0.5f;

    g.drawImageWithin(
        isOn ? lampOnImage : lampOffImage,
        (int)lampArea.getX(),
        (int)lampArea.getY(),
        (int)lampArea.getWidth(),
        (int)lampArea.getHeight(),
        juce::RectanglePlacement::centred
    );
}
//------------------------------------------------------
void GateComponent::resized()
{
    auto area = getLocalBounds().reduced(12);

    // ===== SLIDERS ROW =====
    auto topY = area.getY() + 10;
    auto botY = area.getY() + 150;

    // left top
    thresholdSlider.setBounds(
        juce::Rectangle<int>(area.getX() + 20, topY, 120, 120)
    );

    // right top
    releaseSlider.setBounds(
        juce::Rectangle<int>(area.getRight() - 140, topY, 120, 120)
    );

    // bottom center
    attackSlider.setBounds(
        juce::Rectangle<int>(area.getCentreX() - 60, botY, 120, 120)
    );

    // ===== THRESHOLD LABEL =====
    thresholdLabel.setBounds(
        juce::Rectangle<int>(
            area.getX() + 20,
            topY + 120,
            120,
            20
        )
    );

    // ===== RELEASE LABEL =====
    releaseLabel.setBounds(
        juce::Rectangle<int>(
            area.getRight() - 140,
            topY + 120,
            120,
            20
        )
    );

    // ===== ATTACK LABEL =====
    attackLabel.setBounds(
        juce::Rectangle<int>(
            area.getCentreX() - 60,
            botY + 120,
            120,
            20
        )
    );

    // ===== POWER BUTTON POSITION =====
    auto buttonSize = area.getWidth() * 0.18f;

    juce::Rectangle<int> buttonBounds(
        (int)(area.getCentreX() - buttonSize * 0.5f),
        (int)(area.getCentreY() + area.getHeight() * 0.29f - buttonSize * 0.5f),
        (int)buttonSize,
        (int)buttonSize
    );

    powerButton.setBounds(buttonBounds);
}
//------------------------------------------------------
GateComponent::~GateComponent()
{
    apvtsRef.removeParameterListener(ParamIDs::gateBypass, this);

    thresholdSlider.setLookAndFeel(nullptr);
    attackSlider.setLookAndFeel(nullptr);
    releaseSlider.setLookAndFeel(nullptr);
}