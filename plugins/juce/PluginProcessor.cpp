#include "PluginProcessor.h"

#include "PluginEditor.h"

namespace {
constexpr double kFallbackTempo = 174.0;

float readParam(juce::AudioProcessorValueTreeState& apvts, const char* id, float fallback) {
    if (auto* p = apvts.getRawParameterValue(id)) {
        return p->load();
    }
    return fallback;
}

void addFloat(juce::AudioProcessorValueTreeState::ParameterLayout& layout,
                                    const juce::String& id,
                                    const juce::String& name,
                                    float min,
                                    float max,
                                    float def) {
    auto parameter = std::make_unique<juce::AudioParameterFloat>(id, name, min, max, def);
    layout.add(std::move(parameter));
}
}  // namespace

RaptorBreakcoreAudioProcessor::RaptorBreakcoreAudioProcessor()
    : juce::AudioProcessor(BusesProperties().withInput("Input", juce::AudioChannelSet::stereo(), true)
                                               .withOutput("Output", juce::AudioChannelSet::stereo(), true)),
      apvts_(*this, nullptr, "PARAMETERS", createParameterLayout()) {}

void RaptorBreakcoreAudioProcessor::prepareToPlay(double sampleRate, int /*samplesPerBlock*/) {
    engine_.prepare(sampleRate, resolveTempoBpm());
    syncParametersFromAPVTS();
}

void RaptorBreakcoreAudioProcessor::releaseResources() {}

bool RaptorBreakcoreAudioProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const {
    const auto input = layouts.getChannelSet(true, 0);
    const auto output = layouts.getChannelSet(false, 0);
    return (input == juce::AudioChannelSet::mono() || input == juce::AudioChannelSet::stereo()) && input == output;
}

void RaptorBreakcoreAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer&) {
    juce::ScopedNoDenormals noDenormals;

    for (int channel = getTotalNumInputChannels(); channel < getTotalNumOutputChannels(); ++channel) {
        buffer.clear(channel, 0, buffer.getNumSamples());
    }

    engine_.setTempoBpm(resolveTempoBpm());
    syncParametersFromAPVTS();

    if (buffer.getNumChannels() >= 2) {
        engine_.process(buffer.getWritePointer(0), buffer.getWritePointer(1), static_cast<std::size_t>(buffer.getNumSamples()));
    } else if (buffer.getNumChannels() == 1) {
        auto* mono = buffer.getWritePointer(0);
        engine_.process(mono, mono, static_cast<std::size_t>(buffer.getNumSamples()));
    }
}

juce::AudioProcessorEditor* RaptorBreakcoreAudioProcessor::createEditor() {
    return new RaptorBreakcoreAudioProcessorEditor(*this);
}

const juce::String RaptorBreakcoreAudioProcessor::getName() const {
    return JucePlugin_Name;
}

void RaptorBreakcoreAudioProcessor::setCurrentProgram(int) {}

const juce::String RaptorBreakcoreAudioProcessor::getProgramName(int) {
    return {};
}

void RaptorBreakcoreAudioProcessor::changeProgramName(int, const juce::String&) {}

void RaptorBreakcoreAudioProcessor::getStateInformation(juce::MemoryBlock& destData) {
    auto state = apvts_.copyState();
    std::unique_ptr<juce::XmlElement> xml(state.createXml());
    copyXmlToBinary(*xml, destData);
}

void RaptorBreakcoreAudioProcessor::setStateInformation(const void* data, int sizeInBytes) {
    std::unique_ptr<juce::XmlElement> xmlState(getXmlFromBinary(data, sizeInBytes));
    if (xmlState == nullptr) {
        return;
    }
    if (xmlState->hasTagName(apvts_.state.getType())) {
        apvts_.replaceState(juce::ValueTree::fromXml(*xmlState));
    }
}

juce::AudioProcessorValueTreeState::ParameterLayout RaptorBreakcoreAudioProcessor::createParameterLayout() {
    juce::AudioProcessorValueTreeState::ParameterLayout layout;

    addFloat(layout, "damage", "Damage", 0.0F, 1.0F, 0.35F);
    addFloat(layout, "chaos", "Chaos", 0.0F, 1.0F, 0.20F);
    addFloat(layout, "tone", "Tone", 0.0F, 1.0F, 0.50F);
    addFloat(layout, "punch", "Punch", 0.0F, 1.0F, 0.30F);
    addFloat(layout, "mix", "Mix", 0.0F, 1.0F, 0.65F);
    addFloat(layout, "output", "Output", 0.0F, 1.0F, 0.80F);

    addFloat(layout, "retriggerProbability", "Retrigger Probability", 0.0F, 1.0F, 0.12F);
    addFloat(layout, "reverseProbability", "Reverse Probability", 0.0F, 1.0F, 0.05F);
    addFloat(layout, "gateDepth", "Gate Depth", 0.0F, 1.0F, 0.25F);
    layout.add(std::make_unique<juce::AudioParameterBool>("safetyLimiterEnabled", "Safety Limiter", true));

    return layout;
}

void RaptorBreakcoreAudioProcessor::syncParametersFromAPVTS() {
    raptorfx::MacroParameters macros;
    macros.damage = readParam(apvts_, "damage", 0.35F);
    macros.chaos = readParam(apvts_, "chaos", 0.20F);
    macros.tone = readParam(apvts_, "tone", 0.50F);
    macros.punch = readParam(apvts_, "punch", 0.30F);
    macros.mix = readParam(apvts_, "mix", 0.65F);
    macros.output = readParam(apvts_, "output", 0.80F);

    raptorfx::AdvancedParameters advanced;
    advanced.retriggerProbability = readParam(apvts_, "retriggerProbability", 0.12F);
    advanced.reverseProbability = readParam(apvts_, "reverseProbability", 0.05F);
    advanced.gateDepth = readParam(apvts_, "gateDepth", 0.25F);
    advanced.safetyLimiterEnabled = readParam(apvts_, "safetyLimiterEnabled", 1.0F) > 0.5F;

    engine_.setMacros(macros);
    engine_.setAdvanced(advanced);
}

double RaptorBreakcoreAudioProcessor::resolveTempoBpm() const {
    if (auto* playHead = getPlayHead()) {
        juce::AudioPlayHead::CurrentPositionInfo info;
        if (playHead->getCurrentPosition(info) && info.bpm > 0.0) {
            return info.bpm;
        }
    }
    return kFallbackTempo;
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter() {
    return new RaptorBreakcoreAudioProcessor();
}
