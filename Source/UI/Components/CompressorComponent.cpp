#include "CompressorComponent.h"
#include "BinaryData.h"
#include "../../App/ParamIDs.h"

namespace GUI
{
    CompressorComponent::CompressorComponent(juce::AudioProcessorValueTreeState& apvts)
        : apvtsRef(apvts)
    {
        loadImages();
        initButtons();
        initSliders();
        initLabels();
        initAttachments();

        apvtsRef.addParameterListener(ParamIDs::compBypass, this);
        apvtsRef.addParameterListener(ParamIDs::compAttack, this);
        apvtsRef.addParameterListener(ParamIDs::compRatio, this);
        apvtsRef.addParameterListener(ParamIDs::compRelease, this);
        apvtsRef.addParameterListener(ParamIDs::compThreshold, this);
        apvtsRef.addParameterListener(ParamIDs::compMakeup, this);
    }

    //------------------------------------------------------
    void CompressorComponent::setupKnob(juce::Slider& s, double min, double max)
    {
        s.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
        s.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
        s.setNumDecimalPlacesToDisplay(0);
        s.setRange(min, max);
        s.setPopupDisplayEnabled(true, true, this);
    }

    void CompressorComponent::setupLabel(juce::Label& l, const juce::String& text)
    {
        l.setText(text, juce::dontSendNotification);
        l.setJustificationType(juce::Justification::centred);
        l.setFont(juce::Font(20.0f));
        l.setColour(juce::Label::textColourId, juce::Colours::black.withAlpha(0.7f));
    }

    void CompressorComponent::loadImages()
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

    void CompressorComponent::initButtons()
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

    void CompressorComponent::initSliders()
    {
        setupKnob(thresholdSlider, -100.0, 0.0);
        setupKnob(ratioSlider, 1.0, 60.0);
        setupKnob(attackSlider, 0.1, 200.0);
        setupKnob(releaseSlider, 10.0, 1000.0);
        setupKnob(makeupGainSlider, -12.0, 24.0);

        knobLnf = std::make_unique<KnobLookAndFeel>(knobImage, knobShadowImage);

        thresholdSlider.setLookAndFeel(knobLnf.get());
        ratioSlider.setLookAndFeel(knobLnf.get());
        attackSlider.setLookAndFeel(knobLnf.get());
        releaseSlider.setLookAndFeel(knobLnf.get());
        makeupGainSlider.setLookAndFeel(knobLnf.get());

        addAndMakeVisible(thresholdSlider);
        addAndMakeVisible(ratioSlider);
        addAndMakeVisible(releaseSlider);
        addAndMakeVisible(attackSlider);
        addAndMakeVisible(makeupGainSlider);
    }

    void CompressorComponent::initLabels()
    {
        setupLabel(thresholdLabel, "Threshold (dB)");
        setupLabel(ratioLabel, "Ratio X:1");
        setupLabel(attackLabel, "Attack (ms.)");
        setupLabel(releaseLabel, "Release (ms.)");
        setupLabel(makeupGainLabel, "Makeup Gain (dB)");

        addAndMakeVisible(thresholdLabel);
        addAndMakeVisible(ratioLabel);
        addAndMakeVisible(attackLabel);
        addAndMakeVisible(releaseLabel);
        addAndMakeVisible(makeupGainLabel);
    }

    void CompressorComponent::initAttachments()
    {
        attachments.threshold = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
            apvtsRef, ParamIDs::compThreshold, thresholdSlider);
        
        attachments.ratio = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
            apvtsRef, ParamIDs::compRatio, ratioSlider);

        attachments.attack = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
            apvtsRef, ParamIDs::compAttack, attackSlider);

        attachments.release = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
            apvtsRef, ParamIDs::compRelease, releaseSlider);

        attachments.makeupGain = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
            apvtsRef, ParamIDs::compMakeup, makeupGainSlider);

        attachments.bypass = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(
            apvtsRef, ParamIDs::compBypass, powerButton);
    }

    //------------------------------------------------------
    void CompressorComponent::parameterChanged(const juce::String&, float)
    {
        juce::MessageManager::callAsync([this]
        {
            repaint();
        });
    }
    //------------------------------------------------------
    void CompressorComponent::paint(juce::Graphics& g)
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

        auto* bypass = apvtsRef.getRawParameterValue(ParamIDs::compBypass);
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
    void CompressorComponent::resized()
    {
        const int knobSize = 100;
        const int gapX = 20;
        const int labelH = 20;

        auto area = getLocalBounds().reduced(12);

        // ===== SLIDERS ROW =====
        auto topY = area.getY() + 10;
        auto botY = area.getY() + 130;

        // left top
        thresholdSlider.setBounds(
            juce::Rectangle<int>(area.getX() + gapX, topY, knobSize, knobSize)
        );

        // right top
        ratioSlider.setBounds(
            juce::Rectangle<int>(area.getRight() - gapX - knobSize, topY, knobSize, knobSize)
        );

        // left mid
        releaseSlider.setBounds(
            juce::Rectangle<int>(area.getX() + gapX, botY, knobSize, knobSize)
        );
        
        // right mid
        attackSlider.setBounds(
            juce::Rectangle<int>(area.getRight() - gapX - knobSize, botY, knobSize, knobSize)
        );

        // center bottom
        makeupGainSlider.setBounds(
            juce::Rectangle<int>(area.getCentreX() - knobSize / 2, botY + knobSize + 20, knobSize, knobSize
            )
        );

        // ===== THRESHOLD LABEL =====
        thresholdLabel.setBounds(
            juce::Rectangle<int>(
                area.getX() + gapX,
                topY + knobSize,
                knobSize,
                labelH
            )
        );

        // ===== RATIO LABEL =====
        ratioLabel.setBounds(
            juce::Rectangle<int>(
                area.getRight() - gapX - knobSize,
                topY + knobSize,
                knobSize,
                labelH
            )
        );

        // ===== RELEASE LABEL =====
        releaseLabel.setBounds(
            juce::Rectangle<int>(
                area.getX() + gapX,
                botY + knobSize,
                knobSize,
                labelH
            )
        );

        // ===== ATTACK LABEL =====
        attackLabel.setBounds(
            juce::Rectangle<int>(
                area.getRight() - gapX - knobSize,
                botY + knobSize,
                knobSize,
                labelH
            )
        );

        // ===== MAKEUP GAIN LABEL =====
        makeupGainLabel.setBounds(
            juce::Rectangle<int>(
                makeupGainSlider.getX(),
                makeupGainSlider.getBottom(),
                knobSize,
                labelH
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
    CompressorComponent::~CompressorComponent()
    {
        apvtsRef.removeParameterListener(ParamIDs::compBypass, this);
        apvtsRef.removeParameterListener(ParamIDs::compAttack, this);
        apvtsRef.removeParameterListener(ParamIDs::compRatio, this);
        apvtsRef.removeParameterListener(ParamIDs::compRelease, this);
        apvtsRef.removeParameterListener(ParamIDs::compThreshold, this);
        apvtsRef.removeParameterListener(ParamIDs::compMakeup, this);

        thresholdSlider.setLookAndFeel(nullptr);
        ratioSlider.setLookAndFeel(nullptr);
        attackSlider.setLookAndFeel(nullptr);
        releaseSlider.setLookAndFeel(nullptr);
        makeupGainSlider.setLookAndFeel(nullptr);
    }
}