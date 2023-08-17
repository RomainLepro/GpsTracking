#include <Arduino.h>
#include "PLANE.h"

///< The class which defines how we will talk to this device over I2C
// wirering VCC => 5v Rx => TX1 TX => RX1 GND => GND
#include <SPI.h>
#include <SD.h>
#include "string.h"



#ifndef SDCARD_H
#define SDCARD_H

#define PIND_SD 7

class SDCARD
{
public:
  //SDCARD(PLANE plane) :plane_m(plane) {};

  int countFiles();

  void init ();

  void write(String id,float val);

  void write(float val[],int size);

  void write(String val[],int size);

  void end();

  bool succes = false;
  File myFile;
  String path = "LOGS/";
  String name = "log";
  int writeCount = 0;

private:
  PLANE plane_m;
};


#endif