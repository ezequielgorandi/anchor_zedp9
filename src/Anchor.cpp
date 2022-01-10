/**
 * Project anchor
 */
#include "Anchor.h"
//#include <bits/stdc++.h>

void anchorTask(void *pvParameters)
{
  Anchor *anchor = (Anchor *)pvParameters;
  for (;;)
  {
    // Serial.println("Tengo que leer boton");
    M5.update();
    if (M5.BtnA.wasPressed())
    {
      if (!xQueueReceive(gpsQueue, &anchor->position, 10000) || anchor->position.status != FIXED)
      {
        Serial.println("Invalid Position: ");
      }
      else
      {
        Serial.println("Anchor Position: ");
        Serial.println(anchor->position.longitude);
        Serial.println(anchor->position.latitude);
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
      anchorTask,   // Function to implement the task.  线程对应函数名称(不能有返回值)
      "anchorTask", //线程名称
      4096,         // The size of the task stack specified as the number of * bytes.任务堆栈的大小(字节)
      (void *)this, // Pointer that will be used as the parameter for the task * being created.  创建作为任务输入参数的指针
      5,            // Priority of the task.  任务的优先级
      NULL,         // Task handler.  任务句柄
      0);           // Core where the task should run.  将任务挂载到指定内核
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
  return position;
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
long Anchor::getDistance(position_t referencePosition)
{
  return 0;
}