#include "PluginEditor.h"

CosmosXAudioProcessorEditor::CosmosXAudioProcessorEditor(CosmosXAudioProcessor& p)
 : AudioProcessorEditor(&p), processor(p) {
 setSize(940, 690);
 title.setText("ASTORIAS", juce::dontSendNotification);
 title.setFont(juce::Font(juce::FontOptions(38.0f, juce::Font::bold)));
 title.setColour(juce::Label::textColourId, juce::Colour(0xffeaf6ff));
 addAndMakeVisible(title);
 subtitle.setText("PROFESSIONAL POLYPHONIC SYNTHESIZER", juce::dontSendNotification);
 subtitle.setColour(juce::Label::textColourId, juce::Colour(0xff73cfff));
 addAndMakeVisible(subtitle);

 auto setup = [this](juce::Slider& s) {
  s.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
  s.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 82, 20);
  s.setColour(juce::Slider::rotarySliderFillColourId, juce::Colour(0xff43c7ff));
  s.setColour(juce::Slider::rotarySliderOutlineColourId, juce::Colour(0xff263e5d));
  s.setColour(juce::Slider::thumbColourId, juce::Colour(0xffe7f8ff));
  s.setColour(juce::Slider::textBoxTextColourId, juce::Colour(0xffe7f3ff));
  s.setColour(juce::Slider::textBoxBackgroundColourId, juce::Colour(0xff101d32));
  s.setColour(juce::Slider::textBoxOutlineColourId, juce::Colour(0xff263e5d));
  addAndMakeVisible(s);
 };
 for (auto* s : { &master, &cutoff, &resonance, &attack, &decay, &sustain, &release,
                  &oscMix, &subLevel, &detune, &lfoRate, &lfoDepth, &drive }) setup(*s);

 waveform.addItem("SAW", 1); waveform.addItem("SQUARE", 2);
 waveform.addItem("SINE", 3); waveform.addItem("TRIANGLE", 4);
 filterMode.addItem("LOW PASS", 1); filterMode.addItem("HIGH PASS", 2); filterMode.addItem("BAND PASS", 3);
 for (auto* c : { &waveform, &filterMode, &preset }) {
  c->setColour(juce::ComboBox::backgroundColourId, juce::Colour(0xff101d32));
  c->setColour(juce::ComboBox::textColourId, juce::Colour(0xffeaf6ff));
  c->setColour(juce::ComboBox::outlineColourId, juce::Colour(0xff315777));
  addAndMakeVisible(*c);
 }
 preset.addItem("INIT", 1);
 preset.addItem("PSY BASS", 2);
 preset.addItem("DEEP SUB", 3);
 preset.addItem("TRANCE LEAD", 4);
 preset.addItem("SUPER SAW", 5);
 preset.addItem("DREAM PAD", 6);
 preset.addItem("PLUCK", 7);
 preset.addItem("DARK ACID", 8);
 preset.addItem("LASER", 9);
 preset.addItem("WARM KEYS", 10);
 preset.setSelectedId(1, juce::dontSendNotification);
 preset.onChange = [this] { loadPreset(preset.getSelectedId()); };

 auto& v = processor.parameters;
 aMaster = std::make_unique<SliderAttachment>(v, "gain", master);
 aCutoff = std::make_unique<SliderAttachment>(v, "cutoff", cutoff);
 aResonance = std::make_unique<SliderAttachment>(v, "resonance", resonance);
 aAttack = std::make_unique<SliderAttachment>(v, "attack", attack);
 aDecay = std::make_unique<SliderAttachment>(v, "decay", decay);
 aSustain = std::make_unique<SliderAttachment>(v, "sustain", sustain);
 aRelease = std::make_unique<SliderAttachment>(v, "release", release);
 aOscMix = std::make_unique<SliderAttachment>(v, "oscMix", oscMix);
 aSubLevel = std::make_unique<SliderAttachment>(v, "subLevel", subLevel);
 aDetune = std::make_unique<SliderAttachment>(v, "detune", detune);
 aLfoRate = std::make_unique<SliderAttachment>(v, "lfoRate", lfoRate);
 aLfoDepth = std::make_unique<SliderAttachment>(v, "lfoDepth", lfoDepth);
 aDrive = std::make_unique<SliderAttachment>(v, "drive", drive);
 aWave = std::make_unique<ComboAttachment>(v, "wave", waveform);
 aFilterMode = std::make_unique<ComboAttachment>(v, "filterMode", filterMode);
}

void CosmosXAudioProcessorEditor::loadPreset(int index) {
 struct Patch { float cutoff, resonance, attack, decay, sustain, release, mix, sub, detune, rate, depth, drive, gain; int wave, filter; };
 static const Patch patches[] = {
  { 14000, .12f, .01f, .25f, .80f, .30f, .35f, .25f, 7, 2, 0, .08f, .72f, 0, 0 },
  { 320, .28f, .001f, .12f, .15f, .08f, .52f, .72f, 9, 1.5f, .04f, .38f, .78f, 0, 0 },
  { 180, .08f, .01f, .30f, .95f, .25f, .15f, .95f, 0, .3f, .01f, .05f, .80f, 2, 0 },
  { 7800, .22f, .01f, .45f, .62f, .55f, .68f, .10f, 12, 5.2f, .10f, .18f, .66f, 0, 0 },
  { 10500, .16f, .08f, .65f, .82f, 1.2f, .78f, .18f, 22, .7f, .05f, .12f, .58f, 0, 0 },
  { 4200, .10f, 1.2f, 2.8f, .72f, 4.5f, .44f, .20f, 5, .18f, .22f, .04f, .62f, 2, 0 },
  { 5200, .36f, .001f, .32f, .08f, .22f, .30f, .18f, 4, 3.0f, .06f, .16f, .72f, 3, 0 },
  { 1400, .82f, .001f, .22f, .30f, .12f, .45f, .42f, 8, 4.0f, .24f, .48f, .70f, 0, 2 },
  { 9000, .42f, .001f, .18f, .12f, .25f, .82f, .10f, 18, 8.0f, .62f, .55f, .65f, 1, 1 },
  { 6200, .08f, .02f, .9f, .66f, 1.6f, .24f, .12f, 3, .25f, .04f, .02f, .68f, 2, 0 }
 };
 if (index < 1 || index > 10) return;
 const auto& patch = patches[index - 1];
 auto set = [this](const char* id, float value) {
  if (auto* param = processor.parameters.getParameter(id))
   param->setValueNotifyingHost(param->convertTo0to1(value));
 };
 set("cutoff", patch.cutoff); set("resonance", patch.resonance);
 set("attack", patch.attack); set("decay", patch.decay); set("sustain", patch.sustain);
 set("release", patch.release); set("oscMix", patch.mix); set("subLevel", patch.sub);
 set("detune", patch.detune); set("lfoRate", patch.rate); set("lfoDepth", patch.depth);
 set("drive", patch.drive); set("gain", patch.gain);
 set("wave", (float)patch.wave); set("filterMode", (float)patch.filter);
}

void CosmosXAudioProcessorEditor::paint(juce::Graphics& g) {
 g.fillAll(juce::Colour(0xff050b16));
 juce::ColourGradient bg(juce::Colour(0xff122b49), 0, 0, juce::Colour(0xff080e1b),
                         (float)getWidth(), (float)getHeight(), false);
 g.setGradientFill(bg);
 g.fillRoundedRectangle(getLocalBounds().toFloat().reduced(8), 18.0f);
 g.setColour(juce::Colour(0xff39bfff));
 g.drawLine(28, 91, (float)getWidth() - 28, 91, 1.2f);
 g.setColour(juce::Colour(0xff183d5c));
 g.drawRoundedRectangle(22, 145, 896, 178, 12, 1.0f);
 g.drawRoundedRectangle(22, 337, 896, 178, 12, 1.0f);
 g.drawRoundedRectangle(22, 529, 896, 143, 12, 1.0f);
 g.setColour(juce::Colour(0xff6ecfff));
 g.setFont(13.0f);
 g.drawText("OSCILLATOR / TONE", 38, 151, 250, 20, juce::Justification::left);
 g.drawText("FILTER / MODULATION", 38, 343, 280, 20, juce::Justification::left);
 g.drawText("AMPLITUDE / OUTPUT", 38, 535, 280, 20, juce::Justification::left);
 g.setColour(juce::Colour(0xff9bb6d0));
 g.setFont(11.0f);
 g.drawText("ASTORIAS  •  32 VOICES  •  VST3", 650, 48, 250, 20, juce::Justification::centredRight);
}
void CosmosXAudioProcessorEditor::resized() {
 title.setBounds(30, 18, 360, 52);
 subtitle.setBounds(34, 65, 380, 20);
 preset.setBounds(650, 65, 250, 28);
 waveform.setBounds(42, 184, 150, 30);
 oscMix.setBounds(215, 185, 125, 125);
 subLevel.setBounds(390, 185, 125, 125);
 detune.setBounds(565, 185, 125, 125);
 drive.setBounds(740, 185, 125, 125);
 cutoff.setBounds(65, 378, 135, 125);
 resonance.setBounds(245, 378, 135, 125);
 filterMode.setBounds(425, 386, 150, 30);
 lfoRate.setBounds(600, 378, 125, 125);
 lfoDepth.setBounds(765, 378, 125, 125);
 attack.setBounds(55, 562, 120, 105);
 decay.setBounds(220, 562, 120, 105);
 sustain.setBounds(385, 562, 120, 105);
 release.setBounds(550, 562, 120, 105);
 master.setBounds(735, 562, 120, 105);
}