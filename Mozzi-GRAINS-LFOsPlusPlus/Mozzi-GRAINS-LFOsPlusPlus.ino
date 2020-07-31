/*
# 'GRAINS Mozzi LFOs++ (Variable Waveform + Squarewave with crossmodulation, Envelope, WhiteNoise, Random generator') 
  
an experimental Firmware for the AE Modular GRAINS module by tangible waves, using the Mozzi library: https://sensorium.github.io/Mozzi

The intention of this Firmware is to provide two LFOs with various options, especially useful for generative patches.
    
To set up the environment needed to install this firmware, please refer to the AeManual for GRAINS on the AE Modular Wiki: http://wiki.aemodular.com/pmwiki.php/AeManual/GRAINS
To include the Mozzi library with your sketch please also refer to "Installation" at https://sensorium.github.io/Mozzi.
Documentation on Mozzi can be found here: https://sensorium.github.io/Mozzi/doc/html/index.html

Demotrack available here: https://soundcloud.com/taitekatto/mozzi-grains-lfosplusplus
  
# Usage of this Firmware with GRAINS:

__Mode-Switch__ 

 * G/M Switch has to be set to 'M' to make GRAINS work properly with this Mozzi based sketch!
  
__Inputs__ 
  
* IN1 / Pot1: Set Frequency of variable waveform LFO-1 (up to audible range if set to high values)
* IN2 / Pot2: Set Frequency of square-wave LFO-2 (up to audible range if set to high values)
* IN3:        Scaling-factor for speed of LFO-1, use this to get real slow LFOs (functionality turned off if CV==0V or unpatched)
* Pot3:       Waveform of variable waveform LFO / simple envelope if set to min / random values for internal S&H random value if set to max.
* A:          Trigger-input for envelope or S&H random value, only active if LFO-1 is set accordingly, you may use LFO-2 as a trigger or any other Gate-signal

__Outputs__

* OUT:        Variable wave-form LFO/Oscillator out or Envelope/S&H random value if P3 is set to min. or max.
* D:          Square-wave LFO out

__LFO 1 has the following options, to be set via P3__

* Envelope (fast attack, long release), to be triggered via gate on 'A'-input (variable lenght via Frequency of LFO1) 
* FM-tone-generator (Tonecontrol/modulation via IN2 / POT 2, which also sets the frequency of LFO 2)
* Sinus    
* Cosinus, slightly out of phase
* Sawtooth
* Cycling envelope (fast attack, long release)
* White Noise with variable character or to be used as automatic random generator, depending on frequency set by IN-1/Pot1 + optionally IN-3
* Random Generator, to be triggered via gate on 'A'-input (random values may differ depending on frequency set by IN-1/Pot1 + optionally IN-3)

__LFO 2 has the following options, to be set via P3__

* Pulsewidth will be modified by LFO 1, if LFO 1 is set to FM-tone-generator or Sinus
* Squarewave will be selected for all other settings of LFO 1

__Patching/Settings suggestions__

* Trigger the Random Generator with LFO 2, modify the speed of LFO 2 and a filter with the generated random values - this will result in a self-playing patch
* Use White Noise at very slow frequency to constantly modify a filter-frequency
* Use the Envelope in connection with modulation of LFO 1 speed to get Envelopes with varible lenght
* Use the FM-tone-generator at very slow speeds to get odd LFOs
* Use the FM-tone-generator at high speeds to get rich audible tones
* Set LFO 1 to a very slow sinewave, using IN-3, set LFO 2 to slow speed as well and try to find interesting constantly changing PWM-patterns for LFO 2
* Modulate a filter with LFO 1 and LFO 2 at once, try to find interesting combinations of the two in addition

# Notes:
  
Caution! Use at your own risk (according to GNU General Public License '16. Limitation of Liability')

This program in combination with the hardware it is applied to can produce harsh and loud frequencies that may be of harm to speakers or your ears!
Permanent hearing loss may result from exposure to sound at high volumes. Use as low a volume as possible.

'GRAINS Mozzi LFOs++' an experimental firmware for the AE Modular GRAINS module by tangible waves
    
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
#include <RollingAverage.h>                     // Allow smooting of CV/pots by averaging the values detected
#define CONTROL_RATE  1024                      // Frequency for reading of CV/Pot values of GRAINS, we use a higher update-rate here, because we calculate FM/PM in updateControl()

// --- Wavetables used by the oscillators constructing the LFOs later ---
// #include <tables/square_no_alias512_int8.h>     // Square table for LFO (Square wave itself will be output via Gate-out only)
#include <tables/sin256_int8.h>                 // This is our reference-wave to control the square-wave / LFO2
#include <tables/sin8192_int8.h>                // Sine table for LFO
#include <tables/envelop2048_uint8.h>           // Loopable-Envelope in typical "church form"
#include <tables/cosphase2048_int8.h>           // Slightly out of phase cosinus for LFO
#include <tables/saw2048_int8.h>                // Sawtooth wave for LFO
#include <tables/whitenoise8192_int8.h>         // White Noise for S&H and optional wave-output

// --- GRAINS CV ins, outs and Pots, using 'Arduino-PINS' according to: https://wiki.aemodular.com/pmwiki.php/AeManual/GRAINS ---
#define CV_POT_IN1    A2
#define CV_POT_IN2    A1
#define CV_POT3       A0
#define CV_IN3        A3
#define CV_AUDIO_IN   A4
#define CV_AUDIO_OUT  9
#define CV_GATE_OUT   8

// --- Oscillator-objects containing wavetables for the LFOs ---
static Oscil <SIN256_NUM_CELLS, AUDIO_RATE> lfo2SineForSquare(SIN256_DATA);
static Oscil <SIN8192_NUM_CELLS, AUDIO_RATE> lfo1Sine(SIN8192_DATA);
static Oscil <COSPHASE2048_NUM_CELLS, AUDIO_RATE> lfo1Cosphase(COSPHASE2048_DATA);
static Oscil <COSPHASE2048_NUM_CELLS, AUDIO_RATE> lfo2Mod(COSPHASE2048_DATA);       // Modulator of LFO-2 for FM/PM effects ???###
static Oscil <SAW2048_NUM_CELLS, AUDIO_RATE> lfo1Mod(SAW2048_DATA);                 // Modulator of LFO-1 for FM/PM effects
static Oscil <SAW2048_NUM_CELLS, AUDIO_RATE> lfo1Saw(SAW2048_DATA);
static Oscil <ENVELOP2048_NUM_CELLS, AUDIO_RATE> lfo1Envelope(ENVELOP2048_DATA);
static Oscil <WHITENOISE8192_NUM_CELLS, AUDIO_RATE> lfo1WhiteNoise(WHITENOISE8192_DATA);

// --- Objects to smooth input data from pots or CV via rolling averages ---
static RollingAverage <int, 32> cv_pot_in1_average;  
static RollingAverage <int, 32> cv_pot_in2_average;  
static RollingAverage <int, 32> cv_pot3_average;
static RollingAverage <int, 32> cv_in3_average; 

// --- The arduino setup() function, initialise the Mozzi-Framework here as well ---
void setup()
{
  pinMode(CV_GATE_OUT, OUTPUT);               // We will use GRAINS Gate-out to generate a square-wave LFO 
  Serial.begin(115200);
  startMozzi(CONTROL_RATE);                   // Initialize the Mozzi-framework
}

// --- After arduino's loop() function is entered, the Mozzi-Framework will take over control of the application
void loop()
{
  audioHook();                                // Hand over control to the Mozzi-framework!
}

// --- We use these variables to communicate between updateControl() and updateAudio() ---
static int8_t lfo1_select = 0;              // Normally it would be clearer to have these values encapsulated as member-variable of a class, we still use globals to make processing for the ATmega faster!
static int cv_pot_in1, cv_pot_in2, cv_pot3, cv_in3, cv_audio_in;            // variables to store CV/Pot values of GRAINS-module
static float lfo1_freq, lfo1_mod_freq, square_freq = 0;  // LFO-1 frequency,  and also frequency of possible FM/PM modulator of LFO-1 and LFO2 frequency (sqarewave)
static bool new_trigger = false;        // We need this for the Sample and Hold of White Noise  
static Q15n16 next_FM_mod = 0;              // We store values for our FM/PM modulator during update_control() for update_audio() to gain processing time! 

// --- Helper function: supply a means to map floating-point values according to CV / Pot selection ---
float mapfloat(float x, float in_min, float in_max, float out_min, float out_max)
{
 return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

// --- Get next byte from the oscillator's wavetable, depending on the oscillator currently selected ---
int8_t lfo1Wave_next()
{
static int8_t random_val, envelop_val = 0;    // This is the random result of our WhiteNoise->S&H component or a temp-value for our triggered envelope 
  
  switch(lfo1_select)                         // This is connected to Pot3 of the GRAINS and has been set during updateControl() before
  {
    case 0:                                   // Non-looping envelope, to be triggered from a gate-signal via audio-in!
      if( (lfo1Envelope.getPhaseFractional()>>OSCIL_F_BITS) < 360 )   // Our envelope is stored as a wavetable of 0-360 degrees in phase, as a fix point number format, we retrieve the actual phase-angle, 360 meaning end of table
        envelop_val = lfo1Envelope.next()-128;// Wavetable provided with values from 0-255, we have to adjust this for regular audio-out, here.
      else
        envelop_val = -128;                   // Envelope reached its end after trigger: return zero-value
      return envelop_val;  
    case 1:
      return lfo1Sine.phMod(next_FM_mod);     // We apply the value of our FM/PM modulator here, we have calculated this from a sine-wave during update_control() already! 
    case 2:
      return lfo1Sine.next();
    case 3:
      return lfo1Cosphase.next();  
    case 4: 
      return lfo1Saw.next();
    case 5: 
      return lfo1Envelope.next()-128;   // Looping envelope-shape: Wavetable provided with values from 0-255, we have to adjust this for regular audio-out, here.
    case 6: 
      return lfo1WhiteNoise.next();  
    case 7:                             // S&H trigger mode, we only play new value if Gate is high again at analogue input!
      if( new_trigger )
      {
        random_val = lfo1WhiteNoise.next(); 
        new_trigger = false;
      }
      return random_val;                // If we don't have a new random value the previous "held" value will be used for Sample 'n Hold!
    default:
      return lfo1Sine.next();           // We never should reach here - just in case: this is a dummy, as we can do no proper errorhandling
  }
}

// --- Set frequence of lfo1, according to the oscillator currently selected ---
void lfos_setFreq()
{
  // --- Set frequency of LFO2 / squarewave ---
  lfo2SineForSquare.setFreq(square_freq); 

  // --- Set frequency of LFO1, depending of type currently selected and optionally its modulator if FM/PM is activated ---
  switch(lfo1_select)
  {
    case 0:
      lfo1Envelope.setFreq(lfo1_freq);    // In this context sets the lenght of the envelope, which has a short atack and a long decay as defined in the wavetable
      if( new_trigger )                   // We encountered a new high value on audio-in, which in this context is meant to trigger the wavetable-based envelope 
      {     
        lfo1Envelope.setPhase(0);         // Reset the envelope-waveform to its start for singe-trigger-mode, a phase-degree of 0 within the wavetable meaning sample-start
        new_trigger = false;              // Envelope will run once, we wait for a new trigger now to start it again, if required
      }
      break;
    case 1: 
      lfo1Sine.setFreq(lfo1_freq);
      lfo1Mod.setFreq(lfo1_mod_freq);     // FM/PM Modulator for LFO1, frequency to be changed via identical CV/Pot as square-wave LFO-frequency!
      break;
    case 2:
      lfo1Sine.setFreq(lfo1_freq);
      break;
    case 3:
      lfo1Cosphase.setFreq(lfo1_freq);  
      break;
    case 4: 
      lfo1Saw.setFreq(lfo1_freq);
      break;  
    case 5: 
      lfo1Envelope.setFreq(lfo1_freq);
      break; 
    case 6: 
      lfo1WhiteNoise.setFreq(lfo1_freq);
      break;  
    case 7: 
      lfo1WhiteNoise.setFreq(lfo1_freq);
      break;     
    default:
      lfo1Sine.setFreq(lfo1_freq);        // We never should reach here - just in case: this is a dummy, as we can do no proper errorhandling
  }
}

// --- Here we process all CV data and Pot-settings ---
void updateControl()
{
static float scaling = 0.0;
static bool gate_was_low = true;

  // --- Select LFO1 waveform via pot3 ---
  cv_pot3 = cv_pot3_average.next(mozziAnalogRead(CV_POT3));
  lfo1_select = map(cv_pot3, 0, 900, 0, 7);       // Select from a set of available LFO-waves, 
                                                  // depending on this variable the correct LFO1 will be handled in updateAudio() as well!
  // --- Apply additional scaling to frequency if required for slower LFOs via CV-IN-3 ---
  cv_in3 = cv_in3_average.next(mozziAnalogRead(CV_IN3)); // Smooth the value for scaling the LFO speeds!
  if( cv_in3 > 6 )                                // If CV / Pot is significantly higher than 0 we apply additional scaling on the speed of both LFOs
    scaling = mapfloat((float)cv_in3, 7.0, 1023.0, 0.1, 0.001);
  else
    scaling = 1.0;                                // Initial scaling factor for both LFOs, no change applied if CV==0 / pot is all way left  

  // --- Base-Frequency for LFO1 (variable waveforms ) via CV/IN-1 ---
  cv_pot_in1 = cv_pot_in1_average.next(mozziAnalogRead(CV_POT_IN1));       // Sample frequency: Read CV/Pot1, values will be between 0 and 1023
  if(cv_pot_in1 < 6)                              // Get rid of jitter at lower end of CV/pot
    cv_pot_in1 = 0;
  lfo1_freq = scaling*mapfloat((float)cv_pot_in1, 0.0, 1023.0, 0.01, 300.0);

  // --- Base-Frequency for LFO2 (sqare) and LFO-1 modulator, in case FM/PM for LFO1 is used via CV/IN-2 ---
  cv_pot_in2 = cv_pot_in2_average.next(mozziAnalogRead(CV_POT_IN2));      // Sample frequency: Read CV/Pot2, values will be between 0 and 1023
  if(cv_pot_in2 < 4)                              // Get rid of jitter at lower end of CV/pot
    cv_pot_in2 = 0;
  square_freq = mapfloat((float)cv_pot_in2, 0.0, 1023.0, 0.001, 80.0);
  lfo1_mod_freq = mapfloat((float)cv_pot_in2, 0.0, 1023.0, 0.1, 800.0 );

  // --- Check for S&H Trigger or single-run envelope trigger and save audio-value (S&H trigger only relevant for "SOU", each trigger gives out a different value from the white-noise wave) via audio-in ---
  cv_audio_in = mozziAnalogRead(CV_AUDIO_IN);     // Already make the audio-CV zero-centered
  if( cv_audio_in > 512 )
  {
    if( gate_was_low )
      new_trigger = true;   // We may have a new trigger for S&H - this will be used and reset in lfo1Wave_next() in S&H mode!
    gate_was_low = false;
  }
  else
    gate_was_low = true;

  // --- Set all required frequencies for the LFOs ---
  lfos_setFreq();           // Sets frequency of active LFO-1 and its possible PM modulator, plus LFO2 (square) - this must be a late call, in case we use FM/PD all CVs must be available already!      

  // --- Check if we have to calculate our FM-modulator, we do this here to save time during updateAudio() although it's mathematically not correct, then ---
  if(lfo1_select == 1)      // Calculate value for FM/PM modulator as well now? This is the second LFO-wavetype to be selected with Pot3                                                                    
    next_FM_mod = float_to_Q15n16( mapfloat((float)lfo1Mod.next(), -128, 127, -0.99, 0.99));    // Note: this has to be done after frequencies got adjusted
}

// --- Calculation of audio-data has to be done here in Mozzi's audio-callback function ---
int updateAudio()           // This is the main function of the Mozzi-framework for processing of audio-data, it will be called at audio-rate (16384 Hz)
{                           // We do as little work as possible here, because this routine gets called at audio-rate!
static int8_t my_wave = 0;

  my_wave = lfo1Wave_next();  // --- calculate LFO1 (variable wave) possibly along with FM/PM modulation, we may need this to modulate out square LFO-2, so we do this first!
  
  if( lfo1_select == 1 || lfo1_select == 2 )    // If we have sinus-waves for LFO-1 modulate the pulse-width with that!
  {
    if(lfo2SineForSquare.next()+my_wave > 0)      // --- update LFO2 (square-wave) on Gate-out (to keep things simple we are using a "dummy-oscillator" to control this) ---
      digitalWrite(CV_GATE_OUT, HIGH);
    else
      digitalWrite(CV_GATE_OUT, LOW);
  }
  else                                          // We don't have a sinus-wave for LFO-1, so use a normal square-wave then at the Gate-out
  {
    if(lfo2SineForSquare.next() > 0)            // --- update LFO2 (square-wave) on Gate-out (to keep things simple we are using a "dummy-oscillator" to control this) ---
      digitalWrite(CV_GATE_OUT, HIGH);
    else
      digitalWrite(CV_GATE_OUT, LOW);
  }
  return(my_wave);  // --- update LFO1 (variable wave) on audio-out, possibly along with FM/PM modulation, depending on the wave-type or even a triggered envelope or a new S&H value ---
}
