#ifndef _SCREEN_H
#define _SCREEN_H

#include "Boat.hpp"
#include "Free_Fonts.h"
#include "Gps.h"
#include "SparkFun_u-blox_GNSS_Arduino_Library.h"
#include "position.h"
#include <M5Stack.h>

#define IWIDTH 320
#define IHEIGHT 177 // Si se pone un valor mas alto, deja de mostrarse la pantalla..
#define LINE_HEIGHT 30
#define APP_BACKGROUND_COLOR TFT_BLACK
#define APP_FONT FSSB9
#define APP_FONT_COLOR TFT_WHITE
void screenTask(void *pvParameters);
class Screen
{
public:
	uint32_t ypos;
	TFT_eSprite *img;
	Screen();
	Screen(TFT_eSprite *anImg, Boat *aBoat);
	virtual void init() ;
	virtual void handle(Screen **screen)  ;
	void finish();
	void show();
	virtual void update();
	void showLine(int32_t line);
	void clear();
	Boat *boat;
};

class Screen_1 : public Screen
{
public:
	Screen_1();
	Screen_1(TFT_eSprite *anImg, Boat *aBoat);
	void init() override;
	void handle(Screen **actualScreen) override;
	void update() override;
	void update(position_t position);
	void showLine(int32_t line);

private:
};

class Screen_2 : public Screen
{
public:
	Screen_2();
	Screen_2(TFT_eSprite *anImg, Boat *aBoat);
	void init() override;
	void handle(Screen **actualScreen) override;
	void update()override ;
	void showLine(int32_t line);

private:
	String anchorFixed = "Not Fixed";
};

class Screen_SetAnchorPosition : public Screen
{
public:
	Screen_SetAnchorPosition(){};
	Screen_SetAnchorPosition(TFT_eSprite *anImg, Boat *boat);
	bool set(int *anchorFlags);
	void init();
	void update(position_t aPosition);
	void error();
};
#endif //_SCREEN_H