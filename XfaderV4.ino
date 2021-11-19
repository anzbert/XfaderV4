// Thanks to gustavosilveira@musiconerd.com -> http://github.com/silveirago/ for his code examples
// Use with ATmega32u4 - such as Micro, Pro Micro, Leonardo

// Uses MIDIUSB library for Arduinos with ATmega32u4 chip: (https://github.com/arduino-libraries/MIDIUSB)
#include <MIDIUSB.h>

// Analogread library by Damien Clarke to reduce Analog Noise while maintaining accuracy and responsiveness
// (https://github.com/dxinteractive/ResponsiveAnalogRead)
#include <ResponsiveAnalogRead.h>

// EEPROM library to use permanent storage for xfader calibration
#include <EEPROM.h>

const int powerled = {9};  // powerled PWM pin, if using one
const int calbutton = {2}; // pin of calibration button

// !! MIDI SETTINGS !! //
byte midich = -1 + 10; //* MIDI channel to be used (1-16). default: 10
byte note = 0;         //* Lowest note to be used for buttons and LEDs (0-127)
byte cc = 0;           //* Lowest MIDI CC to be used // (0-127)
byte ccxfader = 10;    // Xfader CC (0-127)

/////////////////////////////////////////////////
// MIDI LEDS - code currently for digital on/off output
const int nled = 0;          // *number of leds
const int ledpin[nled] = {}; //* the number of the LED pins in the desired order - correspond with closest button for easy mapping

// Working variables used for receiving midi notes for LED control
byte rxnoteandchannel = 0;
byte rxchannel = 0;
byte rxnote = 0;
byte rxpitch = 0;
byte rxvelocity = 0;

////////////
// XFADER //
const int xpin = A0;                             // define the xfader pin you want to use for ResponsiveAnalogRead
ResponsiveAnalogRead analogOne(xpin, true, 0.8); // arguments: analogNUMBER(analoginput pin, sleep true/false, amount of snap [default 0.01])

// From Documentation:
// Make a ResponsiveAnalogRead object, pass in the pin, and either true or false depending on if you want sleep enabled
// enabling sleep will cause values to take less time to stop changing and potentially stop changing more abruptly,
// where as disabling sleep will cause values to ease into their correct position smoothly and with slightly greater accuracy.
//
// The next optional argument is snapMultiplier, which is set to 0.01 by default
// you can pass it a value from 0 to 1 that controls the amount of easing
// increase this to lessen the amount of easing (such as 0.1) and make the responsive values more responsive
// but doing so may cause more noise to seep through if sleep is not enabled

// Xfader calibration settings
const int safezone = 3;                     // safezone size added to each end of the calibrated area (scale is 0-1023)
const unsigned long holdtoreverse = 400;    // milliseconds from button press until direction is reversed
const unsigned long holdtocalibrate = 3000; // milliseconds from button press until calibration starts
bool reversedirection = true;               // Default direction of fader on startup. Depends on how the fader is physically installed and connected to the Arduino.

// working variables for xfader
int xminfader = 0;      // range variable MIN 10bit
int xmaxfader = 0;      // range variable MAX 10bit
int xrawstate = 0;      // Raw Analog input 10bit
int xsmoothedstate = 0; // ResponsiveAnalogRead output 10bit
int xmidicstate = 0;    // Current state of the CC midi value 7bit
int xmidipstate = 0;    // Previous loop state of the CC midi value 7bit

// working variables for xfader calibration
int sensorvalue = 0;       // read analoginput during calibration 10bit
bool xcalisuccess = false; // test variable for calibration success
unsigned long starttime;   // start variable for timer for xcalibration
unsigned long currenttime; // test variable for timer for xcalibration

// blinking during calibration
const long interval = 250;        // blink speed in ms
unsigned long previoustime = 250; // timer variable
int ledstate = 0;

// introduce powerled brightness variable
int plbrightness = 0;

////////////////////////////////////
/////////////////// !! SETUP !! ////

void setup()
{
  Serial.begin(9600); // turns on serial readout for debugging and sets transfer rate

  pinMode(powerled, OUTPUT);           // sets power led pin to output mode, if using one
  analogWrite(powerled, plbrightness); // turns powerled ON

  pinMode(calbutton, INPUT_PULLUP); // sets pullup resistor mode for calibration button pin

  for (int i = 0; i < nled; i++)
  {
    pinMode(ledpin[i], OUTPUT); // sets all led pins to output mode
    analogWrite(ledpin[i], 0);  // turns all leds off during setup
  }

  // get fader calibration range from permanent storage
  EEPROM.get(0, xminfader); // INT on bytes 0-1
  EEPROM.get(2, xmaxfader); // INT on bytes 2-3
}

/////////////////////////////////
//////////// !! LOOP !! /////////

void loop()
{
  xfader();     // xfader input to midi out
  xcalipress(); // calibration button function
}

///////////////////////
/////////// XFADER
void xfader()
{

  analogOne.update();                    // update the ResponsiveAnalogRead object input
  xrawstate = analogOne.getRawValue();   // raw input
  xsmoothedstate = analogOne.getValue(); // ResponsiveAnalogRead output

  if (reversedirection == true)
  {                                                                                        // check currently set fader direction
    xmidicstate = map(xsmoothedstate, xminfader + safezone, xmaxfader - safezone, 127, 0); // Maps the current reading of the Xfader to a 7bit Midi value (in reverse)
  }
  else
  {
    xmidicstate = map(xsmoothedstate, xminfader + safezone, xmaxfader - safezone, 0, 127); // Maps the current reading of the Xfader to a 7bit Midi value
  }

  xmidicstate = constrain(xmidicstate, 0, 127); // Make sure reading stays within 7bit

  plbrightness = map(xmidicstate, 0, 127, 47, 255);
  analogWrite(powerled, plbrightness);

  if (xmidipstate != xmidicstate)
  { // only send CC change if midistate has changed

    controlChange(midich, ccxfader, xmidicstate); // send control change (channel, CC, value) to Midi Buffer
    MidiUSB.flush();                              // flush Midi Buffer (-> send CC)

    xmidipstate = xmidicstate; // Stores the current Midistate to compare with the next on next loop

    // DEBUG xfader
    Serial.print("Midistate: ");
    Serial.print(xmidicstate); // print midi position
    Serial.print(" / Resp-Analogreading: ");
    Serial.print(xsmoothedstate); // print 10bit reading
    Serial.print(" / RAW-Analogreading: ");
    Serial.println(xrawstate); // print 10bit reading

    //DEBUG mem storage - print EEPROM content whenever in center position
    //    int temp1 = 0;
    //    int temp2 = 0;
    //    if (xsmoothedstate > 500 && xsmoothedstate < 510 ) {
    //      Serial.print("Range in operational variables ");
    //      Serial.print(xminfader);
    //      Serial.print(" - ");
    //      Serial.println(xmaxfader);
    //
    //
    //      Serial.print("Range in EEPROM: ");
    //      Serial.print(EEPROM.get( 0, temp1 ));
    //      Serial.print(" - ");
    //      Serial.println(EEPROM.get( 2, temp2 )); }
  }
}

/////////////////////////////////
///// XFADER CALIBRATION BUTTON
void xcalipress()
{
  if (digitalRead(calbutton) == LOW)
  {
    starttime = millis(); // get initial start time

    while (digitalRead(calbutton) == LOW)
    {
      currenttime = millis(); //get the current "time"

      if (currenttime - starttime >= holdtocalibrate) //test whether the hold to calibrate time has elapsed
      {

        // Reset min and max to center of xfader before calibration
        xmaxfader = 512;
        xminfader = 511;
        xcalisuccess = true;

        while (digitalRead(calbutton) == LOW)
        {
          analogOne.update();                            // update the ResponsiveAnalogRead object during while-loop
          sensorvalue = analogOne.getValue();            // read crossfader value with responsive analogread library
          sensorvalue = constrain(sensorvalue, 0, 1023); // keep input within 10bit range

          // DEBUG xfader calibration
          //          Serial.print("sensorvalue: ");
          //          Serial.println(sensorvalue); // print midi position

          // record the maximum sensor value
          if (sensorvalue > xmaxfader)
          {
            xmaxfader = sensorvalue;
          }

          // record the minimum sensor value
          if (sensorvalue < xminfader)
          {
            xminfader = sensorvalue;
          }

          // DEBUG calibration
          //          Serial.print("New Range in operational variables ");
          //          Serial.print(xminfader);
          //          Serial.print(" - ");
          //          Serial.println(xmaxfader);

          //  BLINK the powerLED slowly during calibration
          if (millis() - previoustime >= interval)
          {
            previoustime = millis(); // save the last time you blinked the LED

            // if the LED is off turn it on and vice-versa:
            if (ledstate == 0)
            {
              ledstate = plbrightness;
            }
            else
            {
              ledstate = 0;
            }
            analogWrite(powerled, ledstate); // set the LED with the ledstate variable:
          }
        }
      }

      if (currenttime - starttime > holdtoreverse) //test whether the hold to reverse time has elapsed
      {
        analogWrite(powerled, 0); // turn the powerLED off
      }
    }
    // DEBUG calibration
    Serial.print("New FINAL Range in operational variables ");
    Serial.print(xminfader);
    Serial.print(" - ");
    Serial.println(xmaxfader);

    // save new calibration to permanent storage if calibration successful
    if (xcalisuccess == true)
    {
      analogWrite(powerled, 0); // turn the powerLED off

      EEPROM.put(0, xminfader); // position 0 (INT variable will use byte 0 and 1)
      delay(10);                // saving to eeprom takes about 3ms - delay just to be save :)
      EEPROM.put(2, xmaxfader); // position 2 (INT variable will use byte 2 and 3)
      delay(10);                // saving to eeprom takes about 3ms - delay just to be save :)

      Serial.println("success");

      // debug eeprom
      int temp1 = 0;
      int temp2 = 0;
      Serial.print("Range in EEPROM: ");
      Serial.print(EEPROM.get(0, temp1));
      Serial.print(" - ");
      Serial.println(EEPROM.get(2, temp2));

      delay(600);
      analogWrite(powerled, plbrightness); // turn the powerLED back on permanently
      xcalisuccess = false;                // turn calivariable back off
    }

    // if button press time between hold to reverse and hold to calibrate -> reverse direction
    if (currenttime - starttime < holdtocalibrate && currenttime - starttime > holdtoreverse)
    {
      reversedirection = !reversedirection; // reverse xfader direction

      //  BLINK the powerLED 2 times fast to confirm reversal
      for (int count = 0; count <= 2; count++)
      {
        analogWrite(powerled, 0);            // turn the powerLED off
        delay(80);                           // wait
        analogWrite(powerled, plbrightness); // turn the powerLED on
        delay(80);                           // wait
      }
    }
  }
}

////////////////////////////////////////////////////////////////////////////////////////////
// Arduino (pro)micro midi functions MIDIUSB Library for sending CCs and noteON and noteOFF
void noteOn(byte channel, byte pitch, byte velocity)
{
  midiEventPacket_t noteOn = {0x09, 0x90 | channel, pitch, velocity};
  MidiUSB.sendMIDI(noteOn);
}

void noteOff(byte channel, byte pitch, byte velocity)
{
  midiEventPacket_t noteOff = {0x08, 0x80 | channel, pitch, velocity};
  MidiUSB.sendMIDI(noteOff);
}

void controlChange(byte channel, byte control, byte value)
{
  midiEventPacket_t event = {0x0B, 0xB0 | channel, control, value};
  MidiUSB.sendMIDI(event);
}
