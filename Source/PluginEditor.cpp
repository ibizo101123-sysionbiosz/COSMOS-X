#include "PluginEditor.h"

CosmosXAudioProcessorEditor::CosmosXAudioProcessorEditor(CosmosXAudioProcessor& p)
 : AudioProcessorEditor(&p), processor(p) {
 setSize(1120, 760);
 title.setText("ASTORIAS", juce::dontSendNotification);
 title.setFont(juce::Font(juce::FontOptions(42.0f, juce::Font::bold)));
 title.setColour(juce::Label::textColourId, juce::Colour(0xfff2f7ff));
 addAndMakeVisible(title);
 subtitle.setText("SYNTHESIS SYSTEMS  /  GALACTIC SOUND DIVISION", juce::dontSendNotification);
 subtitle.setFont(juce::Font(juce::FontOptions(11.0f)));
 subtitle.setColour(juce::Label::textColourId, juce::Colour(0xff67e8ff));
 addAndMakeVisible(subtitle);

 auto setup = [this](juce::Slider& s) {
  s.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
  s.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 76, 18);
  s.setColour(juce::Slider::rotarySliderFillColourId, juce::Colour(0xff43dcff));
  s.setColour(juce::Slider::rotarySliderOutlineColourId, juce::Colour(0xff263a5c));
  s.setColour(juce::Slider::thumbColourId, juce::Colour(0xfff2fbff));
  s.setColour(juce::Slider::textBoxTextColourId, juce::Colour(0xffe9f5ff));
  s.setColour(juce::Slider::textBoxBackgroundColourId, juce::Colour(0xff0a1427));
  s.setColour(juce::Slider::textBoxOutlineColourId, juce::Colour(0xff263a5c));
  addAndMakeVisible(s);
 };
 for (auto* s : { &master, &cutoff, &resonance, &attack, &decay, &sustain, &release,
                  &oscMix, &subLevel, &detune, &lfoRate, &lfoDepth, &drive }) setup(*s);

 waveform.addItem("SAW", 1); waveform.addItem("SQUARE", 2);
 waveform.addItem("SINE", 3); waveform.addItem("TRIANGLE", 4);
 filterMode.addItem("LOW PASS", 1); filterMode.addItem("HIGH PASS", 2); filterMode.addItem("BAND PASS", 3);
 for (auto* c : { &waveform, &filterMode, &preset }) {
  c->setColour(juce::ComboBox::backgroundColourId, juce::Colour(0xff0a1427));
  c->setColour(juce::ComboBox::textColourId, juce::Colour(0xffedf8ff));
  c->setColour(juce::ComboBox::outlineColourId, juce::Colour(0xff28718c));
  addAndMakeVisible(*c);
 }
 auto styleNav = [](juce::TextButton& b) {
  b.setColour(juce::TextButton::buttonColourId, juce::Colour(0xff102943));
  b.setColour(juce::TextButton::buttonOnColourId, juce::Colour(0xff15506a));
  b.setColour(juce::TextButton::textColourOffId, juce::Colour(0xff74edff));
  b.setColour(juce::TextButton::textColourOnId, juce::Colour(0xffffffff));
  b.setConnectedEdges(juce::Button::ConnectedOnLeft | juce::Button::ConnectedOnRight);
 };
 styleNav(previousPreset); styleNav(nextPreset);
 addAndMakeVisible(previousPreset); addAndMakeVisible(nextPreset);

 static const char* categories[] = { "PSY BASS", "ROLLING BASS", "SUB BASS", "TRANCE LEAD", "PSY LEAD", "SUPERSAW", "PLUCK", "PADS", "ARPS", "FX" };
 static const char* prefix[] = { "Neon", "Hyper", "Velvet", "Quantum", "Solar", "Crystal" };
 static const char* suffix[] = { "Circuit", "Pulse", "Motion", "Dream", "Signal" };
 for (int category = 0; category < 10; ++category)
  for (int variation = 0; variation < 30; ++variation) {
   const int id = category * 30 + variation + 1;
   const int a = variation / 5, b = variation % 5;
   juce::String name = juce::String(prefix[a]) + " " + suffix[b];
   preset.addItem(juce::String(categories[category]) + "  /  " + name, id);
  }
 preset.setSelectedId(1, juce::dontSendNotification);
 preset.onChange = [this] { loadPreset(preset.getSelectedId()); };
 previousPreset.onClick = [this] {
  int id = preset.getSelectedId(); if (id <= 1) id = 300; else --id;
  preset.setSelectedId(id, juce::sendNotification);
 };
 nextPreset.onClick = [this] {
  int id = preset.getSelectedId(); if (id >= 300) id = 1; else ++id;
  preset.setSelectedId(id, juce::sendNotification);
 };

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
 loadPreset(1);
}

void CosmosXAudioProcessorEditor::loadPreset(int index) {
 if (index < 1 || index > 300) return;
 const int category = (index - 1) / 30;
 const int v = (index - 1) % 30;
 const float t = (float)v / 29.0f;
 const float wobble = 0.5f + 0.5f * std::sin((float)(v + 1) * 1.731f + category * 0.83f);
 const float ripple = 0.5f + 0.5f * std::cos((float)(v + 2) * 1.137f + category * 1.19f);
 float cutoffValue = 1800.0f, resonanceValue = 0.2f, attackValue = 0.01f, decayValue = 0.25f;
 float sustainValue = 0.6f, releaseValue = 0.3f, mixValue = 0.4f, subValue = 0.15f;
 float detuneValue = 5.0f, rateValue = 2.0f, depthValue = 0.1f, driveValue = 0.1f, gainValue = 0.68f;
 int waveValue = v % 4, filterValue = 0;
 switch (category) {
  case 0: cutoffValue=150.0f+v*31.0f; resonanceValue=.12f+wobble*.32f; decayValue=.07f+t*.20f; sustainValue=.04f+ripple*.16f; mixValue=.28f+wobble*.48f; subValue=.45f+ripple*.42f; detuneValue=1.0f+t*9.0f; driveValue=.25f+wobble*.55f; gainValue=.70f; waveValue=0; break;
  case 1: cutoffValue=220.0f+v*48.0f; resonanceValue=.08f+ripple*.30f; decayValue=.09f+wobble*.24f; sustainValue=.08f+ripple*.22f; mixValue=.35f+t*.50f; subValue=.25f+wobble*.48f; detuneValue=2.0f+t*12.0f; driveValue=.18f+ripple*.50f; waveValue=0; break;
  case 2: cutoffValue=65.0f+v*13.0f; resonanceValue=.02f+wobble*.12f; attackValue=.002f+t*.03f; decayValue=.15f+ripple*.55f; sustainValue=.75f+wobble*.23f; releaseValue=.12f+t*.7f; mixValue=.02f+t*.2f; subValue=.75f+wobble*.23f; detuneValue=0; driveValue=.01f+ripple*.12f; waveValue=2; break;
  case 3: cutoffValue=3500.0f+v*420.0f; resonanceValue=.08f+ripple*.28f; attackValue=.005f+t*.12f; decayValue=.25f+wobble*.65f; sustainValue=.45f+ripple*.48f; releaseValue=.25f+t*1.1f; mixValue=.48f+wobble*.48f; subValue=.02f+t*.18f; detuneValue=4.0f+t*24.0f; rateValue=.3f+wobble*8.0f; depthValue=.02f+ripple*.20f; driveValue=.03f+wobble*.20f; waveValue=v%2; break;
  case 4: cutoffValue=700.0f+v*560.0f; resonanceValue=.25f+wobble*.65f; decayValue=.08f+t*.7f; sustainValue=.12f+ripple*.48f; mixValue=.25f+wobble*.68f; detuneValue=3.0f+t*22.0f; rateValue=1.0f+wobble*13.0f; depthValue=.15f+ripple*.75f; driveValue=.2f+wobble*.72f; waveValue=v%4; filterValue=v%3; break;
  case 5: cutoffValue=5000.0f+v*430.0f; resonanceValue=.05f+ripple*.20f; attackValue=.015f+t*.20f; decayValue=.35f+wobble*1.2f; sustainValue=.65f+ripple*.33f; releaseValue=.4f+t*2.0f; mixValue=.65f+wobble*.34f; subValue=.04f+ripple*.22f; detuneValue=8.0f+t*22.0f; rateValue=.1f+wobble*4.0f; depthValue=.01f+ripple*.18f; waveValue=0; break;
  case 6: cutoffValue=1200.0f+v*510.0f; resonanceValue=.12f+wobble*.5f; attackValue=.001f+t*.015f; decayValue=.08f+wobble*.65f; sustainValue=.02f+ripple*.18f; releaseValue=.08f+t*.55f; mixValue=.15f+wobble*.75f; subValue=.01f+ripple*.12f; detuneValue=t*14.0f; rateValue=2.0f+wobble*12.0f; depthValue=.02f+ripple*.32f; driveValue=.04f+wobble*.3f; waveValue=3; break;
  case 7: cutoffValue=900.0f+v*480.0f; resonanceValue=.03f+ripple*.18f; attackValue=.25f+t*3.0f; decayValue=.7f+wobble*3.2f; sustainValue=.45f+ripple*.48f; releaseValue=1.0f+t*6.0f; mixValue=.2f+wobble*.6f; subValue=.02f+ripple*.2f; detuneValue=2.0f+t*18.0f; rateValue=.05f+wobble*2.5f; depthValue=.08f+ripple*.52f; driveValue=.01f+wobble*.15f; waveValue=2; break;
  case 8: cutoffValue=1500.0f+v*520.0f; resonanceValue=.12f+wobble*.4f; attackValue=.001f+t*.03f; decayValue=.08f+wobble*.38f; sustainValue=.05f+ripple*.28f; releaseValue=.08f+t*.45f; mixValue=.3f+wobble*.65f; subValue=.01f+ripple*.16f; detuneValue=1.0f+t*18.0f; rateValue=3.0f+wobble*16.0f; depthValue=.08f+ripple*.55f; driveValue=.04f+wobble*.3f; waveValue=v%4; break;
  default: cutoffValue=350.0f+v*570.0f; resonanceValue=.2f+wobble*.72f; attackValue=.001f+t*.9f; decayValue=.08f+wobble*2.8f; sustainValue=.02f+ripple*.72f; releaseValue=.1f+t*3.5f; mixValue=.1f+wobble*.85f; subValue=.01f+ripple*.4f; detuneValue=2.0f+t*28.0f; rateValue=.5f+wobble*18.0f; depthValue=.2f+ripple*.78f; driveValue=.15f+wobble*.8f; waveValue=v%4; filterValue=v%3; break;
 }
 auto set = [this](const char* id, float value) {
  if (auto* param = processor.parameters.getParameter(id))
   param->setValueNotifyingHost(param->convertTo0to1(value));
 };
 set("cutoff",cutoffValue); set("resonance",resonanceValue); set("attack",attackValue);
 set("decay",decayValue); set("sustain",sustainValue); set("release",releaseValue);
 set("oscMix",mixValue); set("subLevel",subValue); set("detune",detuneValue);
 set("lfoRate",rateValue); set("lfoDepth",depthValue); set("drive",driveValue);
 set("gain",gainValue); set("wave",(float)waveValue); set("filterMode",(float)filterValue);
}

void CosmosXAudioProcessorEditor::paint(juce::Graphics& g) {
 const auto bounds = getLocalBounds().toFloat();
 g.fillAll(juce::Colour(0xff030712));
 juce::ColourGradient bg(juce::Colour(0xff10294a), 0, 0, juce::Colour(0xff050916),
                         (float)getWidth(), (float)getHeight(), true);
 bg.addColour(0.48, juce::Colour(0xff11112b));
 g.setGradientFill(bg);
 g.fillRoundedRectangle(bounds.reduced(7.0f), 22.0f);
 // Subtle futuristic starfield
 for (int i=0;i<58;++i) {
  const float x=(float)((i*173+37)%1080)+15.0f;
  const float y=(float)((i*97+19)%735)+8.0f;
  g.setColour(juce::Colour(0xff6deaff).withAlpha((i%4==0)?0.34f:0.12f));
  g.fillEllipse(x,y,(i%7==0)?2.8f:1.4f,(i%7==0)?2.8f:1.4f);
 }
 g.setColour(juce::Colour(0xff39dfff).withAlpha(0.8f));
 g.drawRoundedRectangle(bounds.reduced(8.0f), 20.0f, 1.3f);
 g.setColour(juce::Colour(0xffa36bff).withAlpha(0.65f));
 g.drawLine(30, 94, (float)getWidth()-30, 94, 1.0f);
 auto panel=[&g](float x,float y,float w,float h) {
  g.setColour(juce::Colour(0xff08162b).withAlpha(0.94f));
  g.fillRoundedRectangle(x,y,w,h,15.0f);
  g.setColour(juce::Colour(0xff2c7697).withAlpha(0.72f));
  g.drawRoundedRectangle(x,y,w,h,15.0f,1.0f);
  g.setColour(juce::Colour(0xff43dfff).withAlpha(0.12f));
  g.drawLine(x+18,y+29,x+w-18,y+29,1.0f);
 };
 panel(22,132,1076,184); panel(22,329,1076,184); panel(22,526,1076,207);
 g.setColour(juce::Colour(0xff7beaff));
 g.setFont(juce::Font(juce::FontOptions(12.0f,juce::Font::bold)));
 g.drawText("01  /  OSCILLATOR MATRIX",40,139,330,18,juce::Justification::left);
 g.drawText("02  /  FILTER & MOTION ENGINE",40,336,360,18,juce::Justification::left);
 g.drawText("03  /  ENVELOPE & OUTPUT",40,533,330,18,juce::Justification::left);
 g.setColour(juce::Colour(0xffaebed8));
 g.setFont(10.5f);
 g.drawText("WAVEFORM",42,165,150,16,juce::Justification::centred);
 g.drawText("OSC MIX",215,165,125,16,juce::Justification::centred);
 g.drawText("SUB LEVEL",390,165,125,16,juce::Justification::centred);
 g.drawText("DETUNE",565,165,125,16,juce::Justification::centred);
 g.drawText("DRIVE",740,165,125,16,juce::Justification::centred);
 g.drawText("CUTOFF",65,358,135,16,juce::Justification::centred);
 g.drawText("RESONANCE",245,358,135,16,juce::Justification::centred);
 g.drawText("FILTER TYPE",425,366,150,16,juce::Justification::centred);
 g.drawText("LFO RATE",600,358,125,16,juce::Justification::centred);
 g.drawText("LFO DEPTH",765,358,125,16,juce::Justification::centred);
 g.drawText("ATTACK",55,552,120,16,juce::Justification::centred);
 g.drawText("DECAY",220,552,120,16,juce::Justification::centred);
 g.drawText("SUSTAIN",385,552,120,16,juce::Justification::centred);
 g.drawText("RELEASE",550,552,120,16,juce::Justification::centred);
 g.drawText("MASTER",735,552,120,16,juce::Justification::centred);
 g.setColour(juce::Colour(0xff8b9dbb));
 g.setFont(10.0f);
 g.drawText("300 PATCHES  /  10 SOUND WORLDS  /  32 VOICES",720,35,360,20,juce::Justification::centredRight);
 g.setColour(juce::Colour(0xff42dfff).withAlpha(0.7f));
 g.drawRoundedRectangle(585,57,495,55,12.0f,1.0f);
 g.setColour(juce::Colour(0xffa36bff).withAlpha(0.45f));
 g.drawLine(35,716,(float)getWidth()-35,716,1.0f);
}
void CosmosXAudioProcessorEditor::resized() {
 title.setBounds(30,15,390,54);
 subtitle.setBounds(36,67,470,18);
 previousPreset.setBounds(600,69,38,30);
 preset.setBounds(642,69,378,30);
 nextPreset.setBounds(1024,69,38,30);
 waveform.setBounds(42,184,150,30);
 oscMix.setBounds(215,185,125,125);
 subLevel.setBounds(390,185,125,125);
 detune.setBounds(565,185,125,125);
 drive.setBounds(740,185,125,125);
 cutoff.setBounds(65,378,135,125);
 resonance.setBounds(245,378,135,125);
 filterMode.setBounds(425,386,150,30);
 lfoRate.setBounds(600,378,125,125);
 lfoDepth.setBounds(765,378,125,125);
 attack.setBounds(55,570,120,105);
 decay.setBounds(220,570,120,105);
 sustain.setBounds(385,570,120,105);
 release.setBounds(550,570,120,105);
 master.setBounds(735,570,120,105);
}
