#include "AmpComponent.h"
#include "BinaryData.h"
#include "../../App/ParamIDs.h"

namespace GUI
{
    namespace
    {
        void placeCentered(juce::Component& component,
                            const juce::Rectangle<float>& body,
                            GUI::AmpLayout::ControlDesc desc)
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

    AmpComponent::AmpComponent(juce::AudioProcessorValueTreeState& apvts)
        : apvtsRef(apvts)
    {
        loadImages();
        initButtons();
        initSliders();
        initAttachments();

        modelNameLabel_.setJustificationType(juce::Justification::centredLeft);
        modelNameLabel_.setFont(juce::Font(13.0f));
        modelNameLabel_.setColour(juce::Label::textColourId, juce::Colours::grey);
        modelNameLabel_.setMinimumHorizontalScale(1.0f);
        addAndMakeVisible(modelNameLabel_);

        loadModelButton_.setButtonText("Load NAM model");
        loadModelButton_.setTooltip("Load NAM model");
        loadModelButton_.onClick = [this]
        {
            fileChooser_ = std::make_unique<juce::FileChooser>(
                "Load NAM Model", juce::File{}, "*.nam");

            fileChooser_->launchAsync(
                juce::FileBrowserComponent::openMode |
                juce::FileBrowserComponent::canSelectFiles,
                [this](const juce::FileChooser& fc)
                {
                    auto file = fc.getResult();
                    if (file.existsAsFile() && onModelLoad)
                        onModelLoad(file);
                });
        };
        addAndMakeVisible(loadModelButton_);

        apvtsRef.state.addListener(this);
        updateModelLabel_();

        apvtsRef.addParameterListener(ParamIDs::Amplifier::Bypass,      this);
        apvtsRef.addParameterListener(ParamIDs::Amplifier::Bass,        this);
        apvtsRef.addParameterListener(ParamIDs::Amplifier::Mid,         this);
        apvtsRef.addParameterListener(ParamIDs::Amplifier::Treble,      this);
        apvtsRef.addParameterListener(ParamIDs::Amplifier::Presence,    this);
        apvtsRef.addParameterListener(ParamIDs::Amplifier::Gain,        this);
        apvtsRef.addParameterListener(ParamIDs::Amplifier::Level,       this);
    }

    //------------------------------------------------------
    void AmpComponent::setupKnob(juce::Slider& s)
    {
        s.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
        s.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
        s.setNumDecimalPlacesToDisplay(0);
        s.setPopupDisplayEnabled(true, true, this);
    }

    void AmpComponent::loadImages()
    {
        images_.amp        = juce::ImageCache::getFromMemory(BinaryData::ampHead_png,  BinaryData::ampHead_pngSize);
        images_.cab        = juce::ImageCache::getFromMemory(BinaryData::ampCab_png,   BinaryData::ampCab_pngSize);
        images_.lampOff    = juce::ImageCache::getFromMemory(BinaryData::lampOff_png,  BinaryData::lampOff_pngSize);
        images_.lampOn     = juce::ImageCache::getFromMemory(BinaryData::lampOn_png,   BinaryData::lampOn_pngSize);
        images_.buttonOff  = juce::ImageCache::getFromMemory(BinaryData::ampOff_png,   BinaryData::ampOff_pngSize);
        images_.buttonOn   = juce::ImageCache::getFromMemory(BinaryData::ampOn_png,    BinaryData::ampOn_pngSize);
        images_.knob       = juce::ImageCache::getFromMemory(BinaryData::ampKnob_png,  BinaryData::ampKnob_pngSize);
    }

    void AmpComponent::initButtons()
    {
        powerButton_.setClickingTogglesState(true);

        powerButton_.setImages(
            true, true, true,
            images_.buttonOff, 1.0f, juce::Colours::transparentBlack,
            images_.buttonOff, 1.0f, juce::Colours::transparentBlack,
            images_.buttonOn,  1.0f, juce::Colours::transparentBlack
        );
        addAndMakeVisible(powerButton_);
    }

    void AmpComponent::initSliders()
    {
        setupKnob(knobs_.bass);
        setupKnob(knobs_.mid);
        setupKnob(knobs_.treble);
        setupKnob(knobs_.level);
        setupKnob(knobs_.gain);
        setupKnob(knobs_.presence);

        knobLnf_ = std::make_unique<KnobLookAndFeel>(images_.knob);

        knobs_.bass     .setLookAndFeel(knobLnf_.get());
        knobs_.mid      .setLookAndFeel(knobLnf_.get());
        knobs_.treble   .setLookAndFeel(knobLnf_.get());
        knobs_.level    .setLookAndFeel(knobLnf_.get());
        knobs_.gain     .setLookAndFeel(knobLnf_.get());
        knobs_.presence .setLookAndFeel(knobLnf_.get());

        addAndMakeVisible(knobs_.bass);
        addAndMakeVisible(knobs_.mid);
        addAndMakeVisible(knobs_.treble);
        addAndMakeVisible(knobs_.level);
        addAndMakeVisible(knobs_.gain);
        addAndMakeVisible(knobs_.presence);
    }

    void AmpComponent::initAttachments()
    {
        attachments_.bass = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
            apvtsRef, ParamIDs::Amplifier::Bass, knobs_.bass);

        attachments_.mid = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
            apvtsRef, ParamIDs::Amplifier::Mid, knobs_.mid);
        
        attachments_.treble= std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
            apvtsRef, ParamIDs::Amplifier::Treble, knobs_.treble);
        
        attachments_.level = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
            apvtsRef, ParamIDs::Amplifier::Level, knobs_.level);

        attachments_.gain = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
            apvtsRef, ParamIDs::Amplifier::Gain, knobs_.gain);

        attachments_.presence = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
            apvtsRef, ParamIDs::Amplifier::Presence, knobs_.presence);

        attachments_.bypass = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(
            apvtsRef, ParamIDs::Amplifier::Bypass, powerButton_);
    }

    void AmpComponent::parameterChanged(const juce::String&, float)
    {
        juce::MessageManager::callAsync([this]
        {
            repaint();
        });
    }

    void AmpComponent::paint(juce::Graphics& g)
    {
        auto cab = getCabRect();
        g.drawImageWithin(images_.cab,
            cab.getX(), cab.getY(), cab.getWidth(), cab.getHeight(),
            juce::RectanglePlacement::stretchToFit);

        auto amp = getAmpRect();
        g.drawImageWithin(images_.amp,
            amp.getX(), amp.getY(), amp.getWidth(), amp.getHeight(),
            juce::RectanglePlacement::stretchToFit);

        auto ampF = amp.toFloat();
        
        float lampSize = ampF.getWidth() * AmpLayout::lampSize;

        auto lampArea = juce::Rectangle<float>(
            ampF.getX() + ampF.getWidth()  * AmpLayout::lamp.x - lampSize * 0.5f,
            ampF.getY() + ampF.getHeight() * AmpLayout::lamp.y - lampSize * 0.5f,
            lampSize,
            lampSize
        );

        auto* bypass = apvtsRef.getRawParameterValue(ParamIDs::Amplifier::Bypass);

        auto isOn = bypass != nullptr && bypass->load() > 0.5f;

        g.drawImageWithin(
            isOn ? images_.lampOn : images_.lampOff,
            (int)lampArea.getX(), (int)lampArea.getY(),
            (int)lampArea.getWidth(), (int)lampArea.getHeight(),
            juce::RectanglePlacement::centred
        );
    }
    //------------------------------------------------------
    void AmpComponent::resized()
    {
        auto body = getAmpRect().toFloat();

        placeCentered(knobs_.gain,      body,   AmpLayout::gain);
        placeCentered(knobs_.bass,      body,   AmpLayout::bass);
        placeCentered(knobs_.mid,       body,   AmpLayout::mid);
        placeCentered(knobs_.treble,    body,   AmpLayout::treble);
        placeCentered(knobs_.presence,  body,   AmpLayout::presence);
        placeCentered(knobs_.level,     body,   AmpLayout::level);
        placeCentered(powerButton_,     body,   AmpLayout::button);

        const int btnH   = 32;
        const int btnW   = 140;
        const int labelW = (int)body.getWidth() - btnW - 8;
        const int stripY = 6;
        const int startX = 6;

        loadModelButton_.setBounds(startX,              stripY, btnW,   btnH);
        modelNameLabel_ .setBounds(startX + btnW + 8,   stripY, labelW, btnH);
    }

    juce::Rectangle<int> AmpComponent::getAmpRect() const
    {
        constexpr float aspect = AmpLayout::ampBodyAspect;
        auto bounds = getLocalBounds();
        int w = bounds.getWidth() * AmpLayout::ampSize;
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

    juce::Rectangle<int> AmpComponent::getCabRect() const
    {
        constexpr float cabAspect = AmpLayout::cabBodyAspect;
        auto bounds = getLocalBounds();
        int w = bounds.getWidth();
        int h = (int)(w / cabAspect);
        auto amp = getAmpRect();
        int overlap = (int)(w * AmpLayout::cabOverlap);
        return juce::Rectangle<int>(
            (bounds.getWidth() - w) / 2,
            amp.getBottom() - overlap,
            w, h
        );
    }

    void AmpComponent::updateModelLabel_()
    {
        auto path = apvtsRef.state.getProperty("namPath").toString();
        auto name = path.isNotEmpty()
            ? juce::File(path).getFileNameWithoutExtension()
            : "No model";
        auto colour = path.isNotEmpty() ? juce::Colours::white : juce::Colours::grey;

        juce::MessageManager::callAsync([this, name, colour]
        {
            modelNameLabel_.setText(name, juce::dontSendNotification);
            modelNameLabel_.setColour(juce::Label::textColourId, colour);
        });
    }

    //------------------------------------------------------
    AmpComponent::~AmpComponent()
    {
        apvtsRef.removeParameterListener(ParamIDs::Amplifier::Bypass,   this);
        apvtsRef.removeParameterListener(ParamIDs::Amplifier::Bass,     this);
        apvtsRef.removeParameterListener(ParamIDs::Amplifier::Mid,      this);
        apvtsRef.removeParameterListener(ParamIDs::Amplifier::Treble,   this);
        apvtsRef.removeParameterListener(ParamIDs::Amplifier::Presence, this);
        apvtsRef.removeParameterListener(ParamIDs::Amplifier::Gain,     this);
        apvtsRef.removeParameterListener(ParamIDs::Amplifier::Level,    this);

        apvtsRef.state.removeListener(this);

        knobs_.bass     .setLookAndFeel(nullptr);
        knobs_.mid      .setLookAndFeel(nullptr);
        knobs_.treble   .setLookAndFeel(nullptr);
        knobs_.level    .setLookAndFeel(nullptr);
        knobs_.gain     .setLookAndFeel(nullptr);
        knobs_.presence .setLookAndFeel(nullptr);
    }
}