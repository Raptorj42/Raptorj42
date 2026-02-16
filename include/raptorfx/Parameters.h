#pragma once

namespace raptorfx {

struct MacroParameters {
    float damage = 0.35F;  // 0..1
    float chaos = 0.20F;   // 0..1
    float tone = 0.50F;    // 0..1 (dark->bright)
    float punch = 0.30F;   // 0..1
    float mix = 0.65F;     // 0..1
    float output = 0.80F;  // 0..1 -> linear gain map
};

struct AdvancedParameters {
    float preHighPassHz = 35.0F;
    float postLowPassHz = 16000.0F;
    float retriggerProbability = 0.12F;  // 0..1
    int retriggerSubdivision = 8;        // 1/8 note style chunk
    float reverseProbability = 0.05F;    // 0..1
    float gateDepth = 0.25F;             // 0..1
    int oversamplingFactor = 1;          // 1, 2, 4
    bool safetyLimiterEnabled = true;
};

}  // namespace raptorfx
