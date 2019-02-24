#ifndef _HEADWATER_UI_H_
#define _HEADWATER_UI_H_

#include "lcd.h"
#include "headwater_lcd.h"
#include "headwater_state.h"

#define UI_DISPLAY LCD
#define UI_STATE HeadwaterState

#include "ui.h"

UIScreen headwater_ui_main_screen(void);

#endif
