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

    void GateModule::setThreshold(float v) { gate_.setThreshold(v); }
    void GateModule::setAttack(float ms) { gate_.setAttack(ms); }
    void GateModule::setRelease(float ms) { gate_.setRelease(ms); }
    void GateModule::setBypassed(bool b) { bypassed_ = b; }
}