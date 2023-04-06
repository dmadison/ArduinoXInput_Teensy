# Teensy XInput USB Mode
[![Build Status](https://github.com/dmadison/ArduinoXInput_Teensy/workflows/build/badge.svg?branch=master)](https://github.com/dmadison/ArduinoXInput_Teensy/actions?query=workflow%3Abuild)

The files in this repository will add an additional USB mode ("XInput") to your Teensy board, allowing it to emulate an Xbox gamepad.

This is meant to be used in conjunction with the [ArduinoXInput library](https://github.com/dmadison/ArduinoXInput).
 
## Installation

You must have both [the Arduino IDE](https://www.arduino.cc/en/main/software) and [Teensyduino](https://www.pjrc.com/teensy/td_download.html) installed before proceeding.

Double-check that your installed Teensyduino version matches the files provided in this repository. This repository is currently using [**Teensyduino 1.58**](https://www.pjrc.com/teensy/td_158). You can download older versions on [the releases page](../../releases).

If you don't know your Teensyduino version, compile a blank sketch with a Teensy board selected and the Teensy Loader will open. In the Teensy Loader window select `Help -> About` and it will tell you the version number. If your version does not match you will have to reinstall or update the Teensyduino software.

It is recommended that you make a backup of your hardware folder before proceeding in case something goes wrong or if you want to revert the installation.

To install the XInput modifications, copy the files from the repository's "teensy" directory into the hardware folder and replace any pre-existing files. This will not affect your saved sketches.

To remove the XInput modifications, restore your Teensy hardware folder from backup or reinstall the Teensyduino software.

#### Arduino IDE 1.x

For IDE version 1.x the Teensy hardware files are located in your Arduino IDE installation directory. On Windows this is typically in your "Program Files" folder:

```
C:\Program Files (x86)\Arduino\hardware\teensy\avr
```

#### Arduino IDE 2.x

For IDE version 2.x the Teensy hardware files are located in your application data folder. On Windows this is typically at the following path:

```
%AppData%\..\Local\Arduino15\packages\teensy\hardware\avr\{version}
```

Where `{version}` is the installed version number of the Teensyduino software.

## Supported Boards

* [Teensy 3.6](https://www.pjrc.com/store/teensy36.html)
* [Teensy 3.5](https://www.pjrc.com/store/teensy35.html)
* [Teensy 3.1](https://www.pjrc.com/store/teensy31.html) / [3.2](https://www.pjrc.com/store/teensy32.html)
* [Teensy LC](https://www.pjrc.com/store/teensylc.html)
* [Teensy 4.0](https://www.pjrc.com/store/teensy40.html)
* [Teensy 4.1](https://www.pjrc.com/store/teensy41.html)
* [Teensy MicroMod](https://www.sparkfun.com/products/16402)

## Limitations

### No Console Support

This will *not* enable you to use your Teensy with an Xbox console! Genuine controllers use a dedicated hardware security chip that handshakes with the console. The Teensy boards do not have this chip, and its security method has not been (openly) broken.

### No Commercial Use

These board definitions make use of Microsoft's VID and PID in order to latch on to the PC driver. As such this is strictly for educational or development use by non-commercial entities.

## Credits and Contributions

A massive thank you to Zach Littell, whose did all of the original legwork in putting this together. Check out some of his stuff at [zlittell.com](http://www.zlittell.com).

Another big thanks for Tom Mason ([@wheybags](https://github.com/wheybags)), who added support for the Teensy 4 boards ([#26](https://github.com/dmadison/ArduinoXInput_Teensy/pull/26)).

## License

The original Teensy core files and their modified versions are licensed under a modified version of the permissive [MIT license](https://opensource.org/licenses/MIT). Newly contributed files are licensed under the MIT license with no additional stipulations.

See the [LICENSE](LICENSE.txt) file for more information.
