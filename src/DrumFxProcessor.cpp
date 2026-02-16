#include "raptorfx/DrumFxProcessor.h"

#include <algorithm>
#include <cmath>

namespace raptorfx {

void DrumFxProcessor::prepare(double sampleRate, double tempoBpm) {
    sampleRate_ = sampleRate;
    tempoBpm_ = tempoBpm > 30.0 ? tempoBpm : 174.0;

    distortion_.setSampleRate(sampleRate_ * std::max(1, advanced_.oversamplingFactor));
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

    if (advanced_.oversamplingFactor != 2 && advanced_.oversamplingFactor != 4) {
        advanced_.oversamplingFactor = 1;
    }
    advanced_.postLowPassHz = clamp(advanced_.postLowPassHz, 6000.0F, 22000.0F);

    breakcore_.setRetriggerProbability(clamp(advanced_.retriggerProbability, 0.0F, 1.0F));
    breakcore_.setReverseProbability(clamp(advanced_.reverseProbability, 0.0F, 1.0F));
    breakcore_.setGateDepth(clamp(advanced_.gateDepth, 0.0F, 1.0F));

    distortion_.setSampleRate(sampleRate_ * advanced_.oversamplingFactor);
}

void DrumFxProcessor::process(float* left, float* right, std::size_t numSamples) {
    ensureScratchSize(numSamples);

    std::copy(left, left + numSamples, dryL_.begin());
    std::copy(right, right + numSamples, dryR_.begin());
    std::copy(dryL_.begin(), dryL_.begin() + numSamples, wetL_.begin());
    std::copy(dryR_.begin(), dryR_.begin() + numSamples, wetR_.begin());

    wetL_.resize(numSamples);
    wetR_.resize(numSamples);

    breakcore_.processInPlace(wetL_);
    breakcore_.processInPlace(wetR_);

    if (advanced_.oversamplingFactor > 1) {
        upsampleLinear(wetL_, advanced_.oversamplingFactor, osL_);
        upsampleLinear(wetR_, advanced_.oversamplingFactor, osR_);
        distortion_.processInPlace(osL_);
        distortion_.processInPlace(osR_);
        antiAliasLowPass(osL_, advanced_.postLowPassHz);
        antiAliasLowPass(osR_, advanced_.postLowPassHz);
        downsampleAverage(osL_, advanced_.oversamplingFactor, wetL_);
        downsampleAverage(osR_, advanced_.oversamplingFactor, wetR_);
    } else {
        distortion_.processInPlace(wetL_);
        distortion_.processInPlace(wetR_);
        antiAliasLowPass(wetL_, advanced_.postLowPassHz);
        antiAliasLowPass(wetR_, advanced_.postLowPassHz);
    }

    const float mix = clamp(macros_.mix, 0.0F, 1.0F);
    const float outGain = 0.2F + clamp(macros_.output, 0.0F, 1.0F) * 1.8F;

    for (std::size_t i = 0; i < numSamples; ++i) {
        left[i] = ((1.0F - mix) * dryL_[i] + mix * wetL_[i]) * outGain;
        right[i] = ((1.0F - mix) * dryR_[i] + mix * wetR_[i]) * outGain;
    }

    if (advanced_.safetyLimiterEnabled) {
        safetyLimit(left, numSamples);
        safetyLimit(right, numSamples);
    }
}

float DrumFxProcessor::clamp(float value, float minValue, float maxValue) {
    return std::min(maxValue, std::max(minValue, value));
}

void DrumFxProcessor::safetyLimit(float* channel, std::size_t numSamples) {
    constexpr float ceiling = 0.98F;
    for (std::size_t i = 0; i < numSamples; ++i) {
        if (std::fabs(channel[i]) > ceiling) {
            channel[i] = (channel[i] > 0.0F ? ceiling : -ceiling);
        }
    }
}

void DrumFxProcessor::ensureScratchSize(std::size_t numSamples) {
    if (dryL_.size() != numSamples) {
        dryL_.resize(numSamples);
        dryR_.resize(numSamples);
        wetL_.resize(numSamples);
        wetR_.resize(numSamples);
    }
}

void DrumFxProcessor::upsampleLinear(const std::vector<float>& input, int factor, std::vector<float>& output) {
    if (factor <= 1) {
        output = input;
        return;
    }

    output.resize(input.size() * static_cast<std::size_t>(factor));
    for (std::size_t i = 0; i < input.size(); ++i) {
        const float a = input[i];
        const float b = (i + 1 < input.size()) ? input[i + 1] : a;
        for (int j = 0; j < factor; ++j) {
            const float t = static_cast<float>(j) / static_cast<float>(factor);
            output[i * static_cast<std::size_t>(factor) + static_cast<std::size_t>(j)] = a + (b - a) * t;
        }
    }
}

void DrumFxProcessor::downsampleAverage(const std::vector<float>& input, int factor, std::vector<float>& output) {
    if (factor <= 1) {
        output = input;
        return;
    }

    const std::size_t outSize = input.size() / static_cast<std::size_t>(factor);
    output.resize(outSize);

    for (std::size_t i = 0; i < outSize; ++i) {
        float sum = 0.0F;
        for (int j = 0; j < factor; ++j) {
            sum += input[i * static_cast<std::size_t>(factor) + static_cast<std::size_t>(j)];
        }
        output[i] = sum / static_cast<float>(factor);
    }
}

void DrumFxProcessor::antiAliasLowPass(std::vector<float>& channel, float cutoffHz) const {
    const float sr = static_cast<float>(sampleRate_ * std::max(1, advanced_.oversamplingFactor));
    const float nyquistSafe = std::min(cutoffHz, sr * 0.45F);
    const float alpha = std::exp(-2.0F * 3.14159265358979323846F * nyquistSafe / sr);

    float state = 0.0F;
    for (float& s : channel) {
        state = alpha * state + (1.0F - alpha) * s;
        s = state;
    }
}

}  // namespace raptorfx
