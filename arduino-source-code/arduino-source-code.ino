#include <SoftwareSerial.h>
#include <avr/pgmspace.h>
#include <Servo.h>

// This code is build to use on an Arduino Nano board
// For reference:
// Pins that are PWM
// 3,5,6,9,10,11
// A6, A7 are only analog read

// PINS
const int statusLedsPins[] = {0, 1, 13, A4, A5}; //DIGITAL
const int servoPin = 5; // PWM
const int buzzerPin = 6; // PWM
const int rfidReadPin = 7; //DIGITAL
const int wifiWritePin = 8; //DIGITAL
const int rgbLedRedPin = 9; // PWM
const int rgbLedGreenPin = 10; // PWM
const int rgbLedBluePin = 11; // PWM
const int keySwitchPin = 12; //DIGITAL INPUT_PULL

const int keypadPins[] = {A0, A1, A2, A3}; // DIGITAL INPUT_PULLUP
//For reference:
//First pin of the keypad -> pin A1
//Second pin of the keypad -> pin A0
//Third pin of the keypad -> pin A3
//Fourth pin of the keypad -> pin A2
//Fifth pin of the keypad -> ground

const int pothoResistorPin = A7; // ANALOG
const int buttonsPins[] = {2, 3, 4}; //DIGITAL INPUT_PULL

char rfidTag[13] = "123456789abc"; // Initial contents are just arbitrary. We need 13 length, even though we just store 12 chars.
int rfidIncomingByte;
byte rfidByteIndex;
char validRfidTag1[13] = "41003DF2D45A"; // First card (for Juan)
char validRfidTag2[13] = "41003DE841D5"; // Second card (for Elena)
char validRfidTag3[13] = "3D0021738DE2"; // Keychain (for me. Reference number: 0002192269)
SoftwareSerial rfidDevice(rfidReadPin, 15);

int keypadPassword[] = { -1, -1, -1, -1, -1};
SoftwareSerial wifiDevice(13, wifiWritePin);

Servo servo;
int servoPosition = 0;

int keySwitchValue;

int keypadButtonState = 0;
int keypadLast5ButtonsPressed[] = {0, 0, 0, 0, 0};
int keypadNumberOfButtonsPressed = 0;

int rgbLedNumberOfButtonsPressed = 0;
int rgbLedLast5ButtonsPressed[] = {0, 0, 0, 0, 0};
const int rgbLedSolution[] = {2, 6, 4, 5, 3};
// For reference, this are the colours as int
// 0 - WHITE
// 1 - YELLOW
// 2 - PURPLE
// 3 - CYAN
// 4 - RED
// 5 - GREEN
// 6 - BLUE


// Song
const int numberOfNotes = 1440;

const uint16_t melody[] PROGMEM = {
  988, 0, 0, 784, 659, 494, 392, 330, 165, 0, 330, 0, 165, 247, 330, 392, 165, 0, 0, 247, 330, 392, 165, 0, 494, 0, 392, 0, 247, 330, 392, 0, 247, 330, 392, 0, 147, 0, 294, 0, 220, 294, 370, 147, 0, 220, 294, 370, 147, 494, 147, 659, 147, 784, 0, 0, 988, 165, 0, 330, 0, 247, 330, 392, 165, 0, 247, 330, 392, 165, 0, 494, 0, 392, 0, 247, 330, 392, 0, 247, 330, 392, 0, 147, 0, 294, 0, 220, 294, 370, 147, 0, 220, 294, 370, 147, 0, 165, 0, 247, 330, 392, 0, 247, 330, 392, 0, 494, 659, 0, 392, 0, 247, 330, 392, 659, 0, 247, 330, 392, 784, 740, 0, 659, 220, 294, 370, 587, 220, 294, 370, 0, 0, 0, 659, 196, 262, 330, 131, 0, 196, 262, 330, 131, 98, 0, 196, 247, 294, 587, 196, 247, 294, 0, 392, 587, 523, 196, 247, 294, 494, 196, 247, 294, 587, 523, 0, 262, 0, 220, 262, 330, 523, 220, 262, 330, 0, 494, 0, 196, 247, 330, 82, 0, 0, 0, 247, 330, 82, 0, 494, 659, 82, 0, 784, 0, 0, 988, 82, 0, 330, 0, 659, 247, 330, 392, 659, 0, 247, 330, 392, 0, 494, 659, 247, 330, 392, 0, 247, 330, 392, 784, 740, 659, 220, 294, 370, 587, 220, 294, 370, 0, 131, 0, 659, 0, 196, 262, 330, 0, 131, 0, 196, 262, 330, 131, 0, 330, 0, 196, 262, 330, 0, 196, 262, 330, 740, 784, 0, 247, 0, 196, 247, 294, 784, 0, 784, 196, 247, 294, 0, 440, 880, 220, 880, 262, 0, 220, 262, 0, 740, 147, 220, 294, 370, 147, 0, 220, 294, 370, 0, 784, 740, 0, 659, 220, 294, 370, 587, 740, 220, 294, 370, 740, 784, 0, 784, 0, 98, 0, 784, 247, 294, 392, 784, 247, 294, 392, 0, 740, 123, 247, 311, 370, 123, 0, 247, 311, 370, 0, 659, 165, 247, 330, 392, 165, 0, 247, 330, 392, 784, 0, 784, 740, 659, 220, 294, 370, 587, 220, 294, 370, 740, 784, 0, 98, 247, 294, 392, 784, 247, 294, 392, 0, 0, 740, 123, 247, 311, 370, 0, 247, 311, 370, 0, 659, 165, 247, 330, 392, 0, 247, 330, 392, 784, 740, 0, 659, 220, 294, 370, 587, 220, 294, 370, 0, 659, 0, 131, 196, 262, 330, 659, 196, 262, 330, 0, 659, 196, 262, 330, 131, 0, 196, 262, 330, 0, 131, 0, 392, 0, 247, 0, 659, 196, 262, 330, 659, 0, 196, 262, 330, 0, 587, 523, 196, 247, 294, 494, 196, 247, 294, 587, 523, 110, 0, 220, 440, 262, 0, 523, 220, 523, 262, 110, 0, 494, 82, 247, 330, 392, 0, 247, 330, 392, 82, 0, 494, 0, 392, 0, 247, 330, 392, 0, 247, 330, 392, 0, 147, 0, 123, 220, 294, 370, 147, 0, 220, 294, 370, 0, 494, 0, 659, 0, 784, 0, 988, 0, 988, 0, 988, 165, 0, 330, 0, 247, 330, 392, 165, 0, 247, 330, 392, 165, 0, 392, 0, 247, 330, 392, 0, 247, 330, 392, 0, 494, 0, 294, 0, 220, 294, 370, 587, 0, 220, 294, 370, 0, 659, 1047, 0, 330, 131, 330, 392, 523, 784, 0, 784, 0, 330, 392, 523, 165, 0, 784, 0, 196, 0, 330, 392, 523, 0, 330, 392, 523, 0, 1047, 0, 988, 0, 988, 0, 988, 0, 988, 0, 880, 330, 392, 523, 880, 0, 330, 392, 523, 1047, 0, 0, 988, 0, 988, 0, 988, 0, 988, 0, 98, 784, 294, 392, 494, 784, 0, 294, 392, 494, 123, 0, 392, 0, 784, 147, 294, 392, 494, 0, 294, 392, 494, 0, 392, 0, 247, 0, 294, 392, 494, 0, 294, 392, 494, 784, 0, 784, 0, 440, 880, 0, 147, 0, 587, 294, 440, 523, 587, 0, 294, 440, 523, 185, 0, 587, 0, 220, 294, 440, 523, 0, 294, 440, 523, 0, 988, 0, 988, 0, 988, 0, 988, 0, 880, 294, 440, 523, 784, 0, 784, 294, 440, 523, 880, 0, 988, 0, 988, 0, 988, 0, 247, 165, 0, 330, 392, 494, 784, 0, 784, 0, 330, 392, 494, 123, 0, 784, 0, 784, 0, 247, 98, 0, 330, 392, 494, 659, 0, 330, 392, 494, 82, 0, 330, 659, 0, 330, 392, 494, 0, 330, 392, 494, 0, 370, 0, 247, 0, 330, 392, 494, 392, 0, 330, 392, 494, 0, 440, 0, 87, 0, 262, 349, 440, 0, 262, 349, 440, 110, 0, 1047, 0, 131, 0, 220, 262, 349, 0, 220, 262, 349, 175, 0, 349, 0, 523, 0, 262, 349, 440, 0, 262, 349, 440, 0, 1047, 0, 523, 0, 220, 262, 349, 0, 220, 262, 349, 0, 1319, 988, 392, 784, 392, 659, 392, 494, 0, 392, 0, 330, 165, 0, 392, 494, 659, 0, 392, 494, 659, 123, 0, 784, 0, 98, 0, 392, 494, 659, 0, 392, 494, 659, 82, 0, 740, 0, 392, 494, 659, 0, 392, 494, 659, 0, 740, 0, 784, 0, 392, 494, 659, 370, 0, 392, 494, 659, 0, 392, 0, 262, 0, 131, 0, 311, 392, 523, 0, 0, 311, 392, 523, 156, 0, 784, 0, 196, 0, 262, 311, 392, 0, 262, 311, 392, 262, 523, 0, 392, 0, 311, 392, 523, 0, 311, 392, 523, 0, 784, 0, 392, 0, 262, 311, 392, 0, 311, 392, 0, 1175, 587, 880, 587, 740, 587, 440, 587, 370, 0, 294, 370, 440, 587, 0, 370, 440, 587, 220, 0, 0, 185, 370, 440, 587, 0, 370, 440, 587, 147, 0, 659, 0, 370, 440, 587, 0, 370, 440, 587, 0, 659, 0, 740, 0, 370, 440, 587, 147, 0, 370, 440, 587, 165, 392, 165, 494, 165, 0, 659, 165, 0, 0, 784, 0, 988, 0, 988, 165, 659, 0, 247, 330, 392, 659, 0, 247, 330, 392, 784, 294, 0, 740, 0, 220, 0, 659, 220, 294, 370, 587, 220, 294, 370, 0, 330, 0, 659, 0, 196, 262, 330, 131, 0, 196, 262, 330, 131, 0, 98, 0, 0, 0, 247, 0, 587, 392, 587, 0, 247, 294, 392, 0, 392, 587, 0, 294, 523, 0, 247, 494, 392, 494, 247, 0, 587, 392, 587, 523, 0, 262, 0, 262, 440, 523, 262, 523, 440, 0, 392, 0, 494, 0, 247, 0, 392, 82, 0, 247, 392, 82, 494, 0, 0, 659, 0, 784, 82, 988, 82, 0, 247, 330, 392, 659, 0, 247, 330, 392, 0, 330, 0, 659, 0, 247, 330, 392, 0, 247, 330, 392, 0, 784, 0, 784, 740, 0, 440, 659, 0, 220, 294, 370, 587, 220, 294, 370, 0, 523, 0, 659, 0, 196, 262, 330, 131, 0, 196, 262, 330, 131, 0, 392, 0, 262, 523, 392, 0, 262, 0, 0, 392, 740, 494, 784, 0, 247, 494, 392, 784, 247, 392, 0, 880, 0, 262, 523, 440, 0, 262, 440, 0, 740, 0, 147, 0, 220, 294, 370, 0, 784, 220, 294, 370, 784, 0, 784, 0, 740, 0, 659, 0, 220, 294, 370, 587, 220, 294, 370, 740, 0, 784, 98, 0, 247, 294, 392, 784, 247, 294, 392, 0, 740, 123, 0, 247, 311, 370, 0, 247, 311, 370, 0, 659, 165, 0, 247, 330, 392, 0, 247, 330, 392, 784, 740, 0, 659, 0, 294, 370, 440, 587, 294, 370, 440, 740, 0, 784, 0, 784, 0, 98, 0, 294, 392, 494, 784, 294, 392, 494, 0, 740, 123, 0, 311, 370, 494, 0, 311, 370, 494, 0, 659, 165, 0, 330, 392, 494, 0, 330, 392, 494, 784, 740, 659, 0, 370, 440, 587, 370, 440, 587, 0, 659, 0, 131, 0, 330, 392, 523, 659, 330, 392, 523, 0, 659, 0, 131, 0, 330, 392, 523, 0, 330, 392, 523, 131, 0, 98, 0, 294, 0, 247, 392, 0, 587, 247, 0, 587, 392, 0, 440, 523, 0, 330, 494, 262, 440, 523, 0, 262, 0, 440, 0, 494, 165, 0, 82, 0, 247, 330, 392, 0, 247, 330, 392, 0, 82, 0, 370, 0, 165, 0, 82, 0, 247, 330, 392, 659, 0, 247, 330, 392, 0, 659, 0, 247, 330, 392, 0, 247, 330, 392, 784, 740, 0, 659, 220, 294, 370, 587, 220, 294, 370, 0, 659, 0, 131, 0, 262, 330, 392, 659, 262, 330, 392, 131, 0, 131, 0, 262, 330, 392, 0, 262, 330, 392, 0, 880, 0, 440, 784, 0, 784, 0, 740, 220, 294, 370, 740, 0, 220, 294, 370, 880, 784, 0, 659, 185, 220, 294, 0, 740, 185, 220, 294, 0, 196, 247, 392, 330, 494, 659, 0
};

const uint16_t noteDurations[] PROGMEM = {
  9, 1, 1, 17, 17, 16, 13, 18, 24, 55, 15, 124, 12, 12, 15, 15, 12, 106, 6, 12, 12, 15, 85, 51, 16, 153, 15, 137, 12, 12, 18, 123, 12, 12, 18, 135, 14, 155, 14, 139, 14, 10, 19, 41, 81, 14, 10, 19, 27, 14, 14, 17, 14, 15, 7, 15, 17, 134, 22, 15, 137, 12, 12, 18, 30, 92, 12, 9, 18, 122, 17, 16, 153, 15, 137, 12, 12, 18, 123, 12, 12, 18, 135, 14, 155, 14, 139, 14, 10, 19, 41, 81, 14, 10, 19, 116, 19, 249, 68, 12, 12, 18, 123, 12, 12, 18, 135, 8, 135, 21, 15, 137, 12, 12, 18, 109, 13, 12, 12, 20, 141, 177, 4, 126, 14, 14, 16, 123, 14, 10, 16, 137, 8, 5, 305, 10, 15, 15, 84, 39, 10, 15, 18, 145, 286, 20, 10, 16, 17, 121, 15, 12, 14, 137, 8, 145, 164, 15, 16, 14, 119, 15, 16, 17, 129, 164, 9, 15, 137, 14, 11, 18, 122, 14, 11, 15, 119, 249, 89, 10, 16, 15, 36, 82, 10, 5, 16, 15, 24, 8, 6, 17, 36, 6, 11, 12, 7, 10, 24, 137, 15, 128, 9, 12, 12, 18, 74, 48, 12, 12, 18, 135, 8, 308, 12, 12, 18, 123, 12, 9, 18, 143, 169, 144, 14, 10, 19, 121, 14, 14, 13, 137, 15, 5, 287, 10, 10, 15, 15, 10, 92, 18, 10, 15, 18, 176, 127, 15, 137, 10, 15, 15, 124, 10, 15, 18, 116, 164, 22, 16, 136, 10, 12, 17, 28, 1, 96, 15, 12, 14, 137, 5, 312, 18, 7, 15, 124, 18, 23, 166, 173, 123, 14, 10, 19, 20, 102, 14, 7, 16, 3, 143, 143, 21, 152, 9, 17, 16, 111, 8, 14, 10, 19, 122, 3, 1, 164, 18, 133, 6, 13, 12, 14, 18, 121, 12, 14, 15, 137, 220, 105, 8, 13, 16, 32, 91, 12, 13, 16, 124, 204, 134, 12, 15, 15, 12, 111, 12, 12, 20, 125, 1, 6, 169, 152, 14, 14, 16, 123, 9, 17, 16, 143, 181, 17, 122, 8, 10, 18, 125, 12, 14, 18, 1, 157, 173, 130, 12, 10, 16, 124, 12, 13, 16, 166, 169, 128, 12, 12, 15, 125, 12, 12, 20, 133, 169, 4, 152, 14, 14, 16, 121, 14, 14, 14, 146, 155, 39, 115, 15, 8, 21, 125, 10, 15, 15, 145, 317, 10, 15, 15, 76, 47, 10, 15, 15, 6, 99, 32, 15, 154, 16, 132, 5, 10, 15, 18, 108, 13, 10, 15, 15, 137, 177, 143, 15, 12, 17, 121, 10, 12, 17, 163, 160, 118, 12, 18, 7, 15, 6, 118, 18, 8, 15, 118, 56, 178, 109, 12, 12, 15, 125, 12, 12, 18, 122, 14, 16, 153, 15, 137, 12, 12, 18, 123, 12, 12, 18, 135, 14, 155, 154, 9, 14, 19, 102, 19, 14, 10, 16, 32, 16, 18, 17, 9, 17, 17, 3, 1, 9, 1, 3, 146, 10, 15, 137, 12, 9, 18, 30, 95, 12, 12, 18, 152, 152, 15, 137, 12, 12, 18, 123, 12, 12, 15, 112, 152, 43, 14, 139, 14, 10, 19, 78, 44, 14, 10, 16, 99, 33, 126, 47, 18, 138, 12, 13, 17, 42, 1, 33, 47, 12, 13, 17, 97, 38, 139, 13, 158, 7, 12, 13, 15, 124, 12, 13, 17, 127, 118, 51, 23, 1, 59, 1, 9, 1, 40, 13, 12, 12, 13, 17, 89, 34, 12, 13, 19, 65, 68, 1, 9, 1, 59, 1, 9, 1, 58, 22, 153, 4, 14, 10, 18, 117, 5, 14, 8, 16, 121, 18, 15, 6, 139, 163, 7, 13, 14, 124, 14, 13, 14, 137, 15, 154, 16, 136, 14, 13, 14, 124, 14, 8, 16, 48, 1, 69, 21, 9, 117, 43, 136, 5, 10, 14, 11, 17, 109, 14, 14, 11, 17, 135, 3, 148, 9, 159, 14, 14, 15, 124, 14, 11, 17, 118, 70, 1, 9, 1, 59, 1, 23, 9, 160, 17, 14, 15, 101, 1, 22, 14, 11, 17, 109, 26, 39, 1, 60, 1, 25, 43, 8, 122, 22, 12, 13, 18, 25, 1, 69, 26, 12, 13, 18, 113, 5, 52, 1, 88, 39, 12, 122, 17, 12, 13, 18, 117, 5, 12, 13, 18, 122, 13, 9, 172, 136, 12, 13, 16, 124, 12, 13, 16, 128, 151, 26, 16, 136, 12, 13, 18, 87, 35, 12, 13, 16, 120, 130, 56, 126, 26, 11, 14, 16, 123, 11, 14, 16, 73, 63, 16, 136, 122, 47, 14, 11, 14, 125, 14, 11, 17, 109, 26, 14, 155, 15, 137, 11, 14, 16, 123, 11, 14, 16, 136, 16, 153, 15, 137, 14, 11, 14, 125, 14, 11, 17, 114, 17, 17, 10, 15, 13, 17, 13, 16, 17, 15, 19, 18, 140, 11, 13, 12, 17, 123, 13, 12, 17, 89, 47, 17, 152, 92, 60, 13, 12, 17, 123, 13, 12, 17, 85, 51, 16, 309, 13, 12, 17, 123, 13, 12, 17, 136, 16, 153, 17, 136, 13, 12, 17, 54, 69, 13, 12, 17, 140, 110, 55, 15, 44, 92, 6, 6, 13, 17, 8, 116, 13, 8, 17, 135, 5, 17, 152, 117, 35, 11, 13, 18, 123, 11, 13, 18, 118, 117, 69, 15, 137, 13, 13, 15, 124, 13, 13, 15, 137, 17, 152, 15, 137, 11, 13, 18, 135, 13, 15, 103, 17, 9, 17, 3, 18, 43, 16, 8, 16, 86, 116, 11, 14, 15, 123, 14, 11, 17, 150, 1, 145, 162, 11, 14, 15, 123, 8, 18, 15, 143, 5, 5, 309, 14, 11, 17, 123, 14, 11, 17, 136, 17, 152, 16, 136, 14, 11, 17, 123, 5, 8, 11, 17, 12, 13, 12, 14, 12, 5, 14, 12, 1, 1, 10, 6, 3, 1, 2, 36, 106, 191, 12, 12, 18, 109, 13, 12, 12, 20, 141, 7, 3, 134, 17, 14, 20, 118, 14, 10, 19, 123, 14, 10, 16, 137, 15, 27, 223, 56, 10, 15, 15, 53, 70, 10, 15, 18, 107, 27, 173, 7, 3, 137, 16, 3, 5, 18, 104, 18, 16, 10, 15, 136, 8, 143, 14, 17, 117, 19, 20, 6, 15, 123, 16, 3, 5, 18, 134, 164, 5, 15, 137, 15, 27, 122, 19, 6, 16, 137, 15, 19, 281, 6, 16, 12, 13, 85, 39, 24, 18, 24, 14, 12, 8, 14, 12, 13, 24, 14, 36, 280, 12, 12, 18, 109, 13, 12, 12, 18, 135, 15, 6, 244, 51, 12, 12, 18, 123, 12, 12, 18, 9, 94, 1, 48, 142, 6, 16, 131, 6, 14, 10, 19, 123, 14, 10, 16, 137, 15, 23, 257, 22, 10, 15, 18, 46, 75, 10, 15, 18, 161, 142, 15, 137, 19, 6, 15, 125, 15, 3, 2, 18, 147, 8, 170, 135, 20, 6, 18, 121, 16, 23, 138, 316, 5, 19, 6, 16, 124, 15, 25, 120, 173, 17, 143, 9, 14, 10, 16, 120, 4, 14, 14, 16, 47, 1, 89, 9, 139, 13, 134, 27, 14, 10, 19, 123, 9, 17, 16, 100, 64, 122, 153, 20, 12, 14, 18, 117, 12, 14, 15, 166, 143, 146, 7, 12, 13, 16, 124, 12, 13, 16, 132, 152, 109, 68, 12, 12, 18, 123, 12, 12, 20, 133, 135, 38, 117, 31, 14, 11, 18, 123, 14, 11, 18, 104, 52, 124, 1, 34, 14, 112, 19, 14, 13, 16, 122, 14, 13, 14, 154, 147, 121, 40, 13, 14, 14, 124, 13, 13, 14, 137, 161, 115, 49, 12, 13, 16, 124, 12, 13, 18, 134, 165, 143, 14, 14, 11, 140, 13, 11, 17, 157, 134, 14, 107, 49, 12, 13, 17, 118, 12, 13, 17, 165, 155, 35, 99, 6, 12, 13, 17, 123, 12, 13, 17, 107, 16, 143, 39, 14, 138, 16, 26, 5, 117, 16, 3, 7, 15, 137, 5, 151, 9, 18, 134, 15, 27, 109, 14, 15, 10, 16, 157, 148, 12, 18, 73, 50, 12, 12, 18, 123, 12, 12, 18, 17, 85, 33, 16, 153, 12, 22, 109, 9, 16, 12, 18, 100, 23, 12, 12, 18, 144, 152, 165, 12, 12, 15, 125, 12, 12, 20, 158, 143, 9, 144, 14, 10, 19, 121, 14, 14, 14, 154, 138, 10, 122, 38, 11, 15, 15, 123, 11, 15, 15, 107, 218, 107, 28, 11, 15, 15, 123, 11, 15, 15, 135, 126, 43, 5, 68, 1, 40, 35, 4, 14, 10, 19, 104, 14, 18, 10, 19, 126, 159, 9, 156, 11, 14, 17, 3, 120, 11, 14, 17, 6, 66, 8, 51, 6, 71, 249, 71
};

int numberOfPuzzlesSolved = 0;

bool isLightPuzzleSolved = false;
bool isRfidPuzzleSolved = false;
bool isKeypadPuzzleSolved = false;
bool isButtonsAndRGBPuzzleSolved = false;
bool isKeySwitchPuzzleSolved = false;

bool hasDoneTheAfterCompletedRoutine = false;
bool isClosingBox = false;

unsigned long timeSinceSwitchedOn;

const unsigned long msAfterBoxOpenAutomatically = 86400000; // 24 hours

bool isDebugEnabled = false;

void displayTextOnSerial(const char* text) {
  if(isDebugEnabled){
    Serial.print(text);
  }
}

void displayLineOnSerial(const char* text) {
  if(isDebugEnabled){
    Serial.println(text);
  }
}

void setup() {
  if(isDebugEnabled){
    Serial.begin(115200);
  }
  wifiDevice.begin(115200);
  wifiDevice.listen();

  pinMode(statusLedsPins[0], OUTPUT);
  pinMode(statusLedsPins[1], OUTPUT);
  pinMode(statusLedsPins[2], OUTPUT);
  pinMode(statusLedsPins[3], OUTPUT);
  pinMode(statusLedsPins[4], OUTPUT);

  pinMode(rgbLedRedPin, OUTPUT);
  pinMode(rgbLedGreenPin, OUTPUT);
  pinMode(rgbLedBluePin, OUTPUT);

  pinMode(rgbLedRedPin, LOW);
  pinMode(rgbLedGreenPin, LOW);
  pinMode(rgbLedBluePin, LOW);

  pinMode(keySwitchPin, INPUT_PULLUP);

  pinMode(keypadPins[0], INPUT_PULLUP);
  pinMode(keypadPins[1], INPUT_PULLUP);
  pinMode(keypadPins[2], INPUT_PULLUP);
  pinMode(keypadPins[3], INPUT_PULLUP);

  pinMode(buttonsPins[0], INPUT_PULLUP);
  pinMode(buttonsPins[1], INPUT_PULLUP);
  pinMode(buttonsPins[2], INPUT_PULLUP);

  randomSeed(analogRead(pothoResistorPin));

  setKeypadPassword();

  digitalWrite(statusLedsPins[0], HIGH);
  digitalWrite(statusLedsPins[1], HIGH);
  digitalWrite(statusLedsPins[2], HIGH);
  digitalWrite(statusLedsPins[3], HIGH);
  digitalWrite(statusLedsPins[4], HIGH);

  displayLineOnSerial("Arduino Ready");

}

void loop() {

  timeSinceSwitchedOn = millis();

  if (hasDoneTheAfterCompletedRoutine) {
    if (!isClosingBox) {
      int keypadButtonState0 = digitalRead(keypadPins[0]);
      int keypadButtonState1 = digitalRead(keypadPins[1]);
      int keypadButtonState2 = digitalRead(keypadPins[2]);
      int keypadButtonState3 = digitalRead(keypadPins[3]);

      int rgbButtonState0 = digitalRead(buttonsPins[0]);
      int rgbButtonState1 = digitalRead(buttonsPins[1]);
      int rgbButtonState2 = digitalRead(buttonsPins[2]);

      delay(100);

      if (keypadButtonState0 == LOW
          || keypadButtonState1 == LOW
          || keypadButtonState2 == LOW
          || keypadButtonState3 == LOW
          || rgbButtonState0 == LOW
          || rgbButtonState1 == LOW
          || rgbButtonState2 == LOW
         ) {
        isClosingBox = true;
        closeBox();
      }
    }
  }
  else {
    // Automatically open box after a specific time as a safe guard in case something goes wrong
    if (timeSinceSwitchedOn > msAfterBoxOpenAutomatically) {
      hasDoneTheAfterCompletedRoutine = true;
      openBox();
    }
    else if (hasCompletedAllPuzzles()) {
      hasDoneTheAfterCompletedRoutine = true;
      openBox();
      playSong();
    }
    else {

      if (!isKeypadPuzzleSolved) {
        checkKeypad();
      }

      if (!isRfidPuzzleSolved) {
        checkRFID();
      }

      if (!isKeySwitchPuzzleSolved) {
        checkKeySwitch();
      }

      if (!isLightPuzzleSolved) {
        checkPhotoResistor();
      }

      if (!isButtonsAndRGBPuzzleSolved) {
        checkRGBLedButtons();
      }

    }
  }
}

void setKeypadPassword() {

  long random1 = random(1, 5);
  long random2 = random(1, 5);
  long random3 = random(1, 5);
  long random4 = random(1, 5);
  long random5 = random(1, 5);

  String keypadCombinationString = String("") + random1 + random2 + random3 + random4 + random5;

  char keypadCombination[6] = {};
  strcpy(keypadCombination, keypadCombinationString.c_str());

  keypadPassword[0] = random1;
  keypadPassword[1] = random2;
  keypadPassword[2] = random3;
  keypadPassword[3] = random4;
  keypadPassword[4] = random5;

  wifiDevice.println("AT+RST"); //Reset the module so we can configure it again
  delay(1000);
  wifiDevice.println("AT+CWMODE_DEF=2"); //Mode AP
  delay(1000);
  // We configure the essid, password, channel, encryption type:
  // "La clave de la caja es" is spanish for: "The password of the box is"
  wifiDevice.println("AT+CWSAP_DEF=\"La clave de la caja es " + keypadCombinationString + "\",\"PruebasMuchas\",1,4"); 

  displayTextOnSerial("La clave de la caja es "); 
  displayLineOnSerial(keypadCombination);

  rfidDevice.begin(9600);
  rfidDevice.listen();
}

void checkKeypad() {

  for (int index = 0; index < 4; index++)
  {

    keypadButtonState = digitalRead(keypadPins[index]);

    if (keypadButtonState == LOW) {

      tone(buzzerPin, 247);
      delay(100);
      noTone(buzzerPin);

      delay(450); // We include a delay after a button is pushed to avoid registering as pushed multiple times.

      int buttonPressed = (index + 1);

      keypadNumberOfButtonsPressed++;

      if (keypadNumberOfButtonsPressed > 5) {
        keypadLast5ButtonsPressed[0] = keypadLast5ButtonsPressed[1];
        keypadLast5ButtonsPressed[1] = keypadLast5ButtonsPressed[2];
        keypadLast5ButtonsPressed[2] = keypadLast5ButtonsPressed[3];
        keypadLast5ButtonsPressed[3] = keypadLast5ButtonsPressed[4];
        keypadLast5ButtonsPressed[4] = buttonPressed;
      }
      else {
        keypadLast5ButtonsPressed[keypadNumberOfButtonsPressed - 1] = buttonPressed;
      }

      if (keypadNumberOfButtonsPressed >= 5) {
        checkIfKeypadCombinationIsCorrect();
      }

    }

  }
}

void checkIfKeypadCombinationIsCorrect() {

  bool isSolved = true;

  for (int solutionIndex = 0; solutionIndex < 5; solutionIndex++)
  {
    if (keypadLast5ButtonsPressed[solutionIndex] != keypadPassword[solutionIndex]) {
      isSolved = false;
    }
  }

  if (isSolved) {
    isKeypadPuzzleSolved = true;
    puzzleSolved();
    wifiDevice.write("AT+CWMODE_CUR=1"); //Sleep mode requires station mode = 1;
    wifiDevice.write("AT+SLEEP=1"); //Sleep to save energy
    displayTextOnSerial("KEYPAD PUZZLE SOLVED!");
  }

}

void checkRFID() {

  if (rfidDevice.available() > 0) {

    rfidIncomingByte = rfidDevice.read();

    if (rfidIncomingByte == 2) {
      rfidByteIndex = 0;
    }

    else {
      if (rfidByteIndex < 12) {
        rfidTag[rfidByteIndex] = rfidIncomingByte;
        rfidByteIndex++;
      };
    }
    // Although seems this logic seems to work just fine,
    // we could potentially get weird data on incomingByte. 
    // It would be best to check that incomingByte is 0-9 or A-B
    if (rfidIncomingByte == 3) {
      displayTextOnSerial(("ID Tag: "));
      displayLineOnSerial(rfidTag);

      if (strcmp(rfidTag, validRfidTag1) == 0 || strcmp(rfidTag, validRfidTag2) == 0 || strcmp(rfidTag, validRfidTag3) == 0) {
        isRfidPuzzleSolved = true;
        puzzleSolved();
        displayLineOnSerial("RFID PUZZLE SOLVED");
      }

    };
  }
}

void checkKeySwitch() {

  keySwitchValue = digitalRead(keySwitchPin);
  if (keySwitchValue == LOW) {
    isKeySwitchPuzzleSolved = true;
    puzzleSolved();
    displayLineOnSerial(("KEY SWITCH PUZZLE SOLVED"));
  }
}

void checkPhotoResistor() {

  int sensorValue = analogRead(pothoResistorPin); // returns a value between 0 to 1023
  // Serial.print(("Light value: "));
  // Serial.println(sensorValue);

  if (sensorValue < 250) {
    isLightPuzzleSolved = true;
    puzzleSolved();
    displayLineOnSerial(("LIGHT PUZZLE SOLVED!"));
  }

}

void checkRGBLedButtons() {

  bool isFirstRedButtonPressed = (digitalRead(buttonsPins[0]) == LOW);
  bool isFirstGreenButtonPressed = (digitalRead(buttonsPins[1]) == LOW);
  bool isFirstBlueButtonPressed = (digitalRead(buttonsPins[2]) == LOW);

  if (isFirstRedButtonPressed || isFirstGreenButtonPressed || isFirstBlueButtonPressed ) {

    delay(250); // Allow a bit of time to avoid registering the same button pushed multiple times

    bool isSecondRedButtonPressed = (digitalRead(buttonsPins[0]) == LOW);
    bool isSecondGreenButtonPressed = (digitalRead(buttonsPins[1]) == LOW);
    bool isSecondBlueButtonPressed = (digitalRead(buttonsPins[2]) == LOW);

    bool isRedButtonPressed = (isFirstRedButtonPressed || isSecondRedButtonPressed);
    bool isGreenButtonPressed = (isFirstGreenButtonPressed || isSecondGreenButtonPressed);
    bool isBlueButtonPressed = (isFirstBlueButtonPressed || isSecondBlueButtonPressed);

    int colorSelectedAsInt;

    if (isRedButtonPressed && isGreenButtonPressed && isBlueButtonPressed) {
      // 0 - WHITE
      analogWrite(rgbLedRedPin, 250);
      analogWrite(rgbLedGreenPin, 250);
      analogWrite(rgbLedBluePin, 250);

      colorSelectedAsInt = 0;
      displayLineOnSerial("White");
    }
    else if (isRedButtonPressed && isGreenButtonPressed) {
      // 1 - YELLOW
      analogWrite(rgbLedRedPin, 250);
      analogWrite(rgbLedGreenPin, 250);
      analogWrite(rgbLedBluePin, 0);

      colorSelectedAsInt = 1;
      displayLineOnSerial("Yellow");
    }
    else if (isRedButtonPressed && isBlueButtonPressed) {
      // 2 - PURPLE
      analogWrite(rgbLedRedPin, 250);
      analogWrite(rgbLedGreenPin, 0);
      analogWrite(rgbLedBluePin, 250);

      colorSelectedAsInt = 2;
      displayLineOnSerial("Purple");
    }
    else if (isGreenButtonPressed && isBlueButtonPressed) {
      // 3 - CYAN
      analogWrite(rgbLedRedPin, 0);
      analogWrite(rgbLedGreenPin, 250);
      analogWrite(rgbLedBluePin, 250);

      colorSelectedAsInt = 3;
      displayLineOnSerial("Cyan");
    }
    else if (isRedButtonPressed) {
      // 4 - RED
      analogWrite(rgbLedRedPin, 250);
      analogWrite(rgbLedGreenPin, 0);
      analogWrite(rgbLedBluePin, 0);

      colorSelectedAsInt = 4;
      displayLineOnSerial("Red");
    }
    else if (isGreenButtonPressed) {
      // 5 - GREEN
      analogWrite(rgbLedRedPin, 0);
      analogWrite(rgbLedGreenPin, 250);
      analogWrite(rgbLedBluePin, 0);

      colorSelectedAsInt = 5;
      displayLineOnSerial("Green");
    }
    else if (isBlueButtonPressed) {
      // 6 - BLUE
      analogWrite(rgbLedRedPin, 0);
      analogWrite(rgbLedGreenPin, 0);
      analogWrite(rgbLedBluePin, 250);

      colorSelectedAsInt = 6;
      displayLineOnSerial("Blue");
    }

    // We need a delay after pressed or it will registered as pressed many times. 
    // Also the LED needs to be on for a while for the user to see it
    delay(1000); 

    //Switch off LED
    analogWrite(rgbLedRedPin, 0);
    analogWrite(rgbLedGreenPin, 0);
    analogWrite(rgbLedBluePin, 0);

    rgbLedNumberOfButtonsPressed++;

    if (rgbLedNumberOfButtonsPressed > 5) {
      rgbLedLast5ButtonsPressed[0] = rgbLedLast5ButtonsPressed[1];
      rgbLedLast5ButtonsPressed[1] = rgbLedLast5ButtonsPressed[2];
      rgbLedLast5ButtonsPressed[2] = rgbLedLast5ButtonsPressed[3];
      rgbLedLast5ButtonsPressed[3] = rgbLedLast5ButtonsPressed[4];
      rgbLedLast5ButtonsPressed[4] = colorSelectedAsInt;
    }
    else {
      rgbLedLast5ButtonsPressed[rgbLedNumberOfButtonsPressed - 1] = colorSelectedAsInt;
    }

    if (rgbLedNumberOfButtonsPressed >= 5) {
      checkIfRGBLedButtonsCombinationIsCorrect();
    }

  }

}

void checkIfRGBLedButtonsCombinationIsCorrect() {

  bool isSolved = true;

  for (int solutionIndex = 0; solutionIndex < 5; solutionIndex++)
  {
    if (rgbLedLast5ButtonsPressed[solutionIndex] != rgbLedSolution[solutionIndex]) {
      isSolved = false;
    }
  }

  if (isSolved) {
    analogWrite(rgbLedRedPin, 0);
    analogWrite(rgbLedGreenPin, 0);
    analogWrite(rgbLedBluePin, 0);
    isButtonsAndRGBPuzzleSolved = true;
    puzzleSolved();
    displayLineOnSerial(("RGB LED PUZZLE SOLVED!"));
  }

}

bool hasCompletedAllPuzzles() {
  //return isLightPuzzleSolved  && isRfidPuzzleSolved && isKeypadPuzzleSolved && isButtonsAndRGBPuzzleSolved && isKeySwitchPuzzleSolved;
  return numberOfPuzzlesSolved == 5;
}

void openBox() {
  displayLineOnSerial("Opening Box");
  servo.attach(servoPin);
  servo.write(0);
  delay(1000);
  servo.detach();
}

void closeBox() {
  displayLineOnSerial("Closing Box");
  servo.attach(servoPin);
  servo.write(105);
}

void playSong() {

  for (int thisNote = 0; thisNote < numberOfNotes; thisNote++)
  {
    if (pgm_read_word_near(melody + thisNote) > 0)
    {
      tone(buzzerPin, pgm_read_word_near(melody + thisNote));
    }
    else
      noTone(buzzerPin);
      delay(pgm_read_word_near(noteDurations + thisNote));
  }

}

void puzzleSolved() {
  numberOfPuzzlesSolved++;
  digitalWrite(statusLedsPins[(5 - numberOfPuzzlesSolved)], LOW);
  tone(buzzerPin, 988);
  delay(100);
  noTone(buzzerPin);

  delay(100);

  tone(buzzerPin, 988);
  delay(100);
  noTone(buzzerPin);
}
