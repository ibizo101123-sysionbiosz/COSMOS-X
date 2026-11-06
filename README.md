# ASTORIAS

ASTORIAS is a Windows VST3 polyphonic synthesizer for Cubase and other compatible hosts.

The expanded sound-bank build includes:
- 32-voice polyphony
- Two detunable oscillators, plus a sub oscillator
- Saw, square, sine and triangle waveforms
- Low-pass, high-pass and band-pass filter modes
- Resonance and LFO modulation of the filter cutoff
- Drive/saturation control
- ADSR amplitude envelope
- 150 selectable categorized sound patches in the preset browser:
  - Psy Bass (15)
  - Rolling Bass (15)
  - Sub Bass (15)
  - Trance Lead (15)
  - Psy Lead (15)
  - Supersaw (15)
  - Pluck (15)
  - Pads (15)
  - Arps (15)
  - FX-style synth tones (15)
- Dark, gold-accented ASTORIAS interface with an expanded preset selector

Important: these are synth-generated parameter patches, not sampled Nexus libraries. The FX category contains effect-like synth tones; this build does not yet have dedicated delay/reverb effects, wavetable import, a full modulation matrix, or an arpeggiator.

Open **Actions** and wait for the latest **ASTORIAS Windows Build** run to finish successfully. Download the `ASTORIAS_COMPLETE` artifact, extract the ZIP, then run `ASTORIAS_Installer.exe`. The installer places the VST3 in the shared Windows VST3 folder. Restart Cubase and rescan plug-ins.

JUCE 8.0.6 is fetched from the official JUCE repository at build time. Review JUCE licensing before commercial distribution.
