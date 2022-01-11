/*Se implemento la cola y funciona OK. Falta reemplazar el uso del GPS en la terea de pantalla, por el uso de la cola (hay que hacerlo?)*/

#include "Anchor.h"
#include "Free_Fonts.h"
#include "Gps.h"
#include "Screen.h"
#include "SparkFun_u-blox_GNSS_Arduino_Library.h"
#include <Arduino.h>
#include <M5Stack.h>

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
  // Falta borrar el Gps
  Screen_1 screen_1 = Screen_1(&img, &gps);
  Screen_2 screen_2 = Screen_2(&img, &gps);
  Screen_SetAnchorPosition screen_setAnchorPos = Screen_SetAnchorPosition(&img);
  position_t aPosition;

  screen_1.init();
  screen_1.update(aPosition);
  screen_1.show();
  delay(2000);
  while (1)
  {
    if (M5.BtnC.wasPressed())
    {
      M5.Power.powerOFF();
    }
    
    //Anchor is being fixed
    if (anchor.newPositionFlag != 0)
    {
      if ( screen_setAnchorPos.set(&anchor.newPositionFlag, anchor.data) == true )
        delay(3000);
    }

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
          float distance;
          screen_2.init();
          if(anchor.data.isFixed == true )
            distance = anchor.getDistance(aPosition);
          else
            distance = 0;
          screen_2.update(aPosition, distance, anchor.data);
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

void initM5Stack()
{
  M5.begin(); // Init M5Core.
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
      screenTask,
      "screen",
      4096,
      NULL,
      tskIDLE_PRIORITY,
      NULL,
      0);
}
void loop()
{
  
}