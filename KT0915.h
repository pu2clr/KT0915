
/**
 * @mainpage PU2CLR KT0915 Arduino Library 
 * @brief PU2CLR KT0915 Arduino Library implementation 
 * @details This is an Arduino library for the KT0915, BROADCAST RECEIVER.  
 * @details It works with I2C protocol and can provide an easier interface for controlling the KT0915 device.<br>
 * @details This library was built based on KT0915 Datasheet from KTMicro (Monolithic Digital FM/MW/SW/LW Receiver Radio-on-a-Chip TM). 
 * @details Others sources help the author to build this library.  
 * @details They are referenced in the documentation for this library on: https://github.com/pu2clr/KT0915
 * @details This library uses the I2C protocols to read and write KT0915 registers. In this context, registers are memory position into the device.
 * @details The KT0915 is a full band AM (LW, MW and SW) and FM DSP receiver that can provide you a easy way to build a high quality radio with low cost. 
 * @details This device, will surprise hobbyists and experimenters with its simplicity. 
 * 
 * This library can be freely distributed using the MIT Free Software model.
 * 
 * Copyright (c) 2020 Ricardo Lima Caratti.  
 * Contact: pu2clr@gmail.com
 */

#include <Arduino.h>
#include <Wire.h>

#define AKC595X_I2C_ADRESS ??

#define REG_CHIP_ID 0x01
#define REG_SEEK 0x02
#define REG_TUNE 0x03
#define REG_VOLUME 0x04
#define REG_DSPCFGA 0x05
#define REG_LOCFGA 0x0A
#define REG_LOCFGC 0x0C
#define REG_RXCFG 0x0F
#define REG_STATUSA 0x12
#define REG_STATUSB 0x13
#define REG_STATUSC 0x14
#define REG_AMSYSCFG 0x16
#define REG_AMCHAN 0x17
#define REG_AMCALI 0x18
#define REG_GPIOCFG 0x1D
#define REG_AMDSP 0x22
#define REG_AMSTATUSA 0x24
#define REG_AMSTATUSB 0x25
#define REG_SOFTMUTE 0x2E
#define REG_USERSTARTCH 0x2F
#define REG_USERGUARD 0x30
#define REG_USERCHANNUM 0x31
#define REG_AMCFG 0x33
#define REG_AMCFG2 0x34
#define REG_AFC 0x3C

typedef union {
    uint16_t chip_id;
    char chip_id_ascii[2];
} kt09xx_chip_id;

typedef union {
    struct
    {
        uint8_t DMUTEL : 1;   //!< Left Channel Mute Control; 0 = Left channel mute enable; 1 = Left channel mute disable
        uint8_t DMUTER : 1;   //!< Right Channel Mute Control; 0 = Left channel mute enable; 1 = Left channel mute disable
        uint8_t FMSPACE : 2;  //!< FM Channel Spacing; 00 = 200KHz; 01 = 100KHz; 10 = 50KHz
        uint8_t RESERVED : 11 //!< Reserved
    } refinied;
    unit16_t raw;
} kt09xx_seek;

typedef union {
    struct
    {
        uint8_t FMCHAN : 12;  //!< FM Channel Setting FMCHAN<11:0>=Frequency (KHz) / 50KHz. For example, if desired channel is 86MHz, then the FMCHAN<11:0> should be 0x06B8.
        uint8_t RESERVED : 3; //!< Reserved
        uint8_t FMTUNE : 1;   //!< FM Tune Enable;  0 = Normal operation 1 = Start to tune to desired FM channel
    } refined;
    unit16_t raw;
} kt09xx_tune;

typedef union {
    struct
    {
        uint8_t RESERVED1 : 4;  //!< Reserved
        uint8_t POP : 2;        //!< Audio DAC Anti - pop Configuration00 : 100uF AC - coupling capacitor 01 : 10 : 11 : Reserved 60uF AC - coupling capacitor 20uF AC - coupling capacitor 10uF AC - coupling capacitor.
        uint8_t RESERVED2 : 2;  //!< Reserved
        uint8_t BASS : 2;       //!< Bass Boost Effect Mode Selection; 00 = Disable;  01 = Low ; 10 = Med;  11 = High.
        uint8_t RESERVED3 : 3;  //!< Reserved
        uint8_t DMUTE : 1;      //!< Mute Disable; 0 = Mute enable; 1 = Mute disable.
        uint8_t AMDSMUTE : 1;   //!< AM Softmute Disable; 0 = AM softmute enable; 1 = AM softmute disable.
        uint8_t FMDSMUTE : 1;   //!< AM Softmute Disable; 0 = FM softmute enable; 1 = FM softmute disable.
    } refined;
    unit16_t raw;
} kt09xx_volume;


typedef union {
    struct
    {
        uint8_t RESERVED1 : 5;  //!< Reserved
        uint8_t DBLND : 1;      //!< Blend disable; 0 = Blend enable; 1 = Blend disable
        uint8_t RESERVED2 : 2;  //!< Reserved
        uint8_t BLNDADJ : 2;    //! Stereo/Mono Blend; Level Adjustment 00 = High; 01 = Highest 10; = Lowest 11 = Low
        uint8_t RESERVED3 : 1;  //!< Reserved
        uint8_t DE : 1;         //!< De-emphasis Time Constant Selection. 0 = 75us;  1 = 50us.
        uint8_t RESERVED4 : 3;  //!< Reserved
        uint8_t MONO : 1;       //!< Mono Select; 0 = Stereo; 1 = Force mono
    } refined;
    unit16_t raw;
} kt09xx_dspcfga;

typedef union {
    struct
    {
        uint8_t RESERVED1 : 8;  //!< Reserved
        uint8_t FMAFCD : 1;     //!< AFC Disable Control Bit; 0 = AFC enable; 1 = AFC disable.
        uint8_t RESERVED2 : 7;  //!< Reserved
    } refined; 
    unit16_t raw;
} kt09xx_locfga; // LOCFGA

typedef union {
    struct
    {
        uint8_t RESERVED1 : 3;      //!< Reserved
        uint8_t CAMPUSBAND_EN : 1;  //!< Campus FM Band Enable; 0 = User can only use 64MHz ~110MHz;  1 = User can extend the FM band down to 32MHz
        uint8_t RESERVED2 : 12;     //!< Reserved
    } refined;
    unit16_t raw;
} kt09xx_locfgc; // LOCFGC


typedef union {
    struct
    {
        uint8_t VOLUME : 5;         //!< Volume Control 11111 = 0dB 11110 = -2dB 11101 = -4dB .... 00010 = -58dB 00001 = -60dB 00000 = Mute
        uint8_t RESERVED1 : 7;      //!< Campus FM Band Enable; 0 = User can only use 64MHz ~110MHz;  1 = User can extend the FM band down to 32MHz
        uint8_t STDBY : 1;          //!< Standby Mode Enable. 0 = Disable; 1 = Enable
        uint8_t RESERVED2 : 3;      //!< Reserved
    } refined;
    unit16_t raw;
} kt09xx_rxcfg; // RXCFG




class KT0915 {

protected:
    int deviceAddress = AKC595X_I2C_ADRESS;
    int resetPin = -1;

    uint8_t volume;                             //!< Store the current volume
    uint16_t currentStep;                       //!< Strore the current step
    uint16_t currentFrequency;                  //!< Store the current frequency
    uint8_t currentMode;                        //!< Store the current mode
    uint8_t currentCrystalType = CRYSTAL_32KHZ; //!< Store the crystal type 

public:
    void reset();
    void setI2CBusAddress(int deviceAddress);

    void setup(int reset_pin);
    void setup(int reset_pin, uint8_t crystal_type);

};

