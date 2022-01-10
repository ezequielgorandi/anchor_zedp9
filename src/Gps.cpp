/**
 * Project anchor
 */

#include "Gps.h"
//#define _DEBUG_GPS
#define MAX_FAIL_READ 3
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
  }
}

Gps::Gps()
{
  status = NOT_CONNECTED;
}

void Gps::initTask()
{
  gpsQueue = xQueueCreate(
      /* The number of items the queue can hold. */
      1,
      /* Size of each item is big enough to hold the
      whole structure. */
      sizeof(position));

  xTaskCreatePinnedToCore(
      gpsTask,      // Function to implement the task.  线程对应函数名称(不能有返回值)
      "gps",        //线程名称
      4096,         // The size of the task stack specified as the number of * bytes.任务堆栈的大小(字节)
      (void *)this, // Pointer that will be used as the parameter for the task * being created.  创建作为任务输入参数的指针
      4,            // Priority of the task.  任务的优先级
      NULL,         // Task handler.  任务句柄
      1);           // Core where the task should run.  将任务挂载到指定内核
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
  // Serial.print(F("Configurando GPS"));
  // gpsController.powerSaveMode(false);
  // gpsController.setI2COutput(COM_TYPE_UBX);                 // Set the I2C port to output UBX only (turn off NMEA noise)
  // gpsController.saveConfigSelective(VAL_CFG_SUBSEC_IOPORT); // Save (only) the communications port settings to flash and BBR
  // gpsController.setAutoPVT(true);                           // Tell the GNSS to "send" each solution
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