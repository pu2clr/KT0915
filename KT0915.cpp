/**
 * @file KT0915.cpp
 * @author Ricardo LIma Caratti (you@domain.com)
 * @brief PU2CLR KT0915 Arduino Library
 * @details KT0915 Arduino Library implementation. This is an Arduino library for the KT0915, BROADCAST RECEIVER.  
 * @details It works with I2C protocol and can provide an easier interface for controlling the KT0915 device.<br>
 * @details This library was built based on KT0915 Datasheet from KTMicro (Monolithic Digital FM/MW/SW/LW Receiver Radio-on-a-Chip TM). 
 * 
 * This library can be freely distributed using the MIT Free Software model.
 * @copyright Copyright (c) 2020 Ricardo Lima Caratti. 
 */

#include <KT0915.h>

/** 
 * @defgroup GA03 Basic Methods 
 * @section  GA03 Basic 
 * @details  Low level functions used to operate with the KT09XX registers
 */

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
    Wire.write(param.refined.lowByte);              
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
    result.refined.lowByte = Wire.read();
    result.refined.highByte = Wire.read();
    
    delayMicroseconds(2000);

    return result.raw;
}

/**
 * @ingroup GA03
 * @brief Sets the Crystal Type
 * @details Configures the Crystal Type you are using in your circuit.
 * @details The valid crystal type are 12MHz or 32.768Khz
 * 
 * @param crystal 
 */
void KT0915::setCrystalType(uint8_t crystal)
{
    kt09xx_amsyscfg reg;
    reg.raw = getRegister(REG_AMSYSCFG);  // Gets the current value of the register
    reg.refined.REFCLK = crystal;         // Changes just crystal parameter
    setRegister(REG_AMSYSCFG, reg.raw);   // Strores the new value to the register 
}

/**
 * @ingroup GA03
 * @brief Resets the system. 
 * @details This function can be used to reset the KT0915 device. you can also use the RTS pin of your MCU. 
 * @details In this case, the RESET pin have to be set to  -1. This setup can be configured calling KT0915::setup method.
 * 
 * @see setup
 */
void KT0915::reset()
{
    if (this->resetPin < 0)
        return; // Do nothing.

    pinMode(this->resetPin, OUTPUT);
    delay(10);
    digitalWrite(this->resetPin, LOW);
    delay(10);
    digitalWrite(this->resetPin, HIGH);
    delay(10);
}

/**
 * @ingroup GA03
 * @todo check the real i2c address 
 * @brief Set I2C bus address 
 * 
 * @param deviceAddress  I2C address
 */
void KT0915::setI2CBusAddress(int deviceAddress) {
    this->deviceAddress = deviceAddress;
};

/**
 * @ingroup GA03
 * @brief    Receiver startup 
 * @details  Use this method to define the MCU (Arduino) RESET pin and the crystal type you are using. 
 * @details  The options for the crystal can be seen in the table below.
 * @details  If you omit the crystal type parameter, will be considered  0 (32.768KHz). 
 * @details  The code below shows how to use the setup function.
 * 
 * @code
 * #include <KT0915.h>
 * #define RESET_PIN 12   // set it to -1 if you want to use the RST pin of your MCU.
 * KT0915 radio;
 * void setup() {
 *    // Set RESET_PIN to -1 if you are using the Arduino RST pin; Select CRYSTAL_32KHZ, CRYSTAL_12MHZ etc
 *    // radio.setup(RESET_PIN); Instead the line below, if you use this line, the crystal type considered will be 32.768KHz.   
 *    radio.setup(RESET_PIN, CRYSTAL_12MHZ);
 * }
 * @endcode
 * 
 * Crystal type table
 * | Dec | binary | Description | defined constant |
 * | --  | ------ | ----------- | ---------------  |
 * | 0   | 0000   | 32.768KHz   | CRYSTAL_32KHZ    |
 * | 1   | 0001   | 6.5MHz      | CRYSTAL_6_5MHZ   |
 * | 2   | 0010   | 7.6MHz      | CRYSTAL_7_6MHZ   |
 * | 3   | 0011   | 12MHz       | CRYSTAL_12MHZ    |
 * | 4   | 0100   | 13MHz       | CRYSTAL_13MHZ    |
 * | 5   | 0101   | 15.2MHz     | CRYSTAL_15_2MHZ  |
 * | 6   | 0110   | 19.2MHz     | CRYSTAL_19_2MHZ  | 
 * | 7   | 0111   | 24MHz       | CRYSTAL_24MHZ    |
 * | 8   | 1000   | 26MHz       | CRYSTAL_26MHZ    |
 * | 9   | 1001   | ?? 38KHz ?? | CRYSTAL_38KHz    |
 * 
 * @param resetPin      if >= 0,  then you control the RESET. if -1, you are using ths Arduino RST pin. 
 * @param crystal_type  See the table above.
 */
void KT0915::setup(int reset_pin, uint8_t crystal_type) {
    this->resetPin = reset_pin;
    reset();
    setCrystalType(crystal_type);
}

