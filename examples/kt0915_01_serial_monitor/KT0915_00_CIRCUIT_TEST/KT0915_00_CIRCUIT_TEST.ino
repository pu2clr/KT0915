/*
   Test and validation of the circuit.
   This sketch will check the I2C communication bus and try to tune the receiver on a given frequency.
 
   ATTENTION:  Please, avoid using the computer connected to the mains during testing.

   The main advantages of using this sketch are: 
    1) It is a easy way to check if your circuit is working;
    2) You do not need to connect any display device to make your radio works;
    3) You do not need connect any push buttons or encoders to change volume and frequency;
    4) The Arduino IDE is all you need to check your circuit.  
   
   This sketch has been successfully tested on:
    1) Pro Mini 3.3V; 
    2) UNO (by using a voltage converter); 

    The table below shows the Si4735 and Arduino Pro Mini pin connections 
    
    | KT0915 pin      |  Arduino Pin  |
    | ----------------| ------------  |
    | ENABLE (pin )   |     12        |
    | SDA  (pin 15)   |     A4        |
    | CLK  (pin 16)   |     A5        |


  I strongly recommend starting with this sketch.

  See schematic: https://pu2clr.github.io/KT0915/

  PU2CLR KT0915 API documentation: https://pu2clr.github.io/KT0915/extras/docs/html/index.html

  By Ricardo Lima Caratti, 2020.
*/

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
