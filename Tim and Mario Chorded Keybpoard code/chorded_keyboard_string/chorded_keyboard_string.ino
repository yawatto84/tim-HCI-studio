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
String KEYBOARD_NAME = "Tim board";

const String keyMap[64] = {"0", "hello ", "goodbye ", "call you later ", "see you later ", ":-) "
                          };

// pin settings
const int button0 = 5;
const int button1 = 6;
const int button2 = 9;
const int button3 = 10;
const int button4 = 11;


// variables to control button repeat
byte lastButtonState = B000000;
// delay time in milliseconds before button press is counted
const int keyPressDelay = 800;
// the last time the buttons were pressed
long timeOfLastKeyPress = 0;
// the time in milliseconds before auto repeat
const int autoRepeatTime = 2000;

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
        // send String
        sendStr(buttonState);
      }
    } else {
      timeOfLastKeyPress = millis();
      // send String
      sendStr(buttonState);
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

void sendStr(byte buttonState) {
  Serial.print("Sending Byte: ");
  Serial.print(buttonState, BIN);
  Serial.print(" Number: ");
  Serial.print(buttonState, DEC);
  Serial.print(", String: ");
  Serial.println(keyMap[buttonState]);

  // copy string to char array
  const unsigned int len = 128;
  char keys[len];
  keyMap[buttonState].toCharArray(keys, len);

  ble.print("AT+BleKeyboard=");
  ble.println(keys);

  // wait for feedback
  if ( ble.waitForOK() )
  {
    Serial.println("OK!");
  } else
  {
    Serial.println("FAILED!");
  }
}
