/**
 * Project anchor
 */

#ifndef _POSITION_T_H
#define _POSITION_T_H
#include "stdint.h"

typedef struct{
uint8_t minute = 0;
uint8_t hour = 0;
uint8_t day = 0;
uint8_t month = 0;
uint8_t year = 0;
} date_t ;
typedef struct {
	float longitude = 0;
	float latitude = 0;
	uint32_t accuracy = 0;
	uint8_t status = 0;
	date_t date;
	uint32_t epoch;
} position_t;

#endif //_POSITION_T_H