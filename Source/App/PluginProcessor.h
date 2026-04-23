#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_dsp/juce_dsp.h>
#include "../DSP/GateModule.h"
#include "../DSP/CompressorModule.h"
#include "../DSP/DistortionModule.h"
#include "../DSP/AmpModule.h"
#include "../DSP/MonoModule.h"
#include "../DSP/CabModule.h"

//==============================================================================
class AudioPluginAudioProcessor final : public juce::AudioProcessor
{
public:
    //==============================================================================
    AudioPluginAudioProcessor();
    ~AudioPluginAudioProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;
    using AudioProcessor::processBlock;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    float getRmsInValue(const int channel) const;
    float getRmsOutValue(const int channel) const;

    // state
    juce::AudioProcessorValueTreeState apvts;
    static juce::AudioProcessorValueTreeState::ParameterLayout createParameters();

    void setMonoEnabled(bool enabled) { monoEnabled_ = enabled; }
    bool getMonoEnabled() const { return monoEnabled_; }

    void loadCabIR(const juce::File& file);

private:
    void initParameters_();

    bool modelLoaded_ = false;
    bool monoEnabled_ = false;

    std::atomic<float>* gateThreshold_ = nullptr;
    std::atomic<float>* gateAttack_ = nullptr;
    std::atomic<float>* gateRelease_ = nullptr;

    std::atomic<float>* compThreshold_ = nullptr;
    std::atomic<float>* compRatio_ = nullptr;
    std::atomic<float>* compAttack_ = nullptr;
    std::atomic<float>* compRelease_ = nullptr;
    std::atomic<float>* compMakeup_ = nullptr;

    std::atomic<float>* distTone_ = nullptr;
    std::atomic<float>* distLevel_ = nullptr;
    std::atomic<float>* distDist_ = nullptr;

    std::atomic<float>* ampBass_ = nullptr;
    std::atomic<float>* ampMid_ = nullptr;
    std::atomic<float>* ampTreble_ = nullptr;
    std::atomic<float>* ampGain_ = nullptr;
    std::atomic<float>* ampPresence_ = nullptr;
    std::atomic<float>* ampLevel_ = nullptr;

    std::atomic<float>* gateBypass_ = nullptr;
    std::atomic<float>* compBypass_ = nullptr;
    std::atomic<float>* distBypass_ = nullptr;
    std::atomic<float>* ampBypass_ = nullptr;

    std::atomic<float>* cabBypass_ = nullptr;

    MonoModule mono_;

    //==============================================================================
    enum {
        gateIndex,          // [0]
        compressorIndex,    // [1]
        distortionIndex,    // [2]
        ampIndex,           // [3]
        cabIndex            // [4]
    };
    juce::dsp::ProcessorChain<
        GateModule,
        CompressorModule,
        DistortionModule,
        AmpModule,
        CabModule
    > chain_;

    juce::LinearSmoothedValue<float> rmsInLevelLeft_, rmsInLevelRight_, rmsOutLevelLeft_, rmsOutLevelRight_;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AudioPluginAudioProcessor)
};
