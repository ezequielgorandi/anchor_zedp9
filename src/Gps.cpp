/**
 * Project anchor
 */

#include "Gps.h"
#define _DEBUG_GPS
/**
 * Gps implementation
 */

Gps::Gps()
{
  status = NOT_CONNECTED;
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
  return gpsController.begin();
}

void Gps::config()
{
  //Serial.print(F("Configurando GPS"));
  //gpsController.powerSaveMode(false);
  //gpsController.setI2COutput(COM_TYPE_UBX);                 // Set the I2C port to output UBX only (turn off NMEA noise)
  //gpsController.saveConfigSelective(VAL_CFG_SUBSEC_IOPORT); // Save (only) the communications port settings to flash and BBR
  //gpsController.setAutoPVT(true);                           // Tell the GNSS to "send" each solution
  gpsController.setVal16(UBLOX_CFG_RATE_MEAS, 500);
  gpsController.setI2COutput(COM_TYPE_UBX); //Set the I2C port to output UBX only (turn off NMEA noise)
  gpsController.saveConfigSelective(VAL_CFG_SUBSEC_IOPORT); //Save (only) the communications port settings to flash and BBR
}

void Gps::readPosition()
{

  #ifdef _DEBUG_GPS
  Serial.print(F("FixType: "));
  Serial.print(gpsController.getFixType());
  Serial.println();
  #endif

  if (gpsController.getFixType() != 3)
  {
    status = LOW_PRECISSION;
    return;
  }
  if (gpsController.getFixType() == 3)
  {
    Serial.println(F("Reading Gps"));
    {
      status = FIXED;
      position.latitude = gpsController.getLatitude();
      position.longitude = gpsController.getLongitude();
      position.accuracy = gpsController.getPositionAccuracy();
      
      #ifdef _DEBUG_GPS
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
    }
  }
  // Will never reach this point
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

Gps gps;