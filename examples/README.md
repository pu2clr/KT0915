# Examples

This project is about a library to control the KT0915 device and the focus of this project is the library and its functionalities. __Please, don't ask me to assist you in your: displays, encoders, buttons or something else out the library scope__. Thanks.

The examples purpose is assisting you about the use of the PU2CLR KT0915 Arduino Library.



__The KT0915 is a 3.3V part. If you are not using a 3.3V version of Arduino, you have to use a kind of 5V to 3.3V bidirectional converter. It is important to say that just power the KT0915 device with 3.3V from Arduino board is not enough. You have to be aware that the Arduino that operates with 5V, the digital pins and the I²C bus will send 5V signals to the KT0915 device. That configuration can make the system unstable or damage the device__.  


## KT0915_01_serial_monitor examples 

The examples stored in this folder should be used to check your circuit and check the functionalities implemented by the library. You will not need to use any display device, push buttons and encoders to make a receiver based on KT0916 works. All you will need is your computer and the Arduino IDE to get a basic receiver.

You can use the circuit below to run the sketches stored in the KT0915_01_serial_monitor folder.

![Picture Basic Schematic](https://github.com/pu2clr/KT0915/blob/master/extras/images/schematic_basic.png)

Check also the Typical Application Circuit suggested by the KTMicro [KT0915 - Monolithic Digital FM/MW/SW/LW ReceiverRadio-on-a-Chip™](http://aitendo3.sakura.ne.jp/aitendo_data/product_img/ic/radio/KT0915%20/KT0915_datasheet_V022_aitendo.pdf); page 24. 


## KT0915_02_OLED 

This example implements a FM, AM (MW AND SW) receiver with an OLED 128X64 I2C. The circuit belows shows the OLED setup. 


![Picture OLED Schematic](https://github.com/pu2clr/KT0915/blob/master/extras/images/schematic_OLED_AM_FM_SW_receiver.png)


