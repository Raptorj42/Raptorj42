cmake_minimum_required(VERSION 3.16)
project(RaptorBreakcoreFX LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

option(RAPTORFX_BUILD_DEMO "Build CLI demo executable" ON)
option(RAPTORFX_BUILD_JUCE_PLUGIN "Build JUCE VST3 plugin target" OFF)

add_library(raptorfx_core
    src/DistortionEngine.cpp
    src/BreakcoreEngine.cpp
    src/DrumFxProcessor.cpp
)

target_include_directories(raptorfx_core PUBLIC include)
target_compile_options(raptorfx_core PRIVATE -Wall -Wextra -Wpedantic)

if(RAPTORFX_BUILD_DEMO)
    add_executable(raptorfx_demo tests/demo_main.cpp)
    target_link_libraries(raptorfx_demo PRIVATE raptorfx_core)
endif()

if(RAPTORFX_BUILD_JUCE_PLUGIN)
    if(NOT DEFINED JUCE_DIR)
        message(FATAL_ERROR "RAPTORFX_BUILD_JUCE_PLUGIN=ON requires -DJUCE_DIR=/path/to/JUCE")
    endif()

    add_subdirectory(${JUCE_DIR} juce)

    juce_add_plugin(RaptorBreakcoreFX
        COMPANY_NAME "Raptorj42"
        IS_SYNTH FALSE
        NEEDS_MIDI_INPUT FALSE
        NEEDS_MIDI_OUTPUT FALSE
        IS_MIDI_EFFECT FALSE
        EDITOR_WANTS_KEYBOARD_FOCUS FALSE
        COPY_PLUGIN_AFTER_BUILD FALSE
        PLUGIN_MANUFACTURER_CODE RptR
        PLUGIN_CODE Rbfx
        FORMATS VST3
        PRODUCT_NAME "Raptor Breakcore FX"
    )

    target_sources(RaptorBreakcoreFX PRIVATE
        plugins/juce/PluginProcessor.cpp
        plugins/juce/PluginProcessor.h
        plugins/juce/PluginEditor.cpp
        plugins/juce/PluginEditor.h
    )

    target_include_directories(RaptorBreakcoreFX PRIVATE include)

    target_link_libraries(RaptorBreakcoreFX
        PRIVATE
            raptorfx_core
            juce::juce_audio_utils
        PUBLIC
            juce::juce_recommended_config_flags
            juce::juce_recommended_lto_flags
            juce::juce_recommended_warning_flags
    )

    target_compile_definitions(RaptorBreakcoreFX PRIVATE
        JUCE_WEB_BROWSER=0
        JUCE_USE_CURL=0
    )
endif()
