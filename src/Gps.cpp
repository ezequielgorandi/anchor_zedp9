/**
 * Project anchor
 */

#include "Gps.h"
//#define _DEBUG_GPS
#define DEGREE_FACTOR 10000000

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
}


// Utility function for converting degrees to radians
long double Gps::toRadians(const long double degree)
{
  long double one_deg = (M_PI) / 180;
  return (one_deg * degree);
}



long double Gps::calculateDistance(long double lat1, long double long1,
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