#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_dsp/juce_dsp.h>

namespace ParamIDs
{
    inline constexpr auto mix = "mix";
    inline constexpr auto output = "output";
}

class GlitchboxProcessor final : public juce::AudioProcessor
{
public:
    GlitchboxProcessor();
    ~GlitchboxProcessor() override = default;

    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override {}
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;
    using AudioProcessor::processBlock;

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override { return true; }

    const juce::String getName() const override { return JucePlugin_Name; }
    bool acceptsMidi() const override { return true; }
    bool producesMidi() const override { return false; }
    bool isMidiEffect() const override { return false; }
    double getTailLengthSeconds() const override { return 0.0; }

    int getNumPrograms() override { return 1; }
    int getCurrentProgram() override { return 0; }
    void setCurrentProgram (int) override {}
    const juce::String getProgramName (int) override { return {}; }
    void changeProgramName (int, const juce::String&) override {}

    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    juce::AudioProcessorValueTreeState& getState() noexcept { return state; }

    // Host tempo, written on the audio thread and read by the editor.
    double getHostBpm() const noexcept { return hostBpm.load(); }
    bool isHostPlaying() const noexcept { return hostPlaying.load(); }

private:
    static juce::AudioProcessorValueTreeState::ParameterLayout createLayout();
    void readHostTransport();

    juce::AudioProcessorValueTreeState state;
    juce::AudioParameterFloat* mixParam = nullptr;
    juce::AudioParameterFloat* outputParam = nullptr;

    juce::AudioBuffer<float> dryBuffer;
    juce::SmoothedValue<float> mixSmoothed, outputSmoothed;

    std::atomic<double> hostBpm { 120.0 };
    std::atomic<bool> hostPlaying { false };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (GlitchboxProcessor)
};
