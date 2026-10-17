#include "PluginEditor.h"

GlitchboxEditor::GlitchboxEditor (GlitchboxProcessor& p)
    : AudioProcessorEditor (&p), processor (p)
{
    setUpKnob (mixKnob, "Mix", ParamIDs::mix);
    setUpKnob (outputKnob, "Output", ParamIDs::output);

    transportLabel.setJustificationType (juce::Justification::centredLeft);
    transportLabel.setColour (juce::Label::textColourId, juce::Colours::white.withAlpha (0.6f));
    addAndMakeVisible (transportLabel);

    setResizable (true, true);
    setResizeLimits (640, 400, 1600, 1000);
    setSize (900, 560);

    startTimerHz (10);
}

void GlitchboxEditor::setUpKnob (LabelledKnob& knob, const juce::String& name, const char* paramID)
{
    knob.label.setText (name, juce::dontSendNotification);
    knob.label.setJustificationType (juce::Justification::centred);
    addAndMakeVisible (knob.label);

    knob.slider.setTextBoxStyle (juce::Slider::TextBoxBelow, false, 80, 20);
    addAndMakeVisible (knob.slider);

    knob.attachment = std::make_unique<SliderAttachment> (processor.getState(), paramID, knob.slider);
}

void GlitchboxEditor::paint (juce::Graphics& g)
{
    g.fillAll (juce::Colour (0xff16181d));

    g.setColour (juce::Colours::white);
    g.setFont (juce::FontOptions (26.0f, juce::Font::bold));
    g.drawText ("Glitchbox", 24, 16, 300, 36, juce::Justification::centredLeft);

    g.setColour (juce::Colours::white.withAlpha (0.4f));
    g.setFont (juce::FontOptions (14.0f));
    g.drawText ("v" JucePlugin_VersionString " - foundation build: audio passes through, effects arrive next",
                24, 52, getWidth() - 48, 20, juce::Justification::centredLeft);
}

void GlitchboxEditor::resized()
{
    auto area = getLocalBounds().reduced (24);
    area.removeFromTop (64);

    transportLabel.setBounds (area.removeFromBottom (24));

    auto knobs = area.removeFromRight (240).withSizeKeepingCentre (240, 160);
    for (auto* knob : { &mixKnob, &outputKnob })
    {
        auto column = knobs.removeFromLeft (120);
        knob->label.setBounds (column.removeFromTop (20));
        knob->slider.setBounds (column);
    }
}

void GlitchboxEditor::timerCallback()
{
    const auto text = juce::String (processor.getHostBpm(), 1) + " BPM, "
                    + (processor.isHostPlaying() ? "playing" : "stopped");
    transportLabel.setText (text, juce::dontSendNotification);
}
