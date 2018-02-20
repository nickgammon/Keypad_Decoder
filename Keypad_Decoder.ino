// Keypad_Decoder
//
// Author: Nick Gammon
// Date: 17th February 2018

// Outputs to Serial in the format: 0b1nnnnnnn for a key down and 0b0nnnnnnn for a key-up
//            nnnnnnn will be the current row/column combination
//            Also every HEARTBEAT_TIME outputs 0xFF if all keys are up (heartbeat)

#include <limits.h>     /* for CHAR_BIT */

const byte ROWS = 4;
const byte COLS = 4;
const bool ENABLE_PULLUPS = true;  // make false if you are using external pull-ups
const unsigned long DEBOUNCE_TIME = 10;     // milliseconds
const unsigned long HEARTBEAT_TIME = 2000;  // milliseconds
const bool DEBUGGING = false;               // make true for human-readable output

// define here where each row and column is connected to
const byte rowPins [ROWS] = {6, 7, 8, 9}; //connect to the row pinouts of the keypad
const byte colPins [COLS] = {2, 3, 4, 5}; //connect to the column pinouts of the keypad


// See: http://c-faq.com/misc/bitsets.html

#define BITMASK(b) (1 << ((b) % CHAR_BIT))
#define BITSLOT(b) ((b) / CHAR_BIT)
#define BITSET(a, b) ((a)[BITSLOT(b)] |= BITMASK(b))
#define BITCLEAR(a, b) ((a)[BITSLOT(b)] &= ~BITMASK(b))
#define BITTEST(a, b) ((a)[BITSLOT(b)] & BITMASK(b))

// number of items in an array
#define ARRAY_SIZE(x) (sizeof(x) / sizeof((x)[0]))

// total number of keys
const byte TOTAL_KEYS = ROWS * COLS;

// remember previous setting of each key
char lastKeySetting [(TOTAL_KEYS + CHAR_BIT) / CHAR_BIT];  // one bit each, 0 = up, 1 = down
unsigned long lastKeyTime [TOTAL_KEYS];       // when that key last changed
unsigned long lastHeartbeat;                  // when we last sent the heartbeat

void setup ()
{
  Serial.begin (115200);
  while (!Serial) { }  // wait for Serial to become ready (Leonardo etc.)

  // set each column to input-pullup (optional)
  if (ENABLE_PULLUPS)
    for (byte i = 0; i < COLS; i++)
      pinMode (colPins [i], INPUT_PULLUP);

}  // end of setup

void loop ()
  {
  byte keyNumber = 0;
  unsigned long now = millis ();  // for debouncing

  // check each row
  for (byte row = 0; row < ROWS; row++)
    {
    // set that row to OUTPUT and LOW
    pinMode (rowPins [row], OUTPUT);
    digitalWrite (rowPins [row], LOW);

    // check each column to see if the switch has driven that column LOW
    for (byte col = 0; col < COLS; col++)
      {
      // debounce - ignore if not enough time has elapsed since last change
      if (now - lastKeyTime [keyNumber] >= DEBOUNCE_TIME)
        {
        bool keyState = digitalRead (colPins [col]) == LOW; // true means pressed
        if (keyState != (BITTEST (lastKeySetting, keyNumber) != 0)) // changed?
          {
          lastKeyTime [keyNumber] = now;  // remember time it changed
          // remember new state
          if (keyState)
            BITSET (lastKeySetting, keyNumber);
          else
            BITCLEAR (lastKeySetting, keyNumber);
          if (DEBUGGING)
            {
            Serial.print (F("Key "));
            Serial.print (keyNumber);
            if (keyState)
              Serial.println (F(" down."));
            else
              Serial.println (F(" up."));
            }  // if debugging
          else
            Serial.write ((keyState ? 0x80 : 0x00) | keyNumber);
          }  // if key state has changed
        }  // debounce time up
      keyNumber++;
      } // end of for each column

    // put row back to high-impedance (input)
    pinMode (rowPins [row], INPUT);
    }  // end of for each row

  // Send a heartbeat code (0xFF) every few seconds in case
  // the receiver loses an occasional keyup.
  // Only send if all keys are not pressed (presumably the normal state).
  if (now - lastHeartbeat >= HEARTBEAT_TIME)
    {
    lastHeartbeat = now;
    bool allUp = true;
    for (byte i = 0; i < ARRAY_SIZE (lastKeySetting); i++)
      if (lastKeySetting [i])
        allUp = false;
    if (allUp)
      {
      if (DEBUGGING)
        Serial.println (F("No keys pressed."));
      else
        Serial.write (0xFF);
      } // end of all keys up
    } // end if time for heartbeat
  } // end of loop



