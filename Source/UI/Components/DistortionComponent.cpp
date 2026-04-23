#include "DistortionComponent.h"
#include "BinaryData.h"
#include "../../App/ParamIDs.h"

DistortionComponent::DistortionComponent(juce::AudioProcessorValueTreeState& apvts)
    : apvtsRef(apvts)
{
    loadImages();
    initButtons();
    initSliders();
    initLabels();
    initAttachments();

    apvtsRef.addParameterListener(ParamIDs::distBypass, this);
}

//------------------------------------------------------
void DistortionComponent::setupKnob(juce::Slider& s, double min, double max)
{
    s.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    s.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    s.setNumDecimalPlacesToDisplay(0);
    s.setRange(min, max);
    s.setPopupDisplayEnabled(true, true, this);
}

void DistortionComponent::setupLabel(juce::Label& l, const juce::String& text)
{
    l.setText(text, juce::dontSendNotification);
    l.setJustificationType(juce::Justification::centred);
    l.setFont(juce::Font(20.0f));
    l.setColour(juce::Label::textColourId, juce::Colours::black.withAlpha(0.7f));
}

void DistortionComponent::loadImages()
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

void DistortionComponent::initButtons()
{
    powerButton.setClickingTogglesState(true);

    powerButton.setImages(
        true,
        true,
        true,

        butOffImage, 1.0f, juce::Colours::transparentBlack,
        butOffImage,  1.0f, juce::Colours::transparentBlack,
        butOnImage,  1.0f, juce::Colours::transparentBlack
    );
    addAndMakeVisible(powerButton);
}

void DistortionComponent::initSliders()
{
    setupKnob(toneSlider, 1600.0, 16000.0);
    setupKnob(distSlider, 0.0, 30.0);
    setupKnob(levelSlider, -12.0, 12.0);

    knobLnf = std::make_unique<KnobLookAndFeel>(knobImage, knobShadowImage);

    toneSlider.setLookAndFeel(knobLnf.get());
    levelSlider.setLookAndFeel(knobLnf.get());
    distSlider.setLookAndFeel(knobLnf.get());

    addAndMakeVisible(toneSlider);
    addAndMakeVisible(levelSlider);
    addAndMakeVisible(distSlider);
}

void DistortionComponent::initLabels()
{
    setupLabel(toneLabel, "Tone");
    setupLabel(levelLabel, "Level");
    setupLabel(distLabel, "Dist");

    addAndMakeVisible(toneLabel);
    addAndMakeVisible(levelLabel);
    addAndMakeVisible(distLabel);
}

void DistortionComponent::initAttachments()
{
    attachments.tone = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        apvtsRef, ParamIDs::distTone, toneSlider);

    attachments.level = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        apvtsRef, ParamIDs::distLevel, levelSlider);

    attachments.dist = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        apvtsRef, ParamIDs::distDist, distSlider);

    attachments.bypass = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(
        apvtsRef, ParamIDs::distBypass, powerButton);
}

//------------------------------------------------------
void DistortionComponent::parameterChanged(const juce::String&, float)
{
    juce::MessageManager::callAsync([this]
    {
        repaint();
    });
}
//------------------------------------------------------
void DistortionComponent::paint(juce::Graphics& g)
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

    auto* bypass = apvtsRef.getRawParameterValue(ParamIDs::distBypass);
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
void DistortionComponent::resized()
{
    const int knobSize = 100;
    const int gapX = 20;
    const int labelH = 20;

    auto area = getLocalBounds().reduced(12);

    // ===== SLIDERS ROW =====
    auto topY = area.getY() + 10;
    auto botY = area.getY() + 130;

    // left top
    toneSlider.setBounds(
        juce::Rectangle<int>(area.getX() + 20, topY, 120, 120)
    );

    // right top
    distSlider.setBounds(
        juce::Rectangle<int>(area.getRight() - 140, topY, 120, 120)
    );

    // bottom center
    levelSlider.setBounds(
        juce::Rectangle<int>(area.getCentreX() - 60, botY, 120, 120)
    );

    // ===== TONE LABEL =====
    toneLabel.setBounds(
        juce::Rectangle<int>(
            area.getX() + 20,
            topY + 120,
            120,
            20
        )
    );

    // ===== DIST LABEL =====
    distLabel.setBounds(
        juce::Rectangle<int>(
            area.getRight() - 140,
            topY + 120,
            120,
            20
        )
    );

    // ===== MAKEUP GAIN LABEL =====
    levelLabel.setBounds(
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
DistortionComponent::~DistortionComponent()
{
    apvtsRef.removeParameterListener(ParamIDs::distBypass, this);

    toneSlider.setLookAndFeel(nullptr);
    levelSlider.setLookAndFeel(nullptr);
    distSlider.setLookAndFeel(nullptr);
}