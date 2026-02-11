#include "raptorfx/BreakcoreEngine.h"

#include <algorithm>

namespace raptorfx {

void BreakcoreEngine::setSampleRate(double sampleRate) {
    sampleRate_ = sampleRate > 8000.0 ? sampleRate : 44100.0;
}

void BreakcoreEngine::setTempoBpm(double bpm) {
    bpm_ = bpm > 30.0 ? bpm : 174.0;
}

void BreakcoreEngine::setChaos(float chaos) {
    chaos_ = std::clamp(chaos, 0.0F, 1.0F);
}

void BreakcoreEngine::setRetriggerProbability(float probability) {
    retriggerProbability_ = std::clamp(probability, 0.0F, 1.0F);
}

void BreakcoreEngine::setReverseProbability(float probability) {
    reverseProbability_ = std::clamp(probability, 0.0F, 1.0F);
}

void BreakcoreEngine::setGateDepth(float depth) {
    gateDepth_ = std::clamp(depth, 0.0F, 1.0F);
}

std::size_t BreakcoreEngine::sliceSamples() const {
    const double quarterNoteSeconds = 60.0 / bpm_;
    const double sixteenth = quarterNoteSeconds / 4.0;
    const double scaled = sixteenth * (1.0 - (chaos_ * 0.65));
    return static_cast<std::size_t>(std::max(16.0, scaled * sampleRate_));
}

void BreakcoreEngine::processInPlace(std::vector<float>& monoBuffer) {
    if (monoBuffer.empty()) {
        return;
    }

    std::uniform_real_distribution<float> chance(0.0F, 1.0F);
    const std::size_t slice = sliceSamples();

    for (std::size_t start = 0; start < monoBuffer.size(); start += slice) {
        const std::size_t end = std::min(start + slice, monoBuffer.size());
        const bool retrigger = chance(rng_) < (retriggerProbability_ * (0.5F + chaos_));
        const bool reverse = chance(rng_) < (reverseProbability_ * (0.4F + chaos_));

        if (retrigger && end - start > 8) {
            const std::size_t localLen = end - start;
            const std::size_t repeatLen = std::max<std::size_t>(4, localLen / 4);
            for (std::size_t i = 0; i < localLen; ++i) {
                monoBuffer[start + i] = monoBuffer[start + (i % repeatLen)];
            }
        }

        if (reverse) {
            std::reverse(monoBuffer.begin() + start, monoBuffer.begin() + end);
        }

        const float gateMix = gateDepth_ * chaos_;
        for (std::size_t i = start; i < end; ++i) {
            const float phase = static_cast<float>((i - start) % 32) / 32.0F;
            const float gate = (phase < 0.5F) ? 1.0F : (1.0F - gateMix);
            monoBuffer[i] *= gate;
        }
    }
}

}  // namespace raptorfx
