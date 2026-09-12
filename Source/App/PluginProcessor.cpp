#include "PluginProcessor.h"
#include <BinaryData.h>
#include "PluginEditor.h"
#include "ParamIDs.h"
#include <juce_core/juce_core.h>

//==============================================================================
AudioPluginAudioProcessor::AudioPluginAudioProcessor()
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       ),
                    apvts(*this, nullptr, "PARAMS", createParameters())
{
    apvts.state.setProperty(PresetManager::presetNameKey, "", nullptr);
    presetManager_ = std::make_unique<PresetManager>(apvts);

    presetManager_->onPresetLoaded = [this](const juce::ValueTree& tree)
    {
        auto namPath = tree.getProperty("namPath").toString();
        if (namPath.isNotEmpty())
            chain_.get<ampIndex>().loadModel(juce::File(namPath));

        auto irPath = tree.getProperty("irPath").toString();
        if (irPath.isNotEmpty())
            chain_.get<cabIndex>().loadIR(juce::File(irPath));
    };

    apvts.state.addListener(this);

    initParameters_();
}
AudioPluginAudioProcessor::~AudioPluginAudioProcessor()
{
    apvts.state.removeListener(this);
}

//==============================================================================
const juce::String AudioPluginAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool AudioPluginAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool AudioPluginAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool AudioPluginAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double AudioPluginAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int AudioPluginAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int AudioPluginAudioProcessor::getCurrentProgram()
{
    return 0;
}

void AudioPluginAudioProcessor::setCurrentProgram (int index)
{
    juce::ignoreUnused (index);
}

const juce::String AudioPluginAudioProcessor::getProgramName (int index)
{
    juce::ignoreUnused (index);
    return {};
}

void AudioPluginAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
    juce::ignoreUnused (index, newName);
}

juce::AudioProcessorValueTreeState::ParameterLayout
AudioPluginAudioProcessor::createParameters()
{
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;

    // NoiseGate Parameters
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        ParamIDs::Gate::Threshold, "Threshold",
        juce::NormalisableRange<float>(-100.0f, 0.0f),
        -50.0f));

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        ParamIDs::Gate::Attack, "Attack",
        juce::NormalisableRange<float>(0.1f, 200.0f),
        100.0f));

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        ParamIDs::Gate::Release, "Release",
        juce::NormalisableRange<float>(10.0f, 1000.0f),
        500.0f));

    params.push_back(std::make_unique<juce::AudioParameterBool>(
        ParamIDs::Gate::Bypass, "Bypass", false));
    
    // Compressor Parameters
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        ParamIDs::Compressor::Threshold, "Threshold",
        juce::NormalisableRange<float>(-60.0f, 0.0f),
        -30.0f));

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        ParamIDs::Compressor::Ratio, "Ratio",
        juce::NormalisableRange<float>(1.0f, 20.0f),
        10.0f));

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        ParamIDs::Compressor::Attack, "Attack",
        juce::NormalisableRange<float>(0.1f, 200.0f),
        100.0f));

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        ParamIDs::Compressor::Release, "Release",
        juce::NormalisableRange<float>(10.0f, 1000.0f),
        500.0f));

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        ParamIDs::Compressor::Makeup, "Makeup",
        juce::NormalisableRange<float>(0.0f, 12.0f),
        0.0f));

    params.push_back(std::make_unique<juce::AudioParameterBool>(
        ParamIDs::Compressor::Bypass, "Bypass", false));

    // Flanger Parameters
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        ParamIDs::Flanger::Rate, "Rate",
        juce::NormalisableRange<float>(0.1f, 8.f),
        0.5f));

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        ParamIDs::Flanger::Range, "Range",
        juce::NormalisableRange<float>(1.f, 15.f),
        7.f));

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        ParamIDs::Flanger::Feedback, "Feedback",
        juce::NormalisableRange<float>(0.f, 1.f),
        0.5f));

    params.push_back(std::make_unique<juce::AudioParameterBool>(
        ParamIDs::Flanger::Bypass, "Bypass", false));

    // Distortion Parameters
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        ParamIDs::Distortion::Tone, "Tone",
        juce::NormalisableRange<float>(1600.0f, 16000.0f, 1.0f, 0.3f, true),
        3000.0f));

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        ParamIDs::Distortion::Level, "Level",
        juce::NormalisableRange<float>(-12.0f, 12.0f, 0.1f),
        0.0f));

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        ParamIDs::Distortion::Dist, "Dist",
        juce::NormalisableRange<float>(0.0f, 70.0f, 0.01f, 0.5f, true),
        10.0f));
    
    params.push_back(std::make_unique<juce::AudioParameterBool>(
        ParamIDs::Distortion::Bypass, "Bypass", false));

    // Amp Parameters
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        ParamIDs::Amplifier::Bass, "Bass",
        juce::NormalisableRange<float>(0.0f, 10.0f),
        5.0f));

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        ParamIDs::Amplifier::Mid, "Mid",
        juce::NormalisableRange<float>(0.0f, 10.0f),
        5.0f));

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        ParamIDs::Amplifier::Treble, "Treble",
        juce::NormalisableRange<float>(0.0f, 10.0f),
        5.0f));

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        ParamIDs::Amplifier::Gain, "Gain",
        juce::NormalisableRange<float>(0.0f, 10.0f),
        5.0f));

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        ParamIDs::Amplifier::Presence, "Presence",
        juce::NormalisableRange<float>(0.0f, 10.0f),
        5.0f));

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        ParamIDs::Amplifier::Level, "Level",
        juce::NormalisableRange<float>(0.0f, 10.0f),
        5.0f));

    params.push_back(std::make_unique<juce::AudioParameterBool>(
        ParamIDs::Amplifier::Bypass, "Bypass", false));

    // Cab Parameters

    params.push_back(std::make_unique<juce::AudioParameterBool>(
        ParamIDs::Cabinet::Bypass, "Bypass", false));

    return { params.begin(), params.end() };
}

//==============================================================================
void AudioPluginAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
    juce::ignoreUnused (sampleRate, samplesPerBlock);

    rmsInLevelLeft_.reset(sampleRate, 0.5f);
    rmsInLevelRight_.reset(sampleRate, 0.5f);
    rmsOutLevelLeft_.reset(sampleRate, 0.5f);
    rmsOutLevelRight_.reset(sampleRate, 0.5f);

    rmsInLevelLeft_.setCurrentAndTargetValue(-100.f);
    rmsInLevelRight_.setCurrentAndTargetValue(-100.f);
    rmsOutLevelLeft_.setCurrentAndTargetValue(-100.f);
    rmsOutLevelRight_.setCurrentAndTargetValue(-100.f);

    juce::dsp::ProcessSpec spec;
    spec.sampleRate = sampleRate;
    spec.maximumBlockSize = (juce::uint32)samplesPerBlock;
    spec.numChannels = (juce::uint32)getTotalNumOutputChannels();

    chain_.prepare(spec);

    // chain_.get<ampIndex>().loadModel(
    //     juce::File("C:\\Users\\yurij\\Desktop\\Projects\\AudioPlugins\\NoiseGate\\models\\PEAVEY_6505_Lead.nam"));

    setLatencySamples(512);
}

void AudioPluginAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

bool AudioPluginAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}

void AudioPluginAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer,
                                              juce::MidiBuffer& midiMessages)
{
    juce::ignoreUnused (midiMessages);
    juce::ScopedNoDenormals noDenormals;

    mono_.setEnabled(monoEnabled_);
    mono_.setMode(DSP::MonoModule::Left);

    juce::dsp::AudioBlock<float> monoBlock(buffer);
    juce::dsp::ProcessContextReplacing<float> monoContext(monoBlock);

    mono_.process(monoContext);

    // ================= INPUT RMS =================
    rmsInLevelLeft_.skip(buffer.getNumSamples());
    rmsInLevelRight_.skip(buffer.getNumSamples());
    {
        const auto value = juce::Decibels::gainToDecibels(buffer.getRMSLevel(0, 0, buffer.getNumSamples()));
        if (value < rmsInLevelLeft_.getCurrentValue())
            rmsInLevelLeft_.setTargetValue(value);
        else
            rmsInLevelLeft_.setCurrentAndTargetValue(value);
    }

    {
        const auto value = juce::Decibels::gainToDecibels(buffer.getRMSLevel(1, 0, buffer.getNumSamples()));
        if (value < rmsInLevelRight_.getCurrentValue())
            rmsInLevelRight_.setTargetValue(value);
        else
            rmsInLevelRight_.setCurrentAndTargetValue(value);
    }

    // INPUT PEAK
    peakInLeft_  = juce::Decibels::gainToDecibels(buffer.getMagnitude(0, 0, buffer.getNumSamples()));
    peakInRight_ = juce::Decibels::gainToDecibels(buffer.getMagnitude(1, 0, buffer.getNumSamples()));

    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    // ================= ОБРАБОТКА ЗВУКА =================
    auto& gate =    chain_.get<gateIndex>();
    auto& comp =    chain_.get<compressorIndex>();
    auto& flanger = chain_.get<flangerIndex>();
    auto& dist =    chain_.get<distortionIndex>();
    auto& amp =     chain_.get<ampIndex>();
    auto& cab =     chain_.get<cabIndex>();

    // Gate
    gate.setThreshold(gateThreshold_->load());
    gate.setAttack(gateAttack_->load());
    gate.setRelease(gateRelease_->load());
    gate.setBypassed(gateBypass_->load() < 0.5f);

    // Compressor
    comp.setThreshold(compThreshold_->load());
    comp.setRatio(compRatio_->load());
    comp.setAttack(compAttack_->load());
    comp.setRelease(compRelease_->load());
    comp.setMakeup(compMakeup_->load());
    comp.setBypassed(compBypass_->load() < 0.5f);

    // Flanger
    flanger.setRate(flangerRate_->load());
    flanger.setRange(flangerRange_->load());
    flanger.setFeedback(flangerFeedback_->load());
    flanger.setBypassed(flangerBypass_->load() < 0.5f);

    // Distortion
    dist.setTone(distTone_->load());
    dist.setLevel(distLevel_->load());
    dist.setDist(distDist_->load());
    dist.setBypassed(distBypass_->load() < 0.5f);

    // Amp
    amp.setGain(ampGain_->load());
    amp.setBass(ampBass_->load());
    amp.setMid(ampMid_->load());
    amp.setTreble(ampTreble_->load());
    amp.setPresence(ampPresence_->load());
    amp.setLevel(ampLevel_->load());
    amp.setBypassed(ampBypass_->load() < 0.5f);

    // Cab
    cab.setBypassed(cabBypass_->load() < 0.5f);

    juce::dsp::AudioBlock<float> block(buffer);
    juce::dsp::ProcessContextReplacing<float> context(block);

    chain_.process(context);

    // ================= OUTPUT RMS =================
    rmsOutLevelLeft_.skip(buffer.getNumSamples());
    rmsOutLevelRight_.skip(buffer.getNumSamples());

    // OUTPUT PEAK
    peakOutLeft_  = juce::Decibels::gainToDecibels(buffer.getMagnitude(0, 0, buffer.getNumSamples()));
    peakOutRight_ = juce::Decibels::gainToDecibels(buffer.getMagnitude(1, 0, buffer.getNumSamples()));

    {
        const auto value = juce::Decibels::gainToDecibels(
            buffer.getRMSLevel(0, 0, buffer.getNumSamples())
        );

        if (value < rmsOutLevelLeft_.getCurrentValue())
            rmsOutLevelLeft_.setTargetValue(value);
        else
            rmsOutLevelLeft_.setCurrentAndTargetValue(value);
    }

    {
        const auto value = juce::Decibels::gainToDecibels(
            buffer.getRMSLevel(1, 0, buffer.getNumSamples())
        );

        if (value < rmsOutLevelRight_.getCurrentValue())
            rmsOutLevelRight_.setTargetValue(value);
        else
            rmsOutLevelRight_.setCurrentAndTargetValue(value);
    }
}

//==============================================================================
bool AudioPluginAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* AudioPluginAudioProcessor::createEditor()
{
    return new AudioPluginAudioProcessorEditor (*this);
}

//==============================================================================
void AudioPluginAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    const auto state = apvts.copyState();
    const auto xml = state.createXml();
    copyXmlToBinary(*xml, destData);
}

void AudioPluginAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    const auto xmlState = getXmlFromBinary(data, sizeInBytes);
    if (xmlState == nullptr)
        return;
    const auto newTree = juce::ValueTree::fromXml(*xmlState);
    if (newTree.isValid())
        apvts.replaceState(newTree);
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new AudioPluginAudioProcessor();
}

float AudioPluginAudioProcessor::getRmsInValue(const int channel) const
{
    jassert(channel == 0 || channel == 1);
        if (channel == 0)
            return rmsInLevelLeft_.getCurrentValue();
        if (channel == 1)
            return rmsInLevelRight_.getCurrentValue();
        return 0.f;    
}

float AudioPluginAudioProcessor::getRmsOutValue(const int channel) const
{
    jassert(channel == 0 || channel == 1);
        if (channel == 0)
            return rmsOutLevelLeft_.getCurrentValue();
        if (channel == 1)
            return rmsOutLevelRight_.getCurrentValue();
        return 0.f;    
}

float AudioPluginAudioProcessor::getPeakInValue(int channel) const
{
    jassert(channel == 0 || channel == 1); 
        if (channel == 0)
            return peakInLeft_.load();
        if (channel == 1)
            return peakInRight_.load();
        return 0.f;  
}

float AudioPluginAudioProcessor::getPeakOutValue(int channel) const
{ 
    jassert(channel == 0 || channel == 1); 
        if (channel == 0)
            return peakOutLeft_.load();
        if (channel == 1)
            return peakOutRight_.load();
        return 0.f; 
}

void AudioPluginAudioProcessor::initParameters_()
{
    gateThreshold_  = apvts.getRawParameterValue(ParamIDs::Gate::Threshold);
    gateAttack_     = apvts.getRawParameterValue(ParamIDs::Gate::Attack);
    gateRelease_    = apvts.getRawParameterValue(ParamIDs::Gate::Release);
    gateBypass_     = apvts.getRawParameterValue(ParamIDs::Gate::Bypass);

    compThreshold_  = apvts.getRawParameterValue(ParamIDs::Compressor::Threshold);
    compRatio_      = apvts.getRawParameterValue(ParamIDs::Compressor::Ratio);
    compAttack_     = apvts.getRawParameterValue(ParamIDs::Compressor::Attack);
    compRelease_    = apvts.getRawParameterValue(ParamIDs::Compressor::Release);
    compMakeup_     = apvts.getRawParameterValue(ParamIDs::Compressor::Makeup);
    compBypass_     = apvts.getRawParameterValue(ParamIDs::Compressor::Bypass);

    flangerRate_    = apvts.getRawParameterValue(ParamIDs::Flanger::Rate);
    flangerRange_   = apvts.getRawParameterValue(ParamIDs::Flanger::Range);
    flangerFeedback_= apvts.getRawParameterValue(ParamIDs::Flanger::Feedback);
    flangerBypass_  = apvts.getRawParameterValue(ParamIDs::Flanger::Bypass);

    distTone_       = apvts.getRawParameterValue(ParamIDs::Distortion::Tone);
    distLevel_      = apvts.getRawParameterValue(ParamIDs::Distortion::Level);
    distDist_       = apvts.getRawParameterValue(ParamIDs::Distortion::Dist);
    distBypass_     = apvts.getRawParameterValue(ParamIDs::Distortion::Bypass);

    ampBass_        = apvts.getRawParameterValue(ParamIDs::Amplifier::Bass);
    ampMid_         = apvts.getRawParameterValue(ParamIDs::Amplifier::Mid);
    ampTreble_      = apvts.getRawParameterValue(ParamIDs::Amplifier::Treble);
    ampPresence_    = apvts.getRawParameterValue(ParamIDs::Amplifier::Presence);
    ampGain_        = apvts.getRawParameterValue(ParamIDs::Amplifier::Gain);
    ampLevel_       = apvts.getRawParameterValue(ParamIDs::Amplifier::Level);
    ampBypass_      = apvts.getRawParameterValue(ParamIDs::Amplifier::Bypass);

    cabBypass_      = apvts.getRawParameterValue(ParamIDs::Cabinet::Bypass);
}

void AudioPluginAudioProcessor::loadNamModel(const juce::File& file)
{
    if (chain_.get<ampIndex>().loadModel(file))
        apvts.state.setProperty("namPath", file.getFullPathName(), nullptr);
}

void AudioPluginAudioProcessor::loadCabIR(const juce::File& file)
{
    chain_.get<cabIndex>().loadIR(file);
    apvts.state.setProperty("irPath", file.getFullPathName(), nullptr);
}

void AudioPluginAudioProcessor::valueTreeRedirected(juce::ValueTree& tree)
{
    auto namPath = tree.getProperty("namPath").toString();
    if (namPath.isNotEmpty())
        chain_.get<ampIndex>().loadModel(juce::File(namPath));

    auto irPath = tree.getProperty("irPath").toString();
    if (irPath.isNotEmpty())
        chain_.get<cabIndex>().loadIR(juce::File(irPath));
}