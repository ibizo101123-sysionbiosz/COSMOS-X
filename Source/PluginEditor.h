#pragma once
#include <JuceHeader.h>
#include "PluginProcessor.h"

class CosmosXAudioProcessorEditor final : public juce::AudioProcessorEditor {
public:
 explicit CosmosXAudioProcessorEditor(CosmosXAudioProcessor&);
 void paint(juce::Graphics&) override;
 void resized() override;
private:
 class PresetModel final : public juce::ListBoxModel {
 public:
  explicit PresetModel(CosmosXAudioProcessorEditor& e) : owner(e) {}
  int getNumRows() override { return 300; }
  void paintListBoxItem(int row, juce::Graphics&, int width, int height, bool selected) override;
  void selectedRowsChanged(int row) override;
 private: CosmosXAudioProcessorEditor& owner;
 };
 CosmosXAudioProcessor& processor;
 juce::MidiKeyboardComponent keyboard;
 juce::Slider master, cutoff, resonance, attack, decay, sustain, release;
 juce::Slider oscMix, subLevel, detune, lfoRate, lfoDepth, drive;
 juce::ComboBox waveform, filterMode;
 juce::TextButton previousPreset { "<" }, nextPreset { ">" }, keyboardToggle { "HIDE KEYBOARD" };
 juce::ListBox presetBrowser;
 PresetModel presetModel;
 juce::Label title, subtitle;
 bool keyboardVisible = true;
 int selectedPreset = 1;
 using SliderAttachment = juce::AudioProcessorValueTreeState::SliderAttachment;
 using ComboAttachment = juce::AudioProcessorValueTreeState::ComboBoxAttachment;
 std::unique_ptr<SliderAttachment> aMaster, aCutoff, aResonance, aAttack, aDecay, aSustain, aRelease;
 std::unique_ptr<SliderAttachment> aOscMix, aSubLevel, aDetune, aLfoRate, aLfoDepth, aDrive;
 std::unique_ptr<ComboAttachment> aWave, aFilterMode;
 void loadPreset(int index);
 void setPreset(int index);
 JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(CosmosXAudioProcessorEditor)
};
