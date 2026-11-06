#pragma once
#include <JuceHeader.h>
#include "PluginProcessor.h"

class CosmosXAudioProcessorEditor final : public juce::AudioProcessorEditor {
public:
 explicit CosmosXAudioProcessorEditor(CosmosXAudioProcessor&);
 void paint(juce::Graphics&) override;
 void resized() override;
private:
 CosmosXAudioProcessor& processor;
 juce::MidiKeyboardComponent keyboard;
 juce::Slider master, cutoff, resonance, attack, decay, sustain, release;
 juce::Slider oscMix, subLevel, detune, lfoRate, lfoDepth, drive;
 juce::ComboBox waveform, filterMode, preset;
 juce::TextButton previousPreset { "<" }, nextPreset { ">" };
 juce::Label title, subtitle;
 using SliderAttachment = juce::AudioProcessorValueTreeState::SliderAttachment;
 using ComboAttachment = juce::AudioProcessorValueTreeState::ComboBoxAttachment;
 std::unique_ptr<SliderAttachment> aMaster, aCutoff, aResonance, aAttack, aDecay, aSustain, aRelease;
 std::unique_ptr<SliderAttachment> aOscMix, aSubLevel, aDetune, aLfoRate, aLfoDepth, aDrive;
 std::unique_ptr<ComboAttachment> aWave, aFilterMode;
 void loadPreset(int index);
 JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(CosmosXAudioProcessorEditor)
};