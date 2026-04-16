#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
AudioPluginAudioProcessorEditor::AudioPluginAudioProcessorEditor(AudioPluginAudioProcessor& p)
    : AudioProcessorEditor(&p), processorRef(p), gateComponent(p.apvts), compressorComponent(p.apvts)
{
    juce::ignoreUnused(processorRef);
    setSize(1200, 800);
    setResizeLimits(600, 400, 2400, 1600);
    // пропорции окна
    getConstrainer()->setFixedAspectRatio(1.4f);
    setResizable(false, false);

    // ================= PANELS =================
    
    addAndMakeVisible(gateComponent);
    addAndMakeVisible(compressorComponent);
    addAndMakeVisible(distPanel);
}

//==============================================================================
AudioPluginAudioProcessorEditor::~AudioPluginAudioProcessorEditor()
{
}

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
}

void AudioPluginAudioProcessorEditor::resized()
{
    auto area = getLocalBounds();

    auto topBar = area.removeFromTop(60);
    auto bottomBar = area.removeFromBottom(80);

    // TOP

    // MAIN
    juce::FlexBox mainRow;
    mainRow.flexDirection = juce::FlexBox::Direction::row;

    mainRow.items.add(juce::FlexItem(gateComponent).withFlex(1).withMargin(10));
    mainRow.items.add(juce::FlexItem(compressorComponent).withFlex(1).withMargin(10));
    mainRow.items.add(juce::FlexItem(distPanel).withFlex(1).withMargin(10));

    mainRow.performLayout(area.reduced(10));

    // bottomLabel.setBounds(bottomBar.reduced(10));
}

