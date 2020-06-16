/**
 * It is FM and AM (MW and SW) KT01915 based receiver controlled by a Arduino Pro Mini. You can also use 
 * other Arduino boards. However, see documentation before try that: https://pu2clr.github.io/KT0915 
 * 
 * This receiver use a OLED 92" (128 x 64), 6 push buttons and an Encoder. 
 *  
 * This sketch uses the following libraries:
 *  PU2CLR KT0915 Arduino Library;
 *  Adafruit_GFX; and 
 *  Adafruit_SSD1306
 *  
 * KT0915 and Arduino Pro Mini wire up
 * 
 *  | KT0915 pin     | Arduino pin | Description | 
 *  | -----------    | ----------- | ----------- |
 *  | ENABLE (pin 9) |   12        | It is optional. The KT0915 pin 9 can be connected to +Vcc directly |
 *  | CLK  (pin 14)  |   A5        |             |
 *  | SDA  (pin 15)  |   A4        |             |
 *    
 * @see schematic on https://pu2clr.github.io/KT0915
 * @see API Documentation on https://pu2clr.github.io/KT0915/extras/docs/html/index.html
 * 
 * @author Ricardo Lima Caratti (pu2clr@gmail.com)
 * @copyright Copyright (c) 2020 - This software can be freely distributed using the MIT Free Software model.
 */

#include <KT0915.h>

#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#include "Rotary.h"

#define RESET_PIN 12            // You can use the Arduino RST pin. In this case, set it to -1

#define OLED_I2C_ADDRESS 0x3C   // Check the I2C bus of your OLED device
#define SCREEN_WIDTH 128        // OLED display width, in pixels
#define SCREEN_HEIGHT 64        // OLED display height, in pixels
#define OLED_RESET -1           // Reset pin # (or -1 if sharing Arduino reset pin)

#define ENCODER_PIN_A 2
#define ENCODER_PIN_B 3

// Buttons controllers
#define BAND_MODE_SWITCH_UP 4     // Switch to: FM -> MW(AM) -> SW1(AM) -> SW2(AM) etc
#define BAND_MODE_SWITCH_DOWN 5   //
#define VOL_UP    6               // Volume Up
#define VOL_DOWN  7               // Volume Down
#define TEST_BUTTON1   8               // Seek Station Up
#define TEST_BUTTON2 9               // Seek Station Down

#define MIN_ELAPSED_TIME 100


long elapsedButton = millis();
long elapsedPull = millis();

byte idxTest1 = 0;
byte idxTest2 = 0;

typedef struct
{
  uint8_t  mode; // Bande mode.
  uint32_t minimum_frequency; // Minimum frequency to the band (KHz)
  uint32_t maximum_frequency; // Maximum frequency to the band (KHz)
  uint32_t default_frequency; // default frequency (KHz)
  uint16_t step;               // step used (KHz)
} akc_band;



char am_bw[] = {'2', '2', '4','6','X'}; 

akc_band band[] = {
    {MODE_FM, 76000, 108000, 103900, 100},
    {MODE_AM, 520, 1710, 810, 10},
    {MODE_AM, 4700, 5600, 4885, 5}, 
    {MODE_AM, 5700, 6400, 6100, 5},    
    {MODE_AM, 6800, 7600, 7205, 5},
    {MODE_AM, 9200, 10500, 9600, 5},
    {MODE_AM, 11400, 12200, 11940, 5},
    {MODE_AM, 13500, 14300, 13600, 5},
    {MODE_AM, 15000, 15900, 15300, 5},
    {MODE_AM, 17400, 17900, 17600, 5},  
    {MODE_AM, 21400, 21900, 21525, 5},
    {MODE_AM, 27000, 28000, 27500, 1}};

const int lastBand = (sizeof band / sizeof(akc_band)) - 1;
int bandIdx = 0; // FM

// The array sizes below can be optimized. 
char oldFreq[15];
char oldMode[15];
char oldUnit[15];
char oldStep[15];
char oldRssi[15];
char oldVolume[15];
char oldStereo[15];
char oldBW[15];

Rotary encoder = Rotary(ENCODER_PIN_A, ENCODER_PIN_B);
// Encoder control variables
volatile int encoderCount = 0;

Adafruit_SSD1306 oled(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

KT0915 radio;

uint32_t currentFM = 103900;
uint32_t currentAM = 810;

uint32_t currentFrequency;

void setup()
{

  pinMode(ENCODER_PIN_A, INPUT_PULLUP);
  pinMode(ENCODER_PIN_B, INPUT_PULLUP);

  pinMode(BAND_MODE_SWITCH_UP, INPUT_PULLUP);
  pinMode(BAND_MODE_SWITCH_DOWN, INPUT_PULLUP);
  pinMode(VOL_UP, INPUT_PULLUP);
  pinMode(VOL_DOWN, INPUT_PULLUP);
  pinMode(TEST_BUTTON1, INPUT_PULLUP);
  pinMode(TEST_BUTTON2, INPUT_PULLUP);  

  // Encoder interrupt
  attachInterrupt(digitalPinToInterrupt(ENCODER_PIN_A), rotaryEncoder, CHANGE);
  attachInterrupt(digitalPinToInterrupt(ENCODER_PIN_B), rotaryEncoder, CHANGE);


  oled.begin(SSD1306_SWITCHCAPVCC, OLED_I2C_ADDRESS);
  oled.clearDisplay();
  oled.setTextColor(SSD1306_WHITE);

  splash();

  // You can select the RESET pin and Crystal type you are using in your circuit. 
  // Set RESET_PIN to -1 if you are using the Arduino RST pin; Select CRYSTAL_32KHZ or CRYSTAL_12MHZ
  radio.setup(RESET_PIN);

  radio.setTuneDialModeOff();   // Sets the KT0915 device to Digital control
  radio.setVolumeDialModeOff();
  
  radio.setVolume(20);
  radio.setFM(band[bandIdx].minimum_frequency, band[bandIdx].maximum_frequency,band[bandIdx].default_frequency, band[bandIdx].step);
 
  showStatus();
}

void rotaryEncoder()
{ // rotary encoder events
  uint8_t encoderStatus = encoder.process();
  if (encoderStatus)
  {
    if (encoderStatus == DIR_CW)
    {
      encoderCount = 1;
    }
    else
    {
      encoderCount = -1;
    }
  }
}

/**
 * Chear all string buffer information
 * These strings are used to avoid blinking on display.
 * See printValue function.
 */
void resetBuffer()
{
  oldFreq[0] = '\0';
  oldMode[0] = '\0';
  oldUnit[0] = '\0';
  oldStep[0] = '\0';
  oldRssi[0] = '\0';
  oldVolume[0] = '\0';
  oldStereo[0] = '\0';
  oldBW[0] = '\0';
}

/**
 * Splash - Change it for your introduction text.
 */
void splash()
{
  oled.setTextSize(1); // Draw 2X-scale text
  oled.setCursor(40, 0);
  oled.print("KT0915");
  oled.setCursor(20, 10);
  oled.print("Arduino Library");
  oled.display();
  delay(500);
  oled.setCursor(30, 35);
  oled.print("OLED-EXAMPLE");
  oled.setCursor(10, 50);
  oled.print("V1.0.1 - By PU2CLR");
  oled.display();
  delay(3000);
}

/*
    Writes just the changed information on Display
    Prevents blinking on display and also noise.
    Erases the old digits if it has changed and print the new digit values.
*/
void printValue(int col, int line, char *oldValue, char *newValue, int space, int textSize)
{
  int c = col;
  char *pOld;
  char *pNew;

  pOld = oldValue;
  pNew = newValue;

  oled.setTextSize(textSize);

  // prints just changed digits
  while (*pOld && *pNew)
  {
    if (*pOld != *pNew)
    {
      oled.setTextColor(SSD1306_BLACK);
      oled.setCursor(c, line);
      oled.write(*pOld);
      oled.setTextColor(SSD1306_WHITE);
      oled.setCursor(c, line);
      oled.write(*pNew);
    }
    pOld++;
    pNew++;
    c += space;
  }
  // Is there anything else to erase?
  oled.setTextColor(SSD1306_BLACK);
  while (*pOld)
  {
    oled.setCursor(c, line);
    oled.write(*pOld);
    pOld++;
    c += space;
  }
  // Is there anything else to print?
  oled.setTextColor(SSD1306_WHITE);
  while (*pNew)
  {
    oled.setCursor(c, line);
    oled.write(*pNew);
    pNew++;
    c += space;
  }

  // Save the current content to be tested next time
  strcpy(oldValue, newValue);
}

/**
 * Show the current frequency
 * 
 */
void showFrequency()
{
  char freq[15];
  char tmp[15];
  char *unit;
  char *bandMode;
  
  currentFrequency = radio.getFrequency();

   if (band[bandIdx].mode ==  MODE_FM) // FM
  {
    sprintf(tmp, "%6.6lu", currentFrequency);
     
    freq[0] = tmp[0];
    freq[1] = tmp[1];
    freq[2] = tmp[2];
    freq[3] = ',';
    freq[4] = tmp[3];
    freq[5] = '\0';
    unit = (char *) "MHz";
    bandMode =  (char *) "FM";
  }
  else // AM
  {
    sprintf(tmp, "%5lu", currentFrequency);
    freq[0] = tmp[0];
    freq[1] = tmp[1];
    freq[2] = tmp[2];
    freq[3] = tmp[3];
    freq[4] = tmp[4];   
    freq[5] = '\0';
    unit = (char *) "KHz";
    bandMode = (char *) "AM";
  }

  printValue(23, 0, oldFreq, freq, 12, 2);
  printValue(0, 0, oldMode, bandMode, 7, 1);
  printValue(105, 0, oldUnit, unit, 7, 1);
  showRSSI();

  oled.display();
}

// Show current frequency
void showStatus()
{
  oled.clearDisplay();
  resetBuffer();

  oled.drawLine(0, 17, 130, 17, SSD1306_WHITE);
  oled.drawLine(0, 52, 130, 52, SSD1306_WHITE);

  showFrequency();

  showVolume();
  showBandwidth();
  
  oled.display();
}



void showBandwidth() {
  char sBw[15];
  uint8_t i = radio.getAmBandwidth();
  if (i > 3 ) return;  

  if (band[bandIdx].mode ==  MODE_AM)  
     sprintf(sBw,"%cKHz",am_bw[i]);
  else    
     strcpy(sBw,"          ");
  
      
   printValue(0, 56, oldBW, sBw, 6, 1);
   oled.display();

}

/* *******************************
   Shows RSSI status
*/
void showRSSI()
{
  char sR[15];

  sprintf(sR,"RSSI:%3.3idBuV", (band[bandIdx].mode ==  MODE_FM)? radio.getFmRssi() : radio.getAmRssi());

  printValue(0, 40, oldRssi, sR, 6, 1);
  oled.display();
}


void showStereo() {
    char *stereo;
    stereo = (radio.isFmStereo())? (char *) "Stereo": (char *)"Mono";
    printValue(0, 20, oldStereo, stereo, 6, 1);
    oled.display();
}

/*
   Shows the volume level on LCD
*/
void showVolume()
{
  char sVolume[15];
  sprintf(sVolume, "Vol: %2.2u", radio.getVolume());
  printValue(80, 56, oldVolume, sVolume, 6, 1);
  oled.display();
}



/*
 * Button - Volume control
 */
void volumeButton(byte d)
{
  
  if (d == 1)
    radio.setVolumeUp();
  else
    radio.setVolumeDown();

  showVolume();
  delay(MIN_ELAPSED_TIME); // waits a little more for releasing the button.
}


void bandUp()
{
  // save the current frequency for the band
  band[bandIdx].default_frequency = currentFrequency;

  if (bandIdx < lastBand)
  {
    bandIdx++;
  }
  else
  {
    bandIdx = 0;
  }
  useBand();
}



void bandDown()
{
  // save the current frequency for the band
  band[bandIdx].default_frequency = currentFrequency;
  
  if (bandIdx > 0)
  {
    bandIdx--;
  }
  else
  {
    bandIdx = lastBand;
  }
  useBand();
}


void useBand() {

  if (band[bandIdx].mode ==  MODE_FM) 
  {
    radio.setFM(band[bandIdx].minimum_frequency, band[bandIdx].maximum_frequency, band[bandIdx].default_frequency, band[bandIdx].step);
    radio.setDeEmphasis(DE_EMPHASIS_75);
    // radio.setSoftMute(TURN_OFF);
    radio.setFmAfc(TURN_ON);
    radio.setMono(TURN_OFF); // Force stereo
  }
  else
  {
    radio.setAM(band[bandIdx].minimum_frequency, band[bandIdx].maximum_frequency, band[bandIdx].default_frequency, band[bandIdx].step);
    radio.setAmAfc(TURN_ON);
    radio.setSoftMute(TURN_OFF);
    radio.setAmSpace(0); // Sets Am space channel to 1Khz. 
  }
  delay(100);
  currentFrequency = band[bandIdx].default_frequency;
  radio.setFrequency(currentFrequency);
  showStatus();
}


/**
 * Used to test the receiver functions implemented by the library
 */
void testButton1() {
    if (idxTest1 == 3) idxTest1 = 0;
    idxTest1++;
    radio.setAmBandwidth(idxTest1);
    showBandwidth();
    showFrequency();
    delay(300);
}

/**
 * Used to test the receiver functions implemented by the library
 */
void testButton2() {
    if (idxTest2 == 3) idxTest2 = 0;
    idxTest2++;
    radio.setAudioGain(idxTest2);
    // radio.setAudioBass(idxTest2); 
    // radio.setAudioMute(idx);
    showBandwidth();
    showFrequency();
    delay(300);
  
}

void loop()
{
  // Check if the encoder has moved.
  if (encoderCount != 0)
  {
    if (encoderCount == 1)
      radio.frequencyUp();
    else
      radio.frequencyDown();

    showFrequency();
    encoderCount = 0;
  }

  // Check button commands
  if ((millis() - elapsedButton) > MIN_ELAPSED_TIME)
  {
    // check if some button is pressed
    if (digitalRead(BAND_MODE_SWITCH_UP) == LOW)
       bandUp(); 
    else if (digitalRead(BAND_MODE_SWITCH_DOWN) == LOW)
      bandDown();
    else if (digitalRead(VOL_UP) == LOW)
      volumeButton(1);
    else if (digitalRead(VOL_DOWN) == LOW)
      volumeButton(-1);
    else if  (digitalRead(TEST_BUTTON1) == LOW)
      testButton1();
    else if  (digitalRead(TEST_BUTTON2) == LOW)
      testButton2();
  }  

  if ( (millis() - elapsedPull) > MIN_ELAPSED_TIME * 15 ) {
    if ( radio.getCurrentMode() == MODE_FM ) {
      showStereo();
    }
    showRSSI();
    elapsedPull = millis();
  }
  delay(10);
}
