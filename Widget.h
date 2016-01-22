/**
 ******************************************************************************
 * @file Widget.h
 * @author Paolo Sassi
 * @date 22 January 2016
 * @brief Contains the type definitions and the macros used for the
 * screen widgets.
 ******************************************************************************
 */

#ifndef WIDGET_H_
#define WIDGET_H_

#include "Event.h"
#include "Touch.h"
#include "fonts.h"

/**
 * @addtogroup widget
 * @{
 */
#define NUMWIDGETS 25

#define BAKCG		0
#define BWATCH		1
#define BSWATCH		2
#define BALARM 		3
#define BTIMER 		4
#define BPLUS 		5
#define BMINUS 		6
#define BSTART 		7
#define BSET		8
#define BRESUME		9
#define BSTOP  		10
#define BRESET		11
#define ALARMEXP	12
#define TIMEREXP	13
#define HRSSTR 		14
#define MINSTR 		15
#define SECSTR 		16
#define TTSSTR 		17
#define SEP1STR 	18
#define SEP2STR 	19
#define TTSSEP		20
#define HRSBKG		21
#define MINBKG		22
#define SECBKG		23
#define TTSBKG		24


#define NOEVENT		0x00
#define WATCHBPRESS 0x01
#define SWATCHBPRESS 0x02
#define ALARMBPRESS 0x04
#define TIMERBPRESS 0x08
#define PLUSBPRESS 0x10
#define MINUSBPRESS 0x20
#define STARTBPRESS 0x40
#define STOPBPRESS 0x80

#define WATCHMODE	0
#define	SWATCHMODE	1
#define	ALARMMODE	2
#define	TIMERMODE	3

typedef enum  {
	BACKGROUND, ICON, TEXT, IMAGE
} WidgetType;

typedef struct {
	unsigned char *image;
} Image;

typedef struct {
	unsigned char *iconp;
	unsigned char *iconr;
	Event		  onpress;
} Icon;

typedef struct {
	sFONT *font;
	unsigned short int	color;
} Text;

typedef struct {
	unsigned short int xl;
	unsigned short int yt;
	unsigned short int xw;
	unsigned short int yh;
	WidgetType wt;
	void     *ws;
} Widget;

#define txtinfo(w) ((Text *)((w)->ws))
#define iconinfo(w) ((Icon *)((w)->ws))
#define imginfo(w) ((Image *)((w)->ws))

/**
 * @}
 */

extern Widget MyWatchScr[];

void DrawInit(Widget ws[]);
unsigned char OnTouch(const Widget ws[], TPoint *press);
unsigned char DrawOn(Widget *w);
unsigned char DrawOff(Widget *w);
unsigned char WPrint(Widget *w, char *s);

#endif /* BUTTON_H_ */
