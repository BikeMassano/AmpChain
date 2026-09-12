#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
AudioPluginAudioProcessorEditor::AudioPluginAudioProcessorEditor(AudioPluginAudioProcessor& p)
    : AudioProcessorEditor(&p), processorRef(p), pedalPage(p.apvts), ampPage(p.apvts), cabPage(p.apvts)
{
    juce::ignoreUnused(processorRef);
    setSize(1200, 800);
    setOpaque(true);
    setResizeLimits(600, 400, 2400, 1600);
    // пропорции окна
    getConstrainer()->setFixedAspectRatio(1.4f);
    setResizable(true, true);

    // ================= PANELS =================
    addAndMakeVisible(topBar);
    addAndMakeVisible(bottomBar);

    // ================= PRESETS =================
    addAndMakeVisible(presetBox);

    addAndMakeVisible(savePresetButton);
    addAndMakeVisible(deletePresetButton);
    addAndMakeVisible(prevPresetButton);
    addAndMakeVisible(nextPresetButton);

    savePresetButton.setButtonText("Save");
    deletePresetButton.setButtonText("Del");
    prevPresetButton.setButtonText("<");
    nextPresetButton.setButtonText(">");


    // заполнение списка
    auto presets = processorRef.getPresetManager().getAllPresets();
    presetBox.clear();

    for (int i = 0; i < presets.size(); ++i)
        presetBox.addItem(presets[i], i + 1);

    auto current = processorRef.getPresetManager().getCurrentPreset();
    if (!current.isEmpty())
    {
        int idx = presets.indexOf(current);
        if (idx >= 0)
            presetBox.setSelectedItemIndex(idx, juce::dontSendNotification);
    }

    savePresetButton.onClick = [this]()
    {
        auto name = presetBox.getText().trim();
        if (name.isEmpty()) return;

        processorRef.getPresetManager().savePreset(name);

        presetBox.clear();
        auto presets = processorRef.getPresetManager().getAllPresets();

        for (int i = 0; i < presets.size(); ++i)
            presetBox.addItem(presets[i], i + 1);

        int idx = presets.indexOf(name);
        if (idx >= 0)
            presetBox.setSelectedItemIndex(idx, juce::dontSendNotification);
    };

    deletePresetButton.onClick = [this]()
    {
        auto name = presetBox.getText();

        processorRef.getPresetManager().deletePreset(name);

        presetBox.clear();
        auto presets = processorRef.getPresetManager().getAllPresets();

        for (int i = 0; i < presets.size(); ++i)
            presetBox.addItem(presets[i], i + 1);
    };

    presetBox.onChange = [this]()
    {
        auto name = presetBox.getText();
        processorRef.getPresetManager().loadPreset(name);
    };

    prevPresetButton.onClick = [this]()
    {
        int index = processorRef.getPresetManager().loadPreviousPreset();

        auto presets = processorRef.getPresetManager().getAllPresets();
        if (index >= 0)
            presetBox.setSelectedItemIndex(index, juce::dontSendNotification);
    };

    nextPresetButton.onClick = [this]()
    {
        int index = processorRef.getPresetManager().loadNextPreset();

        auto presets = processorRef.getPresetManager().getAllPresets();
        if (index >= 0)
            presetBox.setSelectedItemIndex(index, juce::dontSendNotification);
    };

    presetBox.setEditableText(true);

    // ================= RMS METERS =================
    addAndMakeVisible(leftInMeter);
    addAndMakeVisible(rightInMeter);

    addAndMakeVisible(leftOutMeter);
    addAndMakeVisible(rightOutMeter);

    // ================= PAGES BUTTONS =================
    addAndMakeVisible(ampButton);
    addAndMakeVisible(pedalButton);
    addAndMakeVisible(cabButton);

    addAndMakeVisible(monoButton);

    ampButton.setButtonText("Amp");
    pedalButton.setButtonText("Pedals");
    cabButton.setButtonText("Cab");
    monoButton.setButtonText("Mono");

    // начальное состояние
    currentPage = Page::Amp;
    ampButton.setToggleState(true, juce::dontSendNotification);

    ampButton.onClick = [this]()
    {
        currentPage = Page::Amp;

        ampButton.setToggleState(true, juce::dontSendNotification);
        pedalButton.setToggleState(false, juce::dontSendNotification);
        cabButton.setToggleState(false, juce::dontSendNotification);

        resized();
    };

    pedalButton.onClick = [this]()
    {
        currentPage = Page::Pedal;

        pedalButton.setToggleState(true, juce::dontSendNotification);
        ampButton.setToggleState(false, juce::dontSendNotification);
        cabButton.setToggleState(false, juce::dontSendNotification);

        resized();
    };

    cabButton.onClick = [this]()
    {
        currentPage = Page::Cab;

        pedalButton.setToggleState(false, juce::dontSendNotification);
        ampButton.setToggleState(false, juce::dontSendNotification);
        cabButton.setToggleState(true, juce::dontSendNotification);

        resized();
    };

    monoButton.setClickingTogglesState(true);
    monoButton.onClick = [this]
    {
        processorRef.setMonoEnabled(monoButton.getToggleState());
    };
    
    addAndMakeVisible(pedalPage);
    addAndMakeVisible(ampPage);
    addAndMakeVisible(cabPage);

    ampPage.onModelLoad = [&p](const juce::File& f) { p.loadNamModel(f); };
    cabPage.onIRLoad    = [&p](const juce::File& f) { p.loadCabIR(f); };

    startTimerHz(24);
}

//==============================================================================
AudioPluginAudioProcessorEditor::~AudioPluginAudioProcessorEditor()
{
}
//==============================================================================
void AudioPluginAudioProcessorEditor::timerCallback()
{
    leftInMeter     .setLevel(processorRef.getRmsInValue(0));
    rightInMeter    .setLevel(processorRef.getRmsInValue(1));
    leftOutMeter    .setLevel(processorRef.getRmsOutValue(0));
    rightOutMeter   .setLevel(processorRef.getRmsOutValue(1));

    leftInMeter     .setPeak(processorRef.getPeakInValue(0));
    rightInMeter    .setPeak(processorRef.getPeakInValue(1));
    leftOutMeter    .setPeak(processorRef.getPeakOutValue(0));
    rightOutMeter   .setPeak(processorRef.getPeakOutValue(1));

    repaint();
};

//==============================================================================
void AudioPluginAudioProcessorEditor::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colour(25, 28, 32));

    juce::ColourGradient grad(
        juce::Colour(35, 40, 45),
        0, 0,
        juce::Colour(18, 20, 22),
        0, (float)getHeight(),
        false
    );

    g.setGradientFill(grad);
    g.fillRect(getLocalBounds());

    // верхняя и нижняя полосы
    juce::Rectangle<float> topBarRect(
        0.f,
        0.f,
        (float)getWidth(),
        50.f
    );

    g.setColour(juce::Colours::black.withAlpha(0.35f));
    g.fillRoundedRectangle(topBarRect, 0.f);

    g.setColour(juce::Colours::white.withAlpha(0.08f));
    g.drawLine(0.f, 50.f, (float)getWidth(), 50.f, 1.f);

    juce::Rectangle<float> bottomBarRect(
        0.f,
        (float)getHeight() - 50.f,
        (float)getWidth(),
        50.f
    );

    g.setColour(juce::Colours::black.withAlpha(0.35f));
    g.fillRoundedRectangle(bottomBarRect, 0.f);

    g.setColour(juce::Colours::white.withAlpha(0.08f));
    g.drawLine(
        0.f, 
        (float)getHeight() - 50.f, 
        (float)getWidth(), 
        (float)getHeight() - 50.f, 
        1.f
    );
}

void AudioPluginAudioProcessorEditor::resized()
{
    auto area = getLocalBounds();

    auto topArea = area.removeFromTop(50);
    auto bottomArea = area.removeFromBottom(50);

    topBar.setBounds(topArea);

    int rightButtonsWidth = getWidth() * EditorLayout::rightBlockWidth;

    auto buttonArea = topArea.removeFromRight(rightButtonsWidth).reduced(10);

    {
        const int arrowW  = getWidth() * EditorLayout::presetArrowWidth;
        const int actionW = getWidth() * EditorLayout::presetActionWidth;
        const int boxW    = getWidth() * EditorLayout::presetBoxWidth;
        const int totalW  = arrowW + actionW + boxW + actionW + arrowW + 16;
        const int h       = topArea.getHeight() * 0.6;
        const int y       = (topArea.getHeight() - h) / 2;
        const int startX  = (getWidth() - totalW) / 2;

        int x = startX;

        prevPresetButton  .setBounds(x, y, arrowW,  h); x += arrowW  + EditorLayout::presetGap;
        deletePresetButton.setBounds(x, y, actionW, h); x += actionW + EditorLayout::presetGap;
        presetBox         .setBounds(x, y, boxW,    h); x += boxW    + EditorLayout::presetGap;
        savePresetButton  .setBounds(x, y, actionW, h); x += actionW + EditorLayout::presetGap;
        nextPresetButton  .setBounds(x, y, arrowW,  h);
    }

    pedalButton .setBounds(buttonArea.removeFromLeft(getWidth() * EditorLayout::pedalBtnWidth));
    ampButton   .setBounds(buttonArea.removeFromLeft(getWidth() * EditorLayout::pedalBtnWidth));
    cabButton   .setBounds(buttonArea.removeFromLeft(getWidth() * EditorLayout::pedalBtnWidth));

    bottomBar.setBounds(bottomArea);

    auto mainArea = area;

    if (currentPage == Page::Amp)
    {
        ampPage.setVisible(true);
        pedalPage.setVisible(false);
        cabPage.setVisible(false);
        ampPage.setBounds(mainArea);
    }
    else if (currentPage == Page::Cab)
    {
        ampPage.setVisible(false);
        pedalPage.setVisible(false);
        cabPage.setVisible(true);
        cabPage.setBounds(mainArea);
    }
    else if (currentPage == Page::Pedal)
    {
        ampPage.setVisible(false);
        cabPage.setVisible(false);
        pedalPage.setVisible(true);
        pedalPage.setBounds(mainArea);
    }

    auto bottomContent = bottomArea.reduced(10);

    const int meterHeight = 8;
    const int meterWidth  = 150;
    const int gap = 5;

    {
        const int startX = 10;

        auto metersArea = bottomContent.withTrimmedLeft(startX);
        metersArea = metersArea.removeFromLeft(meterWidth);

        auto lArea = metersArea.removeFromTop(meterHeight);
        metersArea.removeFromTop(gap);
        auto rArea = metersArea.removeFromTop(meterHeight);

        leftInMeter.setBounds(lArea);
        rightInMeter.setBounds(rArea);
    }

    {
        const int startX = 10 + meterWidth + 10;

        auto monoArea = bottomContent.withTrimmedLeft(startX);

        monoButton.setBounds(monoArea.removeFromLeft(80).reduced(2));
    }

    {
        auto rightArea = bottomContent.withTrimmedRight(10); 

        rightArea = rightArea.removeFromRight(meterWidth);

        auto lOutArea = rightArea.removeFromTop(meterHeight);
        rightArea.removeFromTop(gap);
        auto rOutArea = rightArea.removeFromTop(meterHeight);

        leftOutMeter.setBounds(lOutArea);
        rightOutMeter.setBounds(rOutArea);
    }
}
