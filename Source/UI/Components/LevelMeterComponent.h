#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

namespace GUI
{
    class LevelMeterComponent final : public juce::Component
    {
    public:
        LevelMeterComponent()
        {
        }

        void paint(juce::Graphics& g) override
        {
            auto bounds = getLocalBounds().toFloat();

            g.setColour(juce::Colours::white.withBrightness(0.04f));
            g.fillRoundedRectangle(bounds, 5.f);

            const auto maxMeterWidth = (float)getWidth();
            const auto scaledX = 
                juce::jmap(rmsLevel_, -60.f, 6.f, 0.f, maxMeterWidth);
            const auto currentWidth = juce::jlimit(0.f, maxMeterWidth, scaledX);

            const auto meterColour = detectClipping() ? juce::Colours::red : juce::Colours::white;
            g.setColour(meterColour);
            
            g.fillRoundedRectangle(bounds.removeFromLeft(currentWidth), 5.f);
        };

        void setLevel(const float value) 
        { 
            if (rmsLevel_ != value)
            {
                rmsLevel_ = value;
                repaint();
            }
        }

        void setPeak(const float value) 
        { 
            if (peakLevel_ != value)
            {
                peakLevel_ = value;
                repaint();
            }
        }

    private:
        float rmsLevel_ = -60.f;
        float peakLevel_ = -60.f;

        bool detectClipping()
        {
            return peakLevel_ >= 0.f;
        }

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(LevelMeterComponent)
    };
}