
/**
 * @mainpage PU2CLR KT0915 Arduino Library 
 * @file KT0915.h
 * @brief PU2CLR KT0915 Arduino Library implementation. <br> 
 * @details This is an Arduino library for the KT0915, BROADCAST RECEIVER.<br>  
 * @details It works with I2C protocol and can provide an easier interface for controlling the KT0915 device.<br>
 * @details This library was built based on KT0915 Datasheet from KTMicro (Monolithic Digital FM/MW/SW/LW Receiver Radio-on-a-Chip TM). 
 * @details Others sources help the author to build this library. They are referenced in the documentation for this library on: https://github.com/pu2clr/KT0915
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

#define AKC595X_I2C_ADRESS 35  // It is needed to check it when the KT0915 device arrives.

#define MODE_FM     0
#define MODE_AM     1


#define CRYSTAL_32KHZ    0      //  32.768KHz 
#define CRYSTAL_6_5MHZ   1      //  6.5MHz 
#define CRYSTAL_7_6MHZ   2      //  7.6MHz
#define CRYSTAL_12MHZ    3      //  12MHz
#define CRYSTAL_13MHZ    4      //  13MHz
#define CRYSTAL_15_2MHZ  5      //  15.2MHz
#define CRYSTAL_19_2MHZ  6      //  19.2MHz
#define CRYSTAL_24MHZ    7      //  24MHz
#define CRYSTAL_26MHZ    8      //  26MHz
#define CRYSTAL_38KHz    9      //  38KHz 

#define REF_CLOCK_ENABLE    1   // 
#define REF_CLOCK_DISABLE   0


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

/**
 * @defgroup GA01 Union, Structure and Defined Data Types  
 * @brief   KT0915 Defined Data Types 
 * @details Defined Data Types is a way to represent the AKC695X registers information
 * @details Some information appears to be inaccurate due to translation problems from Chinese to English.
 * @details The information shown here was extracted from Datasheet:
 * @details AKC6955 stereo FM / TV / MW / SW / LW digital tuning radio documentation.
 * @details Other information seems incomplete even in the original Chinese Datasheet. 
 * @details For example: Reg 10 (0x0A). There is no information about it. The Reg11 and 12 seem wrong  
 */

/**
 * @ingroup GA01
 * @brief 3.10.1. CHIP ID (Address 0x01)
 * @see KT0915; Monolithic Digital FM/MW/SW/LW Receiver Radio on a Chip(TM); page 15.
 */
typedef union {
    uint16_t chip_id;
    char chip_id_ascii[2];
} kt09xx_chip_id;

/**
 * @ingroup GA01
 * @brief 3.10.2. SEEK (Address 0x02)
 * @see KT0915; Monolithic Digital FM/MW/SW/LW Receiver Radio on a Chip(TM); page 15.
 */
typedef union {
    struct
    {
        uint8_t  DMUTEL : 1;   //!< Left Channel Mute Control; 0 = Left channel mute enable; 1 = Left channel mute disable
        uint8_t  DMUTER : 1;   //!< Right Channel Mute Control; 0 = Left channel mute enable; 1 = Left channel mute disable
        uint8_t  FMSPACE : 2;  //!< FM Channel Spacing; 00 = 200KHz; 01 = 100KHz; 10 = 50KHz
        uint16_t RESERVED : 11; //!< Reserved
    } refinied;
    uint16_t raw;
} kt09xx_seek;

/**
 * @ingroup GA01
 * @brief 3.10.3. TUNE (Address 0x03)
 * @see KT0915; Monolithic Digital FM/MW/SW/LW Receiver Radio on a Chip(TM); page 15.
 */
typedef union {
    struct
    {
        uint16_t FMCHAN : 12;  //!< FM Channel Setting FMCHAN<11:0>=Frequency (KHz) / 50KHz. For example, if desired channel is 86MHz, then the FMCHAN<11:0> should be 0x06B8.
        uint8_t RESERVED : 3; //!< Reserved
        uint8_t FMTUNE : 1;   //!< FM Tune Enable;  0 = Normal operation 1 = Start to tune to desired FM channel
    } refined;
    uint16_t raw;
} kt09xx_tune;

/**
 * @ingroup GA01
 * @brief 3.10.4. VOLUME (Address 0x04)
 * @see KT0915; Monolithic Digital FM/MW/SW/LW Receiver Radio on a Chip(TM); page 15/16.
 */
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
    uint16_t raw;
} kt09xx_volume;

/**
 * @ingroup GA01
 * @brief 3.10.5. DSPCFGA (Address 0x05)
 * @see KT0915; Monolithic Digital FM/MW/SW/LW Receiver Radio on a Chip(TM); page 16/17.
 */
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
    uint16_t raw;
} kt09xx_dspcfga;

/**
 * @ingroup GA01
 * @brief 3.10.6. LOCFGA (Address 0x0A)
 * @see KT0915; Monolithic Digital FM/MW/SW/LW Receiver Radio on a Chip(TM); page 17.
 */
typedef union {
    struct
    {
        uint8_t RESERVED1 : 8;  //!< Reserved
        uint8_t FMAFCD : 1;     //!< AFC Disable Control Bit; 0 = AFC enable; 1 = AFC disable.
        uint8_t RESERVED2 : 7;  //!< Reserved
    } refined; 
    uint16_t raw;
} kt09xx_locfga; // LOCFGA

/**
 * @ingroup GA01
 * @brief 3.10.7. LOCFGC (Address 0x0C)
 * @see KT0915; Monolithic Digital FM/MW/SW/LW Receiver Radio on a Chip(TM); page 17.
 */
typedef union {
    struct
    {
        uint8_t RESERVED1 : 3;      //!< Reserved
        uint8_t CAMPUSBAND_EN : 1;  //!< Campus FM Band Enable; 0 = User can only use 64MHz ~110MHz;  1 = User can extend the FM band down to 32MHz
        uint16_t RESERVED2 : 12;     //!< Reserved
    } refined;
    uint16_t raw;
} kt09xx_locfgc; // LOCFGC

/**
 * @ingroup GA01
 * @brief 3.10.8. RXCFG (Address 0x0F)
 * @see KT0915; Monolithic Digital FM/MW/SW/LW Receiver Radio on a Chip(TM); page 17.
 */
typedef union {
    struct
    {
        uint8_t VOLUME : 5;         //!< Volume Control 11111 = 0dB 11110 = -2dB 11101 = -4dB .... 00010 = -58dB 00001 = -60dB 00000 = Mute
        uint8_t RESERVED1 : 7;      //!< Campus FM Band Enable; 0 = User can only use 64MHz ~110MHz;  1 = User can extend the FM band down to 32MHz
        uint8_t STDBY : 1;          //!< Standby Mode Enable. 0 = Disable; 1 = Enable
        uint8_t RESERVED2 : 3;      //!< Reserved
    } refined;
    uint16_t raw;
} kt09xx_rxcfg; // RXCFG

/**
 * @ingroup GA01
 * @brief 3.10.19. STATUSA (Address 0x12)
 * @see KT0915; Monolithic Digital FM/MW/SW/LW Receiver Radio on a Chip(TM); page 18.
 */
typedef union {
    struct
    {
        uint8_t RESERVED1 : 3;      //!< Reserved
        uint8_t FMRSSI : 5;         //!< FM RSSI Value Indicator; RSSI starts from -100dBm and step is 3dB namely; RSSI(dBm) = -100 + FMRSSI<4:0> *3dB
        uint8_t ST : 2;             //!< Stereo Indicator; 11 = Stereo state; Other = Mono state
        uint8_t LO_LOCK : 1;        //!< LO Synthesizer Ready Indicator; 0 = Not ready; 1 = Ready
        uint8_t PLL_LOCK : 1;       //!< System PLL Ready Indicator; 0 = Not ready; 1 = System PLL ready
        uint8_t RESERVED2 : 2;      //!< Reserved
        uint8_t STC : 1;            //!< Seek/Tune Complete; 0 = Not Complete; 1 = Complete; Every time the Seek/tune process begins, the STC bit will clear to zero by hardware.
        uint8_t XTAL_OK : 1;        //!< Crystal ready indictor; 0 = Not ready; 1 = Crystal is ok
    } refined;
    uint16_t raw;
} kt09xx_statusa; // STATUSA

/**
 * @ingroup GA01
 * @brief 3.10.10. STATUSB (Address 0x13)
 * @see KT0915; Monolithic Digital FM/MW/SW/LW Receiver Radio on a Chip(TM); page 18.
 */
typedef union {
    struct
    {
        uint8_t RESERVED1 : 1;      //!< Reserved
        uint16_t RDCHAN : 15;        //!< Current Channel Indicator
    } refined;
    uint16_t raw;
} kt09xx_statusb; // STATUSB

/**
 * @ingroup GA01
 * @brief 3.10.11. STATUSC (Address 0x14)
 * @see KT0915; Monolithic Digital FM/MW/SW/LW Receiver Radio on a Chip(TM); page 18/19.
 */
typedef union {
    struct
    {
        uint8_t RESERVED1 : 6;      //!< Reserved
        uint8_t FMSNR : 7;          //!< Channel SNR value is FM mode.; 0000000 = Minimum SNR; 1111111 = Maximum SNR
        uint8_t CHIPRDY : 1;        //!< Chip Ready Indicator; 0 = Chip is not ready; 1 = Chip is ready, calibration done
        uint8_t RESERVED2 : 1;      //!< Reserved
        uint8_t PWSTATUS : 1;       //!< Power Status Indicator; 0 = Power not ready; 1 = Power ready
    } refined;
    uint16_t raw;
} kt09xx_statusc; // STATUSC

/**
 * @ingroup GA01 
 * @brief 3.10.12. AMSYSCFG (Address 0x16)
 * @details The table below shows  REFCLK possibvle values 
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
 * @see KT0915; Monolithic Digital FM/MW/SW/LW Receiver Radio on a Chip(TM); page 19.
 * 
 */
typedef union {
    struct
    {
        uint8_t AMAFCD : 1;     //!< AFC Disable Control in AM Mode; 0 = Enable; 1 = Disable
        uint8_t RESERVED1 : 5;  //!< Reserved
        uint8_t AU_GAIN : 2;    //!< Audio Gain Selection; 01 = 6dB; 00 = 3dB; 11 = 0dB; 10 = -3dB
        uint8_t REFCLK : 4;     //!< See Crystal type table
        uint8_t RCLK_EN : 1;    //!< Reference Clock Enable; 0 = Crystal; 1 = Reference clock
        uint8_t RESERVED2 : 1;  //!< Reserved
        uint8_t USERBAND : 1;   //!< User Definition Band Enable; 0 = Use internal defined band; 1 = Use user-defined band which is specified in USER_START_CHAN<14:0>, USER_GUARD<8:0> and USER_CHAN_NUM<11:0>
        uint8_t AM_FM : 1;      //!< AM/FM Mode Control; 0 = FM mode; 1 = AM mode
    } refined;
    uint16_t raw;
} kt09xx_amsyscfg; // AMSYSCFG

/**
 * @ingroup GA01
 * @brief 3.10.13. AMCHAN (Address 0x17)
 * @see KT0915; Monolithic Digital FM/MW/SW/LW Receiver Radio on a Chip(TM); page 20.
 */
typedef union {
    struct
    {
        uint16_t AMCHAN : 15;   //!< AM Channel Setting; AMCHAN<14:0> = Frequency(in KHz)
        uint8_t AMTUNE : 1;    //!< AM Tune Enable
    } refined;
    uint16_t raw;
} kt09xx_amchan; // AMCHAN

/**
 * @ingroup GA01
 * @brief 3.10.14. AMCALI (Address 0x18)
 * @see KT0915; Monolithic Digital FM/MW/SW/LW Receiver Radio on a Chip(TM); page 20.
 */
typedef union {
    struct
    {
        uint16_t CAP_INDEX : 14; //!< On Chip Capacitor for AM Antenna Calibration; 0x0000 = Minimum capacitor; 0x3FFF = Maximum capacitor
        uint8_t RESERVED1 : 2;  //!< Reserved
    } refined;
    uint16_t raw;
} kt09xx_amcali; // AMCALI

/**
 * @ingroup GA01
 * @brief 3.10.15. GPIOCFG (Address 0x1D)
 * @see KT0915; Monolithic Digital FM/MW/SW/LW Receiver Radio on a Chip(TM); page 20.
 */
typedef union {
    struct
    {
        uint8_t GPIO1 : 2;      //!< CH Pin Mode Selection; 00 = High Z; 01 = Key controlled channel increase / decrease; 10 = Dial controlled channel increase / decrease; 11 = Reserved
        uint8_t GPIO2 : 2;      //!< VOL Pin Mode Selection; 00 = High Z; 01 = Key controlled volume increase/decrease; 10 = Dial controlled volume increase/decrease; 11 = Reserved
        uint16_t RESERVED : 12;  //!< Reserved  
    } refined;
    uint16_t raw;
} kt09xx_gpiocfg; // GPIOCFG

/**
 * @ingroup GA01
 * @brief 3.10.16. AMDSP (Address 0x22)
 * @see KT0915; Monolithic Digital FM/MW/SW/LW Receiver Radio on a Chip(TM); pages 20 and 21.
 */
typedef union {
    struct
    {
        uint8_t RESERVED1 : 3;      //!< Reserved
        uint8_t INV_LEFT_AUDIO : 1; //!< Left channel inverse control; 0 = Normal operation; 1 = Inversing the left channel audio signal
        uint8_t RESERVED2 : 2;      //!< Reserved
        uint8_t AM_BW : 2;          //!< AM Channel Bandwidth Selection; 00 = 2KHz; 01 = 2KHz; 10 = 4KHz; 11 = 6KHz
        uint8_t RESERVED3 : 8;      //!< Reserved
    } refined;
    uint16_t raw;
} kt09xx_amdsp; // AMDSP

/**
 * @ingroup GA01
 * @brief 3.10.17. AMSTATUSA (Address 0x24)
 * @see KT0915; Monolithic Digital FM/MW/SW/LW Receiver Radio on a Chip(TM); page 21.
 */
typedef union {
    struct
    {
       uint8_t RESERVED1 : 8;      //!< Reserved
       uint8_t AMRSSI : 5;         //!< AM Channel RSSI; AM RSSI starts from -90dBm and step is 3dB, namely AMRSSI(dBm) = -90 + AMRSSI<4:0> *3dB
       uint8_t RESERVED2 : 3;      //!< Reserved

    } refined;
    uint16_t raw;
} kt09xx_amdstatusa; // AMSTATUSA

/**
 * @ingroup GA01
 * @brief 3.10.18. AMSTATUSB (Address 0x25)
 * @see KT0915; Monolithic Digital FM/MW/SW/LW Receiver Radio on a Chip(TM); page 21.
 */
typedef union {
    struct
    {
        uint8_t AM_AFCDELTAF : 8;   //!< Signed binary, max 16KHz , min -16KHz, step is 128Hz.
        uint8_t RESERVED : 8;       //!< Reserved
    } refined;
    uint16_t raw;
} kt09xx_amdstatusb; // AMSTATUSB

/**
 * @ingroup GA01
 * @brief 3.10.19. SOFTMUTE (Address 0x2Eh)
 * @see KT0915; Monolithic Digital FM/MW/SW/LW Receiver Radio on a Chip(TM); page 21.
 */
typedef union {
    struct
    {
        uint8_t FM_SMTH : 3;  //!< FM Softmute Start Threshold; 000 = Lowest ... 111 = Highest
        uint8_t SMMD : 1;     //!< Softmute Mode Selection; 0 = RSSI mode; 1 = SNR mode (only effective in FM mode)
        uint8_t VOLUMET : 5;  //!< Softmute target Volume. 0 = RSSI; 1 = SNR mode (only effective in FM mode)
        uint8_t AM_SMTH : 3;  //!< AM Softmute Start Level.  000 = Lowest ... 111 = Highest
        uint8_t SMUTER : 2;   //!< Softmute Attack/Recover Rate; 00 = Slowest; 01 = Fastest (RSSI mode only); 10 =  Fast; 11 = Slow
        uint8_t SMUTEA : 2;   //!< Softmute Attenuation; 00 = Strong; 01 = Strongest; 10 = Weak; 11 = Weakest
    } refined;
    uint16_t raw;
} kt09xx_softmute; // SOFTMUTE

/**
 * @ingroup GA01
 * @brief 3.10.20. USERSTARTCH (Address 0x2F)
 * @see KT0915; Monolithic Digital FM/MW/SW/LW Receiver Radio on a Chip(TM); page 22.
 */
typedef union {
    struct
    {
        uint16_t USER_START_CHAN : 15; //!< User band start channel, only effect when USERBAND=1. See section 3.7.3. of the Datasheet
        uint8_t RESERVED : 1;         //!< Reserved
    } refined;
    uint16_t raw;
} kt09xx_userstartch; // USERSTARTCH

/**
 * @ingroup GA01
 * @brief  3.10.21. USERGUARD (Address 0x30)
 * @see KT0915; Monolithic Digital FM/MW/SW/LW Receiver Radio on a Chip(TM); page 22.
 */
typedef union {
    struct
    {
        uint16_t USER_GUARD : 9;       //!< User band guard number, only effective when USERBAND=1. See section 3.7.2.
        uint8_t RESERVED : 7;         //!< Reserved
    } refined;
    uint16_t raw;
} kt09xx_userguard; // USERGUARD

/**
 * @ingroup GA01
 * @brief  3.10.22. USERCHANNUM (Address 0x31)
 * @see KT0915; Monolithic Digital FM/MW/SW/LW Receiver Radio-on-a-Chip(TM); page 22.
 */
typedef union {
    struct
    {
        uint16_t USER_CHAN_NUM : 12; //!< User band channel number, only effective when USERBAND=1. See section 3.7.3.
        uint8_t RESERVED : 4;   //!< Reserved
    } refined;
    uint16_t raw;
} kt09xx_userchannum; // USERCHANNUM

/**
 * @ingroup GA01
 * @brief  3.10.23. AMCFG (Address 0x33)
 * @see KT0915; Monolithic Digital FM/MW/SW/LW Receiver Radio-on-a-Chip(TM); page 23.
 */
typedef union {
    struct
    {
        uint8_t RESERVED1 : 5;      //!< Reserved
        uint8_t KEY_MODE : 2;       //!< Working mode selection when key mode is selected.  00 = Working mode A; 01 = Working mode B Others = Reserved;  For detailed information about working mode A and working mode B, please refer to section 3.7.1.
        uint8_t RESERVED2 : 7;      //!< Reserved
        uint8_t AMSPACE : 2;        //!< AM Channel Space Selection; 00 = 1KHz; 01 = 9KHz; 10 = 10KHz; 11 = 10KHz.
    } refined;
    uint16_t raw;
} kt09xx_amcfg; // AMCFG

/**
 * @ingroup GA01
 * @brief 3.10.24. AMCFG2 (Address 0x34h)
 * @see KT0915; Monolithic Digital FM/MW/SW/LW Receiver Radio-on-a-Chip(TM); page 23.
 */
typedef union {
    struct
    {
        uint8_t RESERVED1 : 1;  //!< Reserved
        uint8_t TIME2 : 3;      //!< 000 = Fastest...... 111 = Slowest
        uint8_t TIME1 : 2;      //!< 00 = Shortest...... 11 = Longest
        uint16_t RESERVED2 : 10; //!< Reserved
    } refined;
    uint16_t raw;
} kt09xx_amcfg2; // AMCFG2

/**
 * @ingroup GA01
 * @brief 3.10.25. AFC (Address 0x3Ch)
 * @see KT0915; Monolithic Digital FM/MW/SW/LW Receiver Radio-on-a-Chip(TM); page 23.
 */
typedef union {
    struct
    {
        uint8_t FM_AFC_DELTAF : 8; //!< Frequency difference between CHAN and received signal, calculated by AFC block in two’s complement format. Range is - 127 to +127. Unit is KHz. This register is valid when STC=1
        uint8_t RESERVED1 : 8; //!< Reserved
    } refined;
    uint16_t raw;
} kt09xx_afc; // AFC

/**
 * @ingroup GA01
 * @brief Converts 16 bits word to two bytes 
 */
typedef union {
    struct {
        uint8_t lowByte;
        uint8_t highByte;
    } refined;
    uint16_t raw;
} word16_to_bytes;

/**
 * @ingroup GA01  
 * @brief KT0915 Class 
 * @details This class implements all functions that will help you to control the KT0915 devices. 
 * 
 * @author PU2CLR - Ricardo Lima Caratti 
 */

class KT0915 {

protected:
    int deviceAddress = AKC595X_I2C_ADRESS;
    int resetPin = -1;

    uint8_t volume;                             //!< Store the current volume
    uint16_t currentStep;                       //!< Strore the current step
    uint16_t currentFrequency;                  //!< Store the current frequency
    uint16_t minimumFrequency;                  //!< Stores the minimum frequency for the current band
    uint16_t maximumFrequency;                  //!< Stores the maximum frequency for the current band
    uint8_t currentMode;                        //!< Store the current mode
    uint8_t currentCrystalType = CRYSTAL_32KHZ; //!< Store the crystal type
    char    deviceId[3];

public:
    void setRegister(uint8_t reg, uint16_t parameter);
    uint16_t getRegister(uint8_t reg);

    char *getDeviceId();
    void reset();
    void setI2CBusAddress(int deviceAddress);
    void setCrystalType(uint8_t crystal, uint8_t ref_clock = 0);
    bool isCrystalReady(); 
    void setup(int reset_pin, uint8_t crystal_type = CRYSTAL_32KHZ, uint8_t ref_clock = REF_CLOCK_DISABLE);

    void setFM(uint32_t minimum_frequency, uint32_t maximum_frequency, uint32_t default_frequency, uint16_t step);
    void setAM(uint32_t minimum_frequency, uint32_t maximum_frequency, uint32_t default_frequency, uint16_t step);
    void setFrequency(uint32_t frequency);
    void setStep(uint16_t step);
    void frequencyUp();
    void frequencyDown();
    uint32_t getFrequency();
    void setAntennaTuneCapacitor(uint16_t capacitor); 

};

