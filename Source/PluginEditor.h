#pragma once
#include <JuceHeader.h>
#include "PluginProcessor.h"
class CosmosXAudioProcessorEditor final:public juce::AudioProcessorEditor{
public: explicit CosmosXAudioProcessorEditor(CosmosXAudioProcessor&); void paint(juce::Graphics&)override; void resized()override;
private:
 CosmosXAudioProcessor& processor; juce::Slider master,cutoff,attack,decay,sustain,release; juce::ComboBox waveform; juce::Label title,subtitle;
 using SA=juce::AudioProcessorValueTreeState::SliderAttachment; using CA=juce::AudioProcessorValueTreeState::ComboBoxAttachment;
 std::unique_ptr<SA> aMaster,aCutoff,aAttack,aDecay,aSustain,aRelease; std::unique_ptr<CA> aWave;
 JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(CosmosXAudioProcessorEditor)
};