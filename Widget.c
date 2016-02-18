/**
 ******************************************************************************
 * @file Widget.c
 * @author Paolo Sassi
 * @date 22 January 2016
 * @brief Contains the functions to manage the widgets on the screen.
 ******************************************************************************
 */

#include "Widget.h"
#include "Event.h"
#include "mypictures.h"
#include <stdio.h>
#include "stm32f4_discovery_lcd.h"

/**
 * @defgroup widget Widget
 * @{
 */
/**
 * @defgroup defwidget Widget Definitions
 * @{
 */
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

/**
 * @brief This array contains all the widgets defined for
 * the application.
 */
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

/**
 * @}
 */

/**
 * @brief Checks if the touched point is inside a widget.
 * @param w: Pointer to the widget.
 * @param point: Pointer to the coordinates data structure.
 * @retval 1: The point is inside the widget.
 * @retval 0: The point is outside the widget.
 */
unsigned char contains(Widget *w, TPoint *point){
	if	((point->x >= w->xl) && (point->x <= w->xl + w->xw) &&
		 (point->y >= w->yt) && (point->y <= w->yt + w->yh)) {
		return 1;
	}
	else
		return 0;
}

/**
 * @brief Handles the touch event.
 * @param ws: Pointer to the application widgets array.
 * @param press: Pointer to the coordinates data structure.
 * @retval 1: The touched point is inside one application widget
 * @retval 0: No widget in the application contains the touched point.
 *
 * This function scans the entire widget array defined for the application
 * and for each of them checks whether the coordinates of the touched point
 * are inside the widget.
 */
unsigned char OnTouch(const Widget ws[], TPoint *press){
unsigned char i, res;

	res = 0;
	for(i=0; i<NUMWIDGETS; i++) {
		if (ws[i].wt == ICON) {
			if(contains(&ws[i], press) && iconinfo(&ws[i])->onpress != NOEVENT) {
				if (ws[i].wt == ICON) {
					SetEvt(iconinfo(&ws[i])->onpress);
				}
 			    res = 1;
 			}
		}
	}
	return res;
}

/**
 * @brief Draws the initial GUI of the application.
 * @param ws: Pointer to the application widgets array.
 * @retval None.
 */
void DrawInit(Widget ws[])
{
unsigned char i;

	for(i=0; i<NUMWIDGETS; i++) {
		DrawOff(&ws[i]);
	}
	WPrint(&ws[SEP1STR], ":");
	WPrint(&ws[SEP2STR], ":");
}

/**
 * @brief Draws the 'on' image of a widget.
 * @param w: Pointer to the widget structure.
 * @retval 1: The image was successfully drawn on the screen.
 * @retval 0: Unable to draw the image.
 */
unsigned char DrawOn(Widget *w)
{
char *imgptr = 0;

	switch (w->wt) {
	case ICON:
		imgptr = iconinfo(w)->iconp;
		break;
	case BACKGROUND:
	case IMAGE:
		imgptr = imginfo(w)->image;
		break;
	}
	if (imgptr != 0) {
		LCD_DrawPicture(w->xl, w->yt, w->xw, w->yh, imgptr);
		return 1;
	} else
		return 0;
}


/**
 * @brief Draws the 'off' image of a widget.
 * @param w: Pointer to the widget structure.
 * @retval 1: The image was successfully drawn on the screen.
 * @retval 0: Unable to draw the image.
 */
unsigned char DrawOff(Widget *w)
{
char *imgptr = 0;

	switch (w->wt) {
	case ICON:
		imgptr = iconinfo(w)->iconr;
		break;
	case BACKGROUND:
	case IMAGE:
		imgptr = imginfo(w)->image;
		break;
	}
	if (imgptr != 0) {
		LCD_DrawPicture(w->xl, w->yt, w->xw, w->yh, imgptr);
		return 1;
	}
		return 0;
}

/**
 * @brief Prints a string on the screen.
 * @param w: Pointer to the widget data structure.
 * @param s: Pointer to the string which have to be printed.
 */
unsigned char WPrint(Widget *w, char *s)
{
	if (w->wt == TEXT) {
		LCD_SetTextColor(txtinfo(w)->color);
		LCD_SetFont(txtinfo(w)->font);
		LCD_DisplayStringXY(w->xl, w->yt, s);
		return 1;
	} else
		return 0;
}

/**
 * @}
 */
