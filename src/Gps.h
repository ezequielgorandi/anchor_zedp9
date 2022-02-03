/**
 * Project anchor
 */

#ifndef _GPS_H
#define _GPS_H

#include "SparkFun_u-blox_GNSS_Arduino_Library.h"
#include "position.h"
#include "Anchor.h"
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
	void initTask();
	void setAsDesconnected();
	GPS_STATUS getStatus();

private:
	GPS_STATUS status;
	position_t position;
	SFE_UBLOX_GNSS gpsController;
};

//extern Gps gps;
extern QueueHandle_t gpsQueue;
#endif //_GPS_H