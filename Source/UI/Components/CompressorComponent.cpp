#include "CompressorComponent.h"
#include "BinaryData.h"
#include "../../App/ParamIDs.h"

namespace GUI
{
    namespace
    {
        void placeCentered(juce::Component& component,
                            const juce::Rectangle<float>& body,
                            GUI::CompressorLayout::ControlDesc desc)
        {
            auto size = body.getWidth() * desc.size;
            
            component.setBounds(
                juce::Rectangle<int>(
                    (int)(body.getX() + body.getWidth()  * desc.x - size * 0.5f),
                    (int)(body.getY() + body.getHeight() * desc.y - size * 0.5f),
                    (int)size,
                    (int)size
                )
            );
        }
    }

    CompressorComponent::CompressorComponent(juce::AudioProcessorValueTreeState& apvts)
        : apvtsRef_(apvts)
    {
        loadImages();
        initButtons();
        initSliders();
        initAttachments();

        apvtsRef_.addParameterListener(ParamIDs::Compressor::Bypass,     this);
        apvtsRef_.addParameterListener(ParamIDs::Compressor::Attack,     this);
        apvtsRef_.addParameterListener(ParamIDs::Compressor::Ratio,      this);
        apvtsRef_.addParameterListener(ParamIDs::Compressor::Release,    this);
        apvtsRef_.addParameterListener(ParamIDs::Compressor::Threshold,  this);
        apvtsRef_.addParameterListener(ParamIDs::Compressor::Makeup,     this);
    }

    void CompressorComponent::setupKnob(juce::Slider& s)
    {
        s.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
        s.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
        s.setNumDecimalPlacesToDisplay(1);
        s.setPopupDisplayEnabled(true, true, this);
    }

    void CompressorComponent::loadImages()
    {
        images_.body        = juce::ImageCache::getFromMemory(BinaryData::comp_body_png,  BinaryData::comp_body_pngSize);
        images_.lampOff     = juce::ImageCache::getFromMemory(BinaryData::lampOff_png,    BinaryData::lampOff_pngSize);
        images_.lampOn      = juce::ImageCache::getFromMemory(BinaryData::lampOn_png,     BinaryData::lampOn_pngSize);
        images_.buttonOff   = juce::ImageCache::getFromMemory(BinaryData::butOff_png,     BinaryData::butOff_pngSize);
        images_.buttonOn    = juce::ImageCache::getFromMemory(BinaryData::butOn_png,      BinaryData::butOn_pngSize);
        images_.knob        = juce::ImageCache::getFromMemory(BinaryData::knob_png,       BinaryData::knob_pngSize);
    }

    void CompressorComponent::initButtons()
    {
        powerButton_.setClickingTogglesState(true);
        powerButton_.setImages(
            true, true, true,
            images_.buttonOff,    1.0f, juce::Colours::transparentBlack,
            images_.buttonOff,    1.0f, juce::Colours::transparentBlack,
            images_.buttonOn,     1.0f, juce::Colours::transparentBlack
        );
        addAndMakeVisible(powerButton_);
    }

    void CompressorComponent::initSliders()
    {
        setupKnob(knobs_.threshold);
        setupKnob(knobs_.ratio);
        setupKnob(knobs_.attack);
        setupKnob(knobs_.release);
        setupKnob(knobs_.makeup);

        knobLnf_ = std::make_unique<KnobLookAndFeel>(images_.knob);

        knobs_.threshold.setLookAndFeel(knobLnf_.get());
        knobs_.ratio    .setLookAndFeel(knobLnf_.get());
        knobs_.attack   .setLookAndFeel(knobLnf_.get());
        knobs_.release  .setLookAndFeel(knobLnf_.get());
        knobs_.makeup   .setLookAndFeel(knobLnf_.get());

        addAndMakeVisible(knobs_.threshold);
        addAndMakeVisible(knobs_.ratio);
        addAndMakeVisible(knobs_.release);
        addAndMakeVisible(knobs_.attack);
        addAndMakeVisible(knobs_.makeup);
    }

    void CompressorComponent::initAttachments()
    {
        attachments_.threshold = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
            apvtsRef_, ParamIDs::Compressor::Threshold, knobs_.threshold);
        
        attachments_.ratio = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
            apvtsRef_, ParamIDs::Compressor::Ratio, knobs_.ratio);

        attachments_.attack = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
            apvtsRef_, ParamIDs::Compressor::Attack, knobs_.attack);

        attachments_.release = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
            apvtsRef_, ParamIDs::Compressor::Release, knobs_.release);

        attachments_.makeupGain = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
            apvtsRef_, ParamIDs::Compressor::Makeup, knobs_.makeup);

        attachments_.bypass = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(
            apvtsRef_, ParamIDs::Compressor::Bypass, powerButton_);
    }

    void CompressorComponent::parameterChanged(const juce::String&, float)
    {
        juce::MessageManager::callAsync([this]
        {
            repaint();
        });
    }

    void CompressorComponent::paint(juce::Graphics& g)
    {
        auto body = getBodyRect();

        g.drawImageWithin(images_.body,
            body.getX(), body.getY(), body.getWidth(), body.getHeight(),
            juce::RectanglePlacement::stretchToFit);

        auto bodyF = body.toFloat();
        float lampSize = bodyF.getWidth() * CompressorLayout::lampSize;

        auto lampArea = juce::Rectangle<float>(
            bodyF.getX() + bodyF.getWidth()  * CompressorLayout::lamp.x - lampSize * 0.5f,
            bodyF.getY() + bodyF.getHeight() * CompressorLayout::lamp.y - lampSize * 0.5f,
            lampSize,
            lampSize
        );

        auto* bypass = apvtsRef_.getRawParameterValue(ParamIDs::Compressor::Bypass);
        auto isOn = bypass != nullptr && bypass->load() > 0.5f;

        g.drawImageWithin(
            isOn ? images_.lampOn : images_.lampOff,
            (int)lampArea.getX(), (int)lampArea.getY(),
            (int)lampArea.getWidth(), (int)lampArea.getHeight(),
            juce::RectanglePlacement::centred
        );
    }

    void CompressorComponent::resized()
    {
        auto body = getBodyRect().toFloat();

        placeCentered(knobs_.threshold, body,   CompressorLayout::threshold);
        placeCentered(knobs_.ratio,     body,   CompressorLayout::ratio);
        placeCentered(knobs_.release,   body,   CompressorLayout::release);
        placeCentered(knobs_.attack,    body,   CompressorLayout::attack);
        placeCentered(knobs_.makeup,    body,   CompressorLayout::makeup);
        placeCentered(powerButton_,     body,   CompressorLayout::button);
    }

    juce::Rectangle<int> CompressorComponent::getBodyRect() const
    {
        constexpr float aspect = CompressorLayout::bodyAspect;
        auto bounds = getLocalBounds();
        int w = bounds.getWidth();
        int h = (int)(w / aspect);

        if (h > bounds.getHeight())
        {
            h = bounds.getHeight();
            w = (int)(h * aspect);
        }

        return juce::Rectangle<int>(
            (bounds.getWidth() - w) / 2,
            (bounds.getHeight() - h) / 2,
            w, h
        );
    }

    //------------------------------------------------------
    CompressorComponent::~CompressorComponent()
    {
        apvtsRef_.removeParameterListener(ParamIDs::Compressor::Bypass,      this);
        apvtsRef_.removeParameterListener(ParamIDs::Compressor::Attack,      this);
        apvtsRef_.removeParameterListener(ParamIDs::Compressor::Ratio,       this);
        apvtsRef_.removeParameterListener(ParamIDs::Compressor::Release,     this);
        apvtsRef_.removeParameterListener(ParamIDs::Compressor::Threshold,   this);
        apvtsRef_.removeParameterListener(ParamIDs::Compressor::Makeup,      this);

        knobs_.threshold    .setLookAndFeel(nullptr);
        knobs_.ratio        .setLookAndFeel(nullptr);
        knobs_.attack       .setLookAndFeel(nullptr);
        knobs_.release      .setLookAndFeel(nullptr);
        knobs_.makeup       .setLookAndFeel(nullptr);
    }
}