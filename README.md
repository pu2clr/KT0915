# PU2CLR KT0915 Arduino Library

__ATTENTION: This library is under construction...__ 

![Under construction](https://pu2clr.github.io/KT0915/extras/images/under_construction.png)

__Coming soon__

__Waiting for the device arrives.__

This is an Arduino Library to control the KT0915 DSP receiver from KTMicro.
The KT0915 is an integrated circuit that provides full band receiver on FM and AM (MW/SW/LW).

[Copyright (c) 2020 Ricardo Lima Caratti](https://pu2clr.github.io/KT0915#mit-license).

Contact: __pu2clr@gmail.com__.


Be a member of Facebook group [DSP receivers for hobbyists](https://www.facebook.com/groups/2655942598067211)


# Contents

1. [Preface](https://pu2clr.github.io/KT0915#preface)
2. [Your support is important](https://pu2clr.github.io/KT0915#your-support-is-important)
3. [Library Features](https://pu2clr.github.io/KT0915#library-features)
4. [MIT License](https://pu2clr.github.io/KT0915#mit-license)
5. [Library Installation](https://pu2clr.github.io/KT0915#library-installation)
6. [About KT0915 DSP radio](https://pu2clr.github.io/KT0915#about-kt0915-dsp-radio)
   * [KT0915 pinout](https://pu2clr.github.io/KT0915#kt0915-pinout)
7. [SCHEMATIC](https://pu2clr.github.io/KT0915#schematic)
8.  [API Documentation](https://pu2clr.github.io/KT0915/extras/docs/html/index.html)
9.  [Examples](https://github.com/pu2clr/KT0915/tree/master/examples)
10. [Videos](https://pu2clr.github.io/KT0915#videos)
11. [References](https://pu2clr.github.io/KT0915#references)


## Preface


The KT0915 is a full band AM (LW, MW and SW) and FM DSP receiver that can provide you a easy way to build a high quality radio with low cost. This device, will surprise hobbyists and experimenters with its simplicity. 

In this document you will see Arduino source codes, schematics, examples and tips to help you to build a receiver based on Arduino board and the KT0915 device.  The project and examples shown here do not intend to be a real receiver for exigent listener. However, it is possible to start with it and after add some improvements. On the other hand, with the simple circuits shown here, the experimenter may be surprised with its performance.


## See also

1. [PU2CLR Si4735 Library for Arduino](https://pu2clr.github.io/SI4735/). This library was built based on “Si47XX PROGRAMMING GUIDE; AN332 ” and it has support to FM, AM and SSB modes (LW, MW and SW). It also can be used on all members of the SI47XX family respecting, of course, the features available for each IC version; 
2. [PU2CLR SI4844 Arduino Library](https://github.com/pu2clr/SI4844). This is an Arduino library for the SI4844, BROADCAST ANALOG TUNING DIGITAL DISPLAY AM/FM/SW RADIO RECEIVER,  IC from Silicon Labs.  It is available on Arduino IDE. This library is intended to provide an easier interface for controlling the SI4844.
3. [PU2CLR AKC695X Arduino Library](https://pu2clr.github.io/AKC695X/). The AKC695X is a family of IC DSP receiver from AKC technology. The AKC6955 and AKC6959sx support AM and FM modes. On AM mode the AKC6955 and AKC6959sx work on LW, MW and SW. On FM mode they work from 64MHz to 222MHz.


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

Copyright (c) 2019 Ricardo Lima Caratti

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE ARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

<BR>


## Library Installation

You can install this library on your Arduino environment using different methods. The best ways to do that are described below.  


### Installing via Arduino IDE

// TODO


## About KT0915 DSP radio

According to __"Monolithic Digital FM/MW/SW/LW Receiver Radio on a Chip(TM); KT0915"__ document from KTMicro, "the KT0915 is a fully integrated full band AM/FM digital radio receiver IC with patented technologies that offer full band FM/MW/SW/LW functionality, high quality audio performance, simple design and low BOM cost.
The direct frequency and volume control interface help custumers to maintain the classic looks of their existing products.

Thanks to the patented tuning technology, the receiver maintains good signal reception with short antenna. The chip consumes only 22mA current and can be powered by 2 AAA batteries. Another useful feature is that the volume and channel setup can be preserved in standby mode without external memories. KT0915 supports a wide range of reference clocks from 32.768KHz to 26MHz, hence can share system clocks with a varieties of MCUs further reducing the system BOM cost.

With high audio performance, fully integrated features and low BOM cost, KT0915 is ideal for various FM/MW/SW/LW applications and products".



### KT0915 features

1. Worldwide full band FM/MW/SW/LW support FM: 32MHz-110MHz
   * MW: 500KHz-1710KHz 
   * FM: 32MHz-110MHz
   * MW: 500KHz-1710KHz SW: 1.6MHz~32MHz LW: 150KHz ~ 280KHz
2. Fully integrated frequency synthesizer with no external component
3. High Sensitivity 
   * 1.6uVEMF for FM 
   * 16uVEMF for AM
4. High Fidelity
   * SNR (FM/AM): 60dB/55dB 
   * THD: 0.3%
5. Low supply voltage: 2.2V to 3.6V, can be supplied by 2 AAA batteries
6. Automatic antenna tuning
8. Adjustable AM channel filters (2k/4k/6KHz)
9. Embedded FM SNR meter
10. Fast seek/Tune
11. Integrated stereo headphone driver I2C control interface for MCU
12. Support traditional dial and digital key for frequency tuning and volume control
13. Memorize channel and volume in standby mode 
14. Low Supply Current - 22mA (operating) <15uA (standby) 
15. Support both 32.768KHz and 38KHz crystal
16. Support continuous reference frequency from 32.768KHz to 26MHz

Source: __"Monolithic Digital FM/MW/SW/LW Receiver Radio on a Chip(TM); KT0915"__ document from KTMicro



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

The KT0915 can be configured to deal with mechanical tuning via a variable resistors. For example, you can use a variable resistor to control the audio volume and another to tuninig the receiver. This resource is  enabled by setting the internal register GPIOCFG (Address 0x1D) of the device. The dial can be a variable resistor with the tap connected to CH and VOL as well. The divided voltage at the tap is processed by the internal KT0915  analog to digital converter (ADC). Finally, the converted datas are stored into the internal registers of the KT0915 device.


By default, this library the tune an volume are conttrolled by the Arduino. However, you can sets the mechanical feature by calling setTuneDialModeOn and setVolumeDialModeOn. See [API Documentation](https://pu2clr.github.io/KT0915/extras/docs/html/index.html).

See the KT0915; Monolithic Digital FM/MW/SW/LW Receiver Radio on a Chip(TM); pages: 8, 9, 10 and 20.



## SCHEMATIC




## Videos




## References

[Radio, yes AM-SW-FM radio by DSP radio chip](https://hackaday.io/project/12944-radio-yes-am-sw-fm-radio-by-dsp-radio-chip)
[KT0915_DSP_RADIO-fullscreen-info](https://github.com/kodera2t/KT0915_DSP_RADIO-fullscreen-info)
[KT0915 Arduino](https://easyeda.com/wolfezrawilliam/KT0915-Arduino)
