#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_gui_basics/juce_gui_basics.h>

class KnobLookAndFeel : public juce::LookAndFeel_V4
{
public:
    juce::Image knobImage;
    juce::Image shadowImage;

    KnobLookAndFeel(juce::Image k, juce::Image s)
        : knobImage(k), shadowImage(s) {}

    void drawRotarySlider(juce::Graphics& g,
                          int x, int y, int width, int height,
                          float sliderPosProportional,
                          float rotaryStartAngle,
                          float rotaryEndAngle,
                          juce::Slider&) override
    {
        auto bounds = juce::Rectangle<int>(x, y, width, height).toFloat();

        auto center = bounds.getCentre();

        float angle = rotaryStartAngle
                    + sliderPosProportional * (rotaryEndAngle - rotaryStartAngle);

        // ===== shadow =====
        g.drawImageWithin(
            shadowImage,
            x, y, width, height,
            juce::RectanglePlacement::centred
        );

        // ===== knob rotation =====
        g.addTransform(juce::AffineTransform::rotation(angle, center.x, center.y));

        g.drawImageWithin(
            knobImage,
            x, y, width, height,
            juce::RectanglePlacement::centred
        );
    }
};