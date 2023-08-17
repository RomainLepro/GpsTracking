#include "gps.h"

void GPS::smartDelay(unsigned long ms)
{
  unsigned long start = millis();
  do 
  {
    while (SerialGPS->available())
      encode(SerialGPS->read());
  } while (millis() - start < ms);
}

void GPS::printFloat(float val, bool valid, int len, int prec)
{
  if (!valid)
  {
    while (len-- > 1)
      Serial.print('*');
    Serial.print(' ');
  }
  else
  {
    Serial.print(val, prec);
    int vi = abs((int)val);
    int flen = prec + (val < 0.0 ? 2 : 1); // . and -
    flen += vi >= 1000 ? 4 : vi >= 100 ? 3 : vi >= 10 ? 2 : 1;
    for (int i=flen; i<len; ++i)
      Serial.print(' ');
  }
  smartDelay(0);
}

void GPS::printInt(unsigned long val, bool valid, int len)
{
  char sz[32] = "*****************";
  if (valid)
    sprintf(sz, "%ld", val);
  sz[len] = 0;
  for (int i=strlen(sz); i<len; ++i)
    sz[i] = ' ';
  if (len > 0) 
    sz[len-1] = ' ';
  Serial.print(sz);
  smartDelay(0);
}

void GPS::printDateTime(TinyGPSDate &d, TinyGPSTime &t)
{
  if (!d.isValid())
  {
    Serial.print(F("********** "));
  }
  else
  {
    char sz[32];
    sprintf(sz, "%02d/%02d/%02d ", d.month(), d.day(), d.year());
    Serial.print(sz);
  }
  
  if (!t.isValid())
  {
    Serial.print(F("******** "));
  }
  else
  {
    char sz[32];
    sprintf(sz, "%02d:%02d:%02d ", t.hour(), t.minute(), t.second());
    Serial.print(sz);
  }

  printInt(d.age(), d.isValid(), 5);
  smartDelay(0);
}

void GPS::printStr(const char *str, int len)
{
  int slen = strlen(str);
  for (int i=0; i<len; ++i)
    Serial.print(i<slen ? str[i] : ' ');
  smartDelay(0);
}

GPS::GPS(HardwareSerial &Serial)
{
  SerialGPS = &Serial;
  //SerialGPS->println("begin test serial !!!!");
}

void GPS::init(HardwareSerial &Serial ,int BaudSpeed ,double LAT,double LNG)
{
  if (SerialGPS == nullptr)
  {
    SerialGPS = &Serial;
  }
  SerialGPS->begin(BaudSpeed);
  Serial.println("Serial begin");
  Serial.println(F("Sats HDOP  Latitude   Longitude   Fix  Date       Time     Date Alt    Course Speed Card  Distance Course Card  Chars Sentences Checksum"));
  Serial.println(F("           (deg)      (deg)       Age                      Age  (m)    --- from GPS ----  ---- to London  ----  RX    RX        Fail"));
  Serial.println(F("----------------------------------------------------------------------------------------------------------------------------------------"));
}


bool GPS::update(){

  while (SerialGPS->available() > 0)
  { //read the serial com comming from gps and tronsforming it into data
    encode(SerialGPS->read());
  }

  if (location.isUpdated() && location.lat() != 0.0 && location.lat() != 0.0)
  {
    v = speed.mps();
    distanceToHome =
      (unsigned long)TinyGPSPlus::distanceBetween(
        location.lat(),
        location.lng(),
        HOME_LAT, 
        HOME_LNG);
    printInt(distanceToHome, location.isValid(), 9);

    courseToHome =
      TinyGPSPlus::courseTo(
        location.lat(),
        location.lng(),
        HOME_LAT, 
        HOME_LNG);
    if (v > vmax){vmax = v;}
    if (!set)
    {
      if (v < 1 && millis() > 10000)
      { // if imobile, set home
        set = true;
        HOME_LAT = location.lat();
        HOME_LNG = location.lng();
        alt = altitude.meters();
        Serial.println("home setted");
      }
      else{
        Serial.print("speed to fast: ");Serial.println(v);
      }
    }
    return true;
  }
  return false;
}

void GPS::resetHome(){
  set = false;
}


void GPS::showSerial(uint32_t dt)
{
  if(tSerial+dt < millis()){
    tSerial = millis();
    Serial.println(F("Sats HDOP  Latitude   Longitude   Fix  Date       Time     Date Alt    Course Speed Card  Distance Course Card  Chars Sentences Checksum"));
    printInt(satellites.value(), satellites.isValid(), 5);
    printFloat(hdop.hdop(), hdop.isValid(), 6, 1);
    printFloat(location.lat(), location.isValid(), 11, 6);
    printFloat(location.lng(), location.isValid(), 12, 6);
    printInt(location.age(), location.isValid(), 5);
    printDateTime(date, time);
    printFloat(altitude.meters(), altitude.isValid(), 7, 2);
    printFloat(course.deg(), course.isValid(), 7, 2);
    printFloat(speed.kmph(), speed.isValid(), 6, 2);
    printStr(course.isValid() ? TinyGPSPlus::cardinal(course.deg()) : "*** ", 6);

    printFloat(courseToHome, location.isValid(), 7, 2);
    const char *cardinalToLondon = TinyGPSPlus::cardinal(courseToHome);
    printStr(location.isValid() ? cardinalToLondon : "*** ", 6);
    printInt(charsProcessed(), true, 6);
    printInt(sentencesWithFix(), true, 10);
    printInt(failedChecksum(), true, 9);
    Serial.println();
    Serial.print("speed: ");Serial.print(v);Serial.print("|max_speed: ");Serial.println(vmax);
    if (millis() > 5000 && this->charsProcessed() < 10) Serial.println(F("No GPS data received: check wiring"));
  }
  else{
    smartDelay(0);
  }
  

/*
  Serial.print(F("lat: "));Serial.println(location.lat(), 6);
  Serial.print(F("lng: "));Serial.println(location.lng(), 6);
  Serial.print(F("cor: "));Serial.println(course.deg(), 2);
  Serial.print(F("hom: "));Serial.println(Oz, 2);
  Serial.print(F("dist: "));Serial.print(distanceBetween(location.lat(), location.lng(), HOME_LAT, HOME_LNG));
  Serial.print(F(" alt: "));Serial.println(altitude.meters() - alt, 1);
  Serial.print(F("gps: "));Serial.print(satellites.value());
  Serial.print(F("  set: "));Serial.println(set);
  Serial.print(F("spd: "));Serial.print(v, 1);
  Serial.print(F("   max: "));Serial.println(vmax, 1);
  Serial.print(F("chars: "));Serial.println(charsProcessed());
  Serial.print(F("sentence: "));Serial.println(sentencesWithFix());
  Serial.print(F("failed: "));Serial.println(failedChecksum());
*/
}

