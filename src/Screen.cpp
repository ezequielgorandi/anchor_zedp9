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
  img->setColorDepth(8);
  img->createSprite(IWIDTH, LINE_HEIGHT);
}

void Screen::show()
{
  img->pushSprite(0, 0);
}

void Screen::clear()
{
  img->fillSprite(APP_BACKGROUND_COLOR);
  for (int i = 0; i < 8; i++)
    showLine(i);
}

void Screen::showLine(int32_t line)
{
  img->pushSprite(0, line * LINE_HEIGHT);
}

void Screen::readButtons()
{
}
void Screen::init()
{
}

void Screen::update()
{
}
/**
 * Screen_1 implementation
 */

Screen_1::Screen_1(TFT_eSprite *anImg, Gps *aGps) : Screen(anImg)
{
  gps = aGps;
}

void Screen_1::init()
{
  Screen::clear();
  img->setTextSize(1);
  img->setTextColor(APP_FONT_COLOR, APP_BACKGROUND_COLOR);
  img->setFreeFont(APP_FONT);
  img->drawString("Initializing Gps...", 0, 0, FONT4);
  Screen::showLine(0);
}

void Screen_1::update()
{
  Serial.println("Status: ");
  if (gps->getStatus() == LOW_PRECISSION)
  {
    img->setTextColor(TFT_GREENYELLOW, APP_BACKGROUND_COLOR);
    img->drawString("GPS connected              ", 0, img->fontHeight(FONT4));
    Screen::showLine(1);
  }
  else
  {
    img->setTextColor(TFT_RED, APP_BACKGROUND_COLOR);
    img->drawString("GPS not connected", 0, img->fontHeight(FONT4));
    Screen::showLine(1);
  }
}

void Screen_1::update(position_t position)
{
  if (position.status == LOW_PRECISSION)
  {
    img->setTextColor(TFT_GREENYELLOW, APP_BACKGROUND_COLOR);
    img->drawString("GPS connected              ", 0, img->fontHeight(FONT4));
    Screen::showLine(1);
  }
  else
  {
    img->setTextColor(TFT_RED, APP_BACKGROUND_COLOR);
    img->drawString("GPS not connected", 0, img->fontHeight(FONT4));
    Screen::showLine(1);
  }
}

Screen_2::Screen_2(TFT_eSprite *anImg, Gps *aGps) : Screen(anImg)
{
  gps = aGps;
}

void Screen_2::init(TFT_eSprite *anImg)
{
  Screen::clear();
  ypos = 0;
  img = anImg;
  img->deleteSprite();
  img->setColorDepth(8);
  img->createSprite(160, 144);

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
    img->drawString("Lon: " + String(gps->getPosition().longitude, 4), 0, ypos);
    img->drawString("Lat: " + String(gps->getPosition().latitude, 4), 0, ypos + img->fontHeight(FONT4));
    img->drawString("Acc: " + String(gps->getPosition().accuracy) + "mm", 0, ypos + img->fontHeight(FONT4) * 2);
  }
  else
  {
    img->fillSprite(APP_BACKGROUND_COLOR);
    img->drawString("Fixing position, please wait...", 0, ypos);
  }
  Screen::show();
}

void Screen_2::update(position_t position, float distance, anchorData_t anchor)
{
  if (gps->getStatus() == FIXED)
  {
    img->drawString("Lon: " + String(gps->getPosition().longitude, 4), 0, ypos);
    img->drawString("Lat: " + String(gps->getPosition().latitude, 4), 0, ypos + img->fontHeight(FONT4));
    img->drawString("Acc: " + String(gps->getPosition().accuracy) + "mm", 0, ypos + img->fontHeight(FONT4) * 2);
    if (anchor.isFixed == true)
    {
      img->drawString("Anchor: Fixed", 0, ypos + img->fontHeight(FONT4) * 3);
      img->drawString("Distance: " + String(distance, 0) + "M", 0, ypos + img->fontHeight(FONT4) * 4);
    }
    else
    {
      img->drawString("Anchor: Not Fixed", 0, ypos + img->fontHeight(FONT4) * 3);
    }
  }
  else
  {
    Screen::clear();
    img->drawString("Fixing position, please wait...", 0, 0);
  }
  img->pushSprite(0, 0);
}

Screen_SetAnchorPosition::Screen_SetAnchorPosition(TFT_eSprite *anImg) : Screen(anImg)
{
}

bool Screen_SetAnchorPosition::set(int *anchorFlags, anchorData_t anchor)
{
  bool ret = false;
  init();
  switch (*anchorFlags)
  {
  case 2:
    update(anchor.position);
    *anchorFlags = 0;
    ret = true; // Termina la pantalla
    break;
  case -1:
    error();
    *anchorFlags = 0;
    ret = true; // Termina la pantalla
    break;
  }
  show();
  return ret;
}

void Screen_SetAnchorPosition::init()
{
  uint32_t yref = IHEIGHT * 0.3, xref = IWIDTH * 0.1;
  img->fillRoundRect(xref, yref, IWIDTH * 0.8, IHEIGHT, 1, DARKGREEN);
  img->setTextColor(WHITE, DARKGREEN);
  img->setFreeFont(APP_FONT);
  img->drawString("    Fixing anchor position...", xref, yref + img->fontHeight(FONT4));
}

void Screen_SetAnchorPosition::update(position_t aPosition)
{
  uint32_t yref = IHEIGHT * 0.3, xref = IWIDTH * 0.1;
  img->drawString("    Fixed!", xref, yref + img->fontHeight(FONT4) * 2);
  img->drawString("    Accuraccy: " + String(aPosition.accuracy) + " mm", xref, yref + img->fontHeight(FONT4) * 3);
}

void Screen_SetAnchorPosition::error()
{
  uint32_t yref = IHEIGHT * 0.3, xref = IWIDTH * 0.1;
  img->setTextColor(MAROON, DARKGREEN);
  img->drawString("                NOT FIXED!             ", xref, yref + img->fontHeight(FONT4) * 2);
  img->drawString("             Please try again          ", xref, yref + img->fontHeight(FONT4) * 3);
}