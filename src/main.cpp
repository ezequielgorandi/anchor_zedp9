/*Se implemento la cola y funciona OK. Falta reemplazar el uso del GPS en la terea de pantalla, por el uso de la cola (hay que hacerlo?)*/

#include "Anchor.h"
#include "Boat.hpp"
#include "Firebase.h"
#include "Free_Fonts.h"
#include "Gps.h"
#include "Screen.h"
#include "SparkFun_u-blox_GNSS_Arduino_Library.h"
#include <Arduino.h>
#include <M5Stack.h>
#include <WiFi.h>

#define WIFI_SSID "Centro optico gorandi"
#define WIFI_PASSWORD "Libertador3623"

//#define WIFI_SSID "Telecentro-717c"
//#define WIFI_PASSWORD "UMN32NHDZNHJ"

#define APP_BACKGROUND_COLOR TFT_BLACK
#define APP_FONT FSSB9
#define APP_FONT_COLOR TFT_WHITE

uint32_t ypos = 0;
long lastTime = 0; // Simple local timer. Limits amount if I2C traffic to u-blox module.

Boat boat = Boat();

#define FIREBASE_STACK_SIZE 65000
StaticTask_t firebaseBuffer;
StackType_t firebaseStack[FIREBASE_STACK_SIZE];

#define SCREEN_STACK_SIZE 4096
StaticTask_t screenBuffer;
StackType_t screenStack[SCREEN_STACK_SIZE];

void initM5Stack()
{
  M5.begin();       // Init M5Core.
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

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    delay(300);
  }
  Serial.println();
  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP());
  Serial.println();

  boat.initTask();

  xTaskCreateStaticPinnedToCore(
      screenTask,        /* Function that implements the task. */
      "SCREEN",          /* Text name for the task. */
      SCREEN_STACK_SIZE, /* Number of indexes in the xStack array. */
      (void *)&boat,     /* Parameter passed into the task. */
      0,                 /* Priority at which the task is created. */
      screenStack,       /* Array to use as the task's stack. */
      &screenBuffer,
      0);

  xTaskCreatePinnedToCore(
      anchorTask,
      "anchorTask",
      4096,
      (void *)&boat.anchor,
      5,
      NULL,
      0);

  /* Create the task without using any dynamic memory allocation. */
  xTaskCreateStaticPinnedToCore(
      firebaseTask,        /* Function that implements the task. */
      "NAME",              /* Text name for the task. */
      FIREBASE_STACK_SIZE, /* Number of indexes in the xStack array. */
      (void *)&boat,       /* Parameter passed into the task. */
      2,                   /* Priority at which the task is created. */
      firebaseStack,       /* Array to use as  the task's stack. */
      &firebaseBuffer,
      1);
}
void loop()
{
}