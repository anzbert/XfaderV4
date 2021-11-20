
# Low Friction USB Crossfader

## Background

When I came up with the idea for this project, I was learning how to create old school HipHop beats with Ableton Live. Naturally, I needed some proper turntable scratching sounds and vocal cuts to add some flavour to my mix. Since I didn't want to start an expensive record collection, my plan was to to only use digital samples. 

At first, I stuck to manipulating the pitch of my sounds by manually drawing automation curves onto the delay effect in ableton (set to repitch mode). That creates a nice speed up / slow down sound, as if manipulating a record.
Then i added a few prerecorded cut, scratch and vocal sounds in between in between to create the illusion of a skilled DJ at work 😜.

That worked well for a while, but I wanted the real DJ feel! So next thing, I got a cheap turntable and a timecode vinyl for Native Instruments Traktor, so I could scratch over digital samples. The USB Midi Crossfader was logically the final piece to this puzzle!

I had already made various Midi controllers with the Arduino Pro Micro, which was simple enough, since the Micro (or Leonardo) can be natively used as a USB HID device with the [USB MIDI library](https://github.com/arduino-libraries/MIDIUSB).

## Challenges
- A crossfader in the fast cutting mode used for scratching has a very sharp response curve, meaning that it turns off a channel very close to the edges. The inactive channel area (dead zone) is only a few milimeters wide at most. The fader doesn't really "fade", but cut hard from on to off.

- Scratching crossfaders need to have as little resistance as possible, to allow for fast cutting. Basic faders use brushes and carbon tracks and usually require too much force to move. Ultra-low friction faders are harder to find and contactless faders (magnetic, inductive or optical), which are used in professional DJ equipment, are expensive and not easily available.

## Solution

### Hardware
- 1x Aluminium Project Box from JayCar
- 1x Arduino Micro Pro
- 1x Micro USB Cable
- 1x LED
- 1x 220 Ohm Resistor
- 1x Momentary Push Button
- 1x Linear TT Electronics Conductive Plastic Fader (PS-45G)
- 1x Fader Handle
- 2x 5mm long cuts of flyscreen rubber
- Selleys Ezy Glide Spray (PTFE lubricant, not oil based / electronics safe)
- 4x Stick-on rubber feet

My solution required a suprisingly large amount of research into fader datasheets, specifically the "Operating Force" (OF). I found that some conductive plastic faders had little OF at a reasonable price. I settled on the TT Electronics PS-45G, which has an out-of-the-box OF of 50 gf (gram-force), or 0.5 N (Newton).

Once I had the fader, I took it apart and cleaned all the sticky oil off the rails. 
Then I put about a 5mm cut of hollow flyscreen rubber on both rail ends. These function as bumpers and they bypass the physical deadzones at both ends of the fader. After reassembly, I lubricated the fader with the Ezy Glide spray.

The fader was then connected to an analogue pin (exactly like a potentiometer) and the button and LED were both wired to two digital pins on the Arduino.

I used an angle grinder to cut a slot into the top of the project box and then fitted all parts into it with / glue and screws. A drill was necessary to mount the LED and the button.


### Code

The code is basic Arduino C, since the functionality of the fader is very straight-forward:

- The code converts the analogue fader input into midi CC data. The fader curve is adjusted in the DJ software (Traktor, Serato, etc..). This way it can be used like a smooth fader as well.

- Holding the button enters calibration mode. Moving the faders to the desired left and right endpoints and then releasing the button saves the settings in the Arduino's persistent storage.

- A shorter press inverts the fader direction.

- The LED indicates if the fader is currently in calibration mode and blinks when data is being saved to the EEPROM

### Conclusion
Overall the result was great. The fader slides with almost no resistance and cuts like you would expect in Traktor. I am still pretty terrible at scratching, but I don't think that is the faders fault 🤣. Anyway, with lots of trying and some Ableton post-recording magic, the scratches ended up sounding decent and I had lots of fun!

## Links
[Source Code](https://github.com/anzbert/XfaderV4)

[More Information on DJ Faders](http://rasteri.com/wiki/Faders)

[My Soundcloud](https://soundcloud.com/anzbert)

[Extensive Ressource on building Midi Controllers, by the Nerd Musician](https://www.musiconerd.com/)
