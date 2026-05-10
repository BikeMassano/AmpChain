#include "CabModule.h"

namespace DSP
{
    void CabModule::prepare(const juce::dsp::ProcessSpec& spec)
    {
        sampleRate_ = spec.sampleRate;
        convolution_.prepare (spec);
    }
    void CabModule::reset()
    {
        convolution_.reset();
    }
    void CabModule::process(const juce::dsp::ProcessContextReplacing<float>& context)
    {
            if (bypassed_)
                return;

            convolution_.process(context);
    }

    void CabModule::loadIR(juce::File path)
    {
        // проверка существования файла и его валидности
        if (!path.existsAsFile())
        {
            jassertfalse;
            return;
        }

        // проверка расширения файла
        const auto ext = path.getFileExtension().toLowerCase();
        const bool supported = (ext == ".wav" || ext == ".aif" || ext == ".aiff");
        if (!supported)
        {
            jassertfalse;
            return;
        }

        // загрузка IR
        convolution_.loadImpulseResponse(
            path,
            juce::dsp::Convolution::Stereo::yes,
            juce::dsp::Convolution::Trim::yes,
            0,
            juce::dsp::Convolution::Normalise::yes
        );
    }


    void CabModule::setBypassed(bool b) { bypassed_ = b; }
}
