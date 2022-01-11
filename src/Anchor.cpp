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

// Utility function for converting degrees to radians
long double Anchor::toRadians(const long double degree)
{
  long double one_deg = (M_PI) / 180;
  return (one_deg * degree);
}

long double Anchor::distance(long double lat1, long double long1,
                             long double lat2, long double long2)
{
  // Convert the latitudes and longitudes from degree to radians.
  lat1 = toRadians(lat1);
  long1 = toRadians(long1);
  lat2 = toRadians(lat2);
  long2 = toRadians(long2);

  // Haversine Formula
  long double dlong = long2 - long1;
  long double dlat = lat2 - lat1;

  long double ans = pow(sin(dlat / 2), 2) + cos(lat1) * cos(lat2) * pow(sin(dlong / 2), 2);
  ans = 2 * asin(sqrt(ans));

  // Radius of Earth in Kilometers, R = 6371
  // Use R = 3956 for miles
  long double R = 6371;

  // Calculate the result
  ans = ans * R;

  return ans;
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

/**
 * @param referencePosition
 * @return long
 */
#define WINDOW_SIZE 3
// Esta funcion es llamada desde "screen". Idealmente, debería ejecutarse cada vez que se recibe un nuevo valor de GPS.
float Anchor::getDistance(position_t reference)
{
  static float media[WINDOW_SIZE] = {0, 0, 0};
  static int index = 0;
  float sum = 0;
  media[index] = distance(reference.latitude, reference.longitude, data.position.latitude, data.position.longitude) * 1000;
  // media[index] = distance(-34.6528401716296, -58.61750658743727, -34.65179758741755, -58.61754637124556)*1000;

  index++;
  if (index == WINDOW_SIZE)
    index = 0;
  for (int i = 0; i < WINDOW_SIZE; i++)
  {
    sum += media[i];
  }
  return sum / WINDOW_SIZE;
}