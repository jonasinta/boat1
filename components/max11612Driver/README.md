# ESP32 max11612 Library

This is component based on ESP-IDF for ESP32 

Jonas 1st attempt at making a driver for MAX max11612 adc specifically a 4 channel version.
I will be making driver which will utilise only a minimum amount of the potential functions available from this device, namely;
2 channel differnetial unipolar output (any negative input will give a "0" reading),
single shot measurements which will be called in a periodic fasion and put into a project wide global variable for use from other places.
Good luck to me.
