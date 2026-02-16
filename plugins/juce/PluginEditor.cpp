#include "PluginEditor.h"

RaptorBreakcoreAudioProcessorEditor::RaptorBreakcoreAudioProcessorEditor(RaptorBreakcoreAudioProcessor& processor)
    : juce::AudioProcessorEditor(&processor),
      processor_(processor) {
    setSize(900, 460);

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
    configureSlider(postLowPassHz_, "AA LPF Hz");

    oversampling_.addItemList({"1x", "2x", "4x"}, 1);
    addAndMakeVisible(oversampling_);

    safetyLimiter_.setButtonText("Safety Limiter");
    addAndMakeVisible(safetyLimiter_);

    presetBox_.addItem("Init", 1);
    presetBox_.addItem("Clean Punch", 2);
    presetBox_.addItem("Tape Crunch", 3);
    presetBox_.addItem("Amen Shred", 4);
    presetBox_.addItem("Industrial Burst", 5);
    presetBox_.setSelectedId(1, juce::dontSendNotification);
    presetBox_.addListener(this);
    addAndMakeVisible(presetBox_);

    for (auto* b : {&storeA_, &useA_, &storeB_, &useB_, &swapAB_}) {
        b->addListener(this);
        addAndMakeVisible(*b);
    }

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
    oversamplingAttachment_ = std::make_unique<ComboAttachment>(apvts, "oversamplingFactor", oversampling_);
    postLowPassAttachment_ = std::make_unique<SliderAttachment>(apvts, "postLowPassHz", postLowPassHz_);
    limiterAttachment_ = std::make_unique<ButtonAttachment>(apvts, "safetyLimiterEnabled", safetyLimiter_);

    snapshotA_ = apvts.copyState();
    snapshotB_ = apvts.copyState();
}

void RaptorBreakcoreAudioProcessorEditor::paint(juce::Graphics& g) {
    g.fillAll(juce::Colours::black.withAlpha(0.94F));
    g.setColour(juce::Colours::white.withAlpha(0.2F));
    g.drawRoundedRectangle(getLocalBounds().toFloat().reduced(8.0F), 8.0F, 1.0F);
}

void RaptorBreakcoreAudioProcessorEditor::resized() {
    auto bounds = getLocalBounds().reduced(16);
    title_.setBounds(bounds.removeFromTop(36));

    auto presetRow = bounds.removeFromTop(34);
    presetBox_.setBounds(presetRow.removeFromLeft(200).reduced(2));
    storeA_.setBounds(presetRow.removeFromLeft(90).reduced(2));
    useA_.setBounds(presetRow.removeFromLeft(70).reduced(2));
    storeB_.setBounds(presetRow.removeFromLeft(90).reduced(2));
    useB_.setBounds(presetRow.removeFromLeft(70).reduced(2));
    swapAB_.setBounds(presetRow.removeFromLeft(100).reduced(2));

    bounds.removeFromTop(8);
    auto macroArea = bounds.removeFromTop(220);
    auto advancedArea = bounds;

    constexpr int macroCount = 6;
    const int macroWidth = macroArea.getWidth() / macroCount;
    juce::Slider* macroSliders[macroCount] = {&damage_, &chaos_, &tone_, &punch_, &mix_, &output_};
    for (int i = 0; i < macroCount; ++i) {
        macroSliders[i]->setBounds(macroArea.removeFromLeft(macroWidth).reduced(6));
    }

    auto advancedTop = advancedArea.removeFromTop(150);
    constexpr int advancedCount = 6;
    const int advancedWidth = advancedTop.getWidth() / advancedCount;

    retriggerProbability_.setBounds(advancedTop.removeFromLeft(advancedWidth).reduced(8));
    reverseProbability_.setBounds(advancedTop.removeFromLeft(advancedWidth).reduced(8));
    gateDepth_.setBounds(advancedTop.removeFromLeft(advancedWidth).reduced(8));
    postLowPassHz_.setBounds(advancedTop.removeFromLeft(advancedWidth).reduced(8));
    oversampling_.setBounds(advancedTop.removeFromLeft(advancedWidth).reduced(8));
    safetyLimiter_.setBounds(advancedTop.removeFromLeft(advancedWidth).reduced(8));
}

void RaptorBreakcoreAudioProcessorEditor::configureSlider(juce::Slider& slider, const juce::String& labelText) {
    slider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    slider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 68, 20);
    slider.setPopupDisplayEnabled(true, true, this);
    slider.setName(labelText);
    addAndMakeVisible(slider);

    auto* label = labels_.add(new juce::Label());
    label->setText(labelText, juce::dontSendNotification);
    label->attachToComponent(&slider, false);
    label->setJustificationType(juce::Justification::centredTop);
}

void RaptorBreakcoreAudioProcessorEditor::comboBoxChanged(juce::ComboBox* comboBoxThatHasChanged) {
    if (comboBoxThatHasChanged == &presetBox_) {
        applyPreset(presetBox_.getSelectedId());
    }
}

void RaptorBreakcoreAudioProcessorEditor::buttonClicked(juce::Button* button) {
    auto& apvts = processor_.parameters();
    if (button == &storeA_) {
        snapshotA_ = apvts.copyState();
    } else if (button == &useA_) {
        apvts.replaceState(snapshotA_);
    } else if (button == &storeB_) {
        snapshotB_ = apvts.copyState();
    } else if (button == &useB_) {
        apvts.replaceState(snapshotB_);
    } else if (button == &swapAB_) {
        auto tmp = snapshotA_;
        snapshotA_ = snapshotB_;
        snapshotB_ = tmp;
    }
}

void RaptorBreakcoreAudioProcessorEditor::applyPreset(int presetId) {
    switch (presetId) {
        case 2:  // Clean Punch
            setParamValue("damage", 0.28F);
            setParamValue("chaos", 0.12F);
            setParamValue("tone", 0.50F);
            setParamValue("punch", 0.78F);
            setParamValue("mix", 0.45F);
            setParamValue("output", 0.72F);
            setParamValue("retriggerProbability", 0.06F);
            setParamValue("reverseProbability", 0.03F);
            setParamValue("gateDepth", 0.10F);
            setChoiceIndex("oversamplingFactor", 2);
            setParamValue("postLowPassHz", 18000.0F);
            break;
        case 3:  // Tape Crunch
            setParamValue("damage", 0.48F);
            setParamValue("chaos", 0.20F);
            setParamValue("tone", 0.45F);
            setParamValue("punch", 0.45F);
            setParamValue("mix", 0.62F);
            setParamValue("output", 0.68F);
            setParamValue("retriggerProbability", 0.08F);
            setParamValue("reverseProbability", 0.02F);
            setParamValue("gateDepth", 0.15F);
            setChoiceIndex("oversamplingFactor", 1);
            setParamValue("postLowPassHz", 16000.0F);
            break;
        case 4:  // Amen Shred
            setParamValue("damage", 0.82F);
            setParamValue("chaos", 0.85F);
            setParamValue("tone", 0.62F);
            setParamValue("punch", 0.55F);
            setParamValue("mix", 0.80F);
            setParamValue("output", 0.54F);
            setParamValue("retriggerProbability", 0.35F);
            setParamValue("reverseProbability", 0.25F);
            setParamValue("gateDepth", 0.52F);
            setChoiceIndex("oversamplingFactor", 2);
            setParamValue("postLowPassHz", 14000.0F);
            break;
        case 5:  // Industrial Burst
            setParamValue("damage", 0.92F);
            setParamValue("chaos", 0.72F);
            setParamValue("tone", 0.70F);
            setParamValue("punch", 0.50F);
            setParamValue("mix", 0.86F);
            setParamValue("output", 0.45F);
            setParamValue("retriggerProbability", 0.28F);
            setParamValue("reverseProbability", 0.18F);
            setParamValue("gateDepth", 0.60F);
            setChoiceIndex("oversamplingFactor", 2);
            setParamValue("postLowPassHz", 12000.0F);
            break;
        default:  // Init
            setParamValue("damage", 0.35F);
            setParamValue("chaos", 0.20F);
            setParamValue("tone", 0.50F);
            setParamValue("punch", 0.30F);
            setParamValue("mix", 0.65F);
            setParamValue("output", 0.80F);
            setParamValue("retriggerProbability", 0.12F);
            setParamValue("reverseProbability", 0.05F);
            setParamValue("gateDepth", 0.25F);
            setChoiceIndex("oversamplingFactor", 0);
            setParamValue("postLowPassHz", 16000.0F);
            break;
    }
}

void RaptorBreakcoreAudioProcessorEditor::setParamValue(const juce::String& id, float plainValue) {
    if (auto* param = processor_.parameters().getParameter(id)) {
        param->setValueNotifyingHost(param->convertTo0to1(plainValue));
    }
}

void RaptorBreakcoreAudioProcessorEditor::setChoiceIndex(const juce::String& id, int index) {
    if (auto* param = processor_.parameters().getParameter(id)) {
        param->setValueNotifyingHost(juce::jlimit(0.0F, 1.0F, static_cast<float>(index) / 2.0F));
    }
}
