# Code

This section has two main parts: the Arduino code and the Richwave library.

## Flashback Code

This folder includes the three parts of the code. They must be in the same folder at the time of compiling and uploading, otherwise it will not work. There is only one compiler directive that affects the switching speed between the modules:
> "DISABLE_FILTERING"
This can be left commented to provide a more conservative switching speed (omnidirectional + directional antenna setup) or it can be uncommented to provide higher switching speeds (dual omni antennas).

## Richwave Library

This library is a modified version of the original Richwave library taken from user [Spacefish](https://github.com/Spacefish/RichWave). It has an included function for taking an average reading from the RSSI of a module. To install it in the Arduino environment, you must compress the RichWave folder before following the usual procedure.
