# SDU Rotating Table
## Mechanical Design
The mechanical design can be seen in onshape by clicking [here](https://cad.onshape.com/documents/13b4a704a5bd44536605270a/w/41ba2863a123ab33a2857a64/e/8c5fee0b299377f02b8e49e4).

## Electrical Design
The setup uses a controllino maxi automation, tb6600 stepper driver, nema 24 stepper motor, a limit switch and a 24V 5A power supply. The stepper motor is connected to the stepper driver, which is driven by the controllino with the enable pin(D0), direction pin(D1) and pull pin(D2). The limit switch is connected to the controllino with the C pin(DI0), NO pin(24V) and NC pin(GND).

## Controllino Code
The controllino code is writen using VS code with platformIO extension. The code can be flashed using the a USB cable. Ensure that a working IP and Port is choosen.

## Python Code
The code to control the table is written in python and is contained in the class RotatingTableSDU in the file RotatingTableSDU.py. An example of how to use the class can be seen in the bottom of the RotatingTableSDU.py file.