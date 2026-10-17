#include "PluginProcessor.h"
#include "PluginEditor.h"

GlitchboxProcessor::GlitchboxProcessor()
    : AudioProcessor (BusesProperties()
                          .withInput ("Input", juce::AudioChannelSet::stereo(), true)
                          .withOutput ("Output", juce::AudioChannelSet::stereo(), true)),
      state (*this, nullptr, "Glitchbox", createLayout())
{
    mixParam = dynamic_cast<juce::AudioParameterFloat*> (state.getParameter (ParamIDs::mix));
    outputParam = dynamic_cast<juce::AudioParameterFloat*> (state.getParameter (ParamIDs::output));
    jassert (mixParam != nullptr && outputParam != nullptr);
}

juce::AudioProcessorValueTreeState::ParameterLayout GlitchboxProcessor::createLayout()
{
    using namespace juce;

    auto percent = AudioParameterFloatAttributes().withLabel ("%")
                       .withStringFromValueFunction ([] (float v, int) { return String (roundToInt (v)) + " %"; });
    auto decibels = AudioParameterFloatAttributes().withLabel ("dB")
                        .withStringFromValueFunction ([] (float v, int) { return String (v, 1) + " dB"; });

    return {
        std::make_unique<AudioParameterFloat> (ParameterID { ParamIDs::mix, 1 }, "Mix",
                                               NormalisableRange<float> (0.0f, 100.0f, 0.1f), 100.0f, percent),
        std::make_unique<AudioParameterFloat> (ParameterID { ParamIDs::output, 1 }, "Output",
                                               NormalisableRange<float> (-24.0f, 12.0f, 0.1f), 0.0f, decibels),
    };
}

void GlitchboxProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    dryBuffer.setSize (getTotalNumOutputChannels(), samplesPerBlock, false, false, true);

    mixSmoothed.reset (sampleRate, 0.02);
    outputSmoothed.reset (sampleRate, 0.02);
    mixSmoothed.setCurrentAndTargetValue (mixParam->get() / 100.0f);
    outputSmoothed.setCurrentAndTargetValue (juce::Decibels::decibelsToGain (outputParam->get()));
}

bool GlitchboxProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
    const auto out = layouts.getMainOutputChannelSet();
    if (out != juce::AudioChannelSet::mono() && out != juce::AudioChannelSet::stereo())
        return false;

    return layouts.getMainInputChannelSet() == out;
}

void GlitchboxProcessor::readHostTransport()
{
    if (auto* host = getPlayHead())
    {
        if (auto position = host->getPosition())
        {
            if (auto bpm = position->getBpm())
                hostBpm.store (*bpm);
            hostPlaying.store (position->getIsPlaying());
        }
    }
}

void GlitchboxProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer&)
{
    juce::ScopedNoDenormals noDenormals;

    const auto numChannels = getTotalNumOutputChannels();
    const auto numSamples = buffer.getNumSamples();

    for (auto ch = getTotalNumInputChannels(); ch < numChannels; ++ch)
        buffer.clear (ch, 0, numSamples);

    readHostTransport();

    // Hosts may send a block larger than announced; grow without reallocating each time.
    if (dryBuffer.getNumSamples() < numSamples)
        dryBuffer.setSize (numChannels, numSamples, false, false, true);

    for (auto ch = 0; ch < numChannels; ++ch)
        dryBuffer.copyFrom (ch, 0, buffer, ch, 0, numSamples);

    // The effect chain will process `buffer` here.

    mixSmoothed.setTargetValue (mixParam->get() / 100.0f);
    outputSmoothed.setTargetValue (juce::Decibels::decibelsToGain (outputParam->get()));

    for (auto i = 0; i < numSamples; ++i)
    {
        const auto wet = mixSmoothed.getNextValue();
        const auto gain = outputSmoothed.getNextValue();

        for (auto ch = 0; ch < numChannels; ++ch)
        {
            auto* out = buffer.getWritePointer (ch);
            const auto dry = dryBuffer.getSample (ch, i);
            out[i] = (dry + wet * (out[i] - dry)) * gain;
        }
    }
}

juce::AudioProcessorEditor* GlitchboxProcessor::createEditor()
{
    return new GlitchboxEditor (*this);
}

void GlitchboxProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    if (auto xml = state.copyState().createXml())
        copyXmlToBinary (*xml, destData);
}

void GlitchboxProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    if (auto xml = getXmlFromBinary (data, sizeInBytes))
        if (xml->hasTagName (state.state.getType()))
            state.replaceState (juce::ValueTree::fromXml (*xml));
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new GlitchboxProcessor();
}
