# xwiigun

A line-of-sight accurate Wii Remote lightgun hack.

See the [wiki](https://github.com/hifi/xwiigun/wiki) for setting up yourself.

Demo program output: https://www.youtube.com/watch?v=eDgs1Uta8zo

Accuracy and latency test: https://www.youtube.com/watch?v=IGNFb0cMnkg

## Hardware
- Wii Remote
- Wii Zapper or something like it
- Four wide angle high power IR LEDs (940nm preferred)
- Bluetooth receiver

## Software
- Linux with recent enough kernel with built-in xwiimote driver (3.12+)
- libxwiimote user space library
- SDL2 for testing (xwiigun-test)
- PCSXR for playing PS1 games (libxwiigun.so)
- Mouse emulation for everything else (xwiigun-mouse)

## Ideas and resources
 * https://www.wiimoteproject.com/project-ideas/11-wiimote-lightgun-tracking/
 * https://franklinta.com/2014/09/30/6dof-positional-tracking-with-the-wiimote/
 * http://wiicanetouchgraphic.blogspot.com/2009/03/wii-remote-ir-sensitivity.html
 * http://problemkaputt.de/psx-spx.htm#controllerslightgunsnamcoguncon
