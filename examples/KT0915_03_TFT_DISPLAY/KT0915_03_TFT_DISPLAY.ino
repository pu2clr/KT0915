/*
  THIS SKETCH IS UNDER CONSTRUCTION...

  This sketch uses an Arduino Pro Mini, 3.3V (8MZ) with a SPI TFT ST7735 1.8"
  It is also a FM receiver capable to tune your local FM stations.

  Read more on https://pu2clr.github.io/KT0915/

  Wire up on Arduino UNO, Pro mini

  | Device name       | Device Pin / Description  |  Arduino Pin  |
  | ----------------  | --------------------      | ------------  |
  | Display TFT       |                           |               |
  |                   | RST (RESET)               |      8        |
  |                   | RS  or DC                 |      9        |
  |                   | CS  or SS                 |     10        |
  |                   | SDI                       |     11        |
  |                   | CLK                       |     13        |
  |     KT0915        |                           |               |
  |                   | SDIO/SDA (pin 15)         |     A4        |
  |                   | SCLK/CLK (pin 14)         |     A5        |
  |                   | (*1) Enable (pin 9)       |     12        |
  |     Buttons       |                           |               |
  |                   | Band Up                   |      4        |
  |                   | Band Down                 |      5        |
  |                   | Volume Up                 |      6        |
  |                   | Volume Down               |      7        |
  |                   | setAmBandwidth            |      14/A0    |
  |    Encoder        |                           |               |
  |                   | A                         |      2        |
  |                   | B                         |      3        |

  (*1) The KT0915 pin 9 can be connected to +Vcc directly. In this case you do not need an Arduino pin.
       Use the KT0915 pin 9 connected with Arduino if you want to control the Enable / Disable status.
       See  KT0915::setup documentation.

  About 77% of the space occupied by this sketch is due to the library for the TFT Display.


  Prototype documentation: https://pu2clr.github.io/KT0915/
  PU2CLR KT0915 API documentation: https://pu2clr.github.io/KT0915/extras/apidoc/html/

  By PU2CLR, Ricardo,  Feb  2020.
*/

#include <KT0915.h>

#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ST7735.h> // Hardware-specific library for ST7735
#include "Serif_plain_7.h"
#include "Serif_plain_14.h"
#include "DSEG7_Classic_Mini_Regular_30.h"
#include <SPI.h>

#include "Rotary.h"

// TFT MICROYUM or ILI9225 based device pin setup
#define TFT_RST 8
#define TFT_DC 9
#define TFT_CS 10  // SS
#define TFT_SDI 11 // MOSI
#define TFT_CLK 13 // SCK
#define TFT_LED 0  // 0 if wired to +3.3V directly
#define TFT_BRIGHTNESS 200

#define COLOR_BLACK 0x0000
#define COLOR_YELLOW 0xFFE0
#define COLOR_WHITE 0xFFFF
#define COLOR_RED 0xF800
#define COLOR_BLUE 0x001F
#define COLOR_GREEN 0x7E0
#define COLOR_ORANGE 0xFBE0

#define ENABLE_PIN -1  // The KT0915 pin 9 must be connected to tne +Vcc
#define SDA_PIN A4 // SDA pin used by your Arduino Board

// Enconder PINs
#define ENCODER_PIN_A 2
#define ENCODER_PIN_B 3

// Buttons controllers
#define BAND_MODE_SWITCH_UP   4  // Switch to: FM -> MW(AM) -> SW1(AM) -> SW2(AM) etc
#define BAND_MODE_SWITCH_DOWN 5  //
#define VOL_UP                6  // Volume Up
#define VOL_DOWN              7  // Volume Down
#define SWITCH_BW            14  // Seek Station Up
#define TEST_BUTTON2         15  // Seek Station Down

#define POLLING_TIME  2000
#define MIN_ELAPSED_TIME 100

// The array sizes below can be optimized.
char oldFreq[15];
char oldMode[15];
char oldUnit[15];
char oldStep[15];
char oldRssi[15];
char oldVolume[15];
char oldStereo[15];
char oldBW[15];
char oldBandName[10];


bool bSt = true;
bool bShow = false;

long elapsedButton = millis();
long pollin_elapsed = millis();

// Encoder control variables
volatile int encoderCount = 0;

uint32_t currentFrequency;



typedef struct
{
  uint8_t  mode; // Bande mode.
  char    *name;  
  uint32_t minimum_frequency; // Minimum frequency to the band (KHz)
  uint32_t maximum_frequency; // Maximum frequency to the band (KHz)
  uint32_t default_frequency; // default frequency (KHz)
  uint16_t step;               // step used (KHz)
} akc_band;

akc_band band[] = {
  {MODE_FM, (char *) "VHF", 76000, 108000, 103900, 100},
  {MODE_AM, (char *) "MW ", 520, 1710, 810, 10},
  {MODE_AM, (char *) "60m", 4700, 5600, 4885, 5},
  {MODE_AM, (char *) "49m", 5700, 6400, 6100, 5},
  {MODE_AM, (char *) "41m", 6800, 8200, 7205, 5},
  {MODE_AM, (char *) "31m", 9200, 10500, 9600, 5},
  {MODE_AM, (char *) "25m", 11400, 12200, 11940, 5},
  {MODE_AM, (char *) "22m", 13400, 14300, 13600, 5},
  {MODE_AM, (char *) "19m", 15000, 16100, 15300, 5},
  {MODE_AM, (char *) "16m", 17400, 17900, 17600, 5},
  {MODE_AM, (char *) "13m", 21400, 21900, 21525, 5},
  {MODE_AM, (char *) "11m", 27000, 28000, 27500, 1}, 
  {MODE_AM, (char *) "10m", 28000, 30000, 28400, 1}, 
  {MODE_FM, (char *) "VHF/6m", 50000, 55000, 50125, 10}
};

const int lastBand = (sizeof band / sizeof(akc_band)) - 1;
int bandIdx = 0; // FM

char am_bw[] = {'2', '2', '4', '6', 'X'};
byte bwIdx = 1;


// Encoder control
Rotary encoder = Rotary(ENCODER_PIN_A, ENCODER_PIN_B);

// TFT control
Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST);

KT0915 rx;

void setup()
{
  // Serial.begin(9600);
  pinMode(ENCODER_PIN_A, INPUT_PULLUP);
  pinMode(ENCODER_PIN_B, INPUT_PULLUP);

  // Push button pin
  pinMode(BAND_MODE_SWITCH_UP, INPUT_PULLUP);
  pinMode(BAND_MODE_SWITCH_DOWN, INPUT_PULLUP);
  pinMode(VOL_UP, INPUT_PULLUP);
  pinMode(VOL_DOWN, INPUT_PULLUP);
  pinMode(SWITCH_BW, INPUT_PULLUP);
  pinMode(TEST_BUTTON2, INPUT_PULLUP);

  tft.initR(INITR_BLACKTAB);
  tft.fillScreen(COLOR_BLACK);
  tft.setTextColor(COLOR_BLUE);
  tft.setRotation(1);

  showSplash();

  // Encoder interrupt
  attachInterrupt(digitalPinToInterrupt(ENCODER_PIN_A), rotaryEncoder, CHANGE);
  attachInterrupt(digitalPinToInterrupt(ENCODER_PIN_B), rotaryEncoder, CHANGE);

  rx.setup(ENABLE_PIN); // You can control the enable or disable the device. If ENABLE_PIN is high, the device is enable
  
  // rx.setTuneDialModeOff();   // Sets the KT0915 device to Digital control
  // rx.setVolumeDialModeOff();

  rx.setVolume(20);
  rx.setAudioAntiPop(3);    // Anti-pop Configuration (10uF capacitor).
  
  rx.setFM(band[bandIdx].minimum_frequency, band[bandIdx].maximum_frequency, band[bandIdx].default_frequency, band[bandIdx].step);
  showTemplate();
  showStatus();
}


/*
    Reads encoder via interrupt
    Use Rotary.h and  Rotary.cpp implementation to process encoder via interrupt
*/
void rotaryEncoder()
{ // rotary encoder events
  uint8_t encoderStatus = encoder.process();
  if (encoderStatus)
    encoderCount = (encoderStatus == DIR_CW) ? 1 : -1;
}


/**
   Chear all string buffer information
   These strings are used to avoid blinking on display.
   See printValue function.
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
  oldBandName[0] = '\0';
}


/*
   Shows the static content on  display
*/
void showTemplate()
{
  int maxX1 = tft.width() - 2;
  int maxY1 = tft.height() - 5;

  tft.fillScreen(COLOR_BLACK);

  tft.drawRect(2, 2, maxX1, maxY1, COLOR_YELLOW);
  tft.drawLine(2, 40, maxX1, 40, COLOR_YELLOW);
  tft.drawLine(2, 60, maxX1, 60, COLOR_YELLOW);
}


void clearStatus() {
  char *unit;
  char *bandMode;

  int maxX1 = tft.width() - 6;
  int maxY1 = tft.height() - 6;
  
  tft.fillRect(3, 3, maxX1, 35, COLOR_BLACK);
  tft.fillRect(3,41, maxX1, 19, COLOR_BLACK);
   
  tft.fillRect(3,61,maxX1, maxY1 - 61, COLOR_BLACK);
  
  if (band[bandIdx].mode ==  MODE_FM) {
    unit = (char *) "MHz";
    bandMode =  (char *) "FM";
  } else {
    unit = (char *) "KHz";
    bandMode = (char *) "AM";    
  }
  tft.setFont(&Serif_plain_7);
  printValue(135,15,oldMode, bandMode,7,COLOR_YELLOW);  
  printValue(135,36,oldUnit, unit,7,COLOR_YELLOW); 

  printValue(6, 120, oldBandName, band[bandIdx].name, 7, COLOR_GREEN);

}

/*
  Prevents blinking during the frequency display.
  Erases the old digits if it has changed and print the new digit values.
*/
void printValue(int col, int line, char *oldValue, char *newValue, uint8_t space, uint16_t color)
{
  int c = col;
  char *pOld;
  char *pNew;

  pOld = oldValue;
  pNew = newValue;

  // prints just changed digits
  while (*pOld && *pNew)
  {
    if (*pOld != *pNew)
    {
      // Erases olde value
      tft.setTextColor(COLOR_BLACK);
      tft.setCursor(c, line);
      tft.print(*pOld);
      // Writes new value
      tft.setTextColor(color);
      tft.setCursor(c, line);
      tft.print(*pNew);
    }
    pOld++;
    pNew++;
    c += space;
  }

  // Is there anything else to erase?
  tft.setTextColor(COLOR_BLACK);
  while (*pOld)
  {
    tft.setCursor(c, line);
    tft.print(*pOld);
    pOld++;
    c += space;
  }

  // Is there anything else to print?
  tft.setTextColor(color);
  while (*pNew)
  {
    tft.setCursor(c, line);
    tft.print(*pNew);
    pNew++;
    c += space;
  }

  // Save the current content to be tested next time
  strcpy(oldValue, newValue);
}

/*
   Shows frequency information on Display
*/
void showFrequency()
{
  char freq[15];
  char aux[15];

  currentFrequency = rx.getFrequency();

  // Serial.println(currentFrequency);

  tft.setFont(&DSEG7_Classic_Mini_Regular_30);
  tft.setTextSize(1);
  

  if (band[bandIdx].mode ==  MODE_FM) // FM
  {
    sprintf(aux, "%6.6lu", currentFrequency);
    freq[0] = aux[0];
    freq[1] = aux[1];
    freq[2] = aux[2];
    freq[3] = '\0';
    freq[4] = aux[3];
    freq[5] = aux[4];
    freq[6] = '\0'; 

    printValue(2, 36, &oldFreq[0], &freq[0], 23, COLOR_ORANGE);
    printValue(82, 36, &oldFreq[4], &freq[4], 23, COLOR_ORANGE);
    tft.setCursor(80, 35);
    tft.print('.');
   
  }
  else // AM
  {
    sprintf(aux, "%5lu", currentFrequency);
    freq[0] = aux[0];
    freq[1] = aux[1];
    freq[2] = aux[2];
    freq[3] = aux[3];
    freq[4] = aux[4];
    freq[5] = '\0';
    printValue(2, 36, &oldFreq[0], &freq[0], 23, COLOR_ORANGE);
  }

}

/*
    Show some basic information on display
*/
void showStatus()
{
  resetBuffer();
  clearStatus();
  showFrequency();
  showVolume();
  showBandwidth();

}

/* *******************************
   Shows RSSI status
*/
void showRSSI()
{
  char rssi[15];
  int currentRssi;

  if (band[bandIdx].mode ==  MODE_FM) {
    currentRssi = rx.getFmRssi();
    showStereo();
  } else {
    currentRssi = rx.getAmRssi();
  }

  sprintf(rssi, "%3u dBuV", currentRssi);
  
  tft.setFont(&Serif_plain_14);
  tft.setTextSize(1);
  printValue(3, 55, oldRssi, rssi, 11, COLOR_WHITE);
}



void showBandwidth() {
  char sBw[15];

  if (band[bandIdx].mode !=  MODE_AM) return; 
  
  sprintf(sBw, "BW %cKHz", am_bw[bwIdx]);

  tft.setFont(&Serif_plain_14);
  tft.setTextSize(1);
  printValue(5, 80, oldBW, sBw, 11, COLOR_GREEN);

}

void showStereo() {
  char stereo[10];
  sprintf(stereo, "%s", (rx.isFmStereo()) ? "St" : "Mo");
  tft.setFont(&Serif_plain_14);
  tft.setTextSize(1);
  printValue(125, 55, oldStereo, stereo, 15, COLOR_WHITE);
}

/*
   Shows the volume level on LCD
*/
void showVolume()
{
  char sVolume[15];
  
  sprintf(sVolume, "V %2.2u", rx.getVolume());
  
  tft.setFont(&Serif_plain_14);
  tft.setTextSize(1);
  printValue(100, 80, oldVolume, sVolume, 11, COLOR_GREEN);
}



/*********************************************************

 *********************************************************/

void showSplash()
{
  // Splash
  tft.setFont(&Serif_plain_14);
  tft.setTextSize(1);
  tft.setTextColor(COLOR_YELLOW);
  tft.setCursor(45, 23);
  tft.print("KT0915");
  tft.setCursor(15, 50);
  tft.print("Arduino Library");
  tft.setCursor(25, 80);
  tft.print("By PU2CLR");
  tft.setFont(&Serif_plain_14);
  tft.setTextSize(0);
  tft.setCursor(12, 110);
  tft.print("Ricardo L. Caratti");
  delay(4000);
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
    rx.setFM(band[bandIdx].minimum_frequency, band[bandIdx].maximum_frequency, band[bandIdx].default_frequency, band[bandIdx].step);
    rx.setDeEmphasis(DE_EMPHASIS_75);
    // rx.setSoftMute(TURN_OFF);
    rx.setFmAfc(true);
    rx.setMono(false);  // Force stereo
    // rx.setAudioBass(3); // High.
    // rx.setAudioGain(3); // 3dB
  }
  else
  {
    rx.setAM(band[bandIdx].minimum_frequency, band[bandIdx].maximum_frequency, band[bandIdx].default_frequency, band[bandIdx].step);
    rx.setAmSpace(0);     // Sets Am space channel to 1Khz.
    // Trying to improve the AM audio quality.
    rx.setMono(true);           // Sets mono;
    rx.setAudioBass(3);         // 0=Disable; 1=Low; 2=Med; 3=High
    rx.setAmAfc(false);         // False = Disable AM AFC 
    rx.setAudioGain(0);         // 0=3dB; 1=6dB; 2=-3db; 3=0dB
    rx.setSoftMute(false);      // Makes the tune easier on SW band
    // rx.setKeyControl(0, 0);
  }
  delay(100);
  currentFrequency = band[bandIdx].default_frequency;
  rx.setFrequency(currentFrequency);
  showStatus();
}



/**
 * Used to test the receiver functions implemented by the library
 */
void doBandwidth() {
    if (bwIdx == 3) bwIdx = 0;
    bwIdx++;
    rx.setAmBandwidth(bwIdx);
    showBandwidth();
    showFrequency();
    delay(300);
}



void doStereo() {
  rx.setMono((bSt = !bSt));
  bShow =  true;
  showStereo();
  delay(100);
}

/**
   Process seek command.
   The seek direction is based on the last encoder direction rotation.
*/
void doSeek() {
  delay(200);
  bShow =  true;
  showFrequency();
}



/*
   Button - Volume control
*/
void volumeButton(byte d)
{

  if (d == 1)
    rx.setVolumeUp();
  else
    rx.setVolumeDown();

  showVolume();
  delay(MIN_ELAPSED_TIME); // waits a little more for releasing the button.
}


void loop()
{

  // Check if the encoder has moved.
  if (encoderCount != 0)
  {
    if (encoderCount == 1) {
      rx.setFrequencyUp();
    }
    else {
      rx.setFrequencyDown();
    }
    showFrequency();
    bShow = true;
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
    else if  (digitalRead(SWITCH_BW) == LOW)
      doBandwidth();
  }

  if ( (millis() - pollin_elapsed) > POLLING_TIME ) {
    showRSSI();
    pollin_elapsed = millis();
  }

  delay(100);
}
