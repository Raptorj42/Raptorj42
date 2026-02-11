- 👋 Hi, I’m @Raptorj42
- 👀 I’m interested in learning to code
- 🌱 I’m currently learning Python
- 💞️ I’m looking to collaborate on ...
- 📫 How to reach me ...

## VST3 prototype progress: distortion + breakcore drums

This repository now contains a C++ DSP core **and a JUCE plugin wrapper** that can be built as a VST3 package.

### What is implemented
- Macro-focused drum FX core (`Damage`, `Chaos`, `Tone`, `Punch`, `Mix`, `Output`)
- Breakcore processing (retrigger, reverse chance, rhythmic gate)
- Distortion processing (transient-aware drive and tone shaping)
- Safety limiter and dry/wet blending
- JUCE `AudioProcessor` + `AudioProcessorEditor` integration with APVTS parameter mapping
- CMake options for either CLI demo or JUCE VST3 build

### Project structure
- `include/raptorfx/*.h` and `src/*.cpp`: DSP core
- `plugins/juce/PluginProcessor.*`: JUCE host integration + parameter/state handling
- `plugins/juce/PluginEditor.*`: knob-based UI and parameter attachments
- `tests/demo_main.cpp`: standalone DSP sanity executable

### Build the standalone DSP demo
```bash
cmake -S . -B build -DRAPTORFX_BUILD_DEMO=ON
cmake --build build
./build/raptorfx_demo
```

### Build the JUCE VST3 plugin
1. Clone JUCE somewhere locally (example path `~/JUCE`).
2. Run the helper script:

```bash
./tools/build_vst3.sh $HOME/JUCE
```

Or run CMake directly:

```bash
cmake -S . -B build-vst3 \
  -DRAPTORFX_BUILD_DEMO=OFF \
  -DRAPTORFX_BUILD_JUCE_PLUGIN=ON \
  -DJUCE_DIR=$HOME/JUCE
cmake --build build-vst3 --config Release
```

The produced `.vst3` bundle will be in your build output under JUCE's plugin artifact directories (platform-dependent).

### Next improvements for production quality
- Remove per-block heap allocations in `DrumFxProcessor::process`
- Add oversampling and anti-aliasing options
- Expand preset management and A/B workflow in the editor

<!---
Raptorj42/Raptorj42 is a ✨ special ✨ repository because its `README.md` (this file) appears on your GitHub profile.
You can click the Preview link to take a look at your changes.
--->
