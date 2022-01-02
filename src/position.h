/**
 * Project anchor
 */

#ifndef _POSITION_T_H
#define _POSITION_T_H
#include "stdint.h"
typedef struct {
	long longitude;
	long latitude;
	uint32_t accuracy;
} position_t ;

#endif //_POSITION_T_H