#pragma once

#include "PluginProcessor.h"

class GlitchboxEditor final : public juce::AudioProcessorEditor,
                              private juce::Timer
{
public:
    explicit GlitchboxEditor (GlitchboxProcessor&);
    ~GlitchboxEditor() override = default;

    void paint (juce::Graphics&) override;
    void resized() override;

private:
    void timerCallback() override;

    using SliderAttachment = juce::AudioProcessorValueTreeState::SliderAttachment;

    struct LabelledKnob
    {
        juce::Slider slider { juce::Slider::RotaryHorizontalVerticalDrag, juce::Slider::TextBoxBelow };
        juce::Label label;
        std::unique_ptr<SliderAttachment> attachment;
    };

    void setUpKnob (LabelledKnob&, const juce::String& name, const char* paramID);

    GlitchboxProcessor& processor;
    LabelledKnob mixKnob, outputKnob;
    juce::Label transportLabel;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (GlitchboxEditor)
};
