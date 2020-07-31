# Mozzi-GRAINS LFOs++ (Variable Waveform + Squarewave with crossmodulation, Envelope, White Noise, Random Generator') 
  
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
