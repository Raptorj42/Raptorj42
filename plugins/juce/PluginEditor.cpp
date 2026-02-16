#include "PluginEditor.h"

RaptorBreakcoreAudioProcessorEditor::RaptorBreakcoreAudioProcessorEditor(RaptorBreakcoreAudioProcessor& processor)
    : juce::AudioProcessorEditor(&processor),
      processor_(processor) {
    setSize(760, 360);

    title_.setText("Raptor Breakcore FX", juce::dontSendNotification);
    title_.setFont(juce::FontOptions(22.0F, juce::Font::bold));
    title_.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(title_);

    configureSlider(damage_, "Damage");
    configureSlider(chaos_, "Chaos");
    configureSlider(tone_, "Tone");
    configureSlider(punch_, "Punch");
    configureSlider(mix_, "Mix");
    configureSlider(output_, "Output");

    configureSlider(retriggerProbability_, "Retrig");
    configureSlider(reverseProbability_, "Reverse");
    configureSlider(gateDepth_, "Gate");

    safetyLimiter_.setButtonText("Safety Limiter");
    addAndMakeVisible(safetyLimiter_);

    auto& apvts = processor_.parameters();
    damageAttachment_ = std::make_unique<SliderAttachment>(apvts, "damage", damage_);
    chaosAttachment_ = std::make_unique<SliderAttachment>(apvts, "chaos", chaos_);
    toneAttachment_ = std::make_unique<SliderAttachment>(apvts, "tone", tone_);
    punchAttachment_ = std::make_unique<SliderAttachment>(apvts, "punch", punch_);
    mixAttachment_ = std::make_unique<SliderAttachment>(apvts, "mix", mix_);
    outputAttachment_ = std::make_unique<SliderAttachment>(apvts, "output", output_);

    retriggerAttachment_ = std::make_unique<SliderAttachment>(apvts, "retriggerProbability", retriggerProbability_);
    reverseAttachment_ = std::make_unique<SliderAttachment>(apvts, "reverseProbability", reverseProbability_);
    gateAttachment_ = std::make_unique<SliderAttachment>(apvts, "gateDepth", gateDepth_);
    limiterAttachment_ = std::make_unique<ButtonAttachment>(apvts, "safetyLimiterEnabled", safetyLimiter_);
}

void RaptorBreakcoreAudioProcessorEditor::paint(juce::Graphics& g) {
    g.fillAll(juce::Colours::black.withAlpha(0.94F));
    g.setColour(juce::Colours::white.withAlpha(0.2F));
    g.drawRoundedRectangle(getLocalBounds().toFloat().reduced(8.0F), 8.0F, 1.0F);
}

void RaptorBreakcoreAudioProcessorEditor::resized() {
    auto bounds = getLocalBounds().reduced(16);
    title_.setBounds(bounds.removeFromTop(36));
    bounds.removeFromTop(8);

    auto macroArea = bounds.removeFromTop(190);
    auto advancedArea = bounds;

    constexpr int macroCount = 6;
    const int macroWidth = macroArea.getWidth() / macroCount;
    juce::Slider* macroSliders[macroCount] = {&damage_, &chaos_, &tone_, &punch_, &mix_, &output_};
    for (int i = 0; i < macroCount; ++i) {
        macroSliders[i]->setBounds(macroArea.removeFromLeft(macroWidth).reduced(6));
    }

    auto advancedTop = advancedArea.removeFromTop(130);
    constexpr int advancedCount = 3;
    const int advancedWidth = advancedTop.getWidth() / (advancedCount + 1);
    juce::Slider* advancedSliders[advancedCount] = {&retriggerProbability_, &reverseProbability_, &gateDepth_};
    for (int i = 0; i < advancedCount; ++i) {
        advancedSliders[i]->setBounds(advancedTop.removeFromLeft(advancedWidth).reduced(8));
    }
    safetyLimiter_.setBounds(advancedTop.removeFromLeft(advancedWidth).reduced(8));
}

void RaptorBreakcoreAudioProcessorEditor::configureSlider(juce::Slider& slider, const juce::String& labelText) {
    slider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    slider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 64, 20);
    slider.setPopupDisplayEnabled(true, true, this);
    slider.setName(labelText);
    addAndMakeVisible(slider);

    auto* label = labels_.add(new juce::Label());
    label->setText(labelText, juce::dontSendNotification);
    label->attachToComponent(&slider, false);
    label->setJustificationType(juce::Justification::centredTop);
}
