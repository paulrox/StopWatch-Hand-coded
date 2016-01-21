/*
 * WidgetConfig.c
 *
 *  Created on: 21/ott/2015
 *      Author: admim
 */

#include "Widget.h"
#include "WidgetConfig.h"
#include "mypictures.h"
#include <stdio.h>
#include "stm32f4_discovery_lcd.h"
#include "fonts.h"

Icon watch_b = {
		b_watch_on, b_watch_off, WATCHBPRESS
};

Icon swatch_b = {
		b_swatch_on, b_swatch_off, SWATCHBPRESS
};

Icon alarm_b = {
		b_alarm_on, b_alarm_off, ALARMBPRESS
};

Icon timer_b = {
		b_timer_on, b_timer_off, TIMERBPRESS
};

Icon plus_b = {
		b_plus, hide_plus, PLUSBPRESS
};

Icon minus_b = {
		b_minus, hide_minus, MINUSBPRESS
};

Icon start_b = {
		b_start, hide_start, STARTBPRESS
};

Icon stop_b = {
		b_stop, hide_stop, STOPBPRESS
};

Icon set_b = {
		b_set, hide_start, STARTBPRESS
};

Icon reset_b = {
		b_reset, hide_stop, STOPBPRESS
};

Icon resume_b = {
		b_resume, hide_start, STARTBPRESS
};

Icon alarm_exp_i = {
		alarm_exp_on, alarm_exp_off, NOEVENT
};

Icon timer_exp_i = {
		timer_exp_on, timer_exp_off, NOEVENT
};

Image hrs_back = {
		hrs_bkg
};

Image min_back = {
		min_bkg
};

Image sec_back = {
		sec_bkg
};

Image tts_back = {
		tts_bkg
};

Text txt = {
		&Font32x48, White
};

Image backg = {
	bkg
};

Widget MyWatchScr[NUMWIDGETS] = {
		{0, 0, 320, 240, BACKGROUND, (void *)&backg},
		{0, 0, 80, 45, ICON, (void *)&watch_b},
		{80, 0, 80, 45, ICON, (void *)&swatch_b},
		{160, 0, 80, 45, ICON, (void *)&alarm_b},
		{240, 0, 80, 45, ICON, (void *)&timer_b},
		{142, 125, 36, 35, ICON, (void *)&plus_b},
		{142, 196, 36, 35, ICON, (void *)&minus_b},
		{30, 160, 100, 40, ICON, (void *)&start_b},
		{30, 160, 100, 40, ICON, (void *)&set_b},
		{30, 160, 100, 40, ICON, (void *)&resume_b},
		{190, 160, 100, 40, ICON, (void *)&stop_b},
		{190, 160, 100, 40, ICON, (void *)&reset_b},
		{61, 114, 38, 38, ICON, (void *)&alarm_exp_i},
		{221, 114, 38, 38, ICON, (void *)&timer_exp_i},
		{29, 70, 40, 40, TEXT, (void *)&txt},
		{99, 70, 40, 40, TEXT, (void *)&txt},
		{168, 70, 40, 40, TEXT, (void *)&txt},
		{243, 70, 40, 40, TEXT, (void *)&txt},
		{80, 66, 40, 40, TEXT, (void *)&txt},
		{149, 66, 40, 40, TEXT, (void *)&txt},
		{225, 68, 20, 40, TEXT, (void *)&txt},
		{29, 70, 62, 42, IMAGE, (void *)&hrs_back},
		{99, 70, 62, 42, IMAGE, (void *)&min_back},
		{168, 70, 62, 42, IMAGE, (void *)&sec_back},
		{230, 70, 62, 42, IMAGE, (void *)&tts_back}
};

