/* 
 *  Modified from SevSeg Counter Example and library
    10-09-2022
*/

#include "SevSeg.h"
SevSeg sevseg; //Instantiate a seven segment controller object

// constants won't change. They're used here to set pin numbers:
const int resetPin = A0;     // the input for the pushbutton pin
const int basketPin = A1; // sensor input for scoring a basket

void setup() {
  Serial.begin(115200);
  pinMode(resetPin, INPUT);    // sets the analog pin 0 as input
  pinMode(basketPin, INPUT);   // sets the analog pin 1 as input

  byte numDigits = 4;
  byte digitPins[] = {2, 3, 4, 5};
  byte segmentPins[] = {6, 7, 8, 9, 10, 11, 12, 13};
  bool resistorsOnSegments = false; // 'false' means resistors are on digit pins
  //byte hardwareConfig = COMMON_ANODE; // See README.md for options
  byte hardwareConfig = COMMON_CATHODE; // ** Changed this value, display is common cathode**
  bool updateWithDelays = false; // Default 'false' is Recommended
  bool leadingZeros = true; // Use 'true' if you'd like to keep the leading zeros
  bool disableDecPoint = false; // Use 'true' if your decimal point doesn't exist or isn't connected
  sevseg.begin(hardwareConfig, numDigits, digitPins, segmentPins, resistorsOnSegments,
               updateWithDelays, leadingZeros, disableDecPoint);

  sevseg.setBrightness(90);
}

void loop() {
  static unsigned long timer = millis();

  // variables used for countdown timer
  static int deciSeconds = 30;
  static int displayNum = 0;
  static int val;

  // variables used for points tracking ** Momentary limit switch with long arm in hoop **
  static int points = 0;
  static int basketState = 1;
  static int lastBasketState = 1;
  static unsigned long lastDebounceTime = 0; // the last time the output pin was toggled
  static unsigned long debounceDelay = 50; // the debounce time; increase if the output flickers

  // read basket score pin
  val = digitalRead(basketPin);
  //Serial.println(val); // Console log testing scoring

  /*
     How point scoring works:
     basketState is normally 1, basketState is 0 when sensor detects ball.
     When basketState changes, reset debounce time
     When basketState exceeds debounce time and is going from 1 to 0 state (score action starts) add a point.
     Anything else do nothing
  */

  // basket state was changed reset debounce timer
  if (val != lastBasketState) {
    lastDebounceTime = millis();
  }

  // whatever the reading is at, it's been there for longer than the debounce
  // delay, so take it as the actual current state:
  if ((millis() - lastDebounceTime) > debounceDelay) {

    // if the button state has changed:
    if (val != basketState) {
      //change basket state
      basketState = val;
      // If basket state changed back to 1 and there is time on the clock add the point
      if (basketState == 0 && deciSeconds > 0) {
        points++;
        Serial.println(points);
      }
    }
  }
  lastBasketState = val;

  // read the input pin and reset the game when pressed
  val = digitalRead(resetPin);
  //Serial.println(val); // Console log testing reset button
  if (val == 0) {
    deciSeconds = 30;
    points = 0;
    timer = millis();
  }

  // Remove a second from the timer every second that goes by
  if (millis() - timer > 1000) {
    timer += 1000; // Adjusted to 1000 milliseconds is equal to 1 decisecond
    if (deciSeconds > 0) { // If seconds are still greater than zero keep counting
      deciSeconds--;
    }
  }

  // Update the display
  displayNum = (points * 100) + deciSeconds; // Setting the display value, first two digits are points, last two digits are seconds left
  sevseg.setNumber(displayNum, 2);
  sevseg.refreshDisplay(); // Must run repeatedly
}
