#include "raptorfx/DistortionEngine.h"

#include <algorithm>
#include <cmath>

namespace raptorfx {

namespace {
constexpr float kPi = 3.14159265358979323846F;

float onePoleLowPass(float x, float yPrev, float cutoffHz, double sampleRate) {
    const float alpha = std::exp(-2.0F * kPi * cutoffHz / static_cast<float>(sampleRate));
    return alpha * yPrev + (1.0F - alpha) * x;
}
}  // namespace

void DistortionEngine::setSampleRate(double sampleRate) {
    sampleRate_ = sampleRate > 8000.0 ? sampleRate : 44100.0;
}

void DistortionEngine::setDamage(float damage) {
    const float normalized = std::clamp(damage, 0.0F, 1.0F);
    drive_ = 1.0F + normalized * 20.0F;
}

void DistortionEngine::setTone(float tone) {
    tone_ = std::clamp(tone, 0.0F, 1.0F);
}

void DistortionEngine::setPunch(float punch) {
    punch_ = std::clamp(punch, 0.0F, 1.0F);
}

void DistortionEngine::processInPlace(std::vector<float>& monoBuffer) const {
    float envelope = 0.0F;
    float lpState = 0.0F;

    const float transientBoost = 1.0F + punch_ * 1.8F;
    const float cutoff = 1200.0F + tone_ * 12000.0F;

    for (float& s : monoBuffer) {
        const float absS = std::fabs(s);
        envelope = 0.995F * envelope + 0.005F * absS;
        const float transient = std::max(0.0F, absS - envelope * 0.7F);

        float x = s * (drive_ + transient * transientBoost);
        x = std::tanh(x);

        lpState = onePoleLowPass(x, lpState, cutoff, sampleRate_);
        const float high = x - lpState;
        s = lpState + high * tone_;
    }
}

}  // namespace raptorfx
