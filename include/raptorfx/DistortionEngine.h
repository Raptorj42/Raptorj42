#pragma once

#include <vector>

namespace raptorfx {

class DistortionEngine {
public:
    void setSampleRate(double sampleRate);
    void setDamage(float damage);
    void setTone(float tone);
    void setPunch(float punch);

    void processInPlace(std::vector<float>& monoBuffer) const;

private:
    double sampleRate_ = 44100.0;
    float drive_ = 2.5F;
    float tone_ = 0.5F;
    float punch_ = 0.3F;
};

}  // namespace raptorfx
