/**
 * Project anchor
 */
#ifndef _ANCHOR_H
#define _ANCHOR_H
#include "position.h"
#include <M5Stack.h>
#include "Gps.h"

void anchorTask(void *pvParameters);
typedef struct
{
	bool isFixed;
	position_t position;
} anchorData_t;

class Anchor
{
public:
	
	int newPositionFlag;
	Anchor();
	position_t getPosition();
	void setPosition(position_t position);
	void setFixed(bool value);
	bool isFixed();
	/**
	 * @param north
	 * @param west
	 */
	void move(float north, float west);
	
private:
	anchorData_t data;
};

extern QueueHandle_t anchorNewPositionQueue;
#endif //_ANCHOR_H