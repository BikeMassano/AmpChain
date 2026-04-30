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
    initParameters_();

    try
    {
        juce::MemoryInputStream jsonStream(
            BinaryData::model_json,
            BinaryData::model_jsonSize,
            false
        );

        auto jsonString = jsonStream.readEntireStreamAsString();

        DBG("Model JSON loaded, size: " << jsonString.length());

        auto jsonInput = nlohmann::json::parse(jsonString.toStdString());

        DBG("JSON parsed successfully");

        neuralNetT[0].parseJson(jsonInput);
        neuralNetT[1].parseJson(jsonInput);

        DBG("Neural nets initialized");
    }
    catch (const nlohmann::json::exception& e)
    {
        DBG("JSON error: " << e.what());
        jassertfalse;
    }
    catch (const std::exception& e)
    {
        DBG("STD error: " << e.what());
        jassertfalse;
    }
    catch (...)
    {
        DBG("Unknown error while loading model JSON");
        jassertfalse;
    }
}

AudioPluginAudioProcessor::~AudioPluginAudioProcessor()
{
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
        ParamIDs::gateThreshold, "Gate Threshold",
        juce::NormalisableRange<float>(-100.0f, 0.0f),
        -50.0f));

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        ParamIDs::gateAttack, "Gate Attack",
        juce::NormalisableRange<float>(0.1f, 200.0f),
        100.0f));

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        ParamIDs::gateRelease, "Gate Release",
        juce::NormalisableRange<float>(10.0f, 1000.0f),
        500.0f));

    params.push_back(std::make_unique<juce::AudioParameterBool>(
        ParamIDs::gateBypass, "Gate Bypass", false));
    
    // Compressor Parameters
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        ParamIDs::compThreshold, "Comp Threshold",
        juce::NormalisableRange<float>(-60.0f, 0.0f),
        -30.0f));

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        ParamIDs::compRatio, "Comp Ratio",
        juce::NormalisableRange<float>(1.0f, 20.0f),
        10.0f));

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        ParamIDs::compAttack, "Comp Attack",
        juce::NormalisableRange<float>(0.1f, 200.0f),
        100.0f));

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        ParamIDs::compRelease, "Comp Release",
        juce::NormalisableRange<float>(10.0f, 1000.0f),
        500.0f));

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        ParamIDs::compMakeup, "Comp Makeup",
        juce::NormalisableRange<float>(0.0f, 12.0f),
        0.0f));

    params.push_back(std::make_unique<juce::AudioParameterBool>(
        ParamIDs::compBypass, "Comp Bypass", false));

    // Distortion Parameters
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        ParamIDs::distTone, "Tone",
        juce::NormalisableRange<float>(1600.0f, 16000.0f, 1.0f, 0.3f, true),
        3000.0f));

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        ParamIDs::distLevel, "Level",
        juce::NormalisableRange<float>(-12.0f, 12.0f, 0.1f),
        0.0f));

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        ParamIDs::distDist, "Dist",
        juce::NormalisableRange<float>(0.0f, 30.0f, 0.01f, 0.5f, true),
        10.0f));
    
    params.push_back(std::make_unique<juce::AudioParameterBool>(
        ParamIDs::distBypass, "Dist Bypass", false));

    // // Amp Parameters
    // params.push_back(std::make_unique<juce::AudioParameterFloat>(
    //     ParamIDs::ampBass, "Amp Bass",
    //     juce::NormalisableRange<float>(0.0f, 10.0f),
    //     5.0f));

    // params.push_back(std::make_unique<juce::AudioParameterFloat>(
    //     ParamIDs::ampMid, "Amp Mid",
    //     juce::NormalisableRange<float>(0.0f, 10.0f),
    //     5.0f));

    // params.push_back(std::make_unique<juce::AudioParameterFloat>(
    //     ParamIDs::ampTreble, "Amp Treble",
    //     juce::NormalisableRange<float>(0.0f, 10.0f),
    //     5.0f));

    // params.push_back(std::make_unique<juce::AudioParameterFloat>(
    //     ParamIDs::ampGain, "Amp Gain",
    //     juce::NormalisableRange<float>(0.0f, 10.0f),
    //     5.0f));

    // params.push_back(std::make_unique<juce::AudioParameterFloat>(
    //     ParamIDs::ampPresence, "Amp Presence",
    //     juce::NormalisableRange<float>(0.0f, 10.0f),
    //     5.0f));

    // params.push_back(std::make_unique<juce::AudioParameterFloat>(
    //     ParamIDs::ampLevel, "Amp Level",
    //     juce::NormalisableRange<float>(0.0f, 10.0f),
    //     5.0f));

    // params.push_back(std::make_unique<juce::AudioParameterBool>(
    //     ParamIDs::ampBypass, "Amp Bypass", false));

    // Cab Parameters

    params.push_back(std::make_unique<juce::AudioParameterBool>(
        ParamIDs::cabBypass, "Cab Bypass", false));

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

    neuralNetT[0].reset();
    neuralNetT[1].reset();
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
    mono_.setMode(MonoModule::Left);

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

    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    // ================= ОБРАБОТКА ЗВУКА =================
    auto& gate = chain_.get<gateIndex>();
    auto& comp = chain_.get<compressorIndex>();
    auto& dist = chain_.get<distortionIndex>();
    auto& cab = chain_.get<cabIndex>();

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

    // Distortion
    dist.setTone(distTone_->load());
    dist.setLevel(distLevel_->load());
    dist.setDist(distDist_->load());
    dist.setBypassed(distBypass_->load() < 0.5f);

    // Amp
    for (int ch = 0; ch < buffer.getNumChannels(); ++ch)
    {
        auto* x = buffer.getWritePointer (ch);
        for (int n = 0; n < buffer.getNumSamples(); ++n)
        {
            // float input[] = { x[n] };
            // x[n] = neuralNetT[ch].forward (input);

            float gainValue = 5.0f;
            float input[] = { x[n], gainValue };
            x[n] = neuralNetT[ch].forward(input);
        }
    }

    // Cab
    cab.setBypassed(cabBypass_->load() < 0.5f);

    juce::dsp::AudioBlock<float> block(buffer);
    juce::dsp::ProcessContextReplacing<float> context(block);

    chain_.process(context);

    // ================= OUTPUT RMS =================
    rmsOutLevelLeft_.skip(buffer.getNumSamples());
    rmsOutLevelRight_.skip(buffer.getNumSamples());

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
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
    juce::ignoreUnused (destData);
}

void AudioPluginAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
    juce::ignoreUnused (data, sizeInBytes);
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

void AudioPluginAudioProcessor::loadCabIR(const juce::File& file)
{
    chain_.get<cabIndex>().loadIR(file);
}

void AudioPluginAudioProcessor::initParameters_()
{
    gateThreshold_ = apvts.getRawParameterValue(ParamIDs::gateThreshold);
    gateAttack_    = apvts.getRawParameterValue(ParamIDs::gateAttack);
    gateRelease_   = apvts.getRawParameterValue(ParamIDs::gateRelease);
    gateBypass_ = apvts.getRawParameterValue(ParamIDs::gateBypass);

    compThreshold_ = apvts.getRawParameterValue(ParamIDs::compThreshold);
    compRatio_     = apvts.getRawParameterValue(ParamIDs::compRatio);
    compAttack_    = apvts.getRawParameterValue(ParamIDs::compAttack);
    compRelease_   = apvts.getRawParameterValue(ParamIDs::compRelease);
    compMakeup_    = apvts.getRawParameterValue(ParamIDs::compMakeup);
    compBypass_ = apvts.getRawParameterValue(ParamIDs::compBypass);

    distTone_  = apvts.getRawParameterValue(ParamIDs::distTone);
    distLevel_ = apvts.getRawParameterValue(ParamIDs::distLevel);
    distDist_  = apvts.getRawParameterValue(ParamIDs::distDist);
    distBypass_ = apvts.getRawParameterValue(ParamIDs::distBypass);

    cabBypass_  = apvts.getRawParameterValue(ParamIDs::cabBypass);
}