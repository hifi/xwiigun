# xwiigun

A proof-of-concept of using the Wii remote as an accurate wireless light gun.

*Note*: This code is very crude (but simple) and can be cleaned up.

## Hardware
- Wii Remote
- Wii Zapper or something like it
- Four high power IR LEDs positioned in the middle of each side of the screen
- Bluetooth receiver

## Software
- Linux with recent enough kernel with built-in xwiimote driver (3.12+)
- libxwiimote user space library
- SDL2 for testing
- PCSXR for playing

## How it works

It uses four reference points to calculate the absolute position you are aiming
the Wii remote at.

No manual calibration is needed, after the code picks up all four LEDs once it
can guess the remaining two even if only two are visible. Player position
relative to the screen doesn't matter that much, the geometry is a bit off but
the accuracy is still quite good. Two players would be possible with minimal
changes. This can be improved.

The Wii remote camera has a fairly tight viewing angle so you can't be too
close to the screen. I haven't tried this with a large screen yet, only 24 inch
monitors but it seems very playable (like around 1 meter to 24" screen).

A demo of it can be seen here: https://www.youtube.com/watch?v=eDgs1Uta8zo

## Setting up

For demonstration purposes I've cut down a Cat5 cable and used the four pairs
to solder LEDs to, then solder them in series to a cut down USB cable. It's a
bit over spec for the LEDs but worked fine.

The LEDs need to be properly aligned and preferably have a wide angle for the
light so you can move around with the Wii remote.

## Testing

This repo has a small testing program that shows where you are supposedly
aiming at if you set it up fullscreen. It should be *fairly* accurate. This
repo builds on Ubuntu 18.04 and 19.04 as-is.

## Emulator integration

I only own Time Crisis for PS1 (and TC2 for PS2) so I wrote a PCSXR compatible
GunCon driver to play something with it. It has some accuracy issues which can
likely be fixed by using the correct magic numbers. The driver also emulates
the pedal by ducking when aiming off-screen.

Latency seems good enough for rail shooters, accuracy as well.

## TODO
- Clean up the code, seriously
- Three point calibration when three LEDs are visible
- Possibly port to some other Wii remote user space implementation for
  portability
- More emulator drivers
- Improve accuracy when aiming at an angle
- Allow manual adjustment of line-of-sight relative to IR camera
- Allow manual screen bezel adjustment to improve edge accuracy
- Remember the last four point calibration offline so the gun would have a
  reference immediately when starting up
- Two player mode
- Pillarboxing adjustment in 16:9 (emulator driver side)

## Ideas and resources
 * https://www.wiimoteproject.com/project-ideas/11-wiimote-lightgun-tracking/
 * https://franklinta.com/2014/09/30/6dof-positional-tracking-with-the-wiimote/
 * http://wiicanetouchgraphic.blogspot.com/2009/03/wii-remote-ir-sensitivity.html
 * http://problemkaputt.de/psx-spx.htm#controllerslightgunsnamcoguncon
