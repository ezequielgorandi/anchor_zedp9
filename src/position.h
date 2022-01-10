/**
 * Project anchor
 */

#ifndef _POSITION_T_H
#define _POSITION_T_H
#include "stdint.h"


typedef struct {
	float longitude = 0;
	float latitude = 0;
	uint32_t accuracy = 0;
	uint8_t status = 0;
} position_t ;

#endif //_POSITION_T_H