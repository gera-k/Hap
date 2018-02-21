# Hap
HomeKit Server

Implements __HomeKit Accessory Protocol for IP Accessories__ as defined by the __HomeKit Accessory Protocol Specification (Non-Commertial version) Release R1__.

Currently the server can be compliled as either Windows desktop app or as a Linux app.
The LinuxTest app runs on [C.H.I.P board]( https://getchip.com/pages/chip) and implements one LightBulb Service with On and Brightness characteristics. The Linux test is included into Yocto image chip-image-hap which can be build using https://github.com/gera-k/meta-chip-hap layer.

The LB is the LED on PWM output, which can be controlled from either Apple device or by a potentiometer attached to LRADC pin:

![RedLed](https://github.com/gera-k/Hap/blob/master/Hw/RedLed_bb.png)


