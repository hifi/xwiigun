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

## xwiigun-mouse
UINPUT requires root access in order to function correctly. This causes a problem
as any applications that wish to use the device are not run as root. Simply running
xwiigun-mouse with sudo is not enough.

This can be worked around using 2 different methods.

- Method 1 (Temporary):

   Run the following commands to change permissions of uinput:
   - sudo chmod +0660 /dev/uinput
   - Run xwiigun-mouse

 - Method 2 (Permanent):

   In order to make the uinput rule permanent we need to tell the uinput module
   to run at boot and create a rule file to give uinput user permissions.
   - Navigate to the xwiigun/additional folder
   - run sudo uinput_fix.sh
   - sudo reboot

   Note: If this doesnt work, a small change might be required to the uinput_fix.sh
         file. This is explained within the file itself.

