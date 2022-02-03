#include "Boat.hpp"
#define MAX_FAIL_READ 15


void checkPositionTask(void *pvParameters)
{
  Boat *boat = (Boat *)pvParameters;
  uint32_t failReadCounter = 0;
  while (boat->gps.begin() == false)
    delay(1000);
  boat->gps.config();
  for (;;)
  {
    if (boat->gps.readPosition() == true)
    {
      failReadCounter = 0; // Posicion tomada con exito
      if (boat->anchor.isFixed())
      {
        boat->setAnchorDistance(boat->gps.getPosition());
      }
    }
    else
    {
      Serial.println("failRead");
      failReadCounter++;
    }
    if (failReadCounter == MAX_FAIL_READ)
    {
      boat->gps.setAsDesconnected();
      failReadCounter = 0; // Restarts the counter
    }
    delay(250);
  }
}

Boat::Boat()
{
  anchor = Anchor();
  gps = Gps();
}

void Boat::initTask()
{
  xTaskCreatePinnedToCore(
      checkPositionTask,
      "gps",
      4096,
      (void *)this,
      3,
      NULL,
      0);
}

/**
 * @param referencePosition
 * @return long
 */
#define WINDOW_SIZE 3
// Esta funcion es llamada desde "screen". Idealmente, debería ejecutarse cada vez que se recibe un nuevo valor de GPS.
float Boat::getAnchorDistance()
{
  return distance;
}

void Boat::setAnchorDistance(position_t referencePosition) 
{
  static float media[WINDOW_SIZE] = {0, 0, 0};
  static int index = 0;
  float sum = 0;
  media[index] = gps.calculateDistance(referencePosition.latitude, referencePosition.longitude, anchor.data.position.latitude, anchor.data.position.longitude) * 1000;
  // media[index] = distance(-34.6528401716296, -58.61750658743727, -34.65179758741755, -58.61754637124556)*1000;

  index++;
  if (index == WINDOW_SIZE)
    index = 0;
  for (int i = 0; i < WINDOW_SIZE; i++)
  {
    sum += media[i];
  }
  this->distance = sum / (float)WINDOW_SIZE; 
}

