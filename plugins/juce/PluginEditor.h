#pragma once

#include <JuceHeader.h>

#include "PluginProcessor.h"

class RaptorBreakcoreAudioProcessorEditor : public juce::AudioProcessorEditor {
public:
    explicit RaptorBreakcoreAudioProcessorEditor(RaptorBreakcoreAudioProcessor&);
    ~RaptorBreakcoreAudioProcessorEditor() override = default;

    void paint(juce::Graphics&) override;
    void resized() override;

private:
    using SliderAttachment = juce::AudioProcessorValueTreeState::SliderAttachment;
    using ButtonAttachment = juce::AudioProcessorValueTreeState::ButtonAttachment;

    void configureSlider(juce::Slider& slider, const juce::String& labelText);

    RaptorBreakcoreAudioProcessor& processor_;

    juce::Label title_;

    juce::Slider damage_;
    juce::Slider chaos_;
    juce::Slider tone_;
    juce::Slider punch_;
    juce::Slider mix_;
    juce::Slider output_;

    juce::Slider retriggerProbability_;
    juce::Slider reverseProbability_;
    juce::Slider gateDepth_;
    juce::ToggleButton safetyLimiter_;

    juce::OwnedArray<juce::Label> labels_;

    std::unique_ptr<SliderAttachment> damageAttachment_;
    std::unique_ptr<SliderAttachment> chaosAttachment_;
    std::unique_ptr<SliderAttachment> toneAttachment_;
    std::unique_ptr<SliderAttachment> punchAttachment_;
    std::unique_ptr<SliderAttachment> mixAttachment_;
    std::unique_ptr<SliderAttachment> outputAttachment_;

    std::unique_ptr<SliderAttachment> retriggerAttachment_;
    std::unique_ptr<SliderAttachment> reverseAttachment_;
    std::unique_ptr<SliderAttachment> gateAttachment_;
    std::unique_ptr<ButtonAttachment> limiterAttachment_;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(RaptorBreakcoreAudioProcessorEditor)
};
