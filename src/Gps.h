/**
 * Project anchor
 */

#ifndef _GPS_H
#define _GPS_H

#include "Anchor.h"
#include "SparkFun_u-blox_GNSS_Arduino_Library.h"
#include "position.h"
enum GPS_STATUS
{
	NOT_CONNECTED,
	LOW_PRECISSION,
	FIXED
};

class Gps
{
public:
	Gps();
	position_t getPosition();
	bool isFixed();
	bool begin();
	void config();
	bool readPosition();
	void readPosition_2();
	void setAsDesconnected();
	GPS_STATUS getStatus();
	long double calculateDistance(long double lat1, long double long1,
																long double lat2, long double long2);

private:
	GPS_STATUS status;
	position_t position;
	SFE_UBLOX_GNSS gpsController;
	long double toRadians(const long double degree);
};

// extern Gps gps;
extern QueueHandle_t gpsQueue;
#endif //_GPS_H