/*
  'Lesson 01 Simple Oscillator' (Triangle Wave) 
  a minimalistic Firmware for the AE Modular GRAINS module by tangible waves, using the Mozzi library: https://sensorium.github.io/Mozzi

  The intention of this Firmware is to provide a very basic sketch to build upon if you want to develop your own firmware using Mozzi sketches
    
  To set up the environment needed to install this firmware, please refer to the AeManual for GRAINS on the AE Modular Wiki: http://wiki.aemodular.com/pmwiki.php/AeManual/GRAINS
  To include the Mozzi library with your sketch please also refer to "Installation" at https://sensorium.github.io/Mozzi.
  Documentation on Mozzi can be found here: https://sensorium.github.io/Mozzi/doc/html/index.html
  
  Demotrack available here: https://soundcloud.com/taitekatto/mozzi-simple-example-triangle-wave
  
  Usage of this Firmware with GRAINS:
  Switch 'G'/'M' has to be set to 'M' to work properly with Mozzi based sketches
  ==============================================================================
  IN1 / Pot1: Change frequency of Oscillator
  IN2 / Pot2: [n/a]
  IN3:        [n/a]
  Pot3:       [n/a]
  A:          [n/a]
  
  OUT:        Audio out
  D:          [n/a]

  Notes:
  ------
  Caution! Use at your own risk (according to GNU General Public License '16. Limitation of Liability')
  ----------------------------------------------------------------------------------------------------- 
  This program in combination with the hardware it is applied to can produce harsh and loud frequencies that may be of harm to speakers or your ears!
  Permanent hearing loss may result from exposure to sound at high volumes. Use as low a volume as possible.

  'GRAINS Mozzi Simple Example' (Triangle Wave) an example Firmware for the AE Modular GRAINS module by tangible waves
    
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
// --- This is the basic include to be done to enable the Mozzi-Framework ---
#include <MozziGuts.h>

// --- Add your application-specific stuff to be included here ---
#include <Oscil.h>
#include <tables/triangle_analogue512_int8.h>
Oscil<TRIANGLE_ANALOGUE512_NUM_CELLS, AUDIO_RATE> myTriangleWave(TRIANGLE_ANALOGUE512_DATA);

// --- Below you find parts of the Mozzi-Framwork for initialisation and permanent processing, to be called by the Arduino framework, normally nothing to be changed here ---
void setup()
{
  startMozzi();
}
void loop()
{
  audioHook();
}

// Frequency for reading CV and pot for Mozzi-framework: "use #define for CONTROL_RATE, not a constant, powers of 2 please"
#define CONTROL_RATE  64     

// --- GRAINS CV ins, outs and Pots, using 'Arduino-PINS' according to: https://wiki.aemodular.com/pmwiki.php/AeManual/GRAINS ---
#define CV_POT_IN1    A2
#define CV_POT_IN2    A1
#define CV_POT3       A0
#define CV_IN3        A3
#define CV_AUDIO_IN   A4
#define CV_AUDIO_OUT  D9
#define CV_GATE_OUT   D8


// --- Put any code here where you read CV or potentiometers from your GRAINS ---
static int cv_pot_in1 = 0;                    // Use global variables if you want to exchange data between updateControl() and updateAudio()  
void updateControl()                          // This is the main function of the Mozzi-framework for processing of control-data, it will be called at control-rate (64 Hz)
{
  cv_pot_in1 = mozziAnalogRead(CV_POT_IN1);   // mozziAnalogRead() non-blocking read used instead of Arduino's analogRead() for better audio-perfomance
  int frequency = cv_pot_in1*4 +100;          // Control-Values are from 0-1023, we change this to Frequencies from 100 to 8284 Hz this way.
  myTriangleWave.setFreq(frequency);          // Set the frequency every time we reach here, according to the settin of Pot1 and/or incoming CV on IN-1 
}                                             // Please note: when using Pot3 the frequency may be fluctuating, this can be mitigated by averaging the values read with every call, 
                                              // but was not done here to keep the sketch as simple as possible   

// --- Put any code here where calculation or output of audio-data has to be done for your GRAINS ---
int updateAudio()                             // This is the main function of the Mozzi-framework for processing of audio-data, it will be called at audio-rate (16384 Hz)
{
  return (int)myTriangleWave.next();          // This data will be sent to the audio-output automatically by the Mozzi-Framework, 
                                              // use .next() to get next sample from wavetable with every call at audio-rate
}
