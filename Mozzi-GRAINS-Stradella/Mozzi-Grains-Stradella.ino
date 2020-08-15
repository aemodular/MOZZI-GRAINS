/*
# 'Mozzi-GRAINS Stradella'
 
an experimental Firmware for the AE Modular GRAINS module by tangible waves, using the Mozzi library: https://sensorium.github.io/Mozzi
The intention of this Firmware is to provide chords, laid down over 3-4 octaves in a similar order as with the chord-buttons on an accordeon, hence the name
   
To set up the environment needed to install this firmware, please refer to the AeManual for GRAINS on the AE Modular Wiki: http://wiki.aemodular.com/pmwiki.php/AeManual/GRAINS
To include the Mozzi library with your sketch please also refer to "Installation" at https://sensorium.github.io/Mozzi.
Documentation on Mozzi can be found here: https://sensorium.github.io/Mozzi/doc/html/index.html

This sketch is ment to provide a simple, chord-based polyphonic module, any of the chords are playable with only one key via CV over IN-3
We are using the so called Stradella system as to be found on accordions to provide our chords
1) C-E-G (C major), 2) G-Eb-C (C minor), 3) Bb-E-C (C dominant 7th) 4) Eb-C-A (C diminished 7th)
Main CV will used to define the basetone of the chord, the octave will decide which type of chords to use
All chords will be in a kind of center range, to be generally usable
To be able to use all chords, be sure to set the +2 Octaves switch to 'on' and turn the Pitch knob all way left
Of course there may be applications where deviation from that rule would be the right thing, though.


Demotrack available here: https://soundcloud.com/taitekatto/mozzi-grains-stradella
 
# Usage of this Firmware with GRAINS:

__Mode-Switch__

 * G/M Switch has to be set to 'M' to make GRAINS work properly with this Mozzi based sketch!
 
__Inputs__
 
* IN1 / Pot1: Tuning of the chord's base-notes, +- 12 notes (one octave) Middle position would be "center-C"
* IN2 / Pot2: Select an optional 4th note: +- 12 notes, relative to current setting of the chord-base note
* IN3:        CV-in to play chords, mapped to notes over 3 1/2 octaves
* Pot3:       Select the audio-character (waveform) of the output (left to right): Saw, Various Triangles, Sinus, Rectangle, distorted Saw-Wave
* A:          Gate for Note-4 (as set via IN2/Pot2 and IN1/Pot1) - note will play if either gate is open or trigger toggled it
* D:          Toggle-trigger for Note-4 (as set via IN2/Pot2 and IN1/Pot1) - note will play if either gate is open or trigger toggled it
              Hint: Gate (via audio-in) will not turn the note off, if the toggle (via 'D') still has set it to active!
              (Please note: 'D' is situated on the right side and would normally be used as an input!)              
__Outputs__

* OUT:        Sound output


__Patching and settings suggestions__

* Play chords via MIDI-notes
* Play chords via SEQ8 or SEQ16, trigger 4th note of Chord via toggle or gate from TOPOGRAF or TRIQ164
* Generate chord-sequences using two LFOs and S&H - one LFO providing the pitches via a triangle, the other LFO selecting the steps via a rectangle
* Try using QUANTIZER to narrow down the number of useful combinations with the setting described above
* Change the pitch of the forth note via CV-in via a sequencer in any of the scenarios above
* Change the envelope of the sound output by "Stradella" by 2VCA, driven by 2ENV or ADSR. 2ENV/ADSR should be triggered by the same event as the chords
* Change the sound of Stradella using a filter, DELAY, MULTIFX and so on
* Use Stradella rhythmically played through LOPAG, instead of filters/VCAs. LOPAG should be triggered by the same event and maybe even the CV as the chords  
* Try using WAVEFOLDER, especially with percussive settings as mentioned above

# Notes:
 
Caution! Use at your own risk (according to GNU General Public License '16. Limitation of Liability')

This program in combination with the hardware it is applied to can produce harsh and loud frequencies that may be of harm to speakers or your ears!
Permanent hearing loss may result from exposure to sound at high volumes. Use as low a volume as possible.

'Mozzi-GRAINS Stradella' an experimental firmware for the AE Modular GRAINS module by tangible waves
   
Copyright (C) 2020  Mathias Brüssel

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
any later version.
 
This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.
 
You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

// --- Include files needed for this sketch ---
#include <MozziGuts.h>                          // --- This is the basic include to be done to enable the Mozzi-Framework ---
#include <Oscil.h>                              // Oscillator template, uses the tables as below
#include <Smooth.h>                             // Smoothing of CV using an infinite impulse response low pass filter
#include <IntMap.h>                             // Faster version of Arduino map()
#include <mozzi_midi.h>                         // Needed to calculate frequencies from note-values, coming in via CV
#include <OverSample.h>                         // We use oversampling for CV in to avoid pitch-glitches  
#include <WaveShaper.h>                         // Used for optional waveshaping of Stradella's sound

// --- Wavetables used to generate the sound of Stradella (all compatible to changing "on the fly" because of identical lenght 2048 ---
#include <tables/saw2048_int8.h>
#include <tables/sin2048_int8.h>
#include <tables/square_no_alias_2048_int8.h>
#include <tables/triangle_dist_cubed_2048_int8.h>
#include <tables/triangle_dist_squared_2048_int8.h>
#include <tables/triangle_hermes_2048_int8.h>
#include <tables/triangle_valve_2_2048_int8.h>
#include <tables/triangle_valve_2048_int8.h>
#include <tables/triangle2048_int8.h>

// --- Waveshaper wavetable ---
#include <tables/waveshape_sigmoid_int8.h>

// --- GRAINS CV ins, outs and Pots, using 'Arduino-PINS' according to: https://wiki.aemodular.com/pmwiki.php/AeManual/GRAINS ---
#define CONTROL_RATE        2048    // Frequency for reading of CV/Pot values of GRAINS

#define CV_POT_IN1          A2
#define CV_POT_IN2          A1
#define CV_POT3             A0
#define CV_IN3              A3
#define CV_AUDIO_IN         A4
#define CV_AUDIO_OUT        9
#define CV_GATE_OUT         8

// --- Oscillator-objects containing wavetables  ---
static Oscil <SAW2048_NUM_CELLS, AUDIO_RATE> synth_wave1(SAW2048_DATA);
static Oscil <SAW2048_NUM_CELLS, AUDIO_RATE> synth_wave2(SAW2048_DATA);
static Oscil <SAW2048_NUM_CELLS, AUDIO_RATE> synth_wave3(SAW2048_DATA);
static Oscil <SAW2048_NUM_CELLS, AUDIO_RATE> synth_wave4(SAW2048_DATA);

// --- Objects to smooth input data from pots or CV via rolling averages ---
static Smooth <int> cv_pot_in1_smoothed(0.65F);  
static Smooth <int> cv_pot_in2_smoothed(0.65F);  
static Smooth <int> cv_pot3_smoothed(0.65F);

// --- Objects for other usesful stuff ---
static OverSample <unsigned int, 1> overSampler; // will give 10+1=11 bits resolution, 0->2046
static WaveShaper <char> wave_shaper(WAVESHAPE_SIGMOID_DATA); 

// --- We use these variables to communicate between updateControl() and updateAudio() (or some helper function[s]) ---
static uint8_t sound_mode = 0;              // Waves to use to procuce the current sound
static bool note_4_active = false;          // Note 4 can be optionally enabled via Gate / Trigger inputs true if toggle or Gate is true

// --- Define the chords as used with the Stradella system, this will be used as offsets to construct our transposable note-combinations ---
const int8_t chords[][3] =  // 1) C-E-G (C major), 2) G-Eb-C (C minor), 3) Bb-E-C (C dominant 7th) 4) Eb-C-A (C diminished 7th) 5) C-E-G# (C augmented triad)
{
  {0,4,7}, {0,3,7}, {-2,4,12}, {-9,0,9}, {0,4,8}
};

// --- The arduino setup() function, initialise the Mozzi-Framework here as well ---
void setup()
{
  pinMode(CV_GATE_OUT, INPUT);               // We will use GRAINS Gate-out via digitalRead() to enable note_5 of chord or not
 
  Serial.begin(115200);
 
  synth_wave1.setFreq(440);
  synth_wave2.setFreq(440);
  synth_wave3.setFreq(440);
  synth_wave4.setFreq(440);

  startMozzi(CONTROL_RATE);
}

// --- After arduino's loop() function is entered, the Mozzi-Framework will take over control of the application
void loop()
{
  audioHook();
}

// --- Helper function to select the current sound ---
void select_synth_wave()
{
static uint8_t last_sound_mode = 0;             // Remember the current wave-type playing

  if( sound_mode == last_sound_mode )           // Slight performance optimisation: check if anything to do here, at all!
    return;
   
  switch(sound_mode)
  {
    case 0:
      synth_wave1.setTable(SAW2048_DATA);
      synth_wave2.setTable(SAW2048_DATA);
      synth_wave3.setTable(SAW2048_DATA);
      synth_wave4.setTable(SAW2048_DATA);
      break;

    case 1:
      synth_wave1.setTable(SIN2048_DATA);
      synth_wave2.setTable(SIN2048_DATA);
      synth_wave3.setTable(SIN2048_DATA);
      synth_wave4.setTable(SIN2048_DATA);
      break;  

   case 2:
      synth_wave1.setTable(TRIANGLE2048_DATA);
      synth_wave2.setTable(TRIANGLE2048_DATA);
      synth_wave3.setTable(TRIANGLE2048_DATA);
      synth_wave4.setTable(TRIANGLE2048_DATA);
      break;
   
    case 3:
      synth_wave1.setTable(TRIANGLE_VALVE_2048_DATA);
      synth_wave2.setTable(TRIANGLE_VALVE_2048_DATA);
      synth_wave3.setTable(TRIANGLE_VALVE_2048_DATA);
      synth_wave4.setTable(TRIANGLE_VALVE_2048_DATA);
      break;
     
    case 4:
      synth_wave1.setTable(TRIANGLE_VALVE_2_2048_DATA);
      synth_wave2.setTable(TRIANGLE_VALVE_2_2048_DATA);
      synth_wave3.setTable(TRIANGLE_VALVE_2_2048_DATA);
      synth_wave4.setTable(TRIANGLE_VALVE_2_2048_DATA);
      break;    

    case 5:
      synth_wave1.setTable(TRIANGLE_DIST_CUBED_2048_DATA);
      synth_wave2.setTable(TRIANGLE_DIST_CUBED_2048_DATA);
      synth_wave3.setTable(TRIANGLE_DIST_CUBED_2048_DATA);
      synth_wave4.setTable(TRIANGLE_DIST_CUBED_2048_DATA);
      break;
       
    case 6:
      synth_wave1.setTable(TRIANGLE_DIST_SQUARED_2048_DATA);
      synth_wave2.setTable(TRIANGLE_DIST_SQUARED_2048_DATA);
      synth_wave3.setTable(TRIANGLE_DIST_SQUARED_2048_DATA);
      synth_wave4.setTable(TRIANGLE_DIST_SQUARED_2048_DATA);
      break;

    case 7:
      synth_wave1.setTable(TRIANGLE_HERMES_2048_DATA);
      synth_wave2.setTable(TRIANGLE_HERMES_2048_DATA);
      synth_wave3.setTable(TRIANGLE_HERMES_2048_DATA);
      synth_wave4.setTable(TRIANGLE_HERMES_2048_DATA);
      break;

    case 8:
      synth_wave1.setTable(SQUARE_NO_ALIAS_2048_DATA);
      synth_wave2.setTable(SQUARE_NO_ALIAS_2048_DATA);
      synth_wave3.setTable(SQUARE_NO_ALIAS_2048_DATA);
      synth_wave4.setTable(SQUARE_NO_ALIAS_2048_DATA);
      break;

    case 9:               // Special case: this last entry will be waveshaped before output, to give a rich distorted / noisy sound
      synth_wave1.setTable(SAW2048_DATA);
      synth_wave2.setTable(SAW2048_DATA);
      synth_wave3.setTable(SAW2048_DATA);
      synth_wave4.setTable(SAW2048_DATA);
      break;
  }
  last_sound_mode = sound_mode;
}  

// --- Here we process all CV data and Pot-settings ---
void updateControl()
{
static int cv_pot_in1, cv_pot_in2, cv_pot3, cv_in3, cv_audio_in, cv_gate_in;    // Variables to store CV/Pot values of GRAINS-module, don't use local variables to speed up calling of updateControl()
static bool audio_gate_was_low = true;    // Gate of 4th note via audio-in
static bool d_gate_was_low = true;        // Gate of 5th note via digital-in ('D')
static uint8_t midival = 0;
static uint8_t current_octave = 0;
static int8_t base_pitch = 0;         // Will be 0 which is C in center-position of IN-1/CV1
static uint8_t note_4_pitch = 0;      // This is an optional 4th note, to be set and triggered relatively to the current base-note via P1/CV-1
static bool note_4_gate = false;      // This is used to set gate for note 4 on/off via trigger via audio-in
static bool note_4_toggle = false;    // This is used to toggle note 4 on/off via trigger 'D' (which is gate-output, normally)
 
  // --- Process pot3 ---
  cv_pot3 = cv_pot3_smoothed.next(mozziAnalogRead(CV_POT3));                
  sound_mode = (uint8_t)(((long)cv_pot3*(int)10)>>10);                  
  select_synth_wave();                // Select one of the preloaded wavetables

  // --- Process CV-IN-3 ("MIDI"-notes) ---
  cv_in3 = mozziAnalogRead(CV_IN3); // Smooth the value for scaling the LFO speeds!
  midival = (uint8_t)(((long)overSampler.next(cv_in3)*(int)42)>>11);    // The AE Modular provides 42 (or 43) notes from 0-1023 or 0-2046 (11bit) with oversampling
  current_octave = midival/12;                                          // Depending on the octave we use major, minor, dominant or diminished 7th chords, inspired by the chordknobs of an accordion base on the "Stradella-system"
  midival = (midival%12)+48;                                            // Get 0-11 as MIDI-note per octave!
 
  // --- Process CV/IN-1 ---
  cv_pot_in1 = cv_pot_in1_smoothed.next(mozziAnalogRead(CV_POT_IN1));
  base_pitch = (int8_t)(((long)cv_pot_in1*(int)25)>>10)-12;      // Shift base-note -12...0...+12

  // --- Process CV/IN-2 ---
  cv_pot_in2 = cv_pot_in2_smoothed.next(mozziAnalogRead(CV_POT_IN2));
  note_4_pitch = (int8_t)(((long)cv_pot_in2*(int)25)>>10)-12+base_pitch;      // Shift 4th note -12...0...+12 relatively to base-note

  // --- Set frequencies for all notes that might play: basic chord with or without added noted depending on trigger ---
  synth_wave1.setFreq((int)mtof(midival+chords[current_octave][0]+base_pitch));
  synth_wave2.setFreq((int)mtof(midival+chords[current_octave][1]+base_pitch));
  synth_wave3.setFreq((int)mtof(midival+chords[current_octave][2]+base_pitch));
  synth_wave4.setFreq((int)mtof(midival+chords[current_octave][0]+note_4_pitch));

  // --- Check for VCA-CV via audio-in ---  
  cv_audio_in = mozziAnalogRead(CV_AUDIO_IN);     // Binary read for note_4 on/off
  if( cv_audio_in > 511 )
    note_4_gate = true;                 // Play 4th note of chord in updateAudio()
  else
    note_4_gate = false;

  // --- Check for new trigger to change the connected soundtables per oscillator ---
  cv_gate_in = digitalRead(CV_GATE_OUT);    
  if( cv_gate_in == HIGH )
  {
    if( d_gate_was_low )
      note_4_toggle = !note_4_toggle;    // Play 4th note of chord in updateAudio()?
    d_gate_was_low = false;
  }
  else
    d_gate_was_low = true;
 
  note_4_active = note_4_toggle || note_4_gate;      // Note 4 will be added to chord if either the gate is open via audio-in or a trigger toggled activation via Gate 'D'
}

// --- Calculation of audio-data has to be done here in Mozzi's audio-callback function ---
int updateAudio()           // This is the main function of the Mozzi-framework for processing of audio-data, it will be called at audio-rate (16384 Hz)
{                           // We do as little work as possible here, because this routine gets called at audio-rate!
static int16_t my_wave = 0;                                        // This is the result of our audio-calculation that will be send to audio-out

  if( note_4_active )       // Construct a 3 or 4-voice chord, with pitches already set by updateControl(), depening on pot/CV settings and/or notes played
    my_wave = (synth_wave1.next()+synth_wave2.next()+synth_wave3.next()+synth_wave4.next())>>2;
  else
    my_wave = (synth_wave1.next()+synth_wave2.next()+synth_wave3.next())>>2;

  if( sound_mode == 9 )     // Pot-3 all way to the right is this special noisy waveshaped sound!
    my_wave = wave_shaper.next((int8_t)my_wave)>>1;

  return(my_wave);
}
