#include <Arduino.h>
#include <Wire.h>
#include <TinyGPS++.h>
#include <SoftwareSerial.h>

#ifndef GPS_H
#define GPS_H

///< The class which defines how we will talk to this device over I2C
// wirering VCC => 5v Rx => TX1 TX => RX1 GND => GND
class GPS : public TinyGPSPlus
{
public:
  uint32_t chars;
  double HOME_LAT ;
  double HOME_LNG ;
  double alt = 0;
  double v = 0,vmax = 0;
  double dist = 0;
  double Oz = 0,Oy=0;
  bool set = false;
  int current_index = 0;

  unsigned long distanceToHome;
  double courseToHome;

  GPS(HardwareSerial &Serial = Serial1);
  void init(HardwareSerial &Serial = Serial1,int BaudSpeed = 9600,double LAT= 46.64968,double LNG= 00.36231); //need the serial connection of the gps
  //Lat and LNG useless yet

  bool update(); // return true is position is updated

  void showSerial(uint32_t dt = 1000); // print all infos in serial

  void smartDelay(unsigned long ms);
  void printFloat(float val, bool valid, int len, int prec);
  void printInt(unsigned long val, bool valid, int len);
  void printDateTime(TinyGPSDate &d, TinyGPSTime &t);
  void printStr(const char *str, int len);
  void resetHome();//requiere plane to be still to register the new home location

  inline virtual int get_current_index(){return current_index;}

private:
  HardwareSerial *SerialGPS = nullptr;
  uint32_t tSerial = 0;
};


#endif