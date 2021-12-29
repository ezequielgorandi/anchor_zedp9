/**
 * Project anchor
 */
#include "SparkFun_u-blox_GNSS_Arduino_Library.h"
#include "position.h"

#ifndef _GPS_H
#define _GPS_H

class Gps {
public: 
	
position_t getPosition();
	
bool isFixed();
private: 
	position_t position;
	SFE_UBLOX_GNSS gpsController;
};

#endif //_GPS_H