#include <Arduino.h>
#include "SparkFun_u-blox_GNSS_Arduino_Library.h"
#include <M5Stack.h>
#include "Free_Fonts.h"

#define IWIDTH  320
#define IHEIGHT 128
#define APP_BACKGROUND_COLOR TFT_BLACK
#define APP_FONT FSSB9
#define APP_FONT_COLOR TFT_WHITE
TFT_eSprite img = TFT_eSprite(&M5.Lcd);
SFE_UBLOX_GNSS myGNSS;
uint32_t ypos = 0;
long lastTime = 0; //Simple local timer. Limits amount if I2C traffic to u-blox module.


void initM5Stack()
{
  M5.begin(); //Init M5Core.
  /*
    Power chip connected to gpio21, gpio22, I2C device
    Set battery charging voltage and current
  */
  M5.Power.begin(); //Init Power module.  ???
  
  img.createSprite(IWIDTH, IHEIGHT);
  img.fillSprite(APP_BACKGROUND_COLOR);
  
  // First we test them with a background colour set
  img.setTextSize(1);
  img.setTextColor(APP_FONT_COLOR, APP_BACKGROUND_COLOR);
  img.setFreeFont(APP_FONT);
  img.drawString("Initializing...", 0, 0, FONT4);
  img.pushSprite(0, 0);
  //M5.setWakeupButton(BUTTON_C_PIN);
  //M5.Power.setWakeupButton(BUTTON_C_PIN);
}

void initGps()
{
  // Resets the Gps configuration
  //while ( myGNSS.begin() == false );
  //myGNSS.factoryReset(); //Reset everything: baud rate, I2C address, update rate, everything.
  ypos += img.fontHeight(FONT4);
  img.setTextColor(TFT_RED, APP_BACKGROUND_COLOR);
  img.drawString("GPS not connected",0,ypos);
  img.pushSprite(0, 0);
  while ( myGNSS.begin() == false )
  {
    delay(1000);
    M5.update(); //Read the press state of the key.   
    if (M5.BtnA.wasPressed())
    {
      if (myGNSS.powerSaveMode()) // Defaults to true
        Serial.println(F("Power Save Mode enabled."));
      else
        Serial.println(F("*** Power Save Mode FAILED ***"));
      M5.Power.powerOFF();
    }

  }
  myGNSS.powerSaveMode(false);
  img.setTextColor(TFT_GREENYELLOW, APP_BACKGROUND_COLOR);
  img.drawString("GPS connected              ",0,ypos);
  img.pushSprite(0, 0);  
  myGNSS.setI2COutput(COM_TYPE_UBX); //Set the I2C port to output UBX only (turn off NMEA noise)
  myGNSS.saveConfigSelective(VAL_CFG_SUBSEC_IOPORT); //Save (only) the communications port settings to flash and BBR
  myGNSS.setAutoPVT(true); //Tell the GNSS to "send" each solution
 // if (myGNSS.setDynamicModel(DYN_MODEL_PEDESTRIAN) == false)
 // //if (myGNSS.setDynamicModel(DYN_MODEL_PORTABLE) == false) // Set the dynamic model to PORTABLE
 // {
 //   Serial.println(F("*** Warning: setDynamicModel failed ***"));
 // }
 // else
 // {
 //   Serial.println(F("Dynamic platform model changed successfully!"));
 // }
////Toma una muestra cada 100mS
 // if (myGNSS.setMeasurementRate(250) == false)
 // {
 //   Serial.println(F("Could not set the measurement rate. Freezing."));
 //   while (1);
 // }
//
////Con 100 muestras entrega una nueva posicion (10)
 // if (myGNSS.setNavigationRate(10) == false)
 // {
 //   Serial.println(F("Could not set the navigation rate. Freezing."));
 //   while (1);
 // }
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  Wire.begin();
  Serial.println("SparkFun u-blox Example");
  initM5Stack();
  initGps();
  img.fillSprite(APP_BACKGROUND_COLOR);
  ypos = 0;
  myGNSS.setVal16(UBLOX_CFG_RATE_MEAS, 1000); 
}


void loop() 
{
  M5.update(); //Read the press state of the key.   
  if (M5.BtnA.wasPressed())
  {
    if (myGNSS.powerSaveMode()) // Defaults to true
      Serial.println(F("Power Save Mode enabled."));
    else
      Serial.println(F("*** Power Save Mode FAILED ***"));
    M5.Power.powerOFF();
  }

  if (millis() - lastTime > 1000)
  {    
    lastTime = millis(); //Update the timer
    
    if (myGNSS.getPVT())
    {
      long latitude = myGNSS.getLatitude();
      Serial.print(F("Lat: "));
      Serial.print(latitude);
      
      long longitude = myGNSS.getLongitude();
      Serial.print(F(" Long: "));
      Serial.print(longitude);
    
      long altitude = myGNSS.getAltitude();
      Serial.print(F(" Alt: "));
      Serial.print(altitude);
      //Infica la presición de la posición.
      byte fixType = myGNSS.getFixType();
      Serial.print(F(" Fix: "));
      if(fixType == 0) Serial.print(F("No fix"));
      else if(fixType == 1) Serial.print(F("Dead reckoning"));
      else if(fixType == 2) Serial.print(F("2D"));
      else if(fixType == 3) Serial.print(F("3D"));
      else if(fixType == 4) Serial.print(F("GNSS + Dead reckoning"));
      else if(fixType == 5) Serial.print(F("Time only"));

      byte RTK = myGNSS.getCarrierSolutionType();
      Serial.print(" RTK: ");
      Serial.print(RTK);
      if (RTK == 0) Serial.print(F(" (No solution)"));
      else if (RTK == 1) Serial.print(F(" (High precision floating fix)"));
      else if (RTK == 2) Serial.print(F(" (High precision fix)"));

      long accuracy = myGNSS.getPositionAccuracy();
      Serial.print(F(" 3D Positional Accuracy: "));
      Serial.print(accuracy);
      Serial.println(F(" (mm)"));
      Serial.println();

      img.setTextColor(TFT_WHITE, APP_BACKGROUND_COLOR);

      if (fixType < 3)
      {
        img.fillSprite(APP_BACKGROUND_COLOR);
        img.drawString("Fixing position, please wait...",0,ypos);
        img.drawString("Fix: " + String(fixType), 0, ypos+img.fontHeight(FONT4));
        img.pushSprite(0, 0);
        delay(1000);
      }   
      else
      {
        img.fillSprite(APP_BACKGROUND_COLOR);
        img.drawString("Lon: " + String(longitude), 0, ypos);
        img.drawString("Lat: " + String(latitude), 0, ypos+img.fontHeight(FONT4));
        img.drawString("Acc: " + String(accuracy) + "mm", 0, ypos+img.fontHeight(FONT4)*2);
        img.drawString("Fixa: " + String(fixType), 0, ypos+img.fontHeight(FONT4)*3);
        img.pushSprite(0, 0);
      }
    }
  }  
  //Date ant time
/*
    Serial.println();
    Serial.print(myGNSS.getYear());
    Serial.print("-");
    Serial.print(myGNSS.getMonth());
    Serial.print("-");
    Serial.print(myGNSS.getDay());
    Serial.print(" ");
    Serial.print(myGNSS.getHour());
    Serial.print(":");
    Serial.print(myGNSS.getMinute());
    Serial.print(":");
    Serial.print(myGNSS.getSecond());

    Serial.print("  Time is ");
    if (myGNSS.getTimeValid() == false)
    {
      Serial.print("not ");
    }
  
  }
*/
  //M5.Power.lightSleep(0); //Con 0 duerme hasta que se presione el botton

  /*Display*/
  //M5.Power
  //M5.Lcd.clear(BLACK);
  //M5.lcd.sleep();
  //M5.lcd.wakeup();

  //M5.Power.deepSleep(0);
  //M5.Power.powerOFF();  //USAR!! //Turn off power. Vuelve a encenderse con el boton rojo.
  //M5.powerOFF();
}