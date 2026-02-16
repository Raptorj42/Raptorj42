#pragma once

#include <random>
#include <vector>

namespace raptorfx {

class BreakcoreEngine {
public:
    void setSampleRate(double sampleRate);
    void setTempoBpm(double bpm);
    void setChaos(float chaos);
    void setRetriggerProbability(float probability);
    void setReverseProbability(float probability);
    void setGateDepth(float depth);

    void processInPlace(std::vector<float>& monoBuffer);

private:
    std::size_t sliceSamples() const;

    double sampleRate_ = 44100.0;
    double bpm_ = 174.0;
    float chaos_ = 0.2F;
    float retriggerProbability_ = 0.12F;
    float reverseProbability_ = 0.05F;
    float gateDepth_ = 0.25F;

    std::mt19937 rng_{1337};
};

}  // namespace raptorfx
