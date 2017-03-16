/*********************************************************************

  Human Interface Design (HID)
  Basic Chorded Keyboard
  with chord delay and auto repeat

  by Scott Mitchell
  RMIT University

  based on Adafruit Bluefruit code.
  MIT license

  This code is designed for use with the Adafruit Bluefruit Feather board

*********************************************************************/

#include "keyboardSetup.h"
String KEYBOARD_NAME = "TM Bluefruit";

const char keyMap[32] = {'0','time','person','backspace','year','say','be','first','way','make','do','in','have','to','new','the','day','good','go',
                          'on','get','long','of','a','_','with','for','at','last','that','and','I'
                        };

// pin settings
const int button0 = 6;
const int button1 = 9;
const int button2 = 10;
const int button3 = 11;
const int button4 = 12;
const int LEDpin = 5;

// variables to control button repeat
byte lastButtonState = B000000;
// delay time in milliseconds before button press is counted
const int keyPressDelay = 200;
// the last time the buttons were pressed
long timeOfLastKeyPress =200;
// the time in milliseconds before auto repeat
const int autoRepeatTime = 20000;

//
// Setup the system - run once
//
void setup(void)
{
  while (!Serial);  // wait for Serial to startup - maybe don't need this?
  delay(500);

  Serial.begin(115200);
  Serial.println("Basic Chorded Keyboard");
  Serial.println("---------------------------------------");

  setupKeyboard(KEYBOARD_NAME);

  // setup pins
  pinMode(button0, INPUT_PULLUP);
  pinMode(button1, INPUT_PULLUP);
  pinMode(button2, INPUT_PULLUP);
  pinMode(button3, INPUT_PULLUP);
  pinMode(button4, INPUT_PULLUP);
  pinMode(LEDpin, OUTPUT);
  digitalWrite(LEDpin, HIGH);
}


//
// Loop the program - run forever
//
void loop(void)
{
  // button states are represented by 0s and 1s in a binary number
  byte buttonState = readButtonState();
  // if the button state has changed wait a short time
  // this will prevent incomplete chords from being read
  if (buttonState != lastButtonState) delay(keyPressDelay);
  // check buttons again - this is considered a valid chord
  buttonState = readButtonState();

  // the result will be a number between 0 (no buttons pressed) and 63 (all buttons pressed)
  // This number is used to select a char from the key mapping array
  if (buttonState > 0) {

    // auto repeat
    if (buttonState == lastButtonState) {
      if (millis() - autoRepeatTime > timeOfLastKeyPress) {
        // send char
        sendChar(buttonState);
      }
    } else {
      timeOfLastKeyPress = millis();
      // send char
      sendChar(buttonState);
    }
  }

  // remember the button state
  lastButtonState = buttonState;
}


byte readButtonState() {
  // start with them all off
  byte bState = B000000;

  // turn on the bits if the button is being pressed
  if (digitalRead(button0) == false) bitSet(bState, 0);
  if (digitalRead(button1) == false) bitSet(bState, 1);
  if (digitalRead(button2) == false) bitSet(bState, 2);
  if (digitalRead(button3) == false) bitSet(bState, 3);
  if (digitalRead(button4) == false) bitSet(bState, 4);

  return (bState);
}

void sendChar(byte buttonState) {
  Serial.print("Sending Byte: ");
  Serial.print(buttonState, BIN);
  Serial.print(" Number: ");
  Serial.print(buttonState, DEC);
  Serial.print(", Char: ");
  Serial.println(keyMap[buttonState]);

  ble.print("AT+BleKeyboard=");
  ble.println(keyMap[buttonState]);

  // wait for feedback
  if ( ble.waitForOK() )
  {
    Serial.println("OK!");
  } else
  {
    Serial.println("FAILED!");
  }
}
