#pragma once

#include <JuceHeader.h>

#include "raptorfx/DrumFxProcessor.h"

class RaptorBreakcoreAudioProcessor : public juce::AudioProcessor {
public:
    RaptorBreakcoreAudioProcessor();
    ~RaptorBreakcoreAudioProcessor() override = default;

    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

    bool isBusesLayoutSupported(const BusesLayout& layouts) const override;
    void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override { return true; }

    const juce::String getName() const override;
    bool acceptsMidi() const override { return false; }
    bool producesMidi() const override { return false; }
    bool isMidiEffect() const override { return false; }
    double getTailLengthSeconds() const override { return 0.0; }

    int getNumPrograms() override { return 1; }
    int getCurrentProgram() override { return 0; }
    void setCurrentProgram(int index) override;
    const juce::String getProgramName(int index) override;
    void changeProgramName(int index, const juce::String& newName) override;

    void getStateInformation(juce::MemoryBlock& destData) override;
    void setStateInformation(const void* data, int sizeInBytes) override;

    juce::AudioProcessorValueTreeState& parameters() { return apvts_; }

    static juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();

private:
    void syncParametersFromAPVTS();
    double resolveTempoBpm() const;

    juce::AudioProcessorValueTreeState apvts_;
    raptorfx::DrumFxProcessor engine_;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(RaptorBreakcoreAudioProcessor)
};

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter();
