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
            juce::dsp::Oversampling<float>::filterHalfBandPolyphaseIIR
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

        chain_.get<biasIndex>().setBias(0.2f);

        chain_.get<preGainIndex>().setRampDurationSeconds(0.05f);
        chain_.get<postGainIndex>().setRampDurationSeconds(0.05f);

        sampleRate_ = osSpec.sampleRate;
        *chain_.get<dcFilterIndex>().state = *FilterCoefs::makeHighPass(sampleRate_, 30.0f);
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

    void DistortionModule::setTone(float cutoff)
    {
        auto& filter = chain_.get<filterIndex>();
        *filter.state = *FilterCoefs::makeLowPass(sampleRate_, cutoff);
    }

    void DistortionModule::setLevel(float db)
    {
        chain_.get<postGainIndex>().setGainDecibels(db);
    }

    void DistortionModule::setDist(float db)
    {
        chain_.get<preGainIndex>().setGainDecibels(db);
    }

    void DistortionModule::setBypassed(bool b) { bypassed_ = b; }
}