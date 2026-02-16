#include "raptorfx/DrumFxProcessor.h"

#include <algorithm>
#include <cmath>

namespace raptorfx {

void DrumFxProcessor::prepare(double sampleRate, double tempoBpm) {
    sampleRate_ = sampleRate;
    tempoBpm_ = tempoBpm > 30.0 ? tempoBpm : 174.0;

    distortion_.setSampleRate(sampleRate_);
    breakcore_.setSampleRate(sampleRate_);
    breakcore_.setTempoBpm(tempoBpm_);

    setMacros(macros_);
    setAdvanced(advanced_);
}

void DrumFxProcessor::setTempoBpm(double tempoBpm) {
    tempoBpm_ = tempoBpm > 30.0 ? tempoBpm : 174.0;
    breakcore_.setTempoBpm(tempoBpm_);
}

void DrumFxProcessor::setMacros(const MacroParameters& macros) {
    macros_ = macros;

    distortion_.setDamage(clamp(macros_.damage, 0.0F, 1.0F));
    distortion_.setTone(clamp(macros_.tone, 0.0F, 1.0F));
    distortion_.setPunch(clamp(macros_.punch, 0.0F, 1.0F));

    breakcore_.setChaos(clamp(macros_.chaos, 0.0F, 1.0F));
}

void DrumFxProcessor::setAdvanced(const AdvancedParameters& advanced) {
    advanced_ = advanced;

    breakcore_.setRetriggerProbability(clamp(advanced_.retriggerProbability, 0.0F, 1.0F));
    breakcore_.setReverseProbability(clamp(advanced_.reverseProbability, 0.0F, 1.0F));
    breakcore_.setGateDepth(clamp(advanced_.gateDepth, 0.0F, 1.0F));
}

void DrumFxProcessor::process(float* left, float* right, std::size_t numSamples) {
    std::vector<float> dryL(left, left + numSamples);
    std::vector<float> dryR(right, right + numSamples);

    std::vector<float> wetL = dryL;
    std::vector<float> wetR = dryR;

    breakcore_.processInPlace(wetL);
    breakcore_.processInPlace(wetR);

    distortion_.processInPlace(wetL);
    distortion_.processInPlace(wetR);

    const float mix = clamp(macros_.mix, 0.0F, 1.0F);
    const float outGain = 0.2F + clamp(macros_.output, 0.0F, 1.0F) * 1.8F;

    for (std::size_t i = 0; i < numSamples; ++i) {
        left[i] = ((1.0F - mix) * dryL[i] + mix * wetL[i]) * outGain;
        right[i] = ((1.0F - mix) * dryR[i] + mix * wetR[i]) * outGain;
    }

    if (advanced_.safetyLimiterEnabled) {
        std::vector<float> outL(left, left + numSamples);
        std::vector<float> outR(right, right + numSamples);
        safetyLimit(outL);
        safetyLimit(outR);
        std::copy(outL.begin(), outL.end(), left);
        std::copy(outR.begin(), outR.end(), right);
    }
}

float DrumFxProcessor::clamp(float value, float minValue, float maxValue) {
    return std::min(maxValue, std::max(minValue, value));
}

void DrumFxProcessor::safetyLimit(std::vector<float>& channel) {
    constexpr float ceiling = 0.98F;
    for (float& s : channel) {
        if (std::fabs(s) > ceiling) {
            s = (s > 0.0F ? ceiling : -ceiling);
        }
    }
}

}  // namespace raptorfx
