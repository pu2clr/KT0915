/*
   Test and validation of the PU2CLR KT0915 Arduino Library.
   It is a FM, MW (520 to 1710Khz)
   
   ATTENTION:  Please, avoid using the computer connected to the mains during testing.

   The main advantages of using this sketch are: 
    1) It is a easy way to check if your circuit is working;
    2) You do not need to connect any display device to make your radio works;
    3) You do not need connect any push buttons or encoders to change volume and frequency;
    4) The Arduino IDE is all you need to control the radio.  
   
   This sketch has been successfully tested on:
    1) Pro Mini 3.3V; 
    2) UNO (by using a voltage converter); 

  KT0915 and Arduino Pro Mini wire up
  
   | KT0915 pin     | Arduino pin | Description | 
   | -----------    | ----------- | ----------- |
   | ENABLE (pin 9) |   12        | It is optional. The KT0915 pin 9 can be connected to +Vcc directly |
   | CLK  (pin 14)  |   A5        |             |
   | SDA  (pin 15)  |   A4        |             |


  I strongly recommend starting with this sketch.

  See schematic: https://pu2clr.github.io/KT0915/

  PU2CLR KT0915 API documentation: https://pu2clr.github.io/KT0915/extras/docs/html/index.html

  By Ricardo Lima Caratti, 2020.
*/

#include <KT0915.h>

#define RADIO_ENABLE_PIN 12    // When this pin is high, the radio becomes enable. You can set -1 to control or setup it via circuit.

uint32_t currentFM = 103900;
uint32_t currentAM = 810;
uint32_t currentFrequency;

KT0915 radio; 

// Set your FM station frequency. Default is 107.5MHz (107500KHz)
uint32_t fmFreq = 106500;

void setup() {

  Serial.begin(9600);
  while(!Serial);

  radio.setup(RADIO_ENABLE_PIN, OSCILLATOR_32KHZ,0);
  radio.setVolume(25);

  currentFrequency = currentFM;
  radio.setFM(84000, 108000, currentFrequency, 100);
  showHelp();
  showStatus();
}


void showHelp()
{
  Serial.print("\nCommands\n");
  Serial.println("==================================================");
  Serial.println("Type F to FM; A to MW; 1 to SW");
  Serial.println("Type U to increase and D to decrease the frequency");
  Serial.println("Type S or s to seek station Up or Down");
  Serial.println("Type + or - to volume Up or Down");
  Serial.println("Type 0 to show current status");
  Serial.println("Type ? to this help.");
  Serial.println("==================================================");
  delay(1000);
}

// Show current frequency
void showStatus()
{

  float freq;

  currentFrequency = radio.getFrequency();

  freq = (radio.getCurrentMode() == MODE_FM) ? currentFrequency / 1000.0 : currentFrequency / 1.0;

  Serial.print("\nYou are tuned on ");
  Serial.print(freq);
  // Serial.print("MHz - RSSI: ");
  // Serial.print(radio.getRSSI());
  // Serial.print("V - Volume: ");
  // Serial.print(radio.getVolume());
}

void loop()
{

  if (Serial.available() > 0)
  {
    char key = Serial.read();
    switch (key)
    {
    case '+':
      radio.setVolumeUp();
      break;
    case '-':
      radio.setVolumeDown();
      break;
    case 'a':
    case 'A':
      currentFM = currentFrequency;
      radio.setAM(550, 1710, currentAM, 10);
      break;
    case 'f':
    case 'F':
      currentAM = currentFrequency;
      radio.setFM(87000, 108000, currentFM, 100);
      break;
    case 'U':
    case 'u':
      radio.frequencyUp();
      break;
    case 'D':
    case 'd':
      radio.frequencyDown();
      break;
    case 'S':
      // radio.seekStation();
      break;
    case 's':
      // radio.seekStation(0);
      break;
    case '0':
      showStatus();
      break;
    case '?':
      showHelp();
      break;
    default:
      break;
    }
    showStatus();
  }
}
