#include "CompressorModule.h"

namespace DSP
{
    void CompressorModule::prepare(const juce::dsp::ProcessSpec& spec)
    {
        chain_.prepare(spec);
        chain_.get<1>().setRampDurationSeconds(0.02); // gain smoothing
    }

    void CompressorModule::reset()
    {
        chain_.reset();
    }

    void CompressorModule::process(const juce::dsp::ProcessContextReplacing<float>& context)
    {
        if (bypassed_)
            return;
        
            chain_.process(context);
    }

    void CompressorModule::setThreshold(const float newThresholdDb)
    {
        chain_.get<compressorIndex>().setThreshold(juce::jlimit(-60.f, 0.f, newThresholdDb));
    }

    void CompressorModule::setRatio(const float newRatio)
    {
        chain_.get<compressorIndex>().setRatio(juce::jlimit(1.f, 60.f, newRatio));
    }
    
    void CompressorModule::setAttack(const float newAttackMs)
    {
        chain_.get<compressorIndex>().setAttack(juce::jlimit(0.1f, 100.f, newAttackMs));
    }
    
    void CompressorModule::setRelease(const float newReleaseMs)
    {
        chain_.get<compressorIndex>().setRelease(juce::jlimit(10.f, 1000.f, newReleaseMs));
    }
    
    void CompressorModule::setMakeup(const float newMakeupDb)
    {
        chain_.get<gainIndex>().setGainDecibels(juce::jlimit(0.f, 24.f, newMakeupDb));
    }

    void CompressorModule::setBypassed(const bool b) { bypassed_ = b; }
}