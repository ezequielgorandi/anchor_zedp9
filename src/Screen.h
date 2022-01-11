#ifndef _SCREEN_H
#define _SCREEN_H

#include "Free_Fonts.h"
#include "Gps.h"
#include "Anchor.h"
#include "SparkFun_u-blox_GNSS_Arduino_Library.h"
#include "position.h"
#include <M5Stack.h>

#define IWIDTH 320
#define IHEIGHT 177 //Si se pone un valor mas alto, deja de mostrarse la pantalla..
#define APP_BACKGROUND_COLOR TFT_BLACK
#define APP_FONT FSSB9
#define APP_FONT_COLOR TFT_WHITE

class Screen
{
public:
	uint32_t ypos;
	TFT_eSprite *img;
	Screen(TFT_eSprite *anImg);
	void init();
	void show();
	void update(position_t position);
	void update();
	void readButtons();
};

class Screen_1 : public Screen
{
public:
	Screen_1(TFT_eSprite *anImg, Gps *aGps);
	void init();
	void update();
	void update(position_t position);

private:
	Gps *gps;
};

class Screen_2 : public Screen
{
public:
	Screen_2(TFT_eSprite *anImg, Gps *aGps);
	void init();
	void update();
	void update(position_t position, float distance, anchorData_t data);

private:
	Gps *gps;
};

class Screen_SetAnchorPosition : public Screen
{
public:
	Screen_SetAnchorPosition(TFT_eSprite *anImg);
	bool set(int* anchorFlags, anchorData_t anchor);
	void init();
	void update(position_t aPosition);
	void error();
};
#endif //_SCREEN_H