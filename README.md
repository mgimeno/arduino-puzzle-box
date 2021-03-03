# Arduino Puzzle Box

## What is this?

This is the source code and schematics for a custom puzzle box that I designed and built.

The box is locked from the inside (using a servomotor) and to be opened, one needs to solve the 5 puzzles built-in.

5 red LEDs are lighted up as soon as we switch on the box (by plugging in a micro USB), signalling the number of puzzles remaining to be solved.

Every time we solve one of the puzzles, we can hear a beep and one of the red LEDs is automatically switched off.

Once the 5 puzzles are solved, the box unlocks automatically at the same time as the opening theme of the game "The Secret of Monkey Island" is played through a passive buzzer.

## Solution to the 5 puzzles

1. Insert the key in the lock and turn it
2. Use a source of light through the little hole
3. Use a valid RFID card/keychain on the centre of the box's backside. The list of valid RFID tags is set in the code.
4. Use the correct combination of colours by pushing the buttons below the big RGB LED. The combination is set in the code and currently is: Purple, Blue, Red, Green and Cyan.
5. Use a mobile/computer to scan for WIFI networks. Upon switch-on, the box automatically creates a WIFI access point and its name contains a sequence of numbers (randomly generated every time we switch on the box) that we then need to input by using the box's keypad

## Arduino board

This code is compatible with the Arduino Nano board.

## Development

It's been developed using the official Arduino IDE.
