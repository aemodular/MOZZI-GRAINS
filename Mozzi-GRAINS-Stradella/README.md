# 'Mozzi-GRAINS Stradella'
 
an experimental Firmware for the AE Modular GRAINS module by tangible waves, using the Mozzi library: https://sensorium.github.io/Mozzi
The intention of this Firmware is to provide chords, laid down over 5 octaves in a similar order as with the chord-buttons on an accordeon, hence the name
   
To set up the environment needed to install this firmware, please refer to the AeManual for GRAINS on the AE Modular Wiki: http://wiki.aemodular.com/pmwiki.php/AeManual/GRAINS
To include the Mozzi library with your sketch please also refer to "Installation" at https://sensorium.github.io/Mozzi.
Documentation on Mozzi can be found here: https://sensorium.github.io/Mozzi/doc/html/index.html

This sketch is ment to provide a simple, chord-based polyphonic module, any of the chords are playable with only one key via CV over IN-3
We are using the so called Stradella system as to be found on accordions to provide our chords
1) C-E-G (C major), 2) G-Eb-C (C minor), 3) Bb-E-C (C dominant 7th) 4) Eb-C-A (C diminished 7th) and on some instruments 5) C-E-G# (C augmented triad)
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
* IN3:        CV-in to play chords, mapped to notes over 4 1/2 octaves
* Pot3:       Select the audio-character of the output: 0-position: "phase/strings"...
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