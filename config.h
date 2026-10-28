#ifndef _CONFIG_H
#define _CONFIG_H

// Device ID
const char* DEVICE_ID = "Max7219x8";

// Max7219 display
#define HARDWARE_TYPE MD_MAX72XX::FC16_HW
#define MAX_DEVICES 8

#define CLK_PIN 18  // VSPI_SCK
#define DATA_PIN 23 // VSPI_MOSI
#define CS_PIN 5    // VSPI_SS

#endif // _CONFIG_H