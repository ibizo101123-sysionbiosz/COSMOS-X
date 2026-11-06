#include "PluginProcessor.h"
#include "PluginEditor.h"

namespace {
class AstoriasSound final : public juce::SynthesiserSound {
public:
 bool appliesToNote(int) override { return true; }
 bool appliesToChannel(int) override { return true; }
};
}

class CosmosXAudioProcessor::CosmosVoice final : public juce::SynthesiserVoice {
public:
 explicit CosmosVoice(CosmosXAudioProcessor& p) : processor(p) {}
 bool canPlaySound(juce::SynthesiserSound* s) override { return dynamic_cast<AstoriasSound*>(s) != nullptr; }
 void startNote(int note, float velocity, juce::SynthesiserSound*, int) override {
  frequency = juce::MidiMessage::getMidiNoteInHertz(note);
  level = velocity * 0.22f;
  phase1 = phase2 = phaseSub = lfoPhase = 0.0;
  low = band = 0.0f;
  envelope.noteOn();
 }
 void stopNote(float, bool allowTailOff) override {
  envelope.noteOff();
  if (!allowTailOff) clearCurrentNote();
 }
 void pitchWheelMoved(int) override {}
 void controllerMoved(int, int) override {}
 void setSampleRateForVoice(double sr) { envelope.setSampleRate(sr); }

 void renderNextBlock(juce::AudioBuffer<float>& buffer, int start, int samples) override {
  if (samples <= 0 || !isVoiceActive()) return;
  const double sr = juce::jmax(1.0, getSampleRate());
  auto p = [this](const char* id) { return processor.parameters.getRawParameterValue(id)->load(); };
  juce::ADSR::Parameters adsr { p("attack"), p("decay"), p("sustain"), p("release") };
  envelope.setParameters(adsr);
  const int wave = (int)p("wave");
  const int mode = (int)p("filterMode");
  const float mix = p("oscMix");
  const float subLevel = p("subLevel");
  const float detune = p("detune");
  const float drive = p("drive");
  const float resonance = p("resonance");
  const float lfoRate = p("lfoRate");
  const float lfoDepth = p("lfoDepth");
  const float baseCutoff = p("cutoff");

  for (int i = 0; i < samples; ++i) {
   const float lfo = (float)std::sin(lfoPhase * juce::MathConstants<double>::twoPi);
   lfoPhase += lfoRate / sr;
   if (lfoPhase >= 1.0) lfoPhase -= 1.0;
   const float cutoff = juce::jlimit(30.0f, 18000.0f, baseCutoff * std::pow(2.0f, lfo * lfoDepth * 4.0f));
   const float f = juce::jlimit(0.001f, 0.92f, 2.0f * std::sin(juce::MathConstants<float>::pi * cutoff / (float)sr));
   const float q = juce::jlimit(0.08f, 1.8f, 1.0f - resonance * 0.94f);

   auto osc = [wave](double phase) -> float {
    if (wave == 1) return phase < 0.5 ? 1.0f : -1.0f;
    if (wave == 2) return (float)std::sin(phase * juce::MathConstants<double>::twoPi);
    if (wave == 3) return phase < 0.5 ? (float)(-1.0 + phase * 4.0) : (float)(3.0 - phase * 4.0);
    return (float)(2.0 * phase - 1.0);
   };
   const float a = osc(phase1);
   const float b = osc(phase2);
   const float sub = (float)std::sin(phaseSub * juce::MathConstants<double>::twoPi);
   phase1 += frequency / sr;
   phase2 += frequency * std::pow(2.0, detune / 1200.0) / sr;
   phaseSub += (frequency * 0.5) / sr;
   if (phase1 >= 1.0) phase1 -= 1.0;
   if (phase2 >= 1.0) phase2 -= 1.0;
   if (phaseSub >= 1.0) phaseSub -= 1.0;

   float input = (a * (1.0f - mix) + b * mix) * 0.72f + sub * subLevel * 0.5f;
   input = std::tanh(input * (1.0f + drive * 14.0f));
   low += f * band;
   const float high = input - low - q * band;
   band += f * high;
   float filtered = mode == 1 ? high : (mode == 2 ? band : low);
   const float sample = filtered * envelope.getNextSample() * level * p("gain");
   for (int ch = 0; ch < buffer.getNumChannels(); ++ch)
    buffer.addSample(ch, start + i, sample);
  }
  if (!envelope.isActive()) clearCurrentNote();
 }
private:
 CosmosXAudioProcessor& processor;
 juce::ADSR envelope;
 double phase1 = 0.0, phase2 = 0.0, phaseSub = 0.0, lfoPhase = 0.0, frequency = 440.0;
 float level = 0.0f, low = 0.0f, band = 0.0f;
};

CosmosXAudioProcessor::CosmosXAudioProcessor()
 : AudioProcessor(BusesProperties().withOutput("Output", juce::AudioChannelSet::stereo(), true)),
   parameters(*this, nullptr, "ASTORIAS_STATE", createParameterLayout()) {
 for (int i = 0; i < 32; ++i) synth.addVoice(new CosmosVoice(*this));
 synth.addSound(new AstoriasSound());
}

juce::AudioProcessorValueTreeState::ParameterLayout CosmosXAudioProcessor::createParameterLayout() {
 using Float = juce::AudioParameterFloat;
 std::vector<std::unique_ptr<juce::RangedAudioParameter>> p;
 p.push_back(std::make_unique<Float>("gain", "Master", 0.0f, 1.0f, 0.72f));
 p.push_back(std::make_unique<Float>("cutoff", "Cutoff", juce::NormalisableRange<float>(30.0f, 18000.0f, 1.0f, 0.32f), 14000.0f));
 p.push_back(std::make_unique<Float>("resonance", "Resonance", 0.0f, 1.0f, 0.12f));
 p.push_back(std::make_unique<Float>("attack", "Attack", juce::NormalisableRange<float>(0.001f, 5.0f, 0.001f, 0.35f), 0.01f));
 p.push_back(std::make_unique<Float>("decay", "Decay", juce::NormalisableRange<float>(0.01f, 5.0f, 0.001f, 0.35f), 0.25f));
 p.push_back(std::make_unique<Float>("sustain", "Sustain", 0.0f, 1.0f, 0.8f));
 p.push_back(std::make_unique<Float>("release", "Release", juce::NormalisableRange<float>(0.01f, 8.0f, 0.001f, 0.35f), 0.3f));
 p.push_back(std::make_unique<Float>("oscMix", "Osc Mix", 0.0f, 1.0f, 0.35f));
 p.push_back(std::make_unique<Float>("subLevel", "Sub", 0.0f, 1.0f, 0.25f));
 p.push_back(std::make_unique<Float>("detune", "Detune", -30.0f, 30.0f, 7.0f));
 p.push_back(std::make_unique<Float>("lfoRate", "LFO Rate", 0.05f, 20.0f, 2.0f));
 p.push_back(std::make_unique<Float>("lfoDepth", "LFO Depth", 0.0f, 1.0f, 0.0f));
 p.push_back(std::make_unique<Float>("drive", "Drive", 0.0f, 1.0f, 0.08f));
 p.push_back(std::make_unique<juce::AudioParameterChoice>("wave", "Waveform", juce::StringArray { "Saw", "Square", "Sine", "Triangle" }, 0));
 p.push_back(std::make_unique<juce::AudioParameterChoice>("filterMode", "Filter Type", juce::StringArray { "Low Pass", "High Pass", "Band Pass" }, 0));
 return { p.begin(), p.end() };
}

void CosmosXAudioProcessor::prepareToPlay(double sr, int) {
 synth.setCurrentPlaybackSampleRate(sr);
 for (int i = 0; i < synth.getNumVoices(); ++i)
  if (auto* voice = dynamic_cast<CosmosVoice*>(synth.getVoice(i))) voice->setSampleRateForVoice(sr);
}
void CosmosXAudioProcessor::releaseResources() {}
bool CosmosXAudioProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const {
 const auto output = layouts.getMainOutputChannelSet();
 return output == juce::AudioChannelSet::mono() || output == juce::AudioChannelSet::stereo();
}
void CosmosXAudioProcessor::processBlock(juce::AudioBuffer<float>& audio, juce::MidiBuffer& midi) {
 juce::ScopedNoDenormals noDenormals;
 audio.clear();
 keyboardState.processNextMidiBuffer(midi, 0, audio.getNumSamples(), true);
 synth.renderNextBlock(audio, midi, 0, audio.getNumSamples());
}
juce::AudioProcessorEditor* CosmosXAudioProcessor::createEditor() { return new CosmosXAudioProcessorEditor(*this); }
void CosmosXAudioProcessor::getStateInformation(juce::MemoryBlock& dest) {
 auto state = parameters.copyState();
 std::unique_ptr<juce::XmlElement> xml(state.createXml());
 copyXmlToBinary(*xml, dest);
}
void CosmosXAudioProcessor::setStateInformation(const void* data, int size) {
 std::unique_ptr<juce::XmlElement> xml(getXmlFromBinary(data, size));
 if (xml && xml->hasTagName(parameters.state.getType()))
  parameters.replaceState(juce::ValueTree::fromXml(*xml));
}
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter() { return new CosmosXAudioProcessor(); }
