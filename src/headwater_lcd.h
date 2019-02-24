#ifndef _HEADWATER_LCD_H_
#define _HEADWATER_LCD_H_

#include "headwater_state.h"
// #include "headwater_ui.h"
#include "lcd.h"

void headwater_lcd_update_main_bpm(LCD *lcd, uint16_t value);
void headwater_lcd_update_main(LCD *lcd, HeadwaterState *state);

#endif
