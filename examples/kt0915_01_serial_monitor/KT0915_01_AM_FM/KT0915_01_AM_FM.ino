#include <KT0915.h>

#define RESET_PIN 12

KT0915 radio; 

// Set your FM station frequency. Default is 107.5MHz (107500KHz)
uint32_t fmFreq = 106500;

void setup() {

  Serial.begin(9600);
  while(!Serial);

  if ( !checkI2C() ) {
    Serial.println("Check your circuit!");
    while(1);
  }

  Serial.print("\nCHIP ID....................:");
  Serial.print(radio.getDeviceId(),HEX);

  radio.setup(RESET_PIN, OSCILLATOR_32KHZ,0);
  
  Serial.print("\nCrystal Ready..............:");
  Serial.print(radio.isCrystalReady());

  radio.setVolume(23);
  // Sets the FM mode and tune on fmFreq and 100KHz step
  radio.setFM(84000,108000,fmFreq,100);
  radio.setFrequency(fmFreq);
  Serial.print("\nYou are tuned on ");
  Serial.print(radio.getFrequency()/1000.0);
  Serial.println(" MHz");
  Serial.print("\nIf you got here and are listening to a station, your circuit is probably ok!");
  Serial.print("\nIf you are not listening to a station, try change the value of: fmFreq variable!");
 
}

void loop() {
  

}

/**
 * Returns true if device found
 */
bool checkI2C() {
  byte error, address;
  int nDevices;
  Serial.println("I2C bus Scanning...");
  nDevices = 0;
  for(address = 1; address < 127; address++ ) {
    Wire.beginTransmission(address);
    error = Wire.endTransmission();
    if (error == 0) {
      Serial.print("\nI2C device found at address 0x");
      if (address<16) {
        Serial.print("0");
      }
      Serial.println(address,HEX);
      nDevices++;
    }
    else if (error==4) {
      Serial.print("\nUnknow error at address 0x");
      if (address<16) {
        Serial.print("0");
      }
      Serial.println(address,HEX);
    }    
  }
  if (nDevices == 0) {
    Serial.println("No I2C devices found\n");
    return false;
  }
  else {
    Serial.println("done\n");
    return true;
  }
}
