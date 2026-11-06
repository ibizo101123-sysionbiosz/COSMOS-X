#include "PluginEditor.h"

CosmosXAudioProcessorEditor::CosmosXAudioProcessorEditor(CosmosXAudioProcessor& p)
 : AudioProcessorEditor(&p), processor(p) {
 setSize(1040, 720);
 title.setText("ASTORIAS", juce::dontSendNotification);
 title.setFont(juce::Font(juce::FontOptions(38.0f, juce::Font::bold)));
 title.setColour(juce::Label::textColourId, juce::Colour(0xffeaf6ff));
 addAndMakeVisible(title);
 subtitle.setText("PROFESSIONAL POLYPHONIC SYNTHESIZER", juce::dontSendNotification);
 subtitle.setColour(juce::Label::textColourId, juce::Colour(0xffd6a85f));
 addAndMakeVisible(subtitle);

 auto setup = [this](juce::Slider& s) {
  s.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
  s.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 82, 20);
  s.setColour(juce::Slider::rotarySliderFillColourId, juce::Colour(0xffd6a85f));
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
  c->setColour(juce::ComboBox::outlineColourId, juce::Colour(0xff80663f));
  addAndMakeVisible(*c);
 }
 static const char* categories[] = { "PSY BASS", "ROLLING BASS", "SUB BASS", "TRANCE LEAD", "PSY LEAD", "SUPERSAW", "PLUCK", "PADS", "ARPS", "FX" };
 static const char* names[10][15] = {
  { "Rolling Core", "Rubber Pulse", "Night Driver", "Acid Roll", "Deep Motion", "Tight Groove", "Neon Bounce", "Dark Engine", "Full Power", "Sub Pressure", "Club Roller", "Phase Runner", "Low Orbit", "Hard Pulse", "Mainfloor" },
  { "Classic Roll", "Wide Roll", "Tight Roll", "Dark Roll", "Bright Roll", "Rubber Roll", "Deep Roll", "Driving Roll", "Liquid Roll", "Punch Roll", "Psy Runner", "Night Roll", "Stereo Roll", "Peak Roll", "Festival Roll" },
  { "Pure Sub", "Deep Foundation", "Warm Sub", "Clean Sine", "Soft Weight", "Dark Floor", "Round Low", "Solid Core", "Mono Anchor", "Low Pulse", "Velvet Sub", "Heavy Ground", "Smooth Low", "Deep Space", "Ultra Low" },
  { "Open Horizon", "Skyline", "Euphoria", "Afterglow", "Wide Emotion", "Silver Gate", "Sunrise", "Higher State", "Blue Moment", "Forever", "Starlight", "Dreamline", "Ascension", "Golden Hour", "Last Light" },
  { "Acid Needle", "Neuro Spark", "Psy Cutter", "Laser Mind", "Twisted Signal", "Vortex", "Shamanic", "Mind Split", "Cyber Tribe", "Sharp Attack", "Night Vision", "Hyperdrive", "Psy Storm", "Digital Spirit", "Outer Signal" },
  { "Massive One", "Wide Screen", "Festival Wall", "Bright Anthem", "Heavenly", "Power Chord", "Ultra Wide", "Big Room", "Sky Force", "Epic Motion", "Radiant", "Colossus", "Open Air", "Infinity", "Stadium" },
  { "Crystal Drop", "Soft Bell", "Short Spark", "Glass Key", "Dream Tap", "Digital Wood", "Bright Note", "Velvet Pluck", "Tiny Star", "Echo Pearl", "Warm Mallet", "Liquid Drop", "Moon Bell", "Pure Attack", "Air Pluck" },
  { "Deep Atmosphere", "Frozen Sky", "Warm Clouds", "Eternal Space", "Soft Motion", "Cinematic Bed", "Blue Nebula", "Distant Light", "Slow Bloom", "Velvet Horizon", "Choir Mist", "Ambient Tide", "Dream Field", "Dark Universe", "Last Embrace" },
  { "Pulse Sequence", "Neon Steps", "Clockwork", "Starlight Run", "Digital Rain", "Fast Orbit", "Crystal Pattern", "Psy Motion", "Bright Circuit", "Night Sequence", "Liquid Steps", "Hyper Pattern", "Silver Arp", "Sky Runner", "Infinite Loop" },
  { "Laser Sweep", "Riser Spark", "Falling Star", "Alien Signal", "Impact Tone", "Cosmic Whoosh", "Glitch Ray", "Space Alarm", "Reverse Beam", "Energy Burst", "Dark Portal", "Psy Shimmer", "Digital Impact", "Stardust", "Final Transmission" }
 };
 for (int category = 0; category < 10; ++category)
  for (int variation = 0; variation < 15; ++variation) {
   const int id = category * 15 + variation + 1;
   preset.addItem(juce::String(categories[category]) + "  /  " + names[category][variation], id);
  }
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
 if (index < 1 || index > 150) return;
 const int category = (index - 1) / 15;
 const int v = (index - 1) % 15;
 struct Patch { float cutoff, resonance, attack, decay, sustain, release, mix, sub, detune, rate, depth, drive, gain; int wave, filter; };
 Patch p {};
 const float t = (float)v / 14.0f;
 switch (category) {
  case 0: p = { 210.0f + v*39.0f, .18f+t*.22f, .001f, .10f+t*.10f, .08f+t*.16f, .07f, .42f+t*.25f, .62f, 3.0f+v*.45f, 1.0f+t*2.0f, .02f, .25f+t*.34f, .76f, 0, 0 }; break;
  case 1: p = { 280.0f + v*45.0f, .12f+t*.20f, .001f, .13f+t*.08f, .12f+t*.12f, .08f, .50f+t*.20f, .48f, 4.0f+v*.35f, 1.5f+t*2.5f, .01f+t*.08f, .20f+t*.32f, .74f, 0, 0 }; break;
  case 2: p = { 90.0f + v*10.0f, .04f+t*.12f, .005f, .20f+t*.30f, .80f+t*.18f, .20f+t*.30f, .05f+t*.18f, .88f, 0.0f, .15f+t*.6f, .0f, .02f+t*.10f, .82f, 2, 0 }; break;
  case 3: p = { 5200.0f + v*480.0f, .12f+t*.22f, .008f, .28f+t*.35f, .55f+t*.28f, .35f+t*.60f, .58f+t*.30f, .08f, 4.0f+v*1.0f, 3.0f+t*5.0f, .04f+t*.18f, .08f+t*.22f, .68f, 0, 0 }; break;
  case 4: p = { 1700.0f + v*850.0f, .30f+t*.48f, .001f, .12f+t*.30f, .22f+t*.40f, .10f+t*.22f, .35f+t*.45f, .08f, 5.0f+v*.9f, 2.0f+t*9.0f, .18f+t*.55f, .28f+t*.55f, .67f, v%2, v%3==0 ? 2 : 0 }; break;
  case 5: p = { 6500.0f + v*650.0f, .08f+t*.18f, .04f, .45f+t*.70f, .72f+t*.22f, .70f+t*1.2f, .75f+t*.23f, .10f, 12.0f+v*1.2f, .25f+t*1.2f, .02f+t*.12f, .04f+t*.18f, .58f, 0, 0 }; break;
  case 6: p = { 2600.0f + v*700.0f, .18f+t*.38f, .001f, .12f+t*.38f, .02f+t*.12f, .10f+t*.35f, .20f+t*.48f, .08f, 2.0f+v*.5f, 2.0f+t*7.0f, .02f+t*.18f, .05f+t*.25f, .70f, 3, 0 }; break;
  case 7: p = { 1800.0f + v*550.0f, .05f+t*.15f, .45f+t*2.2f, 1.0f+t*2.8f, .58f+t*.30f, 1.5f+t*4.0f, .30f+t*.40f, .12f, 2.0f+v*.6f, .08f+t*.8f, .12f+t*.42f, .01f+t*.10f, .62f, 2, 0 }; break;
  case 8: p = { 3200.0f + v*650.0f, .18f+t*.30f, .001f, .10f+t*.22f, .10f+t*.25f, .08f+t*.20f, .35f+t*.48f, .06f, 3.0f+v*.8f, 4.0f+t*12.0f, .10f+t*.38f, .10f+t*.28f, .68f, v%4, 0 }; break;
  default: p = { 900.0f + v*1050.0f, .25f+t*.65f, .001f, .15f+t*1.8f, .05f+t*.55f, .15f+t*2.0f, .25f+t*.70f, .05f+t*.30f, 8.0f+v*1.2f, 5.0f+t*14.0f, .35f+t*.60f, .30f+t*.65f, .62f, v%4, v%3 }; break;
 }
 auto set = [this](const char* id, float value) {
  if (auto* param = processor.parameters.getParameter(id))
   param->setValueNotifyingHost(param->convertTo0to1(value));
 };
 set("cutoff",p.cutoff); set("resonance",p.resonance); set("attack",p.attack);
 set("decay",p.decay); set("sustain",p.sustain); set("release",p.release);
 set("oscMix",p.mix); set("subLevel",p.sub); set("detune",p.detune);
 set("lfoRate",p.rate); set("lfoDepth",p.depth); set("drive",p.drive);
 set("gain",p.gain); set("wave",(float)p.wave); set("filterMode",(float)p.filter);
}

void CosmosXAudioProcessorEditor::paint(juce::Graphics& g) {
 g.fillAll(juce::Colour(0xff050b16));
 juce::ColourGradient bg(juce::Colour(0xff122b49), 0, 0, juce::Colour(0xff080e1b),
                         (float)getWidth(), (float)getHeight(), false);
 g.setGradientFill(bg);
 g.fillRoundedRectangle(getLocalBounds().toFloat().reduced(8), 18.0f);
 g.setColour(juce::Colour(0xffd6a85f));
 g.drawLine(28, 91, (float)getWidth() - 28, 91, 1.2f);
 g.setColour(juce::Colour(0xff183d5c));
 g.drawRoundedRectangle(22, 145, 896, 178, 12, 1.0f);
 g.drawRoundedRectangle(22, 337, 896, 178, 12, 1.0f);
 g.drawRoundedRectangle(22, 529, 896, 143, 12, 1.0f);
 g.setColour(juce::Colour(0xffe6c58a));
 g.setFont(13.0f);
 g.drawText("OSCILLATOR / TONE", 38, 151, 250, 20, juce::Justification::left);
 g.drawText("WAVEFORM", 42, 165, 150, 16, juce::Justification::centred);
 g.drawText("OSC MIX", 215, 165, 125, 16, juce::Justification::centred);
 g.drawText("SUB LEVEL", 390, 165, 125, 16, juce::Justification::centred);
 g.drawText("DETUNE", 565, 165, 125, 16, juce::Justification::centred);
 g.drawText("DRIVE", 740, 165, 125, 16, juce::Justification::centred);
 g.drawText("FILTER / MODULATION", 38, 343, 280, 20, juce::Justification::left);
 g.drawText("CUTOFF", 65, 358, 135, 16, juce::Justification::centred);
 g.drawText("RESONANCE", 245, 358, 135, 16, juce::Justification::centred);
 g.drawText("FILTER TYPE", 425, 366, 150, 16, juce::Justification::centred);
 g.drawText("LFO RATE", 600, 358, 125, 16, juce::Justification::centred);
 g.drawText("LFO DEPTH", 765, 358, 125, 16, juce::Justification::centred);
 g.drawText("AMPLITUDE / OUTPUT", 38, 535, 280, 20, juce::Justification::left);
 g.drawText("ATTACK", 55, 546, 120, 16, juce::Justification::centred);
 g.drawText("DECAY", 220, 546, 120, 16, juce::Justification::centred);
 g.drawText("SUSTAIN", 385, 546, 120, 16, juce::Justification::centred);
 g.drawText("RELEASE", 550, 546, 120, 16, juce::Justification::centred);
 g.drawText("MASTER", 735, 546, 120, 16, juce::Justification::centred);
 g.setColour(juce::Colour(0xff9bb6d0));
 g.setFont(11.0f);
 g.drawText("ASTORIAS  •  SIGNATURE SOUND BANK  •  32 VOICES", 590, 43, 420, 20, juce::Justification::centredRight);
}
void CosmosXAudioProcessorEditor::resized() {
 title.setBounds(30, 18, 360, 52);
 subtitle.setBounds(34, 65, 380, 20);
 preset.setBounds(590, 65, 420, 30);
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