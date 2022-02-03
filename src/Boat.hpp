#ifndef BOAT_H
#define BOAT_H

#include "Boat.hpp"
#include "Anchor.h"
#include "Gps.h"

class Boat
{
public:
    Boat();
    void initTask();
    float getAnchorDistance();
    void setAnchorDistance(position_t referencePosition);

    Anchor anchor;
    Gps gps;

private:

    float distance;

};
#endif

