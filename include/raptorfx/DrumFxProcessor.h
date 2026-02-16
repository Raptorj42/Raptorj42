#pragma once

#include <vector>

#include "raptorfx/BreakcoreEngine.h"
#include "raptorfx/DistortionEngine.h"
#include "raptorfx/Parameters.h"

namespace raptorfx {

class DrumFxProcessor {
public:
    void prepare(double sampleRate, double tempoBpm);
    void setTempoBpm(double tempoBpm);
    void setMacros(const MacroParameters& macros);
    void setAdvanced(const AdvancedParameters& advanced);

    // Expects non-interleaved mono channels for now.
    void process(float* left, float* right, std::size_t numSamples);

private:
    static float clamp(float value, float minValue, float maxValue);
    static void safetyLimit(float* channel, std::size_t numSamples);
    void ensureScratchSize(std::size_t numSamples);

    static void upsampleLinear(const std::vector<float>& input, int factor, std::vector<float>& output);
    static void downsampleAverage(const std::vector<float>& input, int factor, std::vector<float>& output);
    void antiAliasLowPass(std::vector<float>& channel, float cutoffHz) const;

    DistortionEngine distortion_;
    BreakcoreEngine breakcore_;

    MacroParameters macros_{};
    AdvancedParameters advanced_{};

    std::vector<float> dryL_;
    std::vector<float> dryR_;
    std::vector<float> wetL_;
    std::vector<float> wetR_;
    std::vector<float> osL_;
    std::vector<float> osR_;

    double sampleRate_ = 44100.0;
    double tempoBpm_ = 174.0;
};

}  // namespace raptorfx
