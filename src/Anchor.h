/**
 * Project anchor
 */
#ifndef _ANCHOR_H
#define _ANCHOR_H
#include "position.h"
#include <M5Stack.h>
#include "Gps.h"

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

	/**
	 * @param north
	 * @param west
	 */
	void move(float north, float west);
	bool isFixed();
};

extern QueueHandle_t anchorPosSettedQueue;
#endif //_ANCHOR_H