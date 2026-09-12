#include "GateComponent.h"
#include "BinaryData.h"
#include "../../App/ParamIDs.h"

namespace GUI
{
    namespace
    {
        void placeCentered(juce::Component& component,
                            const juce::Rectangle<float>& body,
                            GUI::GateLayout::ControlDesc desc)
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

    GateComponent::GateComponent(juce::AudioProcessorValueTreeState& apvts)
        : apvtsRef_(apvts)
    {
        loadImages();
        initButtons();
        initSliders();
        initAttachments();

        apvtsRef_.addParameterListener(ParamIDs::Gate::Bypass,       this);
        apvtsRef_.addParameterListener(ParamIDs::Gate::Threshold,    this);
        apvtsRef_.addParameterListener(ParamIDs::Gate::Attack,       this);
        apvtsRef_.addParameterListener(ParamIDs::Gate::Release,      this);
    }

    void GateComponent::setupKnob(juce::Slider& s)
    {
        s.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
        s.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
        s.setNumDecimalPlacesToDisplay(0);
        s.setPopupDisplayEnabled(true, true, this);
    }

    void GateComponent::loadImages()
    {
        images_.body        = juce::ImageCache::getFromMemory(BinaryData::gate_body_png,  BinaryData::gate_body_pngSize);
        images_.lampOff     = juce::ImageCache::getFromMemory(BinaryData::lampOff_png,    BinaryData::lampOff_pngSize);
        images_.lampOn      = juce::ImageCache::getFromMemory(BinaryData::lampOn_png,     BinaryData::lampOn_pngSize);
        images_.buttonOff   = juce::ImageCache::getFromMemory(BinaryData::butOff_png,     BinaryData::butOff_pngSize);
        images_.buttonOn    = juce::ImageCache::getFromMemory(BinaryData::butOn_png,      BinaryData::butOn_pngSize);
        images_.knob        = juce::ImageCache::getFromMemory(BinaryData::knob_png,       BinaryData::knob_pngSize);
    }

    void GateComponent::initButtons()
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

    void GateComponent::initSliders()
    {
        setupKnob(knobs_.threshold);
        setupKnob(knobs_.attack);
        setupKnob(knobs_.release);

        knobLnf_ = std::make_unique<KnobLookAndFeel>(images_.knob);

        knobs_.threshold.setLookAndFeel(knobLnf_.get());
        knobs_.attack   .setLookAndFeel(knobLnf_.get());
        knobs_.release  .setLookAndFeel(knobLnf_.get());

        addAndMakeVisible(knobs_.threshold);
        addAndMakeVisible(knobs_.release);
        addAndMakeVisible(knobs_.attack);
    }

    void GateComponent::initAttachments()
    {
        attachments_.threshold = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
            apvtsRef_, ParamIDs::Gate::Threshold, knobs_.threshold);

        attachments_.attack = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
            apvtsRef_, ParamIDs::Gate::Attack, knobs_.attack);

        attachments_.release = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
            apvtsRef_, ParamIDs::Gate::Release, knobs_.release);

        attachments_.bypass = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(
            apvtsRef_, ParamIDs::Gate::Bypass, powerButton_);
    }

    void GateComponent::parameterChanged(const juce::String&, float)
    {
        juce::MessageManager::callAsync([this]
        {
            repaint();
        });
    }

    void GateComponent::paint(juce::Graphics& g)
    {
        auto body = getBodyRect();

        g.drawImageWithin(images_.body,
            body.getX(), body.getY(), body.getWidth(), body.getHeight(),
            juce::RectanglePlacement::stretchToFit);

        auto bodyF = body.toFloat();

        float lampSize = bodyF.getWidth() * GateLayout::lampSize;

        auto lampArea = juce::Rectangle<float>(
            bodyF.getX() + bodyF.getWidth()  * GateLayout::lamp.x - lampSize * 0.5f,
            bodyF.getY() + bodyF.getHeight() * GateLayout::lamp.y - lampSize * 0.5f,
            lampSize,
            lampSize
        );

        auto* bypass = apvtsRef_.getRawParameterValue(ParamIDs::Gate::Bypass);

        auto isOn = bypass != nullptr && bypass->load() > 0.5f;

        g.drawImageWithin(
            isOn ? images_.lampOn : images_.lampOff,
            (int)lampArea.getX(), (int)lampArea.getY(),
            (int)lampArea.getWidth(), (int)lampArea.getHeight(),
            juce::RectanglePlacement::centred
        );
    }

    void GateComponent::resized()
    {
        auto body = getBodyRect().toFloat();

        placeCentered(knobs_.threshold, body,   GateLayout::threshold);
        placeCentered(knobs_.release,   body,   GateLayout::release);
        placeCentered(knobs_.attack,    body,   GateLayout::attack);
        placeCentered(powerButton_,     body,   GateLayout::button);
    }

    juce::Rectangle<int> GateComponent::getBodyRect() const
    {
        constexpr float aspect = GateLayout::bodyAspect;
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

    GateComponent::~GateComponent()
    {
        apvtsRef_.removeParameterListener(ParamIDs::Gate::Bypass,    this);
        apvtsRef_.removeParameterListener(ParamIDs::Gate::Threshold, this);
        apvtsRef_.removeParameterListener(ParamIDs::Gate::Attack,    this);
        apvtsRef_.removeParameterListener(ParamIDs::Gate::Release,   this);

        knobs_.threshold.setLookAndFeel(nullptr);
        knobs_.attack   .setLookAndFeel(nullptr);
        knobs_.release  .setLookAndFeel(nullptr);
    }
}