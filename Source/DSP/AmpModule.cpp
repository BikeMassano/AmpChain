#pragma once

#include "AmpModule.h"

AmpModule::AmpModule()
{
    
}

void AmpModule::prepare(const juce::dsp::ProcessSpec& spec)
{
}


void AmpModule::reset()
{

}

void AmpModule::process(const juce::dsp::ProcessContextReplacing<float>& context)
{
    if (bypassed_)
        return;   
}

bool AmpModule::loadModel(const juce::String& modelPath)
{
   return false;
}

void AmpModule::clearModel()
{
}

double AmpModule::getModelSampleRate() const
{
    return 44100.f;
}

void AmpModule::setGain(float db)    
{ 
    preChain_.get<inputGainIndex>().setGainDecibels(db);
}

void AmpModule::setLevel(float db)   
{
    postChain_.get<outputGainIndex>().setGainDecibels(db);
}

void AmpModule::setBypassed(bool v) { bypassed_ = v; }

void AmpModule::setBass(float db) { bassVal_ = db; updateFilters_(); }
void AmpModule::setMid(float db) { midVal_ = db; updateFilters_(); }
void AmpModule::setTreble(float db) { trebleVal_ = db; updateFilters_(); }
void AmpModule::setPresence(float db) { presenceVal_ = db; updateFilters_(); }

void AmpModule::updateFilters_()
{
    if (sampleRate_ <= 0.0) return;
    const float sr = (float)sampleRate_;

    // Bass: low-shelf ±12 дБ вокруг 250 Гц
    {
        const float freq  = 250.f;
        const float gainDB = juce::jmap(bassVal_, 0.f, 1.f, -12.f, 12.f);
        *preChain_.get<1>().state =
            *FilterCoefs::makeLowShelf(sr, freq, 0.707f, juce::Decibels::decibelsToGain(gainDB));
    }
    // Mid: peak ±10 дБ на 800 Гц
    {
        const float freq  = 800.f;
        const float gainDB = juce::jmap(midVal_, 0.f, 1.f, -10.f, 10.f);
        *preChain_.get<2>().state =
            *FilterCoefs::makePeakFilter(sr, freq, 0.9f, juce::Decibels::decibelsToGain(gainDB));
    }
    // Treble: high-shelf ±12 дБ от 3.5 кГц
    {
        const float freq  = 3500.f;
        const float gainDB = juce::jmap(trebleVal_, 0.f, 1.f, -12.f, 12.f);
        *preChain_.get<3>().state =
            *FilterCoefs::makeHighShelf(sr, freq, 0.707f, juce::Decibels::decibelsToGain(gainDB));
    }
    // Presence: high-shelf ±8 дБ от 3 кГц (post)
    {
        const float freq  = 3000.f;
        const float gainDB = juce::jmap(presenceVal_, 0.f, 1.f, -8.f, 8.f);
        *postChain_.get<0>().state =
            *FilterCoefs::makeHighShelf(sr, freq, 0.707f, juce::Decibels::decibelsToGain(gainDB));
    }
}