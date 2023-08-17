#include "SDCARD.h"



int SDCARD::countFiles()
{
    int c=0;
    String fileName;
    while (1){
      fileName= path+name+c+".txt";
      if (SD.exists(fileName)) c++; 
      else break;              
    }
    return c;
  }

void SDCARD::init ()
{
    if (!SD.begin(PIND_SD)) 
    {
      Serial.println("initialization failed!");
      succes = false;
    }
    else
    {
      succes = true;
      Serial.println(countFiles());
    }
    // open the file. note that only one file can be open at a time,
    // so you have to close this one before opening another.
    myFile = SD.open(path+name+countFiles()+".txt", FILE_WRITE);
}

void SDCARD::write(String id,float val)
{
    if(succes && myFile)
    {
      //fields splited by *
      //Serial.print("Writing to test.txt...");
      myFile.print(id+" : ");
      myFile.print(val);
      myFile.print(" * time : ");
      myFile.println(millis());
      //Serial.println("done.");
    }
}

void SDCARD::write(float val[],int size)
{
    if(succes && myFile)
    {
      //fields splited by *
      for(int i = 0;i<size;i++)
      {
        myFile.print(val[i]);
        myFile.print(";");
      }
      myFile.println(millis());

      if(writeCount>=10)
      {
        myFile.flush();
        writeCount = 0;
      }
      else
      {
        writeCount++;
      }
    }
    else
    {
        Serial.println("SD FAILED");
        init();
    }
}

void SDCARD::write(String val[],int size)
{
    if(succes && myFile)
    {
      //fields splited by *
      for(int i = 0;i<size;i++)
      {
        myFile.print(val[i]);
        myFile.print(";");
      }
      myFile.println("millis");

      if(writeCount>=10)
      {
        myFile.flush();
        writeCount = 0;
      }
      else
      {
        writeCount++;
      }
    }
    else
    {
        Serial.println("SD FAILED");
        init();
    }
}


void SDCARD::end()
{
    myFile.close();
}