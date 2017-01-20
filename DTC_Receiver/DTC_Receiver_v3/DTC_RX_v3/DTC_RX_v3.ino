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
char* rx_data = "NONE";
uint8_t rx_len = 0;
// CHANNEL NUMBER
const unsigned int chnl = 100; // Channel to use. 127 is max.

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
  radio.setChannel(chnl);
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
  // Check if there is data ready
  if ( radio.available() )
  {   
    bool done = false;
    while (!done)
    {     
      // Fetch the payload, and only use the last one.
      done = radio.read( rx_data, rx_len );
    }
      if (( strncmp(rx_data, "Enable", rx_len) == 0)  && (digitalRead(3) == 0)){       
          digitalWrite( 3, HIGH );
          delay(100);
          rx_data = "NONE";
         rx_len = 0;
      }else
      if (( strncmp(rx_data, "Off", rx_len) == 0 ) && (digitalRead(3) == 1)){
        // Check to see if pin is already low.
          digitalWrite( 3, LOW );         
          rx_data = "NONE";
          rx_len = 0;
      }   
      rx_data = "NONE";
      rx_len = 0;
    
  }

}

