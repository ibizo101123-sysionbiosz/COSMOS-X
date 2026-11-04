cmake_minimum_required(VERSION 3.22)
project(COSMOSX VERSION 0.1.0)
include(FetchContent)
FetchContent_Declare(JUCE
 GIT_REPOSITORY https://github.com/juce-framework/JUCE.git
 GIT_TAG 8.0.6
 GIT_SHALLOW TRUE
)
FetchContent_MakeAvailable(JUCE)
juce_add_plugin(COSMOSX
 COMPANY_NAME "COSMOS Audio"
 BUNDLE_ID "audio.cosmosx.instrument"
 PLUGIN_MANUFACTURER_CODE Csmx
 PLUGIN_CODE Csx1
 FORMATS VST3
 PRODUCT_NAME "COSMOS X"
 IS_SYNTH TRUE
 NEEDS_MIDI_INPUT TRUE
 NEEDS_MIDI_OUTPUT FALSE
 IS_MIDI_EFFECT FALSE
 COPY_PLUGIN_AFTER_BUILD FALSE
)
juce_generate_juce_header(COSMOSX)
target_sources(COSMOSX PRIVATE Source/PluginProcessor.cpp Source/PluginProcessor.h Source/PluginEditor.cpp Source/PluginEditor.h)
target_compile_features(COSMOSX PRIVATE cxx_std_17)
target_compile_definitions(COSMOSX PUBLIC JUCE_WEB_BROWSER=0 JUCE_USE_CURL=0 JUCE_VST3_CAN_REPLACE_VST2=0)
target_link_libraries(COSMOSX PRIVATE juce::juce_audio_utils PUBLIC
 juce::juce_recommended_config_flags juce::juce_recommended_lto_flags juce::juce_recommended_warning_flags)
