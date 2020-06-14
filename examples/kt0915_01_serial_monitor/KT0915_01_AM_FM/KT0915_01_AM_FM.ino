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
    case '1':
      radio.setAM(7000, 7500, 7205, 5);
      break;
    case '2':
      radio.setAM(11500, 12000, 11940, 5);      
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
      // radio.seekStation(1);
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
