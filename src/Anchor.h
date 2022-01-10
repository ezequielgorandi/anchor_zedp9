/**
 * Project anchor
 */
#ifndef _ANCHOR_H
#define _ANCHOR_H
#include "Gps.h"
#include <M5Stack.h>
#include <math.h>
class Anchor
{
public:
	
	Anchor();
	position_t getPosition();
	position_t position;
	long double distance(long double lat1, long double long1,
															 long double lat2, long double long2);
	/**
	 * @param north
	 * @param west
	 */
	void move(float north, float west);

	/**
	 * @param referencePosition
	 */
	long getDistance(position_t referencePosition);

private:
	long double toRadians(const long double degree);

};

#endif //_ANCHOR_H