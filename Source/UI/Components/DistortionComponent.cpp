#include "DistortionComponent.h"
#include "BinaryData.h"
#include "../../App/ParamIDs.h"

namespace GUI
{
    namespace
    {
        void placeCentered(juce::Component& component,
                            const juce::Rectangle<float>& body,
                            GUI::DistortionLayout::ControlDesc desc)
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

    DistortionComponent::DistortionComponent(juce::AudioProcessorValueTreeState& apvts)
        : apvtsRef_(apvts)
    {
        loadImages();
        initButtons();
        initSliders();
        initAttachments();

        apvtsRef_.addParameterListener(ParamIDs::Distortion::Bypass, this);
        apvtsRef_.addParameterListener(ParamIDs::Distortion::Dist,   this);
        apvtsRef_.addParameterListener(ParamIDs::Distortion::Level,  this);
        apvtsRef_.addParameterListener(ParamIDs::Distortion::Tone,   this);
    }

    //------------------------------------------------------
    void DistortionComponent::setupKnob(juce::Slider& s)
    {
        s.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
        s.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
        s.setNumDecimalPlacesToDisplay(0);
        s.setPopupDisplayEnabled(true, true, this);
    }

    void DistortionComponent::loadImages()
    {
        images_.body        = juce::ImageCache::getFromMemory(BinaryData::drive_body_png, BinaryData::drive_body_pngSize);
        images_.lampOff     = juce::ImageCache::getFromMemory(BinaryData::lampOff_png,    BinaryData::lampOff_pngSize);
        images_.lampOn      = juce::ImageCache::getFromMemory(BinaryData::lampOn_png,     BinaryData::lampOn_pngSize);
        images_.buttonOff   = juce::ImageCache::getFromMemory(BinaryData::butOff_png,     BinaryData::butOff_pngSize);
        images_.buttonOn    = juce::ImageCache::getFromMemory(BinaryData::butOn_png,      BinaryData::butOn_pngSize);
        images_.knob        = juce::ImageCache::getFromMemory(BinaryData::knob_png,       BinaryData::knob_pngSize);
    }

    void DistortionComponent::initButtons()
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

    void DistortionComponent::initSliders()
    {
        setupKnob(knobs_.tone);
        setupKnob(knobs_.dist);
        setupKnob(knobs_.level);

        knobLnf_ = std::make_unique<KnobLookAndFeel>(images_.knob);

        knobs_.tone .setLookAndFeel(knobLnf_.get());
        knobs_.level.setLookAndFeel(knobLnf_.get());
        knobs_.dist .setLookAndFeel(knobLnf_.get());

        addAndMakeVisible(knobs_.tone);
        addAndMakeVisible(knobs_.level);
        addAndMakeVisible(knobs_.dist);
    }

    void DistortionComponent::initAttachments()
    {
        attachments_.tone = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
            apvtsRef_, ParamIDs::Distortion::Tone, knobs_.tone);

        attachments_.level = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
            apvtsRef_, ParamIDs::Distortion::Level, knobs_.level);

        attachments_.dist = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
            apvtsRef_, ParamIDs::Distortion::Dist, knobs_.dist);

        attachments_.bypass = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(
            apvtsRef_, ParamIDs::Distortion::Bypass, powerButton_);
    }

    void DistortionComponent::parameterChanged(const juce::String&, float)
    {
        juce::MessageManager::callAsync([this]
        {
            repaint();
        });
    }

    void DistortionComponent::paint(juce::Graphics& g)
    {
        auto body = getBodyRect();

        g.drawImageWithin(images_.body,
            body.getX(), body.getY(), body.getWidth(), body.getHeight(),
            juce::RectanglePlacement::stretchToFit);

        auto bodyF = body.toFloat();
        float lampSize = bodyF.getWidth() * DistortionLayout::lampSize;

        auto lampArea = juce::Rectangle<float>(
            bodyF.getX() + bodyF.getWidth()  * DistortionLayout::lamp.x - lampSize * 0.5f,
            bodyF.getY() + bodyF.getHeight() * DistortionLayout::lamp.y - lampSize * 0.5f,
            lampSize,
            lampSize
        );

        auto* bypass = apvtsRef_.getRawParameterValue(ParamIDs::Distortion::Bypass);
        auto isOn = bypass != nullptr && bypass->load() > 0.5f;

        g.drawImageWithin(
            isOn ? images_.lampOn : images_.lampOff,
            (int)lampArea.getX(), (int)lampArea.getY(),
            (int)lampArea.getWidth(), (int)lampArea.getHeight(),
            juce::RectanglePlacement::centred
        );
    }

    void DistortionComponent::resized()
    {
        auto body = getBodyRect().toFloat();

        placeCentered(knobs_.dist,  body,   DistortionLayout::drive);
        placeCentered(knobs_.tone,  body,   DistortionLayout::tone);
        placeCentered(knobs_.level, body,   DistortionLayout::level);
        placeCentered(powerButton_, body,   DistortionLayout::button);
    }

    juce::Rectangle<int> DistortionComponent::getBodyRect() const
    {
        constexpr float aspect = DistortionLayout::bodyAspect;
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

    DistortionComponent::~DistortionComponent()
    {
        apvtsRef_.removeParameterListener(ParamIDs::Distortion::Bypass,  this);
        apvtsRef_.removeParameterListener(ParamIDs::Distortion::Dist,    this);
        apvtsRef_.removeParameterListener(ParamIDs::Distortion::Level,   this);
        apvtsRef_.removeParameterListener(ParamIDs::Distortion::Tone,    this);

        knobs_.tone    .setLookAndFeel(nullptr);
        knobs_.level   .setLookAndFeel(nullptr);
        knobs_.dist    .setLookAndFeel(nullptr);
    }
}