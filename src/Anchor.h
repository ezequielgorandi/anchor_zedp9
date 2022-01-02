/**
 * Project anchor
 */
#ifndef _ANCHOR_H
#define _ANCHOR_H
#include "position.h"

class Anchor
{
public:
	
	Anchor();

	position_t getPosition();

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
	position_t position;
};

#endif //_ANCHOR_H