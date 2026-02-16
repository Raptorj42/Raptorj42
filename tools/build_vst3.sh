#!/usr/bin/env bash
set -euo pipefail

if [[ $# -lt 1 ]]; then
  echo "Usage: $0 /path/to/JUCE [build-dir]"
  exit 1
fi

JUCE_DIR="$1"
BUILD_DIR="${2:-build-vst3}"

if [[ ! -d "$JUCE_DIR" ]]; then
  echo "Error: JUCE directory not found: $JUCE_DIR"
  exit 2
fi

cmake -S . -B "$BUILD_DIR" \
  -DRAPTORFX_BUILD_DEMO=OFF \
  -DRAPTORFX_BUILD_JUCE_PLUGIN=ON \
  -DJUCE_DIR="$JUCE_DIR"

cmake --build "$BUILD_DIR" --config Release

printf "\nBuild finished. Looking for VST3 artifacts...\n"
find "$BUILD_DIR" -type d -name "*.vst3" -print || true
find "$BUILD_DIR" -type f -name "*.vst3" -print || true
