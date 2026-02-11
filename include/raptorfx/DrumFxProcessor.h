#pragma once

#include <vector>

#include "raptorfx/BreakcoreEngine.h"
#include "raptorfx/DistortionEngine.h"
#include "raptorfx/Parameters.h"

namespace raptorfx {

class DrumFxProcessor {
public:
    void prepare(double sampleRate, double tempoBpm);
    void setMacros(const MacroParameters& macros);
    void setAdvanced(const AdvancedParameters& advanced);

    // Expects non-interleaved mono channels for now.
    void process(float* left, float* right, std::size_t numSamples);

private:
    static float clamp(float value, float minValue, float maxValue);
    static void safetyLimit(std::vector<float>& channel);

    DistortionEngine distortion_;
    BreakcoreEngine breakcore_;

    MacroParameters macros_{};
    AdvancedParameters advanced_{};

    double sampleRate_ = 44100.0;
    double tempoBpm_ = 174.0;
};

}  // namespace raptorfx
