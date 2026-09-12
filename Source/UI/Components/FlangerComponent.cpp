#include "FlangerComponent.h"
#include "../../App/ParamIDs.h"
#include "BinaryData.h"

namespace GUI
{
    namespace
    {
        void placeCentered(juce::Component& component,
                            const juce::Rectangle<float>& body,
                            GUI::FlangerLayout::ControlDesc desc)
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

    FlangerComponent::FlangerComponent(juce::AudioProcessorValueTreeState& apvts) 
        : apvtsRef_(apvts)
    {
        loadImages();
        initButtons();
        initSliders();
        initAttachments();

        apvtsRef_.addParameterListener(ParamIDs::Flanger::Bypass,    this);
        apvtsRef_.addParameterListener(ParamIDs::Flanger::Rate,      this);
        apvtsRef_.addParameterListener(ParamIDs::Flanger::Range,     this);
        apvtsRef_.addParameterListener(ParamIDs::Flanger::Feedback,  this);
    }
    
    FlangerComponent::~FlangerComponent()
    {
        apvtsRef_.removeParameterListener(ParamIDs::Flanger::Bypass,     this);
        apvtsRef_.removeParameterListener(ParamIDs::Flanger::Rate,       this);
        apvtsRef_.removeParameterListener(ParamIDs::Flanger::Range,      this);
        apvtsRef_.removeParameterListener(ParamIDs::Flanger::Feedback,   this);

        knobs_.rate .setLookAndFeel(nullptr);
        knobs_.range.setLookAndFeel(nullptr);
        knobs_.color.setLookAndFeel(nullptr);
    }

    void FlangerComponent::paint(juce::Graphics& g)
    {
        auto body = getBodyRect();

        g.drawImageWithin(images_.body,
            body.getX(), body.getY(), body.getWidth(), body.getHeight(),
            juce::RectanglePlacement::stretchToFit);

        auto bodyF = body.toFloat();
        float lampSize = bodyF.getWidth() * FlangerLayout::lampSize;

        auto lampArea = juce::Rectangle<float>(
            bodyF.getX() + bodyF.getWidth()  * FlangerLayout::lamp.x - lampSize * 0.5f,
            bodyF.getY() + bodyF.getHeight() * FlangerLayout::lamp.y - lampSize * 0.5f,
            lampSize,
            lampSize
        );

        auto* bypass = apvtsRef_.getRawParameterValue(ParamIDs::Flanger::Bypass);
        auto isOn = bypass != nullptr && bypass->load() > 0.5f;

        g.drawImageWithin(
            isOn ? images_.lampOn : images_.lampOff,
            (int)lampArea.getX(), (int)lampArea.getY(),
            (int)lampArea.getWidth(), (int)lampArea.getHeight(),
            juce::RectanglePlacement::centred
        );
    }

    void FlangerComponent::resized()
    {
        auto body = getBodyRect().toFloat();

        placeCentered(knobs_.range, body,   FlangerLayout::range);
        placeCentered(knobs_.rate,  body,   FlangerLayout::rate);
        placeCentered(knobs_.color, body,   FlangerLayout::color);
        placeCentered(powerButton_, body,   FlangerLayout::button);
    }
    void FlangerComponent::parameterChanged(const juce::String&, float)
    {
        juce::MessageManager::callAsync([this]
        {
            repaint();
        });
    }
    void FlangerComponent::loadImages()
    {
        images_.body        = juce::ImageCache::getFromMemory(BinaryData::flanger_body_png,    BinaryData::flanger_body_pngSize);
        images_.lampOff     = juce::ImageCache::getFromMemory(BinaryData::lampOff_png,         BinaryData::lampOff_pngSize);
        images_.lampOn      = juce::ImageCache::getFromMemory(BinaryData::lampOn_png,          BinaryData::lampOn_pngSize);
        images_.buttonOff   = juce::ImageCache::getFromMemory(BinaryData::butOff_png,          BinaryData::butOff_pngSize);
        images_.buttonOn    = juce::ImageCache::getFromMemory(BinaryData::butOn_png,           BinaryData::butOn_pngSize);
        images_.knob        = juce::ImageCache::getFromMemory(BinaryData::knob_png,            BinaryData::knob_pngSize);
    }
    void FlangerComponent::initButtons()
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
    void FlangerComponent::initSliders()
    {
        setupKnob(knobs_.rate);
        setupKnob(knobs_.range);
        setupKnob(knobs_.color);

        knobLnf_ = std::make_unique<KnobLookAndFeel>(images_.knob);

        knobs_.rate .setLookAndFeel(knobLnf_.get());
        knobs_.range.setLookAndFeel(knobLnf_.get());
        knobs_.color.setLookAndFeel(knobLnf_.get());

        addAndMakeVisible(knobs_.rate);
        addAndMakeVisible(knobs_.range);
        addAndMakeVisible(knobs_.color);
    }
    void FlangerComponent::initAttachments()
    {
        attachments_.rate = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
            apvtsRef_, ParamIDs::Flanger::Rate, knobs_.rate);

        attachments_.range = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
            apvtsRef_, ParamIDs::Flanger::Range, knobs_.range);

        attachments_.color = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
            apvtsRef_, ParamIDs::Flanger::Feedback, knobs_.color);

        attachments_.bypass = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(
            apvtsRef_, ParamIDs::Flanger::Bypass, powerButton_);
    }

    void FlangerComponent::setupKnob(juce::Slider& s)
    {
        s.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
        s.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
        s.setNumDecimalPlacesToDisplay(0);
        s.setPopupDisplayEnabled(true, true, this);
    }

    juce::Rectangle<int> FlangerComponent::getBodyRect() const
    {
        constexpr float aspect = FlangerLayout::bodyAspect;
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
}