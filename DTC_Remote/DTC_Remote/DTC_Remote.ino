#include <SPI.h>
#include <avr/sleep.h>
//#include <RF24_config.h>
#include <nRF24L01.h>
#include <RF24.h>

// Dedicated DTC Remote Controller software
// Written by Mike Studer
// Sep 2015

int sleepStatus = 0;
bool timed_mute = false;
bool manual_mute = false;
bool MUTE_state = false; // Is the LED lit?
bool LINK_BAD = false; // RF link status
bool delay_running = false;
const char* hstate = "HIGH";
const char* lstate = "LOW";
// Timer delays ( in milliseconds )
const unsigned long d0 = 240000; // Baseline delay of 4 minutes
const unsigned long d4 = 30000; // 30 sec
const unsigned long d5 = 60000; // 60 sec
const unsigned long d6 = 60000; // 60 sec
unsigned long total_delay = 0;
const unsigned int chnl = 100; // Channel to use. 127 is max.
//
// Hardware configuration
//
// Set up nRF24L01 radio on SPI bus plus pins 9 & 10
// Pin 9 = CE, Chip Enable
// Pin 10 = CS, Chip Select
RF24 radio(9, 10); //CE, CS
// Pins on the REMOTE for buttons
const uint8_t button_pins[] = { 2, 3, 4, 5, 6 };
// Pin 2 = Manual Mute
// Pin 3 = Timed Mute
// Pin 4 = +X Minute delay
// Pin 5 = +Y Minute delay
// Pin 6 = +Z Minute delay
const uint8_t num_button_pins = sizeof(button_pins);
const uint64_t pipe = 0xDEADBEEF00LL; // Radio address
uint8_t button_states[num_button_pins];

void tm()
{
  timed_mute = true; // IRPT has occurred
}

void mm()
{
  manual_mute = true; // IRPT has occurred
}

void LED_1_blink()
{
  // Shows RF link status
  for(int k = 0; k < 4; k++){
  digitalWrite(7, HIGH); // LED 1 ON
  delay(30);
  digitalWrite(7, LOW); // LED 1 OFF
  delay(30);
  }
}

void LED_2_ON()
{
  digitalWrite(8, HIGH); // LED 2 ON
}

void LED_2_OFF()
{
  digitalWrite(8, LOW); // LED 2 ON
}

// ****************************************************************
// Setup
// ****************************************************************
void setup(void)
{
  // Refer to https://www.arduino.cc/en/Reference/SPI
  // You must declare pin 10 (SS) as an output.
  // Otherwise nothing works.
  pinMode(10, OUTPUT);
  pinMode(7, OUTPUT); // LED 1
  pinMode(8, OUTPUT); // LED 2
  digitalWrite(7, LOW); // LED 1 OFF
  digitalWrite(8, LOW); // LED 2 OFF
  radio.begin();
  radio.setChannel(chnl);
  radio.openWritingPipe(pipe);
  int i = num_button_pins;
  while (i--)
  {
    pinMode(button_pins[i], INPUT); // Set pins as INPUTs
    digitalWrite(button_pins[i], HIGH); // Set button high for OFF
  }
  attachInterrupt(digitalPinToInterrupt(2), mm, FALLING); // aka Pin 2, Manual Mute
  attachInterrupt(digitalPinToInterrupt(3), tm, FALLING); // aka Pin 3, Timed Mute
}
// ****************************************************************
void sleepNow()
{ 
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);   // sleep mode is defined here
  //RF24 powerDown();
  // Insure interrupts are enabled
  //attachInterrupt(digitalPinToInterrupt(2), mm, FALLING); // aka Pin 2, Manual Mute
  //attachInterrupt(digitalPinToInterrupt(3), tm, FALLING); // aka Pin 3, Timed Mute
  sleep_enable();   // makes sleep mode possible. Just a safety pin
  sleep_mode();            // here the device is actually put to sleep!!
  // Device is sleeping here.
  // Zzzz (Execution is PAUSED until an interrupt occurs)
  // UhOh someone just woke me up.
  sleep_disable();         // Put safety pin back in.
  //RF24 powerUp();
  // THE PROGRAM CONTINUES FROM HERE AFTER WAKING UP
}

void runDelay()
{
  LED_2_ON();
  timed_mute = false;
  total_delay = d0;
  if ( !digitalRead(4) )
    total_delay += d4;
  if ( !digitalRead(5) )
    total_delay += d5;
  if ( !digitalRead(6) )
    total_delay += d6;
  //total_delay = 2000;  // DEBUG 2 sec
  bool ok = radio.write( hstate, 4 );  // MUTE ON
  if ( !ok )
  {
    LED_1_blink();
  }
  MUTE_state = true;
  delay_running = true;
  // Enter delay routine
  unsigned long dly = 25;
  while ( (total_delay > 0) && MUTE_state && delay_running && !manual_mute && !timed_mute ) {
    delay(dly); // Do a short delay to allow for interrupt checks
    total_delay -= dly;
  }
  // Delay has ended
  if ( !manual_mute ) {
    ok = radio.write( lstate, 3 );  // MUTE OFF
    if ( !ok )
    {
      LED_1_blink();
    }
    LED_2_OFF();
    MUTE_state = false;
    timed_mute = false;
    delay_running = false;
  }
}

// ****************************************************************
// Loop
// ****************************************************************
void loop(void)
{
  delay(100); // Let everything settle.
  // Send a command to make sure mute is off
  bool ok = radio.write( lstate, 3 );  // MUTE OFF
  if ( !ok )
  {
    LED_1_blink();
  }
  while (1)
  {

    if ( timed_mute )
    {
      // IRPT has occurred
      if ( !MUTE_state )
      {
        runDelay(); // Not muted so do it
      } else
      {
        // UhOh already in a timed mute delay cycle
        // User must want to abort
        bool ok = radio.write( lstate, 3 );  // MUTE OFF
        if ( !ok )
        {
          LED_1_blink();
        }
        timed_mute = false;
        MUTE_state = false;
        delay_running = false;
        LED_2_OFF();
      }
    }

    if ( manual_mute && !MUTE_state)
    {
      LED_2_ON();
      bool ok = radio.write( hstate, 4 );  // MUTE ON
      if ( !ok )
      {
        LED_1_blink();
      }
      MUTE_state = true;
      manual_mute = false;
      delay_running = false;
    }
    
    if ( manual_mute && MUTE_state )
    {
      bool ok = radio.write( lstate, 3 );  // MUTE OFF
      if ( !ok )
      {
        LED_1_blink();
      }
      LED_2_OFF();
      MUTE_state = false;
      manual_mute = false;
      delay_running = false;
    }
    RF24 powerDown();
    delay(100);
    sleepNow();  // Power Saver Mode
    RF24 powerUp();
    delay(100);
  }
}

