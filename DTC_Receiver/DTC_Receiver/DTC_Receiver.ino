#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

//
// Hardware configuration
//

// Set up nRF24L01 radio on SPI bus plus pins 9 & 10
// Pin 9 = CE, Chip Enable
// Pin 10 = CS, Chip Select

RF24 radio(9, 10);
int k = 0;
// Pins on the LED board for LED's
const uint8_t led_pins[] = { 3 };
const uint8_t num_led_pins = sizeof(led_pins);
// Single radio pipe address for the 2 nodes to communicate.
const uint64_t pipe = 0xDEADBEEF00LL;
const uint8_t num_button_pins = 1; // I know there is only one status I need to receive.
uint8_t button_states[num_button_pins];
uint8_t led_states[num_led_pins];


void setup(void)
{
  // Refer to https://www.arduino.cc/en/Reference/SPI
  // You must declare pin 10 (SS) as an output.
  // Otherwise nothing works.
  pinMode(10, OUTPUT);
  //
  // Setup and configure rf radio
  //
  radio.begin();
  //radio.setDataRate(RF24_1MBPS);
  //radio.setDataRate(RF24_250KBPS);
  radio.setChannel(110);
  radio.openReadingPipe(1, pipe);
  radio.startListening();

  // Turn LED's OFF until we start getting keys
  // Adaptation for DTC mute. Only need one LED.
  pinMode(3, OUTPUT);
  digitalWrite(3, LOW);
}

//
// Loop
//

void loop(void)
{
  //
  // LED role.  Receive the state of all buttons, and reflect that in the LEDs
  //


  // if there is data ready
  if ( radio.available() )
  {
    // Dump the payloads until we've gotten everything
    bool done = false;
    while (!done)
    {
      // Fetch the payload, and see if this was the last one.
      done = radio.read( button_states, num_button_pins );
      // For each button, if the button is now on, then toggle the LED
      int i = num_button_pins;
      while (i--)
      {
        if ( button_states[i] )
        {
          led_states[i] ^= HIGH; // I think this is an XOR operation
          // I think that can be re-written like this:
          // led_states[i] = led_states[i] ^ HIGH;
          // Which sets led_state[i] = to result of XOR operation
          digitalWrite(led_pins[i], led_states[i]);
        }
      }
    }
  }
}

