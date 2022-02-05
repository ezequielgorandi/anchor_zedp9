/**
 * Project anchor
 */
#include "Screen.h"

/**
 * Screen implementation
 */
void setScreen(Screen **actualScreen, Screen *screenToSet)
{
  (*actualScreen) = screenToSet;
  screenToSet->finish();
  (*actualScreen)->init();
}

Screen_1 screen_1;
Screen_2 screen_2;
Screen_SetAnchorPosition screen_setAnchorPos;

void screenTask(void *pvParameters)
{
  Boat *boat = (Boat *)pvParameters;
  TFT_eSprite img = TFT_eSprite(&M5.Lcd);
  UBaseType_t uxHighWaterMark;
  Serial.println(uxHighWaterMark);

  screen_1 = Screen_1(&img, boat);
  screen_2 = Screen_2(&img, boat);
  screen_setAnchorPos = Screen_SetAnchorPosition(&img, boat);
  Screen *screen_ = &screen_1; // Este puntero guarda la pantalla a mostrar

  position_t aPosition;
  screen_->init();
  while (1)
  {
    screen_->handle(&screen_);
    screen_->update();
    delay(500);
  }
}

// void screenTask_2(void *pvParameters)
//{
//   TFT_eSprite img = TFT_eSprite(&M5.Lcd);
//   UBaseType_t uxHighWaterMark;
//   uxHighWaterMark = uxTaskGetStackHighWaterMark(NULL);
//   Serial.println(uxHighWaterMark);
//   int count = 1;
//
//   Screen *screen_;
//   screen_1 = Screen_1(&img, &boat.gps);
//   screen_2 = Screen_2(&img, &boat.gps);
//   Screen_SetAnchorPosition screen_setAnchorPos = Screen_SetAnchorPosition(&img);
//   position_t aPosition;
//
//   screen_1.init();
//   screen_1.update(aPosition);
//
//   while(1)
//   {
//     screen_->handle(screen_);
//   }
//
//   while (1)
//   {
//     // Anchor is being fixed
//     if (boat.anchor.newPositionFlag != 0)
//     {
//       if (screen_setAnchorPos.set(&boat.anchor.newPositionFlag, boat.anchor.data) == true)
//         delay(3000);
//     }
//     aPosition = boat.gps.getPosition();
//     switch (aPosition.status)
//     {
//     case NOT_CONNECTED:
//     case LOW_PRECISSION:
//       setScreen(screen_, &screen_1);
//       break;
//     case FIXED:
//       float distance;
//       setScreen(screen_, &screen_2);
//       screen_2.init(&img);
//       if (boat.anchor.data.isFixed == true)
//         distance = boat.getAnchorDistance();
//       else
//         distance = 0;
//       screen_2.update(aPosition, distance, boat.anchor.data);
//       break;
//     }
//     uxHighWaterMark = uxTaskGetStackHighWaterMark(NULL);
//     delay(250);
//     Serial.print("Display:");
//     Serial.println(uxHighWaterMark);
//     // Will never reach this point.
//   }
// }

Screen::Screen()
{
}

Screen::Screen(TFT_eSprite *anImg, Boat *aBoat)
{
  img = anImg;
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
void Screen::init()
{
}
void Screen::handle(Screen **screen)
{
}
void Screen::finish()
{
}
void Screen::update()
{
}

Screen_1::Screen_1()
{
}

/**
 * Screen_1 implementation
 */
Screen_1::Screen_1(TFT_eSprite *anImg, Boat *aBoat) : Screen(anImg, aBoat)
{
  boat = aBoat;
}

void Screen_1::init()
{
  img->deleteSprite();
  img->setColorDepth(8);
  img->createSprite(IWIDTH, LINE_HEIGHT);
  Screen::clear();

  img->setTextSize(1);
  img->setTextColor(APP_FONT_COLOR, APP_BACKGROUND_COLOR);
  img->setFreeFont(APP_FONT);
  img->drawString("Initializing Gps...", 0, 0, FONT4);
  Screen::showLine(0);
  Serial.println("Init Screen 1");
}

void Screen_1::update()
{
  if (boat->gps.getStatus() == LOW_PRECISSION)
  {
    img->setTextColor(TFT_GREENYELLOW, APP_BACKGROUND_COLOR);
    img->drawString("GPS connected", 0, 0, FONT4);
  }
  if (boat->gps.getStatus() == NOT_CONNECTED)
  {
    img->setTextColor(TFT_RED, APP_BACKGROUND_COLOR);
    img->drawString("GPS not connected", 0, 0, FONT4);
  }
  img->pushSprite(0, 1 * LINE_HEIGHT);
}

void Screen_1::handle(Screen **screen_)
{
  if (boat->gps.getPosition().status == FIXED)
  {
    Serial.println("Set screen 2");
    setScreen(screen_, &screen_2);
  }
}

Screen_2::Screen_2()
{
}

Screen_2::Screen_2(TFT_eSprite *anImg, Boat *aBoat) : Screen(anImg, aBoat)
{
  boat = aBoat;
}

void Screen_2::init()
{
  Serial.println("Init Screen 2");
  Screen::clear();
  ypos = 0;
  img->deleteSprite();
  img->setColorDepth(8);
  img->createSprite(160, 144);

  // First we test them with a background colour set
  img->setTextSize(1);
  img->setTextColor(APP_FONT_COLOR, APP_BACKGROUND_COLOR);
  img->setFreeFont(APP_FONT);
}

void Screen_2::handle(Screen **screen_)
{
  position_t position = boat->gps.getPosition();
  if (boat->anchor.newPositionFlag != 0)
  {
    if (screen_setAnchorPos.set(&boat->anchor.newPositionFlag) == true)
      delay(3000);
  }

  if (position.status == LOW_PRECISSION || position.status == NOT_CONNECTED)
  {
    setScreen(screen_, &screen_1);
  }
}

void Screen_2::update()
{
  img->setTextColor(APP_FONT_COLOR, APP_BACKGROUND_COLOR);
  img->fillSprite(APP_BACKGROUND_COLOR);
  if (boat->gps.getStatus() == FIXED)
  {
    img->drawString("Lon: " + String(boat->gps.getPosition().longitude, 4), 0, ypos);
    img->drawString("Lat: " + String(boat->gps.getPosition().latitude, 4), 0, ypos + img->fontHeight(FONT4));
    img->drawString("Acc: " + String(boat->gps.getPosition().accuracy) + "mm", 0, ypos + img->fontHeight(FONT4) * 2);
    if (boat->anchor.isFixed())
    {
      img->setTextColor(GREEN, APP_BACKGROUND_COLOR);
      img->drawString("Anchor: Fixed", 0, ypos + img->fontHeight(FONT4) * 3);
      img->setTextColor(APP_FONT_COLOR, APP_BACKGROUND_COLOR);
      img->drawString("Distance: " + String(boat->getAnchorDistance(), 0) + "M", 0, ypos + img->fontHeight(FONT4) * 4);
    }
    else
    {
      if (boat->anchor.newPositionFlag == 1)
        img->drawString("Anchor: Fixing...", 0, ypos + img->fontHeight(FONT4) * 3);
      else
      {
        img->setTextColor(RED, APP_BACKGROUND_COLOR);
        img->drawString("Anchor: Not Fixed", 0, ypos + img->fontHeight(FONT4) * 3);
        img->setTextColor(APP_FONT_COLOR, APP_BACKGROUND_COLOR);
      }
    }
  }
  else
  {
    img->drawString("Fixing position, please wait...", 0, 0);
  }
  img->pushSprite(0, 0);
}

Screen_SetAnchorPosition::Screen_SetAnchorPosition(TFT_eSprite *anImg, Boat *boat) : Screen(anImg, boat)
{
}

bool Screen_SetAnchorPosition::set(int *anchorFlags)
{
  bool ret = false;
  init();
  switch (*anchorFlags)
  {
  case 2:
    update(boat->anchor.getPosition());
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
  uint32_t yref = img->fontHeight(FONT4), xref = 0;
  img->fillRoundRect(xref, yref, 160, img->fontHeight(FONT4) * 4, 1, DARKGREEN);
  img->setTextColor(WHITE, DARKGREEN);
  img->setFreeFont(APP_FONT);
  img->drawString(" Fixing...", xref, yref + img->fontHeight(FONT4));
}

void Screen_SetAnchorPosition::update(position_t aPosition)
{
  uint32_t yref = img->fontHeight(FONT4), xref = 0;
  img->fillRoundRect(xref, yref, 160, img->fontHeight(FONT4) * 4, 1, DARKGREEN);
  img->drawString(" Fixed!", xref, yref + img->fontHeight(FONT4) * 1);
  img->drawString(" Accuraccy: " + String(aPosition.accuracy) + " mm", xref, yref + img->fontHeight(FONT4) * 2);
}

void Screen_SetAnchorPosition::error()
{
  uint32_t yref = IHEIGHT * 0.3, xref = IWIDTH * 0.1;
  img->setTextColor(MAROON, DARKGREEN);
  img->drawString("                NOT FIXED!             ", xref, yref + img->fontHeight(FONT4) * 2);
  img->drawString("             Please try again          ", xref, yref + img->fontHeight(FONT4) * 3);
}