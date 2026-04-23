#include "MonoModule.h"


void MonoModule::process(const juce::dsp::ProcessContextReplacing<float>& context)
{
    if (!enabled_)
        return;
    
    auto& block = context.getOutputBlock();

    if (block.getNumChannels() < 2)
            return;

    auto* left = block.getChannelPointer(0);
    auto* right = block.getChannelPointer(1);

    const auto numSamples = block.getNumSamples();

    for (size_t i = 0; i < numSamples; ++i)
    {
        float mono = 0.0f;

        switch (mode_)
        {
            case MonoMode::Left:
                mono = left[i];
                break;
            case MonoMode::Right:
                mono = right[i];
                break;
            default:
                break;
        }
        left[i] = mono;
        right[i] = mono;
    }
}

void MonoModule::setEnabled(bool e) { enabled_ = e; }
void MonoModule::setMode(MonoMode m) { mode_ = m; }
