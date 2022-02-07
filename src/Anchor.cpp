/**
 * Project anchor
 */
#include "Anchor.h"
#include "Firebase.h"
#include "setup.h"
QueueHandle_t anchorNewPositionQueue = NULL;

void anchorTask(void *pvParameters)
{
  Anchor *anchor = (Anchor *)pvParameters;
  for (;;)
  {
    M5.update();
    if (M5.BtnB.wasPressed())
    {
      Serial.println("Unfixing");
      anchor->setFixed(false);
      bool push = 1;
      if (!xQueueOverwrite(pushAnchorDataQueue, &push))
        Serial.println(F("Queue Problem"));
    }
    if (M5.BtnA.wasPressed())
    {
      anchor->newPositionFlag = 1;                                            // TODO: Quitar esta variable
      if (!xQueueOverwrite(anchorNewPositionQueue, &anchor->newPositionFlag)) // avisa que se presiono el boton
        Serial.println(F("Queue Problem"));
      position_t position;
      if (!xQueueReceive(gpsQueue, &position, 10000) || position.status != FIXED || position.accuracy > MIN_ANCHOR_ACCURACY)
      {
        anchor->setFixed(false);
        anchor->newPositionFlag = -1;
        Serial.println("Invalid Position: ");
      }
      else
      {
        anchor->setPosition(position);
        anchor->setFixed(true);
        anchor->newPositionFlag = 0;
        // Avisa a firebase que hay datos que sincronizar
        bool push;
        if (!xQueueOverwrite(pushAnchorDataQueue, &push))
          Serial.println(F("Queue Problem"));
        Serial.println("Anchor Position: ");
        Serial.println("IsFixed: " + String(anchor->isFixed()));
        Serial.println(anchor->getPosition().longitude, 6);
        Serial.println(anchor->getPosition().latitude, 6);
      }
      delay(1000);
    }
    delay(100);
  }
}

/**
 * Anchor implementation
 */

/**
 * @param position
 */
Anchor::Anchor()
{
  data.isFixed = false;
  newPositionFlag = 0;
  anchorNewPositionQueue = xQueueCreate(1, sizeof(bool));
}

/**
 * @return position_t
 */
position_t Anchor::getPosition()
{
  return data.position;
}

void Anchor::setPosition(position_t position)
{
  data.position = position;
}

void Anchor::setFixed(bool value)
{
  data.isFixed = value;
}

/**
 * @param north
 * @param west
 */
void Anchor::move(float north, float west)
{
}

bool Anchor::isFixed()
{
  if (data.isFixed == 1)
    return true;
  else
    return false;
}
