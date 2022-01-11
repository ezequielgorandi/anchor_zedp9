/**
 * Project anchor
 */
#ifndef _ANCHOR_H
#define _ANCHOR_H
#include "Gps.h"
#include <M5Stack.h>
#include <math.h>

typedef struct
{
	bool isFixed;
	position_t position;
} anchorData_t;

class Anchor
{
public:
	
	Anchor();
	position_t getPosition();
	int newPositionFlag;
	anchorData_t data;
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
	float getDistance(position_t referencePosition);

private:
	long double toRadians(const long double degree);

};

extern QueueHandle_t anchorPosSettedQueue;
#endif //_ANCHOR_H