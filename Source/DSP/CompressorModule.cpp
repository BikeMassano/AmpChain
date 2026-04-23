#include "CompressorModule.h"

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

void CompressorModule::setThreshold(float v) { chain_.get<0>().setThreshold(v); }
void CompressorModule::setRatio(float v)     { chain_.get<0>().setRatio(v); }
void CompressorModule::setAttack(float ms)    { chain_.get<0>().setAttack(ms); }
void CompressorModule::setRelease(float ms)   { chain_.get<0>().setRelease(ms); }
void CompressorModule::setMakeup(float db)    { chain_.get<1>().setGainDecibels(db); }
void CompressorModule::setBypassed(bool b)   { bypassed_ = b; }