#include "PluginEditor.h"

namespace {
juce::String presetLabel(int row) {
 static const char* categories[] = { "PSY BASS", "ROLLING BASS", "SUB BASS", "TRANCE LEAD", "PSY LEAD", "SUPERSAW", "PLUCK", "PADS", "ARPS", "FX" };
 static const char* prefix[] = { "Neon", "Hyper", "Velvet", "Quantum", "Solar", "Crystal" };
 static const char* suffix[] = { "Circuit", "Pulse", "Motion", "Dream", "Signal" };
 const int category = row / 30, variation = row % 30;
 return juce::String(categories[category]) + "   /   " + prefix[variation / 5] + " " + suffix[variation % 5] + " " + juce::String(variation + 1).paddedLeft('0', 2);
}
}

CosmosXAudioProcessorEditor::CosmosXAudioProcessorEditor(CosmosXAudioProcessor& p)
 : AudioProcessorEditor(&p), processor(p), keyboard(p.keyboardState, juce::MidiKeyboardComponent::horizontalKeyboard), presetModel(*this) {
 setSize(1160, 860);
 title.setText("ASTORIAS", juce::dontSendNotification);
 title.setFont(juce::Font(juce::FontOptions(34.0f, juce::Font::bold | juce::Font::italic)));
 title.setColour(juce::Label::textColourId, juce::Colour(0xfff5f5f6));
 addAndMakeVisible(title);
 subtitle.setText("GALACTIC SYNTHESIS SYSTEM", juce::dontSendNotification);
 subtitle.setFont(juce::Font(juce::FontOptions(10.0f, juce::Font::bold)));
 subtitle.setColour(juce::Label::textColourId, juce::Colour(0xffff3b4f));
 addAndMakeVisible(subtitle);

 auto setup = [this](juce::Slider& s) {
  s.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
  s.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 66, 17);
  s.setColour(juce::Slider::rotarySliderFillColourId, juce::Colour(0xffc7cbd1));
  s.setColour(juce::Slider::rotarySliderOutlineColourId, juce::Colour(0xff535b66));
  s.setColour(juce::Slider::thumbColourId, juce::Colour(0xfff4f5f7));
  s.setColour(juce::Slider::textBoxTextColourId, juce::Colour(0xffe7e9ed));
  s.setColour(juce::Slider::textBoxBackgroundColourId, juce::Colour(0xff101115));
  s.setColour(juce::Slider::textBoxOutlineColourId, juce::Colour(0xff747b84));
  addAndMakeVisible(s);
 };
 for (auto* s : { &master, &cutoff, &resonance, &attack, &decay, &sustain, &release,
                  &oscMix, &subLevel, &detune, &lfoRate, &lfoDepth, &drive }) setup(*s);
 waveform.addItem("SAW",1); waveform.addItem("SQUARE",2); waveform.addItem("SINE",3); waveform.addItem("TRIANGLE",4);
 filterMode.addItem("LOW PASS",1); filterMode.addItem("HIGH PASS",2); filterMode.addItem("BAND PASS",3);
 for (auto* c : { &waveform, &filterMode }) {
  c->setColour(juce::ComboBox::backgroundColourId, juce::Colour(0xff17191e));
  c->setColour(juce::ComboBox::textColourId, juce::Colour(0xfff0f1f3));
  c->setColour(juce::ComboBox::outlineColourId, juce::Colour(0xffaeb4bd));
  addAndMakeVisible(*c);
 }
 auto styleButton = [](juce::TextButton& b) {
  b.setColour(juce::TextButton::buttonColourId, juce::Colour(0xffb8bdc5));
  b.setColour(juce::TextButton::buttonOnColourId, juce::Colour(0xffe0e2e6));
  b.setColour(juce::TextButton::textColourOffId, juce::Colour(0xff202226));
  b.setColour(juce::TextButton::textColourOnId, juce::Colour(0xffffffff));
 };
 styleButton(previousPreset); styleButton(nextPreset); styleButton(keyboardToggle);
 addAndMakeVisible(previousPreset); addAndMakeVisible(nextPreset); addAndMakeVisible(keyboardToggle);
 previousPreset.onClick = [this] { setPreset(selectedPreset <= 1 ? 300 : selectedPreset - 1); };
 nextPreset.onClick = [this] { setPreset(selectedPreset >= 300 ? 1 : selectedPreset + 1); };
 keyboardToggle.onClick = [this] {
  keyboardVisible = !keyboardVisible;
  keyboard.setVisible(keyboardVisible);
  keyboardToggle.setButtonText(keyboardVisible ? "HIDE KEYBOARD" : "SHOW KEYBOARD");
  resized();
 };

 presetBrowser.setModel(&presetModel);
 presetBrowser.setRowHeight(25);
 presetBrowser.setColour(juce::ListBox::backgroundColourId, juce::Colour(0xff101217));
 presetBrowser.setColour(juce::ListBox::outlineColourId, juce::Colour(0xff8e949d));
 presetBrowser.setColour(juce::ListBox::textColourId, juce::Colour(0xffe1e3e7));
 presetBrowser.setOutlineThickness(1);
 addAndMakeVisible(presetBrowser);
 presetBrowser.selectRow(0, juce::dontSendNotification);

 keyboard.setAvailableRange(48,84);
 keyboard.setKeyWidth(27.0f);
 keyboard.setScrollButtonsVisible(false);
 keyboard.setOctaveForMiddleC(4);
 keyboard.setColour(juce::MidiKeyboardComponent::whiteNoteColourId, juce::Colour(0xffe6f0fa));
 keyboard.setColour(juce::MidiKeyboardComponent::blackNoteColourId, juce::Colour(0xff111d32));
 keyboard.setColour(juce::MidiKeyboardComponent::keyDownOverlayColourId, juce::Colour(0xffff3048).withAlpha(0.9f));
 keyboard.setColour(juce::MidiKeyboardComponent::mouseOverKeyOverlayColourId, juce::Colour(0xffff7886).withAlpha(0.4f));
 keyboard.setColour(juce::MidiKeyboardComponent::keySeparatorLineColourId, juce::Colour(0xff626872));
 addAndMakeVisible(keyboard);

 auto& v = processor.parameters;
 aMaster = std::make_unique<SliderAttachment>(v,"gain",master);
 aCutoff = std::make_unique<SliderAttachment>(v,"cutoff",cutoff);
 aResonance = std::make_unique<SliderAttachment>(v,"resonance",resonance);
 aAttack = std::make_unique<SliderAttachment>(v,"attack",attack);
 aDecay = std::make_unique<SliderAttachment>(v,"decay",decay);
 aSustain = std::make_unique<SliderAttachment>(v,"sustain",sustain);
 aRelease = std::make_unique<SliderAttachment>(v,"release",release);
 aOscMix = std::make_unique<SliderAttachment>(v,"oscMix",oscMix);
 aSubLevel = std::make_unique<SliderAttachment>(v,"subLevel",subLevel);
 aDetune = std::make_unique<SliderAttachment>(v,"detune",detune);
 aLfoRate = std::make_unique<SliderAttachment>(v,"lfoRate",lfoRate);
 aLfoDepth = std::make_unique<SliderAttachment>(v,"lfoDepth",lfoDepth);
 aDrive = std::make_unique<SliderAttachment>(v,"drive",drive);
 aWave = std::make_unique<ComboAttachment>(v,"wave",waveform);
 aFilterMode = std::make_unique<ComboAttachment>(v,"filterMode",filterMode);
 loadPreset(1);
}

void CosmosXAudioProcessorEditor::PresetModel::paintListBoxItem(int row, juce::Graphics& g, int width, int height, bool selected) {
 if (selected) {
  g.setColour(juce::Colour(0xff57121c)); g.fillRect(0,0,width,height);
  g.setColour(juce::Colour(0xffff344b)); g.fillRect(0,2,3,height-4);
 } else if (row % 2 == 0) {
  g.setColour(juce::Colour(0xff0b2035)); g.fillRect(0,0,width,height);
 }
 g.setColour(selected ? juce::Colour(0xffffffff) : juce::Colour(0xffd2d5da));
 g.setFont(juce::Font(juce::FontOptions(11.5f, selected ? juce::Font::bold : juce::Font::plain)));
 g.drawText(presetLabel(row), 12, 0, width-20, height, juce::Justification::centredLeft, true);
}
void CosmosXAudioProcessorEditor::PresetModel::selectedRowsChanged(int row) {
 if (row >= 0) owner.loadPreset(row + 1);
}
void CosmosXAudioProcessorEditor::setPreset(int index) {
 selectedPreset = juce::jlimit(1,300,index);
 presetBrowser.selectRow(selectedPreset-1, juce::sendNotification);
 presetBrowser.scrollToEnsureRowIsOnscreen(selectedPreset-1);
}
void CosmosXAudioProcessorEditor::loadPreset(int index) {
 if (index < 1 || index > 300) return;
 selectedPreset = index;
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
 auto b = getLocalBounds().toFloat();
 g.fillAll(juce::Colour(0xff08090c));
 juce::ColourGradient bg(juce::Colour(0xff241015),0,0,juce::Colour(0xff09090d),(float)getWidth(),(float)getHeight(),true);
 bg.addColour(0.48,juce::Colour(0xff180d12)); g.setGradientFill(bg); g.fillRoundedRectangle(b.reduced(6),20);
 for (int i=0;i<105;++i) {
  float x=(float)((i*173+37)%(getWidth()-20))+10, y=(float)((i*97+19)%(getHeight()-15))+5;
  g.setColour(juce::Colour(0xffff6878).withAlpha(i%5==0?0.55f:0.16f));
  g.fillEllipse(x,y,(i%9==0)?2.8f:1.3f,(i%9==0)?2.8f:1.3f);
 }
 // Nebula clouds and orbital arcs
 g.setColour(juce::Colour(0xff9d1e32).withAlpha(0.12f)); g.fillEllipse(360,120,430,390);
 g.setColour(juce::Colour(0xffff263f).withAlpha(0.13f)); g.drawEllipse(325,112,500,410,2.0f);
 g.setColour(juce::Colour(0xffff3048).withAlpha(0.65f)); g.drawRoundedRectangle(b.reduced(7),18,1.2f);
 auto panel=[&g](float x,float y,float w,float h) {
  g.setColour(juce::Colour(0xff07182b).withAlpha(0.96f)); g.fillRoundedRectangle(x,y,w,h,13);
  g.setColour(juce::Colour(0xff87313d).withAlpha(0.8f)); g.drawRoundedRectangle(x,y,w,h,13,1.0f);
 };
 panel(20,96,270,520); panel(300,96,555,520); panel(865,96,275,520);
 // ASTORIAS emblem / small spacecraft silhouette
 juce::Path ship;
 ship.startNewSubPath(1010,26); ship.lineTo(1032,47); ship.lineTo(1022,45); ship.lineTo(1010,56);
 ship.lineTo(998,45); ship.lineTo(988,47); ship.closeSubPath();
 g.setColour(juce::Colour(0xffff3b50).withAlpha(0.9f)); g.fillPath(ship);
 g.setColour(juce::Colour(0xffff5668)); g.drawLine(1004,55,1010,64,2.0f); g.drawLine(1016,55,1010,64,2.0f);
 g.setColour(juce::Colour(0xffff5264)); g.setFont(juce::Font(juce::FontOptions(11.0f,juce::Font::bold)));
 g.drawText("OSCILLATOR BAY",36,110,220,18,juce::Justification::centredLeft);
 g.drawText("ASTORIAS PATCH NAVIGATOR",320,110,360,18,juce::Justification::centredLeft);
 g.drawText("MODULATION / FILTER",885,110,220,18,juce::Justification::centredLeft);
 g.setColour(juce::Colour(0xffc1c4ca)); g.setFont(juce::Font(juce::FontOptions(9.5f,juce::Font::bold)));
 g.drawText("WAVEFORM",34,142,100,15,juce::Justification::centred);
 g.drawText("OSC MIX",35,225,105,15,juce::Justification::centred);
 g.drawText("SUB LEVEL",165,225,105,15,juce::Justification::centred);
 g.drawText("DETUNE",35,360,105,15,juce::Justification::centred);
 g.drawText("DRIVE",165,360,105,15,juce::Justification::centred);
 g.drawText("CUTOFF",875,145,80,15,juce::Justification::centred);
 g.drawText("RESONANCE",975,145,130,15,juce::Justification::centred);
 g.drawText("LFO RATE",875,255,110,15,juce::Justification::centred);
 g.drawText("LFO DEPTH",1000,255,110,15,juce::Justification::centred);
 g.drawText("ATTACK",875,365,75,15,juce::Justification::centred);
 g.drawText("DECAY",965,365,75,15,juce::Justification::centred);
 g.drawText("SUSTAIN",875,475,85,15,juce::Justification::centred);
 g.drawText("RELEASE",975,475,85,15,juce::Justification::centred);
 g.setColour(juce::Colour(0xffff3048)); g.setFont(juce::Font(juce::FontOptions(24.0f,juce::Font::bold | juce::Font::italic)));
 g.setColour(juce::Colour(0xffd2d5da)); g.setFont(juce::Font(juce::FontOptions(9.5f)));
 g.drawText("GALACTIC SYNTHESIS SYSTEM   /   32 VOICES",36,57,400,18,juce::Justification::centredLeft);
 g.drawText("MASTER OUTPUT",165,485,105,15,juce::Justification::centred);
}
void CosmosXAudioProcessorEditor::resized() {
 title.setBounds(30,12,330,44); subtitle.setBounds(36,54,400,20);
 previousPreset.setBounds(320,570,42,30); nextPreset.setBounds(795,570,42,30);
 keyboardToggle.setBounds(965,15,165,32);
 waveform.setBounds(55,160,200,27);
 oscMix.setBounds(35,242,105,105); subLevel.setBounds(165,242,105,105);
 detune.setBounds(35,377,105,105); drive.setBounds(165,377,105,105);
 cutoff.setBounds(870,164,95,92); resonance.setBounds(985,164,120,92);
 lfoRate.setBounds(870,290,105,92); lfoDepth.setBounds(995,290,105,92);
 attack.setBounds(870,400,90,92); decay.setBounds(980,400,90,92);
 sustain.setBounds(870,505,90,92); release.setBounds(980,505,90,92);
 filterMode.setBounds(905,255,170,26);
 master.setBounds(165,500,105,105);
 presetBrowser.setBounds(320,145,515,405);
 keyboard.setVisible(keyboardVisible);
 keyboard.setBounds(24,710,1112,125);
 if (keyboardVisible) keyboard.setBounds(24,710,1112,125);
 else keyboard.setBounds(24,710,1112,1);
}
