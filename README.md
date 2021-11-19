# Low Friction USB Crossfader

![image](https://upload.wikimedia.org/wikipedia/commons/thumb/3/3f/Placeholder_view_vector.svg/200px-Placeholder_view_vector.svg.png)

## Background

When I came up with this project I was playing with making old school HipHop beats in Ableton Live. Naturally, I needed some proper turntable scratches and vocal cuts to add some flavour to the mix. Since I didn't want to start an expensive record collection, my plan was to use digital samples only.

First, I stuck to manipulating the pitch of my sounds, by manually drawing automation curves onto the pitch of the ableton standard delay effect (in pitch mode). Then i added a few prerecorded cut and scratch sounds from a soundpack in between to create the illusion of a skilled DJ at work 😜.

That worked well for a while, but I wanted the real DJ feel! So next thing I got a cheap turntable and a timecode vinyl for Native Instruments Traktor, so I could scratch over digital samples. The USB Midi Crossfader was the final piece of the puzzle!

I had already made Midi controllers with the Arduino Pro Micro at this point, which is simple enough, since the Micro (or Leonardo) can be natively used as a USB HID device with the [USB MIDI library](https://github.com/arduino-libraries/MIDIUSB).

## Challenges

- A crossfader in the fast cutting mode used for scratching has a very sharp response curve, meaning it turns off a channel very close to the edges. The inactive channel area (dead zone) is only a few milimeters wide at most. The fader doesn't really "fade", but cut hard from on to off.

- Scratching crossfaders need to have as little resistance as possible, to allow for fast cutting. Basic faders use brushes and carbon tracks and usually have a lot of movement resistance. Ultra-low friction faders are harder to find and contactless (magnetic, inductive or optical) faders, which are used in professional DJ equipment are expensive and not easily available.

## Solution

### Hardware

- 1x Aluminium Project Box from JayCar
- 1x Arduino Micro Pro
- 1x Micro USB Cable
- 1x LED
- 1x 220 Ohm Resistor
- 1x Momentary Push Button
- 1x TT Electronics Conductive Plastic Fader (PS-45)
- 2x 5mm long cuts of flyscreen rubber
- Selleys Ezy Glide Spray (not oil based / electronics safe)

My solution required a suprisingly large amount of research into fader datasheets, specifically the "Operating Force" (OF), of readily available and affordable sliders. I found that some conductive plastic faders had little OF at a reasonable price.

Once I had the fader, I took it apart and cleaned all the sticky oil off the rails.
Then I put a 5mm cut of hollow flyscreen rubber on both rail ends. These function as bumpers and they physically remove the deadzones at both ends of the fader. After reassembly i lubricated the fader with oil-free Selleys Ezy Glide spray.

The fader was connected to an analogue pin (like a potentiometer) and the button and LED were then both connected to two digital pins on the Arduino.

I used an angle grinder to cut a slot into the project box and then fitted all parts into it with / glue and screws. A drill was necessary to mount the LED and the button.

### Code

The code is basic Arduino C, since the functionality of the fader is very straight forward:

- The code converts the analogue fader input into midi data. The fader curve is adjusted in the DJ software (Traktor, Serato, etc..).

- Holding the button enters calibration mode. Moving the faders to the desired left and right endpoints and then releasing the button saves the settings in the Arduino's persistent storage.

- The LED indicates if the fader is currently in calibration mode and if data has being saved.

## Links

[Source Code](https://github.com/anzbert/XfaderV4)

### More Information

[Information on DJ Faders](http://rasteri.com/wiki/Faders)
[Info on making Midi Controllers, by the Nerd Musician](https://www.musiconerd.com/)
