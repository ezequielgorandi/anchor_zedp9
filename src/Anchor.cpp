/**
 * Project anchor
 */
#include "Anchor.h"
#include "setup.h"

QueueHandle_t anchorPosSettedQueue = NULL;

void anchorTask(void *pvParameters)
{
  Anchor *anchor = (Anchor *)pvParameters;
  for (;;)
  {
    M5.update();
    if (M5.BtnB.wasPressed())
    {
      anchor->data.isFixed = false;
    }
    if (M5.BtnA.wasPressed())
    {
      anchor->newPositionFlag = 1;
      if (!xQueueReceive(gpsQueue, &anchor->data.position, 10000) || anchor->data.position.status != FIXED || anchor->data.position.accuracy > MIN_ANCHOR_ACCURACY)
      {
        anchor->data.isFixed = false;
        anchor->newPositionFlag = -1;
        Serial.println("Invalid Position: ");
      }
      else
      {
        anchor->data.isFixed = true;
        anchor->newPositionFlag = 2;
        Serial.println("Anchor Position: ");
        Serial.println(anchor->data.position.longitude, 6);
        Serial.println(anchor->data.position.latitude, 6);
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
  xTaskCreatePinnedToCore(
      anchorTask,
      "anchorTask",
      4096,
      (void *)this,
      5,
      NULL,
      0);

  anchorPosSettedQueue = xQueueCreate(1, sizeof(data.position));
}

/**
 * @return position_t
 */
position_t Anchor::getPosition()
{
  return data.position;
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
