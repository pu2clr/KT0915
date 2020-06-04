#include <KT0915.h>


KT0915 radio; 

void setup() {

  Serial.begin(9600);
  while(!Serial);

  if ( !checkI2C() ) {
    Serial.println("Check your circuit!");
    while(1);
  }

  Serial.println("CHIP ID....................:");
  Serial.print(radio.getDeviceId());

  radio.setup(-1, OSCILLATOR_32KHZ);

  Serial.println("Crystal Ready..............:");
  Serial.print(radio.isCrystalReady());

}

void loop() {
  // put your main code here, to run repeatedly:

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
