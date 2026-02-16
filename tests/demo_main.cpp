#include <algorithm>
#include <cmath>
#include <iostream>
#include <vector>

#include "raptorfx/DrumFxProcessor.h"

int main() {
    constexpr double sampleRate = 48000.0;
    constexpr std::size_t numSamples = 48000;

    std::vector<float> left(numSamples, 0.0F);
    std::vector<float> right(numSamples, 0.0F);

    // Simple synthetic drum loop: kick + snare-like bursts.
    for (std::size_t i = 0; i < numSamples; ++i) {
        const float t = static_cast<float>(i) / static_cast<float>(sampleRate);
        if (i % 12000 < 300) {
            left[i] += 0.8F * std::sin(2.0F * 3.14159265F * 55.0F * t) * std::exp(-12.0F * (i % 12000) / 12000.0F);
            right[i] = left[i];
        }
        if (i % 12000 > 6000 && i % 12000 < 6200) {
            const float noise = std::sin(2.0F * 3.14159265F * 3700.0F * t);
            left[i] += 0.35F * noise;
            right[i] += 0.35F * noise;
        }
    }

    raptorfx::DrumFxProcessor processor;
    processor.prepare(sampleRate, 172.0);

    raptorfx::MacroParameters macros;
    macros.damage = 0.72F;
    macros.chaos = 0.67F;
    macros.tone = 0.58F;
    macros.punch = 0.63F;
    macros.mix = 0.75F;
    macros.output = 0.55F;
    processor.setMacros(macros);

    raptorfx::AdvancedParameters advanced;
    advanced.retriggerProbability = 0.28F;
    advanced.reverseProbability = 0.16F;
    advanced.gateDepth = 0.45F;
    processor.setAdvanced(advanced);

    processor.process(left.data(), right.data(), numSamples);

    float peak = 0.0F;
    for (float sample : left) {
        peak = std::max(peak, std::fabs(sample));
    }

    std::cout << "Processed " << numSamples << " samples. Peak(L): " << peak << "\n";
    return peak > 0.0F ? 0 : 1;
}
