#!/bin/bash

#Make sure only root can run the script
if [ "$(id -u)" != "0" ]; then
    echo "This script must be run as root"
    exit 1
fi

#Make the uinput module load at boot
echo "uinput" > /etc/modules-load.d/uinput.conf

#Create a rule file that changes the access permissions for uinput.
#Note: This has only been tested on a retropie setup. If this does
#      not work for you, try changing GROUP to GROUP="wheel"
#      and run again.
echo 'KERNEL=="uinput", GROUP="input", MODE="0660"' > /etc/udev/rules.d/99-wiimote.rules
