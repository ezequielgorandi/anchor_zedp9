/**
 * Project anchor
 */

#include "Gps.h"
//#define _DEBUG_GPS
#define MAX_FAIL_READ 6
#define DEGREE_FACTOR 10000000
// Cola por la que se comparte al posicion

QueueHandle_t gpsQueue = NULL;

/**
 * Gps implementation
 */

// Gps gps;
void gpsTask(void *pvParameters)
{
  Gps *gps = (Gps *)pvParameters;
  uint32_t failReadCounter = 0;
  while (gps->begin() == false)
    delay(1000);
  gps->config();
  for (;;)
  {
    if (gps->readPosition() == true)
      failReadCounter = 0; // No fail
    else
    {
      Serial.println("failRead");
      failReadCounter++;
    }
    if (failReadCounter == MAX_FAIL_READ)
    {
      gps->setAsDesconnected();
      failReadCounter = 0; // Restarts the counter
    }
    delay(250);
        UBaseType_t uxHighWaterMark;
  uxHighWaterMark = uxTaskGetStackHighWaterMark( NULL );
    Serial.print("GpsStack:");
    Serial.println(uxHighWaterMark); 
  }
}

Gps::Gps()
{
  status = NOT_CONNECTED;
}

void Gps::initTask()
{
  gpsQueue = xQueueCreate(1, sizeof(position));

  xTaskCreatePinnedToCore(
      gpsTask,
      "gps",
      2048,
      (void *)this,
      3,
      NULL,
      1);
}

GPS_STATUS Gps::getStatus()
{
  return this->status;
}

/**
 * @return bool
 */
bool Gps::isFixed()
{
  return false;
}

bool Gps::begin()
{
  status = NOT_CONNECTED;
  position.status = NOT_CONNECTED; // Borrar uno
  return gpsController.begin();
}

void Gps::config()
{
  gpsController.setVal16(UBLOX_CFG_RATE_MEAS, 500);
  gpsController.setI2COutput(COM_TYPE_UBX);                 // Set the I2C port to output UBX only (turn off NMEA noise)
  gpsController.saveConfigSelective(VAL_CFG_SUBSEC_IOPORT); // Save (only) the communications port settings to flash and BBR
}

bool Gps::readPosition()
{

#ifdef _DEBUG_GPS
  Serial.print(F("FixType: "));
  Serial.print(gpsController.getFixType());
  Serial.println();
#endif
  if (gpsController.isConnected() == false)
  {
    Serial.println("Gps Not Connected");
    return false;
  }
  uint8_t fixType = gpsController.getFixType();
  if (fixType != 3)
  {
    status = LOW_PRECISSION;
    position.status = LOW_PRECISSION; // Borrar uno
  }
  if (fixType == 3)
  {
    status = FIXED;
    position.status = FIXED; // Borrar uno
    position.latitude = gpsController.getLatitude() / (float)DEGREE_FACTOR;
    position.longitude = gpsController.getLongitude() / (float)DEGREE_FACTOR;
    position.accuracy = gpsController.getPositionAccuracy();

#ifdef _DEBUG_GPS
    Serial.println("FixType: " + fixType);
    Serial.print(F("Lat: "));
    Serial.print(position.latitude);
    Serial.println();
    Serial.print(F(" Long: "));
    Serial.print(position.longitude);
    Serial.println();
    Serial.print(F(" 3D Positional Accuracy: "));
    Serial.print(position.accuracy);
    Serial.println(F(" (mm)"));
    Serial.println();
#endif

    // Probando el funcionamiento de las colas
  }
  if (!xQueueOverwrite(gpsQueue, &position))
    Serial.println(F("Queue Problem"));
  return true;
}

void Gps::readPosition_2()
{

  Serial.print(F("FixType: "));
  Serial.print(gpsController.getFixType());
  Serial.println();

  if (gpsController.getFixType() != 3)
  {
    status = LOW_PRECISSION;
    return;
  }
  if (gpsController.getFixType() == 3)
  {
    Serial.println(F("Reading Gps"));
    if (gpsController.getPVT())
    {
      status = FIXED;
      position.latitude = gpsController.getLatitude();
      position.longitude = gpsController.getLongitude();
      position.accuracy = gpsController.getPositionAccuracy();
      Serial.print(F("Lat: "));
      Serial.print(position.latitude);
      Serial.println();
      Serial.print(F(" Long: "));
      Serial.print(position.longitude);
      Serial.println();
      Serial.print(F(" 3D Positional Accuracy: "));
      Serial.print(position.accuracy);
      Serial.println(F(" (mm)"));
      Serial.println();
    }
  }
  // Will never reach this point
}

/**
 * @return position_t
 */
position_t Gps::getPosition()
{
  return position;
}

void Gps::setAsDesconnected()
{
  position.status = NOT_CONNECTED;
  if (!xQueueOverwrite(gpsQueue, &position))
    Serial.println(F("Queue Problem"));
}