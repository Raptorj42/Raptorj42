#pragma once

#include <JuceHeader.h>

#include "PluginProcessor.h"

class RaptorBreakcoreAudioProcessorEditor : public juce::AudioProcessorEditor,
                                            private juce::ComboBox::Listener,
                                            private juce::Button::Listener {
public:
    explicit RaptorBreakcoreAudioProcessorEditor(RaptorBreakcoreAudioProcessor&);
    ~RaptorBreakcoreAudioProcessorEditor() override = default;

    void paint(juce::Graphics&) override;
    void resized() override;

private:
    using SliderAttachment = juce::AudioProcessorValueTreeState::SliderAttachment;
    using ButtonAttachment = juce::AudioProcessorValueTreeState::ButtonAttachment;
    using ComboAttachment = juce::AudioProcessorValueTreeState::ComboBoxAttachment;

    void configureSlider(juce::Slider& slider, const juce::String& labelText);
    void comboBoxChanged(juce::ComboBox* comboBoxThatHasChanged) override;
    void buttonClicked(juce::Button* button) override;

    void applyPreset(int presetId);
    void setParamValue(const juce::String& id, float plainValue);
    void setChoiceIndex(const juce::String& id, int index);

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
    juce::ComboBox oversampling_;
    juce::Slider postLowPassHz_;
    juce::ToggleButton safetyLimiter_;

    juce::ComboBox presetBox_;
    juce::TextButton storeA_{"Store A"};
    juce::TextButton useA_{"Use A"};
    juce::TextButton storeB_{"Store B"};
    juce::TextButton useB_{"Use B"};
    juce::TextButton swapAB_{"Swap A/B"};

    juce::ValueTree snapshotA_;
    juce::ValueTree snapshotB_;

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
    std::unique_ptr<ComboAttachment> oversamplingAttachment_;
    std::unique_ptr<SliderAttachment> postLowPassAttachment_;
    std::unique_ptr<ButtonAttachment> limiterAttachment_;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(RaptorBreakcoreAudioProcessorEditor)
};
