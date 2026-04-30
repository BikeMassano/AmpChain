#include "AmpComponent.h"
#include "BinaryData.h"
#include "../../App/ParamIDs.h"

AmpComponent::AmpComponent(juce::AudioProcessorValueTreeState& apvts)
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
void AmpComponent::setupKnob(juce::Slider& s, double min, double max)
{
    s.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    s.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    s.setNumDecimalPlacesToDisplay(0);
    s.setRange(min, max);
    s.setPopupDisplayEnabled(true, true, this);
}

void AmpComponent::setupLabel(juce::Label& l, const juce::String& text)
{
    l.setText(text, juce::dontSendNotification);
    l.setJustificationType(juce::Justification::centred);
    l.setFont(juce::Font(13.0f));
    l.setColour(juce::Label::textColourId, juce::Colours::black.withAlpha(0.7f));
}

void AmpComponent::loadImages()
{
    ampImage = juce::ImageCache::getFromMemory(
        BinaryData::ampHead_png,
        BinaryData::ampHead_pngSize
    );

    cabImage = juce::ImageCache::getFromMemory(
        BinaryData::ampCab_png,
        BinaryData::ampCab_pngSize
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
        BinaryData::ampOff_png,
        BinaryData::ampOff_pngSize
    );

    butOnImage = juce::ImageCache::getFromMemory(
        BinaryData::ampOn_png,
        BinaryData::ampOn_pngSize
    );

    knobImage = juce::ImageCache::getFromMemory(
        BinaryData::ampKnob_png,
        BinaryData::ampKnob_pngSize
    );

    knobShadowImage = juce::ImageCache::getFromMemory(
        BinaryData::knob_shadow_png,
        BinaryData::knob_shadow_pngSize
    );
}

void AmpComponent::initButtons()
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

void AmpComponent::initSliders()
{
    setupKnob(bassSlider, 0, 10);
    setupKnob(midSlider, 0, 10);
    setupKnob(trebleSlider, 0, 10);
    setupKnob(levelSlider, 0, 10);
    setupKnob(gainSlider, 0, 10);
    setupKnob(presenceSlider, 0, 10);

    knobLnf = std::make_unique<KnobLookAndFeel>(knobImage, knobShadowImage);

    bassSlider.setLookAndFeel(knobLnf.get());
    midSlider.setLookAndFeel(knobLnf.get());
    trebleSlider.setLookAndFeel(knobLnf.get());
    levelSlider.setLookAndFeel(knobLnf.get());
    gainSlider.setLookAndFeel(knobLnf.get());
    presenceSlider.setLookAndFeel(knobLnf.get());

    addAndMakeVisible(bassSlider);
    addAndMakeVisible(midSlider);
    addAndMakeVisible(trebleSlider);
    addAndMakeVisible(levelSlider);
    addAndMakeVisible(gainSlider);
    addAndMakeVisible(presenceSlider);
}

void AmpComponent::initLabels()
{
    setupLabel(bassLabel, "Bass");
    setupLabel(midLabel, "Mid");
    setupLabel(trebleLabel, "Treble");
    setupLabel(levelLabel, "Level");
    setupLabel(gainLabel, "Gain");
    setupLabel(presenceLabel, "Presence");

    addAndMakeVisible(bassLabel);
    addAndMakeVisible(midLabel);
    addAndMakeVisible(trebleLabel);
    addAndMakeVisible(levelLabel);
    addAndMakeVisible(gainLabel);
    addAndMakeVisible(presenceLabel);
}

void AmpComponent::initAttachments()
{
    // attachments.bass = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
    //     apvtsRef, ParamIDs::ampBass, bassSlider);

    // attachments.mid = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
    //     apvtsRef, ParamIDs::ampMid, midSlider);
    
    // attachments.treble= std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
    //     apvtsRef, ParamIDs::ampTreble, trebleSlider);
    
    // attachments.level = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
    //     apvtsRef, ParamIDs::ampLevel, levelSlider);

    // attachments.gain = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
    //     apvtsRef, ParamIDs::ampGain, gainSlider);

    // attachments.presence = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
    //     apvtsRef, ParamIDs::ampPresence, presenceSlider);

    // attachments.bypass = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(
    //     apvtsRef, ParamIDs::ampBypass, powerButton);
}

//------------------------------------------------------
void AmpComponent::parameterChanged(const juce::String&, float)
{
    juce::MessageManager::callAsync([this]
    {
        repaint();
    });
}
//------------------------------------------------------
void AmpComponent::paint(juce::Graphics& g)
{
    float UI_SCALE = 1.1f;
    auto area = getLocalBounds().toFloat();

    float overlap = 72.0f * UI_SCALE;

    float cabTop = 0.0f;
    float cabHeight = 0.0f;

    // ===== CAB =====
    if (cabImage.isValid())
    {
        float imgW = (float)cabImage.getWidth();
        float imgH = (float)cabImage.getHeight();

        float scale = area.getWidth() * UI_SCALE / imgW;

        float drawW = imgW * scale;
        float drawH = imgH * scale;

        float x = area.getCentreX() - drawW * 0.5f;
        float y = area.getBottom() - drawH;

        cabTop = y;
        cabHeight = drawH;

        g.drawImage(cabImage,
                    juce::Rectangle<float>(x, y, drawW, drawH));
    }

    // ===== HEAD (накладывается на CAB) =====
    if (ampImage.isValid())
    {
        float imgW = (float)ampImage.getWidth();
        float imgH = (float)ampImage.getHeight();

        float scale = area.getWidth() * UI_SCALE / imgW;

        float drawW = imgW * scale;
        float drawH = imgH * scale;

        float x = area.getCentreX() - drawW * 0.5f;

        float y = cabTop - drawH + overlap;

        g.drawImage(ampImage,
                    juce::Rectangle<float>(x-8, y, drawW, drawH));
    }
}
//------------------------------------------------------
void AmpComponent::resized()
{
    auto area = getLocalBounds();
    float UI_SCALE = 1.1f;
    juce::Grid grid;
    using Track = juce::Grid::TrackInfo;
    using Fr = juce::Grid::Fr;
    grid.templateColumns = {
        Track(Fr(220)),
        Track(Fr(407)),
        Track(Fr(407)),
        Track(Fr(407)),
        Track(Fr(407)),
        Track(Fr(407)),
        Track(Fr(407)),
        Track(Fr(407)),
        Track(Fr(220))
    };

    grid.templateRows = {
        Track(Fr(113)),
        Track(Fr(9)),
        Track(Fr(3)),
        Track(Fr(83))
    };

    grid.items = {
        juce::GridItem(gainSlider).withArea(2, 2),
        juce::GridItem(bassSlider).withArea(2, 3),
        juce::GridItem(midSlider).withArea(2, 4),
        juce::GridItem(trebleSlider).withArea(2, 5),
        juce::GridItem(presenceSlider).withArea(2, 6),
        juce::GridItem(levelSlider).withArea(2, 7),
        juce::GridItem(powerButton).withArea(2, 8),

        juce::GridItem(gainLabel).withArea(3, 2),
        juce::GridItem(bassLabel).withArea(3, 3),
        juce::GridItem(midLabel).withArea(3, 4),
        juce::GridItem(trebleLabel).withArea(3, 5),
        juce::GridItem(presenceLabel).withArea(3, 6),
        juce::GridItem(levelLabel).withArea(3, 7)
    };

    grid.performLayout(area);
}
//------------------------------------------------------
AmpComponent::~AmpComponent()
{
    apvtsRef.removeParameterListener(ParamIDs::ampBypass, this);

    bassSlider.setLookAndFeel(nullptr);
    midSlider.setLookAndFeel(nullptr);
    trebleSlider.setLookAndFeel(nullptr);
    levelSlider.setLookAndFeel(nullptr);
    gainSlider.setLookAndFeel(nullptr);
    presenceSlider.setLookAndFeel(nullptr);
}