# 'Mozzi-GRAINS Krachmacher' 
  
an experimental Firmware for the AE Modular GRAINS module by tangible waves, using the Mozzi library: https://sensorium.github.io/Mozzi
The intention of this Firmware is to provide noisy tones, useful for all kinds of things, along with optional "VCAs" and a VCF.
    
To set up the environment needed to install this firmware, please refer to the AeManual for GRAINS on the AE Modular Wiki: http://wiki.aemodular.com/pmwiki.php/AeManual/GRAINS
To include the Mozzi library with your sketch please also refer to "Installation" at https://sensorium.github.io/Mozzi.
Documentation on Mozzi can be found here: https://sensorium.github.io/Mozzi/doc/html/index.html

This sketch is an experimental implementation using waveguide technology, as well-known by the Karplus Strong agorithm:
https://en.wikipedia.org/wiki/Karplus%E2%80%93Strong_string_synthesis
It also was inspired by the following publication:
https://www.researchgate.net/publication/330667582_uTubo_-_Development_and_Application_of_an_Alternative_Digital_Musical_Instrument

Demotrack available here: https://soundcloud.com/taitekatto/mozzi-grains-krachmacher
  
# Usage of this Firmware with GRAINS:

__Mode-Switch__ 

 * G/M Switch has to be set to 'M' to make GRAINS work properly with this Mozzi based sketch!
  
__Inputs__ 
  
* IN1 / Pot1: Size of delayline one, changes pitch and timbre
* IN2 / Pot2: Size of delayline two, changes pitch and timbre
* IN3:        Filter cutoff frequency in case if filter is used by routing-setting, optionally controls VCA2 with one routing mode 
* Pot3:       Algorithm- and routing-modes for KM (Krachmacher):  1) KM (fast), 2) KM (slower) 3) KM (slowest), 4) VCA->KM, 5) VCA->KM->VCF, 6) KM->VCF, 7) VCA2->KM->VCA, 8) KM->VCA
* A:          CV for VCA or resonance of filter, depending on routing (resonance only is available in case if VCA is unused!)

* D:          Select Noise-sources for our impulse-sources: a) White+Pink Noise, b) White+White Noise, c) Pink+Pink Noise in "round-robin"-mode
              Please note: 'D' is situated on the right side of GRAINS, but used as a gate/trigger-input in this case! 
              
__Outputs__

* OUT:        Sound output

__Patching and settings suggestions__

* Use with much delay and/or reverb to create "wall of sound"-type ambiences
* Use with a "colouring" module, NYLEFILTER highly recommended!
* Use as percussive sound as input to a VCA and/or Filter with fast attack and release
* Use as percussive sound as input to LOPAG (Low Pass Gate)
* Use a trigger-source like TRIQ164 or TOPOGRAF to change the character of the noise-waves via 'D' (Gate input in this special case)
* Use 2ATT/CV and/or 4ATTMIX to send trigger/gate signal to 'D' by hand (Gateoutput, used as an input with Krachmacher!)
* Use the triggerbutton from 2ENV (short attack!) to to send trigger/gate signal to 'D' by hand (Gateoutput, used as an input with Krachmacher!)
* Use a sequencer like SEQ16 or SEQ8 to change the character of the delaylines and or drive the VCA via IN-3 and/or Audio-in, depending on setting of Pot-3 (routing-mode) 
* Use a sequencer like SEQ16 or SEQ8 to change the VCA-Level, thus resulting in an overall change in soundcharacter via Audio-in and/or IN-3, depending on routing-mode
* Use an LFO to change filtercutoff and/or VCA automatically via IN-3 and/or Audio-in, depending on setting of Pot-3 (routing-mode) 
* Use 2ATT/CV and/or 4ATTMIX to send CV to IN-3 or Audio-In, changing the soundcharacter, depending on setting of Pot-3 (routing-mode) 
* Use a MIDI-keyboard to send CV to IN-3 or Audio-In, changing the soundcharacter, depending on setting of Pot-3 (routing-mode) 
* Combine any of the above or find your own ways to make as much noise as possible (which is what Krachmacher means in German ;-)


# Notes:
  
Caution! Use at your own risk (according to GNU General Public License '16. Limitation of Liability')

This program in combination with the hardware it is applied to can produce harsh and loud frequencies that may be of harm to speakers or your ears!
Permanent hearing loss may result from exposure to sound at high volumes. Use as low a volume as possible.

'Mozzi-GRAINS Krachmacher' an experimental firmware for the AE Modular GRAINS module by tangible waves
    
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