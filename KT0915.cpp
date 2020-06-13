/**
 * @brief  PU2CLR KT0915 Arduino Library
 * @details KT0915 Arduino Library implementation. This is an Arduino library for the KT0915, BROADCAST RECEIVER.  
 * @details It works with I2C protocol and can provide an easier interface to control the KT0915 device.<br>
 * @details This library was built based on KT0915 Datasheet from KTMicro (Monolithic Digital FM/MW/SW/LW Receiver Radio-on-a-Chip TM). 
 * @details This library can be freely distributed using the MIT Free Software model.
 * @copyright Copyright (c) 2020 Ricardo Lima Caratti. 
 * @author Ricardo LIma Caratti (pu2clr@gmail.com)
 */

#include <KT0915.h>

/** 
 * @defgroup GA03 Basic Methods 
 * @section  GA03 Basic 
 * @details  Low level functions used to operate with the KT09XX registers
 */

/**
 * @ingroup GA03
 * @todo check the real i2c address 
 * @brief Set I2C bus address 
 * 
 * @param deviceAddress  I2C address
 */
void KT0915::setI2CBusAddress(int deviceAddress)
{
    this->deviceAddress = deviceAddress;
}

/**
 * @ingroup GA03
 * @brief Sets the a value to a given KT09XX register
 * 
 * @param reg        register number to be written (0x1 ~ 0x3C) - See #define REG_ in KT0915.h 
 * @param parameter  content you want to store 
 */
void KT0915::setRegister(uint8_t reg, uint16_t parameter) {

    word16_to_bytes param; 
    
    param.raw = parameter;

    Wire.beginTransmission(this->deviceAddress);
    Wire.write(reg);
    Wire.write(param.refined.highByte);  
    delay(5);         
    Wire.write(param.refined.lowByte);   
    delay(5);
    Wire.endTransmission();
    delayMicroseconds(3000);
}

/**
 * @ingroup GA03
 * @brief Gets a given KT09XX register content 
 * @details It is a basic function to get a value from a given KT0915 device register
 * @param reg  register number to be read (0x1 ~ 0x3C) - See #define REG_ in KT0915.h 
 * @return the register content
 */
uint16_t KT0915::getRegister(uint8_t reg) {

    word16_to_bytes result;

    Wire.beginTransmission(this->deviceAddress);
    Wire.write(reg);
    Wire.endTransmission();
    delayMicroseconds(3000);
    Wire.requestFrom(this->deviceAddress, 2);
    result.refined.highByte = Wire.read();
    result.refined.lowByte = Wire.read();
    delayMicroseconds(3000);

    return result.raw;
}

/**
 * @ingroup GA03
 * @brief Gets the Device Id 
 * @return uint16_t 16 bits word with the device id number
 */
uint16_t KT0915::getDeviceId()
{
    this->deviceId  = getRegister(REG_CHIP_ID);
    return this->deviceId;
}

/**
 * @brief Gets the Crystal Status information
 * 
 * @return true  
 * @return false 
 */
bool KT0915::isCrystalReady() {
    kt09xx_statusa reg;
    reg.raw = getRegister(REG_STATUSA);
    return reg.refined.XTAL_OK;
}

/**
 * @ingroup GA03
 * @brief Sets the Crystal Type
 * @details Configures the Crystal or reference clock  you are using in your circuit.
 * @details For a low frequency crystal oscillator, selects 32.768KHz or 38KHz crystals. 
 * @details Alternatively, you can use a CMOS level external reference clock may be used by setting 
 * @details the parameter ref_clock to 1 (REF_CLOCK_ENABLE) and setting the reference clock according to the table below.

 *  * Crystal type table
 * | Dec | binary | Description | defined constant    |
 * | --  | ------ | ----------- | ---------------     |
 * | 0   | 0000   | 32.768KHz   | OSCILLATOR_32KHZ    |
 * | 1   | 0001   | 6.5MHz      | OSCILLATOR_6_5MHZ   |
 * | 2   | 0010   | 7.6MHz      | OSCILLATOR_7_6MHZ   |
 * | 3   | 0011   | 12MHz       | OSCILLATOR_12MHZ    |
 * | 4   | 0100   | 13MHz       | OSCILLATOR_13MHZ    |
 * | 5   | 0101   | 15.2MHz     | OSCILLATOR_15_2MHZ  |
 * | 6   | 0110   | 19.2MHz     | OSCILLATOR_19_2MHZ  | 
 * | 7   | 0111   | 24MHz       | OSCILLATOR_24MHZ    |
 * | 8   | 1000   | 26MHz       | OSCILLATOR_26MHZ    |
 * | 9   | 1001   | ?? 38KHz ?? | OSCILLATOR_38KHz    |
 * 
 * @param crystal   Reference Clock Selection. See table above. 
 * @param ref_clock 0 = Crystal (default); 1 = Reference clock enabled.
 */
void KT0915::setReferenceClockType(uint8_t crystal, uint8_t ref_clock)
{
    kt09xx_amsyscfg reg;
    reg.raw = getRegister(REG_AMSYSCFG);  // Gets the current value of the register
    reg.refined.REFCLK = crystal;         // Changes just crystal parameter
    reg.refined.RCLK_EN = ref_clock;      // Reference Clock Enable => Crystal
    setRegister(REG_AMSYSCFG, reg.raw);   // Strores the new value to the register 
}


/**
 * @ingroup GA03
 * @brief Sets the enable pin (9) of the KT0915 high or low
 * @details This function can be used to enable (1) and disable (0) the KT0915 device. You have to select a MCU (Arduino) pin for this function. 
 * @details Also, you can set -1 to used this control via circuit. 
 * 
 * @see setup
 * 
 * @param on_off  1 = enable; 0 = disable
 */
void KT0915::enable( uint8_t on_off)
{
    if (this->enablePin < 0)
        return; 
    digitalWrite(this->enablePin, on_off);
    delay(10);
}

/**
 * @ingroup GA03
 * @todo You need to check mechanical Tune features
 * @brief Sets Tune Dial Mode Interface On  
 * @details This method sets the KT0915 to deal with a mechanical tuning via an external 100K resistor.
 * @details KT0915 supports a unique Dial Mode (mechanical tuning wheel with a variable resistor) which is 
 * @details enabled by GPIO1 to 2 (10). The dial can be a variable resistor with the tap connected to CH (pin 1).  
 * 
 * @see KT0915; Monolithic Digital FM/MW/SW/LW Receiver Radio on a Chip(TM); pages 19 and 20.
 * 
 * @param minimu_frequency   Start frequency for the user band
 * @param maximum_frequency  Final frequency for the user band   
 */
void KT0915::setTuneDialModeOn(uint32_t minimu_frequency, uint32_t maximum_frequency)
{
    kt09xx_amsyscfg reg;
    kt09xx_gpiocfg gpio;
    kt09xx_userstartch uf;
    kt09xx_userguard ug;
    kt09xx_userchannum uc;

    reg.raw = getRegister(REG_AMSYSCFG); // Gets the current value from the register
    reg.refined.USERBAND = this->currentDialMode = DIAL_MODE_ON;
    setRegister(REG_AMSYSCFG, reg.raw); // Strores the new value in the register

    gpio.raw = getRegister(REG_GPIOCFG); // Gets the current value from the register
    gpio.refined.GPIO1 = 2;              // Sets Dial Mode interface (pin 1/CH)
    setRegister(REG_GPIOCFG, gpio.raw);  // Stores the new value in the register

    // TODO: Sets the frequency limits for user and

    if ( this->currentMode == MODE_AM) {
        uf.refined.USER_START_CHAN = minimu_frequency;
        uc.refined.USER_CHAN_NUM = (maximum_frequency - minimu_frequency) / this->currentStep;
        ug.refined.USER_GUARD = 0x0011;
    } else {
        uf.refined.USER_START_CHAN = minimu_frequency / 50;
        uc.refined.USER_CHAN_NUM = ((maximum_frequency - minimu_frequency) / 50) / this->currentStep;
        ug.refined.USER_GUARD = 0x001D;
    }

    setRegister(REG_USERSTARTCH, uf.raw);
    setRegister(REG_USERGUARD, ug.raw);
    setRegister(REG_USERCHANNUM, uc.raw);

};

/**
 * @ingroup GA03
 * @brief Turns the Tune Dial Mode interface Off
 * @see setTuneDialModeOn
 * @see KT0915; Monolithic Digital FM/MW/SW/LW Receiver Radio on a Chip(TM); page 20.
 */
void KT0915::setTuneDialModeOff()
{
    kt09xx_amsyscfg reg;
    kt09xx_gpiocfg gpio;
    reg.raw = getRegister(REG_AMSYSCFG); // Gets the current value of the register
    reg.refined.USERBAND = this->currentDialMode = DIAL_MODE_OFF;
    setRegister(REG_AMSYSCFG, reg.raw); // Strores the new value to the register

    gpio.raw = getRegister(REG_GPIOCFG); // Gets the current value of the register
    gpio.refined.GPIO1 = 0;              // Sets MCU (Arduino) control (High Z)
    setRegister(REG_GPIOCFG, gpio.raw);  // Stores the new value in the register
}

/**
 * @ingroup GA03
 * @brief   Sets Volume Dial Mode Interface On 
 * @details This method sets the KT0915 to deal with a mechanical volume control via an external 100K resistor.
 * @details KT0915 supports a unique Dial Mode  which is enabled by GPIO2 to 2 (10). 
 * @details The dial can be a variable resistor with the tap connected to VOL (pin 16). 
 * 
 * @see KT0915; Monolithic Digital FM/MW/SW/LW Receiver Radio on a Chip(TM); page 20.
 */
void KT0915::setVolumeDialModeOn()
{
    kt09xx_gpiocfg gpio;
    gpio.raw = getRegister(REG_GPIOCFG); // Gets the current value from the register
    gpio.refined.GPIO2 = 2;              // Sets Dial Mode interface (pin 16/VOL)
    setRegister(REG_GPIOCFG, gpio.raw);  // Stores the new value in the register
};

/**
 * @ingroup GA03
 * @brief Turns the Volume Dial Mode interface Off
 * @see setVolumeDialModeOn
 * @see KT0915; Monolithic Digital FM/MW/SW/LW Receiver Radio on a Chip(TM); page 20.
 */
void KT0915::setVolumeDialModeOff()
{
    kt09xx_gpiocfg gpio;
    gpio.raw = getRegister(REG_GPIOCFG); // Gets the current value of the register
    gpio.refined.GPIO2 = 0;              // Sets to MCU (Arduino) control (High Z)
    setRegister(REG_GPIOCFG, gpio.raw);  // Stores the new value in the register
}

/**
 * @brief Sets the audio volume level
 * @details This method is used to control the audio volume level. The value 0 mutes the  device and 31 sets the device to the maximum volume.
 * @param volume between 0 and 31. 
 */
void KT0915::setVolume(uint8_t volume) {
    kt09xx_rxcfg rx;
    if ( volume > 31) return;
    rx.raw = getRegister(REG_RXCFG);
    rx.refined.VOLUME = volume;
    setRegister(REG_RXCFG, rx.raw);
}

    /**
 * @ingroup GA03
 * @brief   Receiver startup 
 * @details Use this method to define the MCU (Arduino) RESET pin and the crystal type you are using. 
 * @details The options for the crystal can be seen in the table below.
 * @details If you omit the crystal type parameter, will be considered  0 (32.768KHz). 
 * @details For a low frequency crystal oscillator, selects 32.768KHz or 38KHz crystals. 
 * @details Alternatively, you can use a CMOS level external reference clock may be used by setting 
 * @details the parameter ref_clock to 1 (REF_CLOCK_ENABLE) and setting the reference clock according to the table below.
 * @details The code below shows how to use the setup function.
 * 
 * @code
 * #include <KT0915.h>
 * #define RESET_PIN 12   // set it to -1 if you want to use the RST pin of your MCU.
 * KT0915 radio;
 * void setup() {
 *    // Set RESET_PIN to -1 if you are using the Arduino RST pin; Select OSCILLATOR_32KHZ, OSCILLATOR_12MHZ etc
 *    // radio.setup(RESET_PIN); Instead the line below, if you use this line, the crystal type considered will be 32.768KHz.   
 *    radio.setup(RESET_PIN, OSCILLATOR_12MHZ, REF_CLOCK_DISABLE );
 * }
 * @endcode
 * 
 * Crystal type table
 * | Dec | binary | Description | defined constant    |
 * | --  | ------ | ----------- | ---------------     |
 * | 0   | 0000   | 32.768KHz   | OSCILLATOR_32KHZ    |
 * | 1   | 0001   | 6.5MHz      | OSCILLATOR_6_5MHZ   |
 * | 2   | 0010   | 7.6MHz      | OSCILLATOR_7_6MHZ   |
 * | 3   | 0011   | 12MHz       | OSCILLATOR_12MHZ    |
 * | 4   | 0100   | 13MHz       | OSCILLATOR_13MHZ    |
 * | 5   | 0101   | 15.2MHz     | OSCILLATOR_15_2MHZ  |
 * | 6   | 0110   | 19.2MHz     | OSCILLATOR_19_2MHZ  | 
 * | 7   | 0111   | 24MHz       | OSCILLATOR_24MHZ    |
 * | 8   | 1000   | 26MHz       | OSCILLATOR_26MHZ    |
 * | 9   | 1001   | 38KHz       | OSCILLATOR_38KHz    |
 * 
 * @see KT0915; Monolithic Digital FM/MW/SW/LW Receiver Radio on a Chip(TM); 3.6 Crystal and reference clock; page 9. 
 * @see setReferenceClockType 
 * 
 * @param enablePin      if >= 0,  then you control the RESET. if -1, you are using ths Arduino RST pin. 
 * @param oscillator_type  Crystal type. See the table above.
 * @param ref_clock     0 = Crystal (Reference clock enabled disabled - default); 1 = Reference clock enabled.
 */
void KT0915::setup(int enable_pin, uint8_t oscillator_type, uint8_t ref_clock)
{
    this->enablePin = enable_pin;
    pinMode(this->enablePin, OUTPUT);

    enable(1);

    // Sets some registers with custom default value
    // setRegister(REG_RXCFG, 0b1000100000000000);
    // setRegister(REG_SOFTMUTE, 0b0010100010001100);
    // setRegister(REG_AMDSP, 0b1010001010001100);
    // setRegister(REG_AMCFG, 0b1101010000000001);

    setReferenceClockType(oscillator_type, ref_clock);

}

/** 
 * @defgroup GA04 Tune Methods 
 * @section  GA04 Tune Methods  
 * @details  Methods to tune and set the receiver mode
 */

/**
 * @ingroup GA04
 * @brief Set AM the Antenna Tune Capacitor 
 * @details Sets a value between 0 and 16383 for AM Antenna Calibration 
 * @param capacitor value between 0 and 16383
 */
void KT0915::setAntennaTuneCapacitor(uint16_t capacitor)
{
    kt09xx_amcali reg;
    reg.refined.CAP_INDEX = capacitor; 
    setRegister(REG_AMCALI, reg.raw);    // Strores the new value to the register
};

/**
 * @ingroup GA04
 * @brief Sets the receiver Stereo or Mono
 * @details Set this parameter to true to force mono or false to stereo
 * @param on_off true = mono; fale = stereo
 */
void KT0915::setMono(bool on_off) 
{
    kt09xx_dspcfga reg;
    reg.raw = getRegister(REG_DSPCFGA);
    reg.refined.MONO = on_off;
    setRegister(REG_DSPCFGA,reg.raw);
}

/**
 * @ingroup GA04
 * @brief Sets the De-emphasis Time Constant Selection
 * @param value 0 = 75us; 1 = 50us 
 */
void KT0915::setDeEmphasis(uint8_t value) 
{
    kt09xx_dspcfga reg;
    reg.raw = getRegister(REG_DSPCFGA);
    reg.refined.DE = value;
    setRegister(REG_DSPCFGA, reg.raw);
}

/**
 * @todo Adjust setTuneDialOn()
 * @ingroup GA04
 * @brief Sets the receiver to FM mode
 * @details Configures the receiver on FM mode; Also sets the band limits, defaul frequency and step.
 * 
 * @param minimum_frequency  minimum frequency for the band
 * @param maximum_frequency  maximum frequency for the band
 * @param default_frequency  default freuency
 * @param step  increment and decrement frequency step
 */
void KT0915::setFM(uint32_t minimum_frequency, uint32_t maximum_frequency, uint32_t default_frequency, uint16_t step)
{
    kt09xx_amsyscfg reg;

   
    this->currentStep = step;
    this->currentFrequency = default_frequency;
    this->minimumFrequency = minimum_frequency;
    this->maximumFrequency = maximum_frequency;
    this->currentMode = MODE_FM;

    reg.raw = 0;
    reg.refined.RESERVED1 = 1;
    reg.refined.AM_FM = MODE_FM;
    reg.refined.USERBAND =  this->currentDialMode;
    reg.refined.REFCLK = this->currentRefClockType;
    reg.refined.RCLK_EN = this->currentRefClockEnabled;
    setRegister(REG_AMSYSCFG, reg.raw); // Stores the new value in the register

    if (this->currentDialMode == DIAL_MODE_ON) 
        setTuneDialModeOn(minimum_frequency, maximum_frequency);
    else
        setFrequency(default_frequency);
  
};

/**
 * @todo Adjust setTuneDialOn()
 * @ingroup GA04
 * @brief Sets the receiver to AM mode
 * @details Configures the receiver on AM mode; Also sets the band limits, defaul frequency and step.
 * 
 * @param minimum_frequency  minimum frequency for the band
 * @param maximum_frequency  maximum frequency for the band
 * @param default_frequency  default freuency
 * @param step  increment and decrement frequency step
 */
void KT0915::setAM(uint32_t minimum_frequency, uint32_t maximum_frequency, uint32_t default_frequency, uint16_t step)
{
    kt09xx_amsyscfg reg;

    this->currentStep = step;
    this->currentFrequency = default_frequency;
    this->minimumFrequency = minimum_frequency;
    this->maximumFrequency = maximum_frequency;
    this->currentMode = MODE_AM;

    reg.raw = getRegister(REG_AMSYSCFG); // Gets the current value from the register
    reg.refined.AM_FM = MODE_AM;
    reg.refined.USERBAND = this->currentDialMode;           
    setRegister(REG_AMSYSCFG, reg.raw);  // Strores the new value in the register
    if (this->currentDialMode == DIAL_MODE_ON)
        setTuneDialModeOn(minimum_frequency, maximum_frequency);
    else
        setFrequency(default_frequency);
}

/**
 * @ingroup GA04
 * @brief Sets the current frequency
 * 
 * @param frequency 
 */
void KT0915::setFrequency(uint32_t frequency)
{
    kt09xx_amchan reg_amchan;
    kt09xx_tune reg_tune;

    if (this->currentMode == MODE_AM)
    {
        reg_amchan.refined.AMTUNE = 0;      // TODO Check        
        reg_amchan.refined.AMCHAN = frequency;
        setRegister(REG_AMCHAN,reg_amchan.raw);
    }
    else
    {
        reg_tune.refined.FMTUNE = 0; // // TODO Check
        reg_tune.refined.RESERVED = 0;
        reg_tune.refined.FMCHAN = frequency / 50;
        setRegister(REG_TUNE, reg_tune.raw);
    }

    this->currentFrequency = frequency;
}

/**
 * @ingroup GA04
 * @brief Increments the frequency one step
 * @details if the frequency plus the step value is greater than the maximum frequency for the band, 
 * @details tne current frequency will be set to minimum frequency.
 * 
 * @see setFrequency
 */
void KT0915::frequencyUp(){
    this->currentFrequency += this->currentStep;
    if (this->currentFrequency > this->maximumFrequency)
        this->currentFrequency = this->minimumFrequency;

    setFrequency(this->currentFrequency);
}

/**
 * @ingroup GA04
 * @brief Decrements the frequency one step
 * @details if the frequency minus the step value is less than the minimum frequency for the band, 
 * @details tne current frequency will be set to minimum frequency.
 * 
 * @see setFrequency
 */
void KT0915::frequencyDown(){
    this->currentFrequency -= this->currentStep;
    if (this->currentFrequency < this->minimumFrequency)
        this->currentFrequency = this->maximumFrequency;

    setFrequency(this->currentFrequency);
};

/**
 * @ingroup GA04
 * @brief Sets the frequency step
 * @details Sets increment and decrement frequency 
 * @param step  Values: 1, 5, 9, 10, 100, 200  in KHz
 */
void KT0915::setStep(uint16_t step)
{
    this->currentStep = step;
}

/**
 * @ingroup GA04
 * @brief Gets the current frequency 
 * @return frequency in KHz
 */
uint32_t KT0915::getFrequency()
{
    return this->currentFrequency;
}
