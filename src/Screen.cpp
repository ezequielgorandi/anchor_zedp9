/**
 * Project anchor
 */

#include "Screen.h"

/**
 * Screen implementation
 */

Screen::Screen(TFT_eSprite *anImg)
{
  ypos = 0;
  img = anImg;
  img->deleteSprite();
  img->createSprite(IWIDTH, IHEIGHT);
}

void Screen::show()
{
  img->pushSprite(0, 0);
}

void Screen::readButtons()
{
}
void Screen::init()
{}

void Screen::update()
{}
/**
 * Screen_1 implementation
 */

Screen_1::Screen_1(TFT_eSprite *anImg, Gps *aGps) : Screen(anImg)
{
  gps = aGps;
}

void Screen_1::init()
{
  img->fillSprite(APP_BACKGROUND_COLOR);
  // First we test them with a background colour set
  img->setTextSize(1);
  img->setTextColor(APP_FONT_COLOR, APP_BACKGROUND_COLOR);
  img->setFreeFont(APP_FONT);
  img->drawString("Initializing Gps...", 0, 0, FONT4);
  img->setTextColor(TFT_RED, APP_BACKGROUND_COLOR);
  img->drawString("GPS not connected", 0, img->fontHeight(FONT4));
}

void Screen_1::update()
{
  if (gps->getStatus() == LOW_PRECISSION)
  {
    img->setTextColor(TFT_GREENYELLOW, APP_BACKGROUND_COLOR);
    img->drawString("GPS connected              ", 0, img->fontHeight(FONT4));
    delay(1000);
  }
}

Screen_2::Screen_2(TFT_eSprite *anImg, Gps *aGps) : Screen(anImg)
{
  gps = aGps;
}

void Screen_2::init()
{
  img->fillSprite(APP_BACKGROUND_COLOR);

  // First we test them with a background colour set
  img->setTextSize(1);
  img->setTextColor(APP_FONT_COLOR, APP_BACKGROUND_COLOR);
  img->setFreeFont(APP_FONT);
}

void Screen_2::update()
{
  if (gps->getStatus() == FIXED)
  {
    img->fillSprite(APP_BACKGROUND_COLOR);
    img->drawString("Lon: " + String(gps->getPosition().longitude), 0, ypos);
    img->drawString("Lat: " + String(gps->getPosition().latitude), 0, ypos + img->fontHeight(FONT4));
    img->drawString("Acc: " + String(gps->getPosition().accuracy) +"mm", 0, ypos + img->fontHeight(FONT4) * 2);
  }
  else
  {
    img->fillSprite(APP_BACKGROUND_COLOR);
    img->drawString("Fixing position, please wait...", 0, ypos);
  }
}
