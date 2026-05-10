#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
AudioPluginAudioProcessorEditor::AudioPluginAudioProcessorEditor(AudioPluginAudioProcessor& p)
    : AudioProcessorEditor(&p), processorRef(p), pedalPage(p.apvts), ampPage(p.apvts), cabPage(p.apvts, [&p](const juce::File& file){
        p.loadCabIR(file);
    })
{
    juce::ignoreUnused(processorRef);
    setSize(1200, 800);
    setResizeLimits(600, 400, 2400, 1600);
    // пропорции окна
    getConstrainer()->setFixedAspectRatio(1.4f);
    setResizable(false, false);

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

    savePresetButton.onClick = [this]()
    {
        auto name = presetBox.getText().trim();
        if (name.isEmpty()) return;

        processorRef.getPresetManager().savePreset(name);

        presetBox.addItem(name, presetBox.getNumItems() + 1);
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
            presetBox.setSelectedItemIndex(index);
    };

    nextPresetButton.onClick = [this]()
    {
        int index = processorRef.getPresetManager().loadNextPreset();

        auto presets = processorRef.getPresetManager().getAllPresets();
        if (index >= 0)
            presetBox.setSelectedItemIndex(index);
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

    // AMP
    ampButton.onClick = [this]()
    {
        currentPage = Page::Amp;

        ampButton.setToggleState(true, juce::dontSendNotification);
        pedalButton.setToggleState(false, juce::dontSendNotification);
        cabButton.setToggleState(false, juce::dontSendNotification);

        resized();
    };

    // PEDAL
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

    startTimerHz(24);
}

//==============================================================================
AudioPluginAudioProcessorEditor::~AudioPluginAudioProcessorEditor()
{
}
//==============================================================================
void AudioPluginAudioProcessorEditor::timerCallback()
{
    leftInMeter.setLevel(processorRef.getRmsInValue(0));
    rightInMeter.setLevel(processorRef.getRmsInValue(1));

    leftOutMeter.setLevel(processorRef.getRmsOutValue(0));
    rightOutMeter.setLevel(processorRef.getRmsOutValue(1));

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

    // ===== TOP / BOTTOM =====
    auto topArea = area.removeFromTop(50);
    auto bottomArea = area.removeFromBottom(50);

    topBar.setBounds(topArea);
    auto buttonArea = topArea.removeFromRight(300).reduced(10);

   {
        const int arrowW  = 30;
        const int actionW = 50;
        const int boxW    = 220;
        const int totalW  = arrowW + actionW + boxW + actionW + arrowW + 16; // 16 = отступы
        const int h       = 30;
        const int y       = (50 - h) / 2;
        const int startX  = (getWidth() - totalW) / 2;

        int x = startX;
        prevPresetButton  .setBounds(x, y, arrowW,  h); x += arrowW  + 4;
        deletePresetButton.setBounds(x, y, actionW, h); x += actionW + 4;
        presetBox         .setBounds(x, y, boxW,    h); x += boxW    + 4;
        savePresetButton  .setBounds(x, y, actionW, h); x += actionW + 4;
        nextPresetButton  .setBounds(x, y, arrowW,  h);
    }

    pedalButton.setBounds(buttonArea.removeFromLeft(90));
    ampButton.setBounds(buttonArea.removeFromLeft(90));
    cabButton.setBounds(buttonArea.removeFromLeft(90));

    bottomBar.setBounds(bottomArea);

    // ===== MAIN PAGE =====
    auto mainArea = area; // после top/bottom

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

    // ===== HORIZONTAL METERS (BOTTOM LEFT) =====
    auto bottomContent = bottomArea.reduced(10);

    const int meterHeight = 8;
    const int meterWidth  = 150;
    const int gap = 5;

    // ================= LEFT SIDE (IN METERS) =================
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

    // ================= MONO BUTTON (RIGHT OF INPUT METERS) =================
    {
        const int startX = 10 + meterWidth + 10; // отступ + ширина метров + небольшой gap

        auto monoArea = bottomContent.withTrimmedLeft(startX);

        monoButton.setBounds(monoArea.removeFromLeft(80).reduced(2));
    }

    // ================= RIGHT SIDE (OUT METERS) =================
    {
        auto rightArea = bottomContent.withTrimmedRight(10); 
        // можно подвинуть чуть от края

        rightArea = rightArea.removeFromRight(meterWidth);

        auto lOutArea = rightArea.removeFromTop(meterHeight);
        rightArea.removeFromTop(gap);
        auto rOutArea = rightArea.removeFromTop(meterHeight);

        leftOutMeter.setBounds(lOutArea);
        rightOutMeter.setBounds(rOutArea);
    }
}
