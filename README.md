# PU2CLR KT0915 Arduino Library

__ATTENTION: This library is under construction...__ 

![Under construction](https://github.com/pu2clr/KT0915/blob/master/extras/images/under_construction.png)

__Waiting for the device arrives.__

This is an Arduino Library for controlling the KT0915 DSP receiver from KTMicro.
The KT0915 is an integrated circuit that provides full band receiver on FM and AM (MW/SW/LW).

[Copyright (c) 2020 Ricardo Lima Caratti](https://pu2clr.github.io/AKC695X/#mit-license).

Contact: __pu2clr@gmail.com__.

Be a member of Facebook group [DSP receivers for hobbyists](https://www.facebook.com/groups/2655942598067211)


# Contents

1. [Preface](https://pu2clr.github.io/KT0915/#preface)
2. [Your support is important](https://pu2clr.github.io/KT0915/#your-support-is-important)
3. [Library Features](https://pu2clr.github.io/KT0915/#library-features)
4. [MIT License](https://pu2clr.github.io/KT0915/#mit-license)
5. [Library Installation](https://pu2clr.github.io/KT0915/#library-installation)
6. [About KT0915 DSP radio](https://pu2clr.github.io/KT0915/#about-KT0915-DSP-radio)
   * [Register setup]()
   * [Register operations]()
   * [KT0915 Pin definitions]()
7. [SCHEMATIC](https://pu2clr.github.io/KT0915/#schematic)
8.  [API Documentation](https://pu2clr.github.io/KT0915/extras/docs/html/index.html)
9.  [Examples](https://github.com/pu2clr/KT0915/tree/master/examples)
10. [Videos](https://github.com/pu2clr/KT0915#videos)
11. [References](https://pu2clr.github.io/KT0915/#references)


## Preface


The KT0915 is a full band AM (LW, MW and SW) and FM DSP receiver that can provide you a easy way to build a high quality radio with low cost. This device, will surprise hobbyists and experimenters with its simplicity. 

In this document you will see Arduino source codes, schematics, examples and tips to help you to build a receiver based on Arduino board and the KT0915 device.  The project and examples shown here do not intend to be a real receiver for exigent listener. However, it is possible to start with it and after add some improvements. On the other hand, with the simple circuits shown here, the experimenter may be surprised with its performance.


## Your support is important

If you would like to support this library development, consider joining this project via Github. Alternatively, make suggestions on new features and report errors if you find them. Thank you!


## Library Features

This library uses the I²C communication protocol and implements most of the functions offered by the KT0915 (BROADCAST AM / FM / SW / LW RECEIVER). 

The main features of this library are listed below.

1. Open Source. It is free. You can use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software. See [MIT License to know more](https://pu2clr.github.io/AKC695X/#mit-license);
2. Built based on [AKC6955 stereo FM / TV / MW / SW / LW digital tuning radio](http://maximradio.altervista.org/akc6955/AKC6955-datasheet-english.pdf);
3. C++ Language and Object-oriented programming. You can easily extend the AKC695X class by adding more functionalities;
4. Available on Arduino IDE (via Manage Libraries). Easy to install and use;
5. Cross-platform. You can compile and run this library on most of board available on Arduino IDE (Examples: ATtiny85, boards based on ATmega328 and ATmega-32u4, ATmega2560, ARM Cortex, STM32, Arduino DUE, ESP32 and more);
6. Simplifies projects based on KT0915 IC;
7. Seeking function support;
8. Support to __32.768KHz, 6.5MHz, 7.6MHz, 12MHz, 13MHz, 15.2MHz, 19.2MHz, 24MHz, and 26MHz__ crystal oscillators;
9. Support to audio controlled by the MCU (Arduino)  or potentiometer;
10. Real tim FM stereo or mono indicator;
11. FM stereo or mono selecting;
12. Real time signal level reading;
13. Real time AM and FM carrier to noise ratio information (dB).
14. Bandwidth selection for FM;
15. Custom band support;
16. More than 40 functions implemented.





## MIT License




## Library Installation




## About KT0915 DSP radio




### KT0915 Pinout

 | Pin Number  | Pin Name  | Description |
 | ----------  | --------  | ----------- |
 | 1           | CH        | Channel Adjustment. Used to deal with mechanical tune option |
 | 2           | DVSS      | Digital Ground | 
 | 3           | ROUT      | Righ channel audio output | 
 | 4           | LOUT      | Left channel audio output | 
 | 5           | AVSS      | Analog ground | 
 | 6           | AVDD      | Power Supply | 
 | 7           | XI/RCLK   | Crystal input or reference clock input setup | 
 | 8           | XO        | Crystal Output | 
 | 9           | ENABLE    | Chip enable. Tied to an internal 600kohm pull down resistor |
 | 10          | AMINN     | AM RF negative input |
 | 11          | AMINP     | AM RF positive input |
 | 12          | RFINP     | FM RF input | 
 | 13          | RFGND     | RF ground | 
 | 14          | SCL       | SCL of I2C bus. Tied to an internal 47kohm pull-up resistor |
 | 15          | SDA       | SDA of I2C bus. Tied to an internal 47kohm pull-up resistor |
 | 16          | VOL       | Volume adjustment. Used to deal with mechanical volume control |



### KT0915 SSOP16L package


![KT0915 SSOP16L package](https://pu2clr.github.io/KT0915/extras/images/KT0915_SSOP16L.png)


#### About the Pin 1 (CH) and Pin 16 (VOL)

KT0915 can be configured to deal with mechanical tuning via a variable resistor. Also, you an use a variable resistor to control the audio volume. It is  enabled by setting the the register GPIOCFG (Address 0x1D). The dial can be a variable resistor with the tap connected to CH and VOL as well. The divided voltage at the tap is processed by the internal KT0915  analog to digital converter (ADC) and converted to a channel value or volume level.

By default, this library the tune an volume are conttrolled by the Arduino. However, you can sets the mechanical feature by calling setTuneDialModeOn and setVolumeDialModeOn. See [API Documentation](https://pu2clr.github.io/KT0915/extras/docs/html/index.html).

See the KT0915; Monolithic Digital FM/MW/SW/LW Receiver Radio on a Chip(TM); pages: 8, 9, 10 and 20.



## SCHEMATIC




## API Documentation




## Examples




## Videos




## References

[Radio, yes AM-SW-FM radio by DSP radio chip](https://hackaday.io/project/12944-radio-yes-am-sw-fm-radio-by-dsp-radio-chip)
[KT0915_DSP_RADIO-fullscreen-info](https://github.com/kodera2t/KT0915_DSP_RADIO-fullscreen-info)
[KT0915 Arduino](https://easyeda.com/wolfezrawilliam/KT0915-Arduino)
