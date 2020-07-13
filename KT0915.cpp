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
void KT0915::setRegister(int reg, uint16_t parameter)
{
    word16_to_bytes param;

    param.raw = parameter;

    Wire.beginTransmission(this->deviceAddress);
    Wire.write(reg);

    Wire.write(param.refined.highByte);
    Wire.write(param.refined.lowByte);

    Wire.endTransmission();
    delayMicroseconds(6000);
}

/**
 * @ingroup GA03
 * @brief Gets a given KT09XX register content 
 * @details It is a basic function to get a value from a given KT0915 device register
 * @param reg  register number to be read (0x1 ~ 0x3C) - See #define REG_ in KT0915.h 
 * @return the register content
 */
uint16_t KT0915::getRegister(int reg)
{

    word16_to_bytes result;

    Wire.beginTransmission(this->deviceAddress);
    Wire.write(reg);
    Wire.endTransmission(false);
    delayMicroseconds(6000);
    Wire.requestFrom(this->deviceAddress, 2);
    result.refined.highByte = Wire.read();
    result.refined.lowByte = Wire.read();
    Wire.endTransmission(true);
    delayMicroseconds(6000);

    return result.raw;
}

/**
 * @ingroup GA03
 * @brief Gets the Device Id 
 * @return uint16_t 16 bits word with the device id number
 */
uint16_t KT0915::getDeviceId()
{
    this->deviceId = getRegister(REG_CHIP_ID);
    return this->deviceId;
}

/**
 * @ingroup GA03
 * @brief Gets the Crystal Status information
 * 
 * @return true  
 * @return false 
 */
bool KT0915::isCrystalReady()
{
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
    reg.raw = getRegister(REG_AMSYSCFG); // Gets the current value of the register
    reg.refined.REFCLK = crystal;        // Changes just crystal parameter
    reg.refined.RCLK_EN = ref_clock;     // Reference Clock Enable => Crystal
    setRegister(REG_AMSYSCFG, reg.raw);  // Strores the new value to the register

    this->currentRefClockType = crystal;
    this->currentRefClockEnabled = ref_clock;
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
void KT0915::enable(uint8_t on_off)
{
    if (this->enablePin < 0)
        return;
        
    pinMode(this->enablePin, OUTPUT);
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

    if (this->currentMode == MODE_AM)
    {
        uf.refined.USER_START_CHAN = minimu_frequency;
        uc.refined.USER_CHAN_NUM = (maximum_frequency - minimu_frequency) / this->currentStep;
        ug.refined.USER_GUARD = 0x0011;
    }
    else
    {
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
 * @see setTuneDialModeOn, setKeyMode, setKeyControl
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
 * @see setVolumeDialModeOn, setKeyMode, setKeyControl
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
 * @ingroup GA03
 * @brief  Key Mode setup 
 * @details KT0915 allows user to control the channel and volume by using keys/buttons to send digital control signals to CH and VOL pins. Please refer to Section 4 for a typical application circuit. The key mode is enabled by setting GPIO1<1:0> and GPIO2<1:0> to 01.
 * @details Mode A:
 * @details If KEY_MODE<1:0> is set to 00, Mode A is selected. In this mode, each time the CHP (CHM) is pressed, the channel frequency increases (decreases) by one step. The step sizes are defined by FMSPACE<1:0> and AMSPACE<1:0>. If the CHP (CHM) key is pressed for and held for a certain time (defined by TIME1<1:0>), the channel frequency will continue to increase (decrease) automatically at a certain pace (as defined by TIME2<2:0>) until the key is released.
 * @details Mode B:
 * @details If KEY_MODE<1:0> is set to 01, Mode B is selected. In this mode, each time the CHP (CHM) is pressed, the channel increases (decreases) by one step. The step sizes are defined by FMSPACE<1:0> and AMSPACE<1:0>. If the CHP (CHM) key is pressed and held for a specific time (TIME1<1:0>), the channel will continue to increase (decrease) automatically at a certain pace (TIME2<2:0>) even if the key is released. The movement is stopped when the key is pressed again.
 * @see KT0915; Monolithic Digital FM/MW/SW/LW Receiver Radio on a Chip(TM); pages 8 and 23.
 * @param value  00 = Working mode A; 01 = Working mode B;  Others = Reserved
 */
void KT0915::setKeyMode(uint8_t value)
{
    kt09xx_amcfg r;
    r.raw = getRegister(REG_AMCFG);   // Gets the current value of the register
    r.refined.KEY_MODE = value;       // Set the key mode
    setRegister(REG_AMCFG, r.raw);    // Stores the new value in the register
}

/**
 * @ingroup GA03
 * @brief  Sets the Audio and Channel Key Control 
 * @details VOL Pin Mode Selection 00 = High Z; 01 = Key controlled volume increase/decrease; 10 = Dial controlled volume increase/decrease; 11 = Reserved
 * @details CH Pin Mode Selection; 00 = High Z; 01 = Key controlled channel increase / decrease; 10 = Dial controlled channel increase / decrease; 11 = Reserved
 * @see KT0915; Monolithic Digital FM/MW/SW/LW Receiver Radio on a Chip(TM); page 20.
 * @param audioControl VOL Pin Mode Selection; 00 = High Z; 01 = Key controlled volume increase/decrease; 10 = Dial controlled volume increase/decrease; 11 = Reserved
 * @param channelControl CH Pin Mode Selection; 00 = High Z; 01 = Key controlled channel increase / decrease; 10 = Dial controlled channel increase / decrease 11 = Reserved
 */
void KT0915::setKeyControl(uint8_t audioControl, uint8_t channelControl)
{
    kt09xx_gpiocfg r;
    r.raw = getRegister(REG_GPIOCFG);  // Gets the current value of the register
    r.refined.GPIO1 = channelControl;  //
    r.refined.GPIO2 = audioControl;    //
    setRegister(REG_GPIOCFG, r.raw);   // Stores the new value in the register
}

/**
 * @ingroup GA03
 * @brief Audio Gain 
 * @details This function set the audio gain you want to use. See table below.
 * | value | Gain Selection |
 * | ----- | -------------- | 
 * |   0   |      3dB       |
 * |   1   |      6dB       |
 * |   2   |     -3dB       | 
 * |   3   |      0dB       |         
 * 
 * @param gain  See table above
 */
void KT0915::setAudioGain(uint8_t gain)
{
    kt09xx_amsyscfg r;
    r.raw = getRegister(REG_AMSYSCFG);
    r.refined.AU_GAIN = gain;
    setRegister(REG_AMSYSCFG, r.raw);
}

/**
 * @ingroup GA03
 * @brief Sets the audio volume level
 * @details This method is used to control the audio volume level. The value 0 mutes the  device and 31 sets the device to the maximum volume.
 * @param volume between 0 and 31. 
 */
void KT0915::setVolume(uint8_t volume)
{
    kt09xx_rxcfg rx;

    rx.raw = getRegister(REG_RXCFG);
    rx.refined.VOLUME = volume;
    setRegister(REG_RXCFG, rx.raw);
    this->currentVolume = volume;
}

/**
 * @ingroup GA03
 * @brief Increases the audio volume
 * 
 */
void KT0915::setVolumeUp()
{
    if (this->currentVolume == 31)
        return;
    setVolume(this->currentVolume + 1);
}

/**
 * @ingroup GA03
 * @brief Decreases the audio volume
 * 
 */
void KT0915::setVolumeDown()
{
    if (this->currentVolume == 0)
        return;
    setVolume(this->currentVolume - 1);
}

/**
 * @ingroup GA03
 * @brief Returns the current audio volume 
 * @details Returns a value between 0 and 31.
 * @return uint8_t Value between 0 and 31.
 */
uint8_t KT0915::getVolume()
{
    return this->currentVolume;
}

/**
 * @ingroup GA03
 * @brief AM and FM Softmute control
 * @details The internal KT0915 device register, FM and AM SMUTE 0 means enable and 1 disable. This function inverts this concept. So 1 means enable and 0 disable.
 *  
 * @param on_off 1 = ON (enable); 0 = OFF disable
 */
void KT0915::setSoftMute(uint8_t on_off)
{
    kt09xx_volume v;
    v.raw = getRegister(REG_VOLUME);
    v.refined.AMDSMUTE = v.refined.FMDSMUTE  = !on_off;
    setRegister(REG_VOLUME, v.raw); // Stores the new values of the register
}

/**
 * @ingroup GA03
 * @brief Sets the bass level
 * @details  Bass Boost Effect Mode Selection
 * | Value | Level   |
 * | ----- | ------- |
 * |   0   | Disable |
 * |   1   | Low     |
 * |   2   | Med     |
 * |   3   | High    | 
 *  
 * @param on_off  see table above
 */
void KT0915::setAudioBass(uint8_t bass)
{
    kt09xx_volume v;
    v.raw = getRegister(REG_VOLUME);
    v.refined.BASS = bass;
    setRegister(REG_VOLUME, v.raw); // Stores the new values of the register
}

/**
 * @brief  Sets the output audio mute
 * 
 * @param mute_on_off 1 = mute; 0 unmute
 */
void KT0915::setAudioMute(uint8_t mute_on_off)
{
    kt09xx_volume v;

    v.raw = getRegister(REG_VOLUME);
    v.refined.DMUTE = !mute_on_off;
    setRegister(REG_VOLUME, v.raw); // Stores the new values of the register
}

/**
 * @ingroup GA03
 * @brief Sets Audio DAC Anti-pop Configuration
 * 
 * @details  Bass Boost Effect Mode Selection
 * | Value | AC - coupling capacitor   |
 * | ----- | ------------------------- |
 * |   0   | 100uF   |
 * |   1   | 60uF    |
 * |   2   | 20uF    |
 * |   3   | 10uF    | 
 *  
 * @param on_off  see table above
 */
void KT0915::setAudioAntiPop(uint8_t value)
{
    kt09xx_volume v;
    v.refined.POP = value;
    setRegister(REG_VOLUME, v.raw); // Stores the new values of the register
}

/**
 * @ingroup GA03
 * @brief Sets the Left Channel Inverse Control 
 * @details If enable, inverts the left channel audio signal
 * @details A fully differential audio signal can be got from LOUT an ROUT if the INV_LEFT_AUDIO bit and MONO bit are set to 1.
 * @param value  ENABLE_ON (1); ENABLE_OFF (0)
 */
void KT0915::setLeftChannelInverseControl(uint8_t enable_disable)
{
    kt09xx_amdsp r;
    r.raw = getRegister(REG_AMDSP);
    r.refined.INV_LEFT_AUDIO = enable_disable;
    setRegister(REG_AMDSP,r.raw);
}

/**
 * @ingroup GA03
 * @brief   Receiver startup 
 * @details You have to use this method to configure the way that the device will work. For example: enable and disable device control; oscillator type and reference clock type (crystal or external) 
 * @details The tabe below shows  the oscillator frequencies supported by the device.
 * @details If you omit the crystal type parameter, will be considered  0 (32.768KHz). 
 * @details For a low frequency crystal oscillator, selects 32.768KHz or 38KHz crystals. 
 * @details Alternatively, you can use a CMOS level external reference clock may be used by setting 
 * @details the parameter ref_clock to 1 (REF_CLOCK_ENABLE) and setting the reference clock according to the table below.
 * @details The code below shows how to use the setup function.
 * @details the enable_pin parameter sets the way you are controlling the KT0915 pin 9. 
 * 
 * @code
 * #include <KT0915.h>
 * #define RESET_PIN 12   // set it to -1 if you want to use the RST pin of your MCU.
 * KT0915 radio;
 * void setup() {
 *    // Set the parameter enablePin to -1 if you are controlling the enable status via circuit; Select OSCILLATOR_32KHZ, OSCILLATOR_12MHZ etc
 *    // radio.setup(RESET_PIN); Instead the line below, if you use this line, the crystal type considered will be 32.768KHz.   
 *    radio.setup(RESET_PIN, OSCILLATOR_12MHZ, REF_CLOCK_DISABLE );
 * }
 * @endcode
 * 
 * Oscillator frequencies supported
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
 * @param enablePin         if >= 0,  then you control the device enable or disable status. if -1, you are using the circuit to crontole that. 
 * @param oscillator_type   oscillator type. You can use crystal or external clock. See comments and table above.
 * @param ref_clock         set to 0 if you are using crystal (Reference clock disabled - default); set to 1 if you are using an external reference clock.
 */
void KT0915::setup(int enable_pin, uint8_t oscillator_type, uint8_t ref_clock)
{
    this->enablePin = enable_pin;
    enable(1);
    setReferenceClockType(oscillator_type, ref_clock);
    setVolume(this->currentVolume);
}

/** 
 * @defgroup GA04 Tune Methods 
 * @section  GA04 Tune Methods  
 * @details  Methods to tune and set the receiver mode
 */


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
    setRegister(REG_DSPCFGA, reg.raw);
}

/**
 * @ingroup GA04
 * @brief Return true if the stereo indicator is set to 3;
 * @return true is stereo
 */
bool KT0915::isFmStereo()
{
    kt09xx_statusa r;
    r.raw = getRegister(REG_STATUSA);
    return (r.refined.ST == 3);
}

/**
 * @ingroup GA04
 * @brief Gets the current FM RSSI 
 * 
 * @return int RSSI value
 */
int KT0915::getFmRssi()
{
    kt09xx_statusa r;
    r.raw = getRegister(REG_STATUSA);
    return (r.refined.FMRSSI * 3);
};

/**
 * @ingroup GA04
 * @brief Gets the current AM RSSI  
 * 
 * @return int RSSI value
 */
int KT0915::getAmRssi()
{
    kt09xx_amdstatusa r;
    r.raw = getRegister(REG_AMSTATUSA);
    return (r.refined.AMRSSI * 3);
};

/**
 * @ingroup GA04
 * @brief Gets current SNR value
 * 
 * @return int  FM SNR value
 */
int KT0915::getFmSnr()
{
    kt09xx_statusc r;
    r.raw = getRegister(REG_STATUSC);
    return (r.refined.FMSNR);
};


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
 * @ingroup GA04
 * @brief Sets FM AFC Disable Control 
 * @details This function inverts the register enable/disable concept. So, here, 1 means enable and 0 disable.
 * @param  on_off  1 = enable AFC; 0 = disable AFC.
 */
void KT0915::setFmAfc(uint8_t on_off)
{
    kt09xx_locfga r;
    r.refined.FMAFCD = !on_off;
    setRegister(REG_LOCFGA, r.raw);
}

/**
 * @ingroup GA04
 * @brief Sets AM AFC Disable Control 
 * @details This function inverts the register enable/disable concept. So, here, 1 means enable and 0 disable.
 * @param  on_off  1 = enable AFC; 0 = disable AFC.
 */
void KT0915::setAmAfc(uint8_t on_off)
{
    kt09xx_amsyscfg r;
    r.raw = getRegister(REG_AMSYSCFG); // Gets the current value of the register
    r.refined.RESERVED1 = 1;
    r.refined.AMAFCD = !on_off;
    setRegister(REG_AMSYSCFG, r.raw);
}

/**
 * @ingroup GA04
 * @brief Sets the AM Space 
 * @details Selects AM channel space 
 * 
 * | value | space |
 * | ----- | ----- | 
 * |   0   |  1KHz | 
 * |   1   |  9KHz |
 * |   2   | 10KHz |
 * |   3   | 10KHz |
 * 
 * @param value  See table above
 */
void KT0915::setAmSpace(uint8_t value)
{
    kt09xx_amcfg r; 
    r.raw = getRegister(REG_AMCFG);
    this->currentAmSpace = r.refined.AMSPACE = value;
    setRegister(REG_AMCFG, r.raw);
   
}

/**
 * @ingroup GA04
 * @brief Sets the FM Space 
 * @details FM Channel Spacing; 00 = 200KHz; 01 = 100KHz; 10 = 50KHz
 * @details Selects FN channel space 
 * 
 * | value | space KHz |
 * | ----- | --------- | 
 * |   0   |    200    | 
 * |   1   |    100    |
 * |   2   |     50    |
 * |   3   |      ?    |
 * 
 * @param value  See table above
 */
void KT0915::setFmSpace(uint8_t value)
{
    kt09xx_seek r;
    r.raw = getRegister(REG_SEEK);
    this->currentFmSpace = r.refined.FMSPACE = value;
    setRegister(REG_SEEK, r.raw);
}

/**
 * @ingroup GA04
 * @brief Sets AM Channel Bandwidth Selection
 * @details Configures the AM Bandwidth 
 * 
 * | value | Bandwidth |
 * | ----- | --------- | 
 * |   0   |  2KHz     | 
 * |   1   |  2KHz     |
 * |   2   |  4KHz     |
 * |   3   |  6KHz     |
 * 
 * @param value  See table above
 */
void KT0915::setAmBandwidth(uint8_t value)
{
    kt09xx_amdsp r;
    r.raw = getRegister(REG_AMDSP);
    r.refined.AM_BW = value;
    setRegister(REG_AMDSP, r.raw);
}

/**
 * @ingroup GA04
 * @brief Gets current AM Channel Bandwidth Selection
 * 
 * | value | Bandwidth |
 * | ----- | --------- | 
 * |   0   |  2KHz     | 
 * |   1   |  2KHz     |
 * |   2   |  4KHz     |
 * |   3   |  6KHz     |
 * 
 * @return See table above
 */
uint8_t KT0915::getAmBandwidth()
{
    kt09xx_amdsp r;
    r.raw = getRegister(REG_AMDSP);
    return r.refined.AM_BW;
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
    kt09xx_locfgc fm32;

    this->currentStep = step;
    this->currentFrequency = default_frequency;
    this->minimumFrequency = minimum_frequency;
    this->maximumFrequency = maximum_frequency;
    this->currentMode = MODE_FM;

    reg.raw = getRegister(REG_AMSYSCFG);
    reg.refined.AM_FM = MODE_FM;
    reg.refined.USERBAND = this->currentDialMode;
    reg.refined.REFCLK = this->currentRefClockType;
    reg.refined.RCLK_EN = this->currentRefClockEnabled;
    setRegister(REG_AMSYSCFG, reg.raw); // Stores the new value in the register

    // Select the right FM band (Campus Band or regular band)
    fm32.raw = getRegister(REG_LOCFGC);
    fm32.refined.CAMPUSBAND_EN = (maximum_frequency <= 64000)? 1:0;
    setRegister(REG_LOCFGC, fm32.raw);

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
void KT0915::setAM(uint32_t minimum_frequency, uint32_t maximum_frequency, uint32_t default_frequency, uint16_t step, uint8_t am_space)
{
    kt09xx_amsyscfg reg;

    this->currentStep = step;
    this->currentFrequency = default_frequency;
    this->minimumFrequency = minimum_frequency;
    this->maximumFrequency = maximum_frequency;
    this->currentMode = MODE_AM;

    reg.raw = getRegister(REG_AMSYSCFG);
    reg.refined.AM_FM = MODE_AM;
    reg.refined.USERBAND = this->currentDialMode;
    reg.refined.REFCLK = this->currentRefClockType;
    reg.refined.RCLK_EN = this->currentRefClockEnabled;
    setRegister(REG_AMSYSCFG, reg.raw); // Stores the new value in the register
    setAmSpace(am_space);

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
        reg_amchan.refined.AMTUNE = 1; // TODO Check
        reg_amchan.refined.AMCHAN = frequency;
        setRegister(REG_AMCHAN, reg_amchan.raw);
    }
    else
    {
        reg_tune.refined.FMTUNE = 1; // // TODO Check
        reg_tune.refined.RESERVED = 0;
        reg_tune.refined.FMCHAN = frequency / 50;
        setRegister(REG_TUNE, reg_tune.raw);
    }

    this->currentFrequency = frequency;

    delay(30);
}

/**
 * @ingroup GA04
 * @brief Increments the frequency one step
 * @details if the frequency plus the step value is greater than the maximum frequency for the band, 
 * @details tne current frequency will be set to minimum frequency.
 * 
 * @see setFrequency
 */
void KT0915::frequencyUp()
{
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
void KT0915::frequencyDown()
{
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

/**
 * @ingroup GA04
 * @brief Gets the FM Channel Setting 
 * @details This method returns the current channel value for FM tune. 
 * @details The channel value multiplied by 50 is the current FM frequency in KHz.  
 * @return  FM Channel number 
 */
uint16_t KT0915::getFmCurrentChannel() {
    kt09xx_tune r;
    r.raw = getRegister(REG_TUNE);
    return r.refined.FMCHAN;
};

/**
 * @ingroup GA04
 * @brief Gets the current AM Channel Setting
 * @details This method returns the current channel value for AM tune
 * @details Actually this value is the AM current frequency in KHz
 * @return  AM Channel number (frequency in KHz) 
 */
uint16_t KT0915::getAmCurrentChannel() {
    kt09xx_amchan r;
    r.raw = getRegister(REG_AMCHAN);
    return r.refined.AMCHAN;
};


/**
 * @todo Not enough information to do this so far. 
 * @ingroup GA04
 * @brief Should Seek a station
 * @details However, there no enough information to implement it.
 */
void KT0915::seekStation()
{
    // TODO
}

