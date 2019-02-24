# Light Ring Controller

The purpose of this project is to provide a light ring for each of the cameras on our robot, to illuminate targets for the computer vision system.

This project is based on an ATTiny85.

It can control up to 3 strings of Neopixels.

Each string is set to a single color by a remote host.

The default 7 bit I2C address is 0x04.

The protocol is a 4 byte write:

- string number, red, green, blue

The strings are numbered 1, 2, and 3

More details are found in the info.txt file.


The power-up default for each string is alternating blue and white, for Lake Zurich Bears.
