#include "DistortionModule.h"

namespace DSP
{
    void DistortionModule::prepare(const juce::dsp::ProcessSpec& spec)
    {
        // Oversampling
        oversampling_ = std::make_unique<juce::dsp::Oversampling<float>>
        (
            spec.numChannels,
            2,
            juce::dsp::Oversampling<float>::filterHalfBandFIREquiripple
        );

        oversampling_->initProcessing(spec.maximumBlockSize);

        juce::dsp::ProcessSpec osSpec = spec;
        osSpec.sampleRate *= oversampling_->getOversamplingFactor();
        osSpec.maximumBlockSize *= oversampling_->getOversamplingFactor();

        auto& shaper = chain_.get<waveshaperIndex>();
        shaper.functionToUse = [] (float x)
        {
            return juce::dsp::FastMathApproximations::tanh(x);
        };

        chain_.prepare(osSpec);

        chain_.get<biasIndex>().setBias(0.1f);

        chain_.get<preGainIndex>().setRampDurationSeconds(0.05f);
        chain_.get<postGainIndex>().setRampDurationSeconds(0.05f);

        sampleRate_ = osSpec.sampleRate;
        *chain_.get<dcFilterIndex>().state = *FilterCoefs::makeHighPass(sampleRate_, 20.0f);
        *chain_.get<filterIndex>().state = *FilterCoefs::makeLowPass(sampleRate_, 10000.0f);
    }

    void DistortionModule::reset()
    {
        chain_.reset();
    }

    void DistortionModule::process(const juce::dsp::ProcessContextReplacing<float>& context)
    {
        if (bypassed_)
            return;
        
        juce::dsp::AudioBlock<float> audioBlock = context.getOutputBlock();

        juce::dsp::AudioBlock<float> upBlock = oversampling_->processSamplesUp(audioBlock);

        juce::dsp::ProcessContextReplacing<float> upContext(upBlock);
        chain_.process(upContext);

        oversampling_->processSamplesDown(audioBlock);
        
    }

    void DistortionModule::setTone(const float newCutoffHz)
    {
        auto& filter = chain_.get<filterIndex>();

        *filter.state = *FilterCoefs::makeLowPass(
            sampleRate_,
            juce::jlimit(1600.f, 12000.f, newCutoffHz)
        );
    }

    void DistortionModule::setLevel(const float newLevelDb)
    {
        chain_.get<postGainIndex>()
            .setGainDecibels(juce::jlimit(-24.f, 24.f, newLevelDb));
    }

    void DistortionModule::setDist(const float newDistDb)
    {
        chain_.get<preGainIndex>()
            .setGainDecibels(juce::jlimit(0.f, 70.f, newDistDb));
    }

    void DistortionModule::setBypassed(const bool b) { bypassed_ = b; }
}