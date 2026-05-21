#include "GateModule.h"

namespace DSP
{
    void GateModule::prepare(const juce::dsp::ProcessSpec& spec)
    {
        gate_.prepare(spec);
    }

    void GateModule::reset()
    {
        gate_.reset();
    }

    void GateModule::process(const juce::dsp::ProcessContextReplacing<float>& context)
    {
        if (bypassed_)
            return;

        gate_.process(context);
    }

    void GateModule::setThreshold(const float newThresholdDb)
    {
        gate_.setThreshold(juce::jlimit(-100.f, 0.f, newThresholdDb));
    }

    void GateModule::setAttack(const float newAttackMs)
    {
        gate_.setAttack(juce::jlimit(0.1f, 50.f, newAttackMs));
    }

    void GateModule::setRelease(const float newReleaseMs)
    {
        gate_.setRelease(juce::jlimit(10.f, 2000.f, newReleaseMs));
    }
    
    void GateModule::setBypassed(const bool b) { bypassed_ = b; }
}