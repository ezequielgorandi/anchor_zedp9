/*Se implemento la cola y funciona OK. Falta reemplazar el uso del GPS en la terea de pantalla, por el uso de la cola (hay que hacerlo?)*/

#include "Anchor.h"
#include "Free_Fonts.h"
#include "Gps.h"
#include "Screen.h"
#include "SparkFun_u-blox_GNSS_Arduino_Library.h"
#include <Arduino.h>
#include <M5Stack.h>

#define IWIDTH 320
#define IHEIGHT 128
#define APP_BACKGROUND_COLOR TFT_BLACK
#define APP_FONT FSSB9
#define APP_FONT_COLOR TFT_WHITE

uint32_t ypos = 0;
long lastTime = 0; // Simple local timer. Limits amount if I2C traffic to u-blox module.
Gps gps = Gps();
Anchor anchor = Anchor();
// Gps gps = Gps();

void screenTask(void *pvParameters)
{
  TFT_eSprite img = TFT_eSprite(&M5.Lcd);
  Screen_1 screen_1 = Screen_1(&img, &gps);
  Screen_2 screen_2 = Screen_2(&img, &gps);
  // Falta borrar el Gps
  position_t aPosition;

  screen_1.init();
  screen_1.update(aPosition);
  screen_1.show();

  while (1)
  {
    // Probando el funcionamiento de las Colas
    if (gpsQueue != NULL)
    {
      if (gpsQueue != NULL && xQueueReceive(gpsQueue, &(aPosition), 0))
      {
        Serial.println(aPosition.status);
        switch (aPosition.status)
        {
        case NOT_CONNECTED:
        case LOW_PRECISSION:
          screen_1.init();
          screen_1.update();
          screen_1.show();
          break;
        case FIXED:
          screen_2.init();
          screen_2.update(aPosition);
          screen_2.show();
          break;
        }
      }
    }
    else
      Serial.println("Queue es NULL");
    delay(250);
  }
  // Will never reach this point.
}

void screenTask_ant(void *pvParameters)
{
  TFT_eSprite img = TFT_eSprite(&M5.Lcd);
  Screen_1 screen_1 = Screen_1(&img, &gps);
  Screen_2 screen_2 = Screen_2(&img, &gps);
  position_t aPosition;

  while (1)
  {
    while (gps.getStatus() != FIXED)
    {
      screen_1.init();
      screen_1.update();
      screen_1.show();
      delay(1000);
    }
    while (gps.getStatus() == FIXED)
    {
      screen_2.init();
      screen_2.update();
      screen_2.show();

      // Probando el funcionamiento de las Colas
      if (gpsQueue != NULL)
      {
        if (xQueueReceive(gpsQueue, &(aPosition), 0))
        {
          Serial.println("LEIDO DE LA COLA!!!!!!! ");
          Serial.println(aPosition.longitude);
          Serial.println(aPosition.latitude);
        }
        else
          Serial.println("Falla el recieved");
      }
      else
        Serial.println("Queue es NULL");

      delay(250);
    }
  }
}

void initM5Stack()
{
  M5.begin(); // Init M5Core.
  /*
    Power chip connected to gpio21, gpio22, I2C device
    Set battery charging voltage and current
  */
  M5.Power.begin(); // Init Power module.  ???

  // M5.setWakeupButton(BUTTON_C_PIN);
  M5.Power.setWakeupButton(BUTTON_C_PIN);
}

void setup()
{
  // put your setup code here, to run once:
  Serial.begin(115200);
  Wire.begin();
  Serial.println("SparkFun u-blox Example");
  initM5Stack();

  gps.initTask();

  xTaskCreatePinnedToCore(
      screenTask,       // Function to implement the task.  线程对应函数名称(不能有返回值)
      "screen",         //线程名称
      4096,             // The size of the task stack specified as the number of * bytes.任务堆栈的大小(字节)
      NULL,             // Pointer that will be used as the parameter for the task * being created.  创建作为任务输入参数的指针
      tskIDLE_PRIORITY, // Priority of the task.  任务的优先级
      NULL,             // Task handler.  任务句柄
      0);               // Core where the task should run.  将任务挂载到指定内核
}

void loop()
{
  // M5.update(); // Read the press state of the key.
  /*
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
      } */
}
// Date ant time
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
// M5.Power.lightSleep(0); //Con 0 duerme hasta que se presione el botton

/*Display*/
// M5.Power
// M5.Lcd.clear(BLACK);
// M5.lcd.sleep();
// M5.lcd.wakeup();

// M5.Power.deepSleep(0);
// M5.Power.powerOFF();  //USAR!! //Turn off power. Vuelve a encenderse con el boton rojo.
// M5.powerOFF();