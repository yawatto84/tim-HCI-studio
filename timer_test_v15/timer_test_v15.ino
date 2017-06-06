#include <TinyWireM.h>
#include <USI_TWI_Master.h>
#include <Wire.h>
#include <SparkFun_ADXL345.h>
#include <Adafruit_NeoPixel.h>

// pin settings
const int hazleftbutton = 5;
const int hazrightbutton = 6;
const int turnleftbutton = 9;
const int turnrightbutton = 10;

const int buttonDebounceTime = 50;   //(milliseconds * 2)
const int accelHistoryDepth = 20;
const int brakingThreshold = 20;

const int hapticleft = A0;
const int hapticright = A1;

ADXL345 accel = ADXL345();

//Adafruit Neopixel Settings
#define PIN 13
Adafruit_NeoPixel rlight = Adafruit_NeoPixel(36, PIN, NEO_GRB + NEO_KHZ800);
const int numLEDS = 36;

uint32_t red = rlight.Color(255, 0, 0);
uint32_t amber = rlight.Color(255, 60, 0);
const int delayvalhazard = 75; 
const int delayvalturning = 75; 
const int delayvalbraking = 75;
const char hazFrames = 7;
const char turnFrames = 6;
const char brakeFrames = 9;

//            RIGHT
//      1, 0, 0, 0, 0, 1, 
//      0, 1, 0, 0, 1, 0,
// TOP  0, 0, 1, 1, 0, 0,
//      0, 0, 1, 1, 0, 0, BOTTOM
//      0, 1, 0, 0, 1, 0,
//      1, 0, 0, 0, 0, 1,
//            LEFT

//Following bool's are matrix frames for each LED signal output
bool hazleftMap[hazFrames][numLEDS] = {
  
  { 1, 0, 0, 0, 0, 1, ///Frame 1 
    0, 1, 0, 0, 1, 0,
    0, 0, 1, 1, 0, 0,
    0, 0, 1, 1, 0, 0,
    0, 1, 0, 0, 1, 0,
    1, 0, 0, 0, 0, 1,
  },

  { 1, 0, 0, 0, 0, 1, ///Frame 2 
    0, 1, 0, 0, 1, 0,
    0, 0, 1, 1, 0, 0,
    0, 0, 1, 1, 0, 0,
    0, 1, 0, 0, 1, 0,
    1, 0, 0, 0, 0, 1,
  },

  { 0, 0, 0, 0, 0, 0, ///Frame 3
    0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0,
  },

  { 0, 0, 0, 0, 0, 0, ///Frame 4 
    0, 0, 0, 0, 0, 0,
    1, 0, 0, 0, 0, 1,
    1, 1, 0, 0, 1, 1,
    0, 1, 1, 1, 1, 0,
    0, 0, 1, 1, 0, 0,
  },

  { 0, 0, 0, 0, 0, 0, ///Frame 5
    0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0,
  },

  { 0, 0, 0, 0, 0, 0, ///Frame 6 
    0, 0, 0, 0, 0, 0,
    1, 0, 0, 0, 0, 1,
    1, 1, 0, 0, 1, 1,
    0, 1, 1, 1, 1, 0,
    0, 0, 1, 1, 0, 0,
  },

  { 0, 0, 0, 0, 0, 0, ///Frame 7
    0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0,
  },
};

bool hazrightMap[hazFrames][numLEDS] = {
  
  { 1, 0, 0, 0, 0, 1, ///Frame 1
    0, 1, 0, 0, 1, 0,
    0, 0, 1, 1, 0, 0,
    0, 0, 1, 1, 0, 0,
    0, 1, 0, 0, 1, 0,
    1, 0, 0, 0, 0, 1,
  },

  { 1, 0, 0, 0, 0, 1, ///Frame 1
    0, 1, 0, 0, 1, 0,
    0, 0, 1, 1, 0, 0,
    0, 0, 1, 1, 0, 0,
    0, 1, 0, 0, 1, 0,
    1, 0, 0, 0, 0, 1,
  },

  { 0, 0, 0, 0, 0, 0, ///Frame 1
    0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0,
  },

  { 0, 0, 1, 1, 0, 0, ///Frame 1
    0, 1, 1, 1, 1, 0,
    1, 1, 0, 0, 1, 1,
    1, 0, 0, 0, 0, 1,
    0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0,
  },

  { 0, 0, 0, 0, 0, 0, ///Frame 1
    0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0,
  },

  { 0, 0, 1, 1, 0, 0, ///Frame 1
    0, 1, 1, 1, 1, 0,
    1, 1, 0, 0, 1, 1,
    1, 0, 0, 0, 0, 1,
    0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0,
  },

  { 0, 0, 0, 0, 0, 0, ///Frame 1
    0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0,
  },
};

bool turnleftMap[turnFrames][numLEDS] = {
  
  { 1, 0, 0, 0, 0, 1, ///Frame 1
    0, 1, 0, 0, 1, 0,
    0, 0, 1, 1, 0, 0,
    0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0,
  },
 
  { 0, 0, 0, 0, 0, 0, ///Frame 2
    1, 0, 0, 0, 0, 1,
    0, 1, 0, 0, 1, 0,
    0, 0, 1, 1, 0, 0,
    0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0,
  },

  { 0, 0, 0, 0, 0, 0, ///Frame 3
    0, 0, 0, 0, 0, 0,
    1, 0, 0, 0, 0, 1,
    0, 1, 0, 0, 1, 0,
    0, 0, 1, 1, 0, 0,
    0, 0, 0, 0, 0, 0,
  },

  { 0, 0, 0, 0, 0, 0, ///Frame 4
    0, 0, 0, 0, 0, 0,
    1, 0, 0, 0, 0, 1,
    1, 1, 0, 0, 1, 1,
    0, 1, 1, 1, 1, 0,
    0, 0, 1, 1, 0, 0,
  },

  { 0, 0, 0, 0, 0, 0, ///Frame 5
    0, 0, 0, 0, 0, 0,
    1, 0, 0, 0, 0, 1,
    1, 1, 0, 0, 1, 1,
    0, 1, 1, 1, 1, 0,
    0, 0, 1, 1, 0, 0,
  },

  { 0, 0, 0, 0, 0, 0, ///Frame 6
    0, 0, 0, 0, 0, 0,
    1, 0, 0, 0, 0, 1,
    1, 1, 0, 0, 1, 1,
    0, 1, 1, 1, 1, 0,
    0, 0, 1, 1, 0, 0,
  },
};

bool turnrightMap[turnFrames][numLEDS] = {
  
  { 0, 0, 0, 0, 0, 0, ///Frame 1
    0, 0, 0, 0, 0, 0, 
    0, 0, 0, 0, 0, 0,
    0, 0, 1, 1, 0, 0,
    0, 1, 0, 0, 1, 0,
    1, 0, 0, 0, 0, 1,
  },

  { 0, 0, 0, 0, 0, 0, ///Frame 2
    0, 0, 0, 0, 0, 0,
    0, 0, 1, 1, 0, 0,
    0, 1, 0, 0, 1, 0,
    1, 0, 0, 0, 0, 1,
    0, 0, 0, 0, 0, 0,
  },

  { 0, 0, 0, 0, 0, 0, ///Frame 3
    0, 0, 1, 1, 0, 0,
    0, 1, 0, 0, 1, 0,
    1, 0, 0, 0, 0, 1,
    0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0,
  },

  { 0, 0, 1, 1, 0, 0, ///Frame 4
    0, 1, 1, 1, 1, 0,
    1, 1, 0, 0, 1, 1,
    1, 0, 0, 0, 0, 1,
    0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0,
  },

  { 0, 0, 1, 1, 0, 0, ///Frame 5
    0, 1, 1, 1, 1, 0,
    1, 1, 0, 0, 1, 1,
    1, 0, 0, 0, 0, 1,
    0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0,
  },

  { 0, 0, 1, 1, 0, 0, ///Frame 6
    0, 1, 1, 1, 1, 0,
    1, 1, 0, 0, 1, 1,
    1, 0, 0, 0, 0, 1,
    0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0,
  },
};

bool hazupMap[hazFrames][numLEDS] = {
  
  { 1, 0, 0, 0, 0, 1, ///Frame 1
    0, 1, 0, 0, 1, 0,
    0, 0, 1, 1, 0, 0,
    0, 0, 1, 1, 0, 0,
    0, 1, 0, 0, 1, 0,
    1, 0, 0, 0, 0, 1,
  },

  { 1, 0, 0, 0, 0, 1, ///Frame 2
    0, 1, 0, 0, 1, 0,
    0, 0, 1, 1, 0, 0,
    0, 0, 1, 1, 0, 0,
    0, 1, 0, 0, 1, 0,
    1, 0, 0, 0, 0, 1,
  },

  { 0, 0, 0, 0, 0, 0, ///Frame 3
    0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0,
  },

  { 0, 0, 1, 1, 0, 0, ///Frame 4
    0, 1, 1, 0, 0, 0,
    1, 1, 0, 0, 0, 0,
    1, 1, 0, 0, 0, 0,
    0, 1, 1, 0, 0, 0,
    0, 0, 1, 1, 0, 0,
  },

  { 0, 0, 0, 0, 0, 0, ///Frame 5
    0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0,
  },

  { 0, 0, 1, 1, 0, 0, ///Frame 6
    0, 1, 1, 0, 0, 0,
    1, 1, 0, 0, 0, 0,
    1, 1, 0, 0, 0, 0,
    0, 1, 1, 0, 0, 0,
    0, 0, 1, 1, 0, 0,
  },

  { 0, 0, 0, 0, 0, 0, ///Frame 7
    0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0,
  },
};

bool brakeMap[brakeFrames][numLEDS] = {
  
  { 0, 0, 0, 0, 0, 0, ///Frame 1
    1, 0, 0, 0, 0, 0,
    0, 1, 0, 0, 0, 0,
    0, 1, 0, 0, 0, 0,
    1, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0,
  },

  { 1, 0, 0, 0, 0, 0, ///Frame 2
    0, 1, 0, 0, 0, 0,
    0, 0, 1, 0, 0, 0,
    0, 0, 1, 0, 0, 0,
    0, 1, 0, 0, 0, 0,
    1, 0, 0, 0, 0, 0,
  },

  { 0, 1, 0, 0, 0, 0, ///Frame 3
    0, 0, 1, 0, 0, 0,
    0, 0, 0, 1, 0, 0,
    0, 0, 0, 1, 0, 0,
    0, 0, 1, 0, 0, 0,
    0, 1, 0, 0, 0, 0,
  },

  { 0, 0, 1, 0, 0, 0, ///Frame 4
    0, 0, 0, 1, 0, 0,
    0, 0, 0, 0, 1, 0,
    0, 0, 0, 0, 1, 0,
    0, 0, 0, 1, 0, 0,
    0, 0, 1, 0, 0, 0,
  },

  { 0, 0, 0, 1, 0, 0, ///Frame 5
    0, 0, 0, 0, 1, 0,
    0, 0, 0, 0, 0, 1,
    0, 0, 0, 0, 0, 1,
    0, 0, 0, 0, 1, 0,
    0, 0, 0, 1, 0, 0,
  },

  { 0, 0, 0, 0, 0, 0, ///Frame 6
    0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0,
  },

  { 0, 0, 1, 1, 0, 0, ///Frame 7
    0, 1, 1, 1, 1, 0,
    1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1,
    0, 1, 1, 1, 1, 0,
    0, 0, 1, 1, 0, 0,
  },

  { 0, 0, 0, 0, 0, 0, ///Frame 8
    0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0,
  },

  { 0, 0, 1, 1, 0, 0, ///Frame 9
    0, 1, 1, 1, 1, 0,
    1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1,
    0, 1, 1, 1, 1, 0,
    0, 0, 1, 1, 0, 0,
  },
};

bool standby01Map[2][numLEDS] = {
  
  { 1, 1, 1, 1, 1, 1, ///Frame 1
    1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1,
  },

  { 0, 0, 0, 0, 0, 0, ///Frame 2
    0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0,
  },
};

void setup() {
    
  Serial.begin(115200);
  // setup pins
  pinMode(hazleftbutton, INPUT_PULLUP);
  pinMode(hazrightbutton, INPUT_PULLUP);
  pinMode(turnleftbutton, INPUT_PULLUP);
  pinMode(turnrightbutton, INPUT_PULLUP);
  pinMode(hapticleft, OUTPUT); //haptic feedback for hazleftbutton & turnleftbutton
  pinMode(hapticright, OUTPUT); //haptic feedback for hazlrightbutton & turnrightbutton

  //initialise Accelerometer
  accel.powerOn();

  //initialise Neopixel
  rlight.begin();
  rlight.show();
  
  // Timer0 is already used for millis() - we'll just interrupt somewhere
  // in the middle and call the "Compare A" function below
  OCR0A = 0xAF;
  TIMSK0 |= _BV(OCIE0A);
}

void loop()
{

}

//  Interrupt is called every millisecond, and all button scanning, debouncing, and animations are clocked from here
//  First the inputs are read, starting with the accelerometer for braking
//  The acceleration reading is loaded into a looped array. The whole contents of this array and the averaged
//  This gives a low pass filtered version of the acceleration readings
//  Next the buttons are read. Theyre state is compared to previous states, and debounce times are monitored
//  
//  Once these inputs are all collated, the desired user state is determined, and this is compared to the previous state
// 
SIGNAL(TIMER0_COMPA_vect) 
{
  //check the accelerometers, with averaging
  //int accelState = getAccelState();
  int accelState = 0;


  //check the switches
  unsigned char buttonState = getButtonState();
  

  //run the LEDs, return the current LED function
  setPixels(accelState, buttonState);



  //cycle the haptic controller
  setHaptics(0);
  
}

unsigned char getButtonState()
{
  //these variables are local to this function only, and are preserved
  //this tracks how many milliseconds button was down for
  static unsigned int hazardDebounceCount = 0;
  static unsigned int turnDebounceCount = 0;
  //this tracks the button state (0=not pressed, 1=transitioning, 2= pressed)
  static unsigned char hazardBTNstate = 0;
  static unsigned char turnBTNstate = 0;
  static unsigned char lastHazardState = 0;
  static unsigned char lastTurnState = 0;

  //scan the hazard buttons
  //if there is hazard button activity, we dont care about turn buttons
  unsigned char tempHazardState = 0;
  if(digitalRead(hazleftbutton) == false) tempHazardState += 2;
  if(digitalRead(hazrightbutton) == false) tempHazardState += 4;

  //if there was any hazard button activity, act on it
  if(tempHazardState > 0)
  {
    //compare activity to recent history
    if(hazardBTNstate == 0)
    {
      hazardBTNstate = 1;                 //transitioning
    }

    else if (hazardBTNstate == 1)
    {
      if(hazardDebounceCount++ > buttonDebounceTime) hazardBTNstate = tempHazardState;
    }
  }

  // no hazard button activity so clear counters and state
  else
  {
    hazardDebounceCount = 0;
    hazardBTNstate = 0;
  }

  //check for hazard activity, only bother reading turn signals if theres none
  if(tempHazardState == 0)
  {
    unsigned char tempTurnState = 0;
    if(digitalRead(turnleftbutton) == false) tempTurnState += 8;
    if(digitalRead(turnrightbutton) == false) tempTurnState += 16;

    if(tempTurnState == 24)   //implausibility - ignore
    {
      turnBTNstate = 0;
      turnDebounceCount = 0;
    }

    else if(tempTurnState == 8)
    {
      if(lastTurnState == 8)
      {
        if(turnBTNstate == 0)
        {
          turnBTNstate = 1;
        }

        else if(turnBTNstate == 1)
        {
          if(turnDebounceCount++ > buttonDebounceTime) turnBTNstate = tempTurnState;
        }
      }

      else
      {
        turnDebounceCount = 0;
        turnBTNstate = 0;
      }
    }

    else if(tempTurnState == 16)
    {
      if(lastTurnState == 16)
      {
        if(turnBTNstate == 0)
        {
          turnBTNstate = 1;
        }

        else if(turnBTNstate == 1)
        {
          if(turnDebounceCount++ > buttonDebounceTime) turnBTNstate = tempTurnState;
        }
      }

      else
      {
        turnDebounceCount = 0;
        turnBTNstate = 0;
      }
    }

    else
    {
      turnBTNstate = 0;
      turnDebounceCount = 0;
    }

    lastTurnState = tempTurnState;
  }

  //there was hazard button activity so keep the turn states cleared
  else
  {
    turnBTNstate = 0;
    turnDebounceCount = 0;
  }

  return ((hazardBTNstate | turnBTNstate) & 0b11111110);
}

int getAccelState()
{
  static unsigned char accelHistory[accelHistoryDepth];
  static unsigned char accelHistoryIndex = 0;

  //temp vars for accel values
  int accel_x, accel_y, accel_z;

  //get the latest accel values
  accel.readAccel(&accel_x, &accel_y, &accel_z); 

  //add the z reading to the history array
  //z axis is longitudinal for the bike
  accelHistory[accelHistoryIndex++] = accel_z;
  //check to see if the history index got biger than the array itself
  if (accelHistoryIndex >= accelHistoryDepth) accelHistoryIndex = 0;

  //sum up all the array members and divide by n to give a time averaged result
  int tempSum = 0;
  for(int i = 0; i < accelHistoryDepth; i++) tempSum += accelHistory[i];
  //divide by n
  tempSum /= accelHistoryDepth;
  
  return tempSum;
}

//  sets the bike state based on sensor and button information, and compares this to the previous state
//  factoring the order of precedence of the different outputs, the display frames are clocked as per the current state
void setPixels(int accelState, unsigned char buttonState)
{
  static unsigned char pixelState = 0;         //matches incoming button pattern
  static unsigned char pageCounter = 0;         //tracks which frame in the sequence we're up to
  static unsigned char frameCounter = 0;        //tracks which set of frames we're up to
  static int millisecondCounter = 0;
  static int frameInterval = 0;                     //holder for the delay value between frames
  static const unsigned char turnRepeat = 5;    //sets how many pattern repeats the turn grfx has
  static const unsigned char hazardRepeat = 5;  //sets how many pattern repeats the hazard grfx has
  
  static int hazardHapticTime = 400;
  static int turnHapticTime = 150;

          for (int x = 0; x < numLEDS; x++) {
            rlight.setPixelColor(x, standby01Map[0][x] * 255, 0, 0);
          }
          rlight.show();
          delay(75);
          for (int x = 0; x < numLEDS; x++) {
            rlight.setPixelColor(x, standby01Map[1][x] * 255, 0, 0);
          }
          rlight.show();


  //see if there is a change in state, only if a hazard isnt running
  if((pixelState == 0) || (pixelState > 7))
  {
      //see if it was a change to an on state
      if((buttonState > 0) && (buttonState != pixelState))
      {
        //update to the latest state, only if its not a hazard in progress
   
        pixelState = buttonState;
        
        //reset all the frame and page counters ready for new hecticity
        pageCounter = 0;
        frameCounter = 0;
        millisecondCounter = 0;
        if(pixelState < 8) frameInterval = delayvalhazard;
        else if(pixelState < 25) frameInterval = delayvalturning;
  
        //load the first frame of the new signal
        //left hazard
        if(pixelState == 2)
        {
          for (int x = 0; x < numLEDS; x++) {
          rlight.setPixelColor(x, hazleftMap[0][x] * red);
          }
          rlight.show();
          setHaptics(hazardHapticTime + pixelState);
        }
  
        //right hazard
        else if(pixelState == 4)
        {
          for (int x = 0; x < numLEDS; x++) {
            rlight.setPixelColor(x, hazrightMap[0][x] * 255, 0, 0);
          }
          rlight.show();
          setHaptics(hazardHapticTime + pixelState);
        }
  
        //forward hazard
        else if(pixelState == 6)
        {
          for (int x = 0; x < numLEDS; x++) {
            rlight.setPixelColor(x, hazupMap[0][x] * 255, 0, 0);
          }
          rlight.show();
          setHaptics(hazardHapticTime + pixelState);
        }
  
        //turn left
        else if(pixelState == 8)
        {
          for (int x = 0; x < numLEDS; x++) {
            rlight.setPixelColor(x, turnleftMap[0][x] * amber);
          }
          rlight.show();
          setHaptics(turnHapticTime + pixelState);
        }
  
        //turn right
        else if(pixelState == 16)
        {
           for (int x = 0; x < numLEDS; x++) {
            rlight.setPixelColor(x, turnrightMap[0][x] * amber);
           }
           rlight.show();
           setHaptics(turnHapticTime + pixelState);
        }
         
      }
    }
  

  //see if anything needs to be drawn
  if(pixelState > 0)
  {
    //see if its a hazard signal
    if(pixelState < 8)  
    {
      if(++millisecondCounter > frameInterval)
      {
        millisecondCounter = 0;
        if(++frameCounter >= hazFrames)
        {
          frameCounter = 0;
          if(++pageCounter >= hazardRepeat)
          {
            pageCounter = 0;
            pixelState = 0;
          }
        }

        //we're not at the end so draw next frame
        if(pixelState > 0)
        {
          if(pixelState == 2){
            for (int x = 0; x < numLEDS; x++) {
              rlight.setPixelColor(x, hazleftMap[frameCounter][x] * red);
            }
            rlight.show();
          }

           else if(pixelState == 4){
            for (int x = 0; x < numLEDS; x++) {
              rlight.setPixelColor(x, hazrightMap[frameCounter][x] * red);
            }
            rlight.show();
          }

           else if(pixelState == 6){
            for (int x = 0; x < numLEDS; x++) {
              rlight.setPixelColor(x, hazupMap[frameCounter][x] * red);
            }
            rlight.show();
          }
        }

        //at the end, clear display array
        else
        {
          for (int x = 0; x < numLEDS; x++) {
            rlight.setPixelColor(x, 0, 0, 0);
          }
           rlight.show();
        }
      }
 
  }

    //see if its a turn signal
    else if(pixelState < 25)
    {
      if(++millisecondCounter > frameInterval)
      {
        millisecondCounter = 0;
        if(++frameCounter >= turnFrames)
        {
          frameCounter = 0;
          if(++pageCounter >= turnRepeat)
          {
            pageCounter = 0;
            pixelState = 0;
          }
        }

        //we're not at the end so draw next frame
        if(pixelState > 0)
        {
          if(pixelState == 8){
            for (int x = 0; x < numLEDS; x++) {
              rlight.setPixelColor(x, turnleftMap[frameCounter][x] * amber);
            }
            rlight.show();
          }

           else if(pixelState == 16){
            for (int x = 0; x < numLEDS; x++) {
              rlight.setPixelColor(x, turnrightMap[frameCounter][x] * amber);
            }
            rlight.show();
          }
        }

        //at the end, clear display array
        else
        {
          for (int x = 0; x < numLEDS; x++) {
            rlight.setPixelColor(x, 0, 0, 0);
          }
           rlight.show();
        }
      }
    }
  }
}

// takes any recent changes of state with the buttons and provides user feedback as appropriate
void setHaptics(int runTimeMode)
{
  static int hapticCountdownTimer = 0;
  
  if(runTimeMode > 0)
  {
      int mode = runTimeMode%50;
      hapticCountdownTimer = runTimeMode - mode;
      switch(mode)
      {
          case 2:
            analogWrite(hapticleft, 255);
            break;
          case 4:
            analogWrite(hapticright, 255);
            break;
          case 6:
            analogWrite(hapticleft, 255);
            analogWrite(hapticright, 255);
            break;
          case 8:
            analogWrite(hapticleft, 255);
            break;
          case 16:
            analogWrite(hapticright, 255);
            break;
          default:
            break;
      }
  }   

  // check the state of the haptic countdown timer
  // if it's non zero, it means the motor(s) are currently running and we should decrement the timer by 1
  // if after decrementing the timer, it reaches zero, we should turn the motor(s) off
  if(hapticCountdownTimer > 0) hapticCountdownTimer--;
  
  else
  {
    analogWrite(hapticleft, 0);
    analogWrite(hapticright, 0);
  }
}


                                     
