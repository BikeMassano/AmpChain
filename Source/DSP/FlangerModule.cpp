#include "FlangerModule.h"

namespace DSP
{
    void FlangerModule::prepare(const juce::dsp::ProcessSpec& spec)
    {
        jassert (spec.sampleRate > 0);
        jassert (spec.numChannels > 0);

        sampleRate_ = spec.sampleRate;

        const auto maxPossibleDelay = std::ceil ((maximumDelayModulation * maxDepth * oscVolumeMultiplier + maxCentreDelayMs)
                                             * sampleRate_ / 1000.0);

        delay_ = juce::dsp::DelayLine<float,
            juce::dsp::DelayLineInterpolationTypes::Linear>
        { 
            static_cast<int> (maxPossibleDelay) 
        };
        delay_.prepare(spec);

        osc_.prepare(spec);
        osc_.initialise([](float x) { return std::sin(x); });

        feedbackVolume_.resize(spec.numChannels);
        lastOutput_.resize(spec.numChannels);

        delayTimes_.setSize(1, (int)spec.maximumBlockSize, false, false, true);

        dryWet_.prepare(spec);
        dryWet_.setMixingRule(juce::dsp::DryWetMixingRule::linear);

        update();
        reset();
    }
    void FlangerModule::reset()
    {
        std::fill (lastOutput_.begin(), lastOutput_.end(), 0.0f);

        delay_.reset();
        osc_.reset();
        dryWet_.reset();

        oscVolume_.reset (sampleRate_, 0.05);

        for (auto& vol : feedbackVolume_)
            vol.reset (sampleRate_, 0.05);
    }

    void FlangerModule::update()
    {
        osc_.setFrequency (rate_);
        oscVolume_.setTargetValue (range_ * oscVolumeMultiplier);

        for (auto& vol : feedbackVolume_)
            vol.setTargetValue (feedback_);
    }

    void FlangerModule::process(const juce::dsp::ProcessContextReplacing<float>& context)
    {
        if (bypassed_)
            return;

        const auto& inputBlock = context.getInputBlock();
        auto& outputBlock      = context.getOutputBlock();
        const auto numChannels = outputBlock.getNumChannels();
        const auto numSamples  = outputBlock.getNumSamples();

        auto delayValuesBlock = juce::dsp::AudioBlock<float>(delayTimes_)
            .getSubBlock(0, numSamples);
        auto contextDelay = juce::dsp::ProcessContextReplacing<float>(delayValuesBlock);
        delayValuesBlock.clear();

        osc_.process(contextDelay);
        delayValuesBlock.multiplyBy(oscVolume_);

        auto* delaySamples = delayTimes_.getWritePointer(0);

        // перевод мс -> сэмплы и добавление центра
        for (size_t i = 0; i < numSamples; ++i)
        {
            auto lfoMs = delayValuesBlock.getSample(0, (int)i) + maxCentreDelayMs;
            delayValuesBlock.setSample(0, (int)i, (float)(lfoMs * sampleRate_ / 1000.0));
        }

        dryWet_.pushDrySamples(inputBlock);

        for (size_t channel = 0; channel < numChannels; ++channel)
        {
            auto* inputSamples  = inputBlock .getChannelPointer(channel);
            auto* outputSamples = outputBlock.getChannelPointer(channel);

            for (size_t i = 0; i < numSamples; ++i)
            {
                auto input = inputSamples[i];
                auto output = input + lastOutput_[channel];

                delay_.pushSample((int)channel, output);
                delay_.setDelay(delaySamples[i]);
                output = delay_.popSample ((int) channel);

                outputSamples[i] = output;
                lastOutput_[channel] = output * feedbackVolume_[channel].getNextValue();
            }
        }

        dryWet_.mixWetSamples(outputBlock);
    }

    void FlangerModule::setRate(const float newRateHz)     
    { 
        rate_ = newRateHz; 
        update(); 
    }
    void FlangerModule::setRange(const float newRangeMs)    
    { 
        range_ = newRangeMs;
        update();
    }
    //feedback
    void FlangerModule::setFeedback(const float newFeedback)
    {
        feedback_ = newFeedback;
        update();
    }
    void FlangerModule::setBypassed(const bool b)   { bypassed_ = b; }
}
