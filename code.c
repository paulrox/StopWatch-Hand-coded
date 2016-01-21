/* ###*B*###
 * ERIKA Enterprise - a tiny RTOS for small microcontrollers
 *
 * Copyright (C) 2002-2013  Evidence Srl
 *
 * This file is part of ERIKA Enterprise.
 *
 * ERIKA Enterprise is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General Public License
 * version 2 as published by the Free Software Foundation,
 * (with a special exception described below).
 *
 * Linking this code statically or dynamically with other modules is
 * making a combined work based on this code.  Thus, the terms and
 * conditions of the GNU General Public License cover the whole
 * combination.
 *
 * As a special exception, the copyright holders of this library give you
 * permission to link this code with independent modules to produce an
 * executable, regardless of the license terms of these independent
 * modules, and to copy and distribute the resulting executable under
 * terms of your choice, provided that you also meet, for each linked
 * independent module, the terms and conditions of the license of that
 * module.  An independent module is a module which is not derived from
 * or based on this library.  If you modify this code, you may extend
 * this exception to your version of the code, but you are not
 * obligated to do so.  If you do not wish to do so, delete this
 * exception statement from your version.
 *
 * ERIKA Enterprise is distributed in the hope that it will be
 * useful, but WITHOUT ANY WARRANTY; without even the implied warranty
 * of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License version 2 for more details.
 *
 * You should have received a copy of the GNU General Public License
 * version 2 along with ERIKA Enterprise; if not, write to the
 * Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301 USA.
 * ###*E*### */

#include "ee.h"
#include "ee_irq.h"
#include <stdio.h>
#include "stm32f4xx_conf.h"
#include "stm32f4_discovery.h"
#include "stm32f4_discovery_lcd.h"
#include "stm32f4xx.h"

#include "STMPE811QTR.h"
#include "mypictures.h"
#include "Widget.h"
#include "WidgetConfig.h"
#include "Touch.h"
#include "Event.h"
#include "lcd_add.h"
#include "fonts.h"
#include "debug.h"
#include "SWatchFSM.h"

static void strencode1digit(char *str, int digit);
static void strencode2digit(char *str, int digit);

void activateAlarm();
void activateTimer();
void activateSwatch();

static SWatchFSM watch;

uint8_t mode, alarm_exp, timer_exp, swatchrun, watchset;
time display_time, watch_time, swatch_time, alarm_time, timer_time;

/*
 * SysTick ISR2
 */
ISR2(systick_handler)
{
	/* count the interrupts, waking up expired alarms */
	CounterTick(myCounter);
}

void activateSwatch() {
	SetRelAlarm(AlarmTaskSwatch, 10, 100);
}

void activateAlarm() {
	SetRelAlarm(AlarmTaskAlarm, 10, 100);
}

void activateTimer() {
	SetRelAlarm(AlarmTaskTimer, 10, 100);
}

void disableAlarm() {
	CancelAlarm(AlarmTaskAlarm);
}

void disableTimer() {
	CancelAlarm(AlarmTaskTimer);
}

void disableSwatch() {
	CancelAlarm(AlarmTaskSwatch);
}

/*
 * TASK LCD
 */

TASK(TaskLCD)
{
unsigned int px, py;
TPoint p;

	if (GetTouch_SC_Async(&px, &py)) {
		p.x = px;
		p.y = py;
		OnTouch(MyWatchScr, &p);
	}
}

TASK(TaskWatch)
{
time t;
int tenths_tot;

	t = watch_time;
	t.tenths = (t.tenths + 1) % 10;
	if (t.tenths == 0) {
		t.seconds = (t.seconds + 1) % 60;
		if (t.seconds == 0) {
			t.minutes = (t.minutes + 1) % 60;
			if (t.minutes == 0) {
				t.hours = (t.hours + 1) % 24;
			}
		}
	}
	watch_time = t;
}

TASK(TaskSwatch)
{
time t;
	if (swatchrun == 1) {
		t = swatch_time;
		t.tenths = (t.tenths + 1) % 10;
		if (t.tenths == 0) {
			t.seconds = (t.seconds + 1) % 60;
			if (t.seconds == 0) {
				t.minutes = (t.minutes + 1) % 60;
				if (t.minutes == 0) {
					t.hours = (t.hours + 1) % 24;
				}
			}
		}
		swatch_time = t;
	}
}

TASK(TaskAlarm)
{
	if ((watch_time.hours == alarm_time.hours) &&
		(watch_time.minutes == alarm_time.minutes)) {
		alarm_exp = 1;
		CancelAlarm(AlarmTaskAlarm);
	}

}

TASK(TaskTimer)
{
time t;
int tenths_tot;

	t.hours = 0;
	t.minutes = 0;
	t.seconds = 0;
	t.tenths = 0;
	tenths_tot = (timer_time.hours * 60 * 60 * 10) + (timer_time.minutes * 60 * 10) +
			(timer_time.seconds * 10) + timer_time.tenths;
	tenths_tot = tenths_tot - 1;

	if (tenths_tot == 0) {
		timer_exp = 1;
		CancelAlarm(AlarmTaskTimer);
	} else {
		while (tenths_tot >= 36000) {
			tenths_tot -= 36000;
			t.hours++;
		}
		while (tenths_tot >= 600) {
			tenths_tot -= 600;
			t.minutes++;
		}
		while (tenths_tot >= 10) {
			tenths_tot -= 10;
			t.seconds++;
		}
		t.tenths = tenths_tot;
	}
}

unsigned char IsUpdateTime()
{
unsigned char res;
static unsigned char oh=0, om=0, os=0;

	if (display_time.hours!=oh || display_time.minutes!=om ||
			display_time.seconds!= os)
		res = 1;
	else
		res = 0;
	oh = display_time.hours;
	om = display_time.minutes;
	os = display_time.seconds;
	return res;
}

void updateScreen(unsigned char om, unsigned char m)
{
char tstr[3];

	switch (om) {
	case WATCHMODE:
		/* avoid overwriting the same picture */
		if (m != WATCHMODE) DrawOff(&MyWatchScr[BWATCH]);
		DrawOff(&MyWatchScr[BPLUS]);
		DrawOff(&MyWatchScr[BMINUS]);
		break;
	case SWATCHMODE:
		DrawOff(&MyWatchScr[BSWATCH]);
		DrawOff(&MyWatchScr[BSTART]);
		DrawOff(&MyWatchScr[BSTOP]);
		break;
	case ALARMMODE : case TIMERMODE:
		DrawOff(&MyWatchScr[BSET]);
		DrawOff(&MyWatchScr[BRESET]);
		DrawOff(&MyWatchScr[BPLUS]);
		DrawOff(&MyWatchScr[BMINUS]);
		if (om == ALARMMODE) {
			DrawOff(&MyWatchScr[BALARM]);
			strencode2digit(tstr, (int)display_time.seconds);
			WPrint(&MyWatchScr[SECSTR], tstr);
		} else {
			DrawOff(&MyWatchScr[BTIMER]);
		}
		break;
	default:
		break;
	}
	switch (m) {
	case WATCHMODE:
		/* avoid overwriting the same picture */
		if (om != WATCHMODE) {
			DrawOn(&MyWatchScr[BWATCH]);
			DrawOn(&MyWatchScr[TTSBKG]);
		}
		if (watchset) {
			DrawOn(&MyWatchScr[BPLUS]);
			DrawOn(&MyWatchScr[BMINUS]);
		}
		break;
	case SWATCHMODE:
		DrawOn(&MyWatchScr[BSWATCH]);
		DrawOn(&MyWatchScr[BSTART]);
		if (swatchrun <= 1) {
			DrawOn(&MyWatchScr[BSTOP]);
		} else {
			DrawOn(&MyWatchScr[BRESET]);
		}
		break;
	case ALARMMODE: case TIMERMODE:
		DrawOn(&MyWatchScr[BSET]);
		DrawOn(&MyWatchScr[BRESET]);
		DrawOn(&MyWatchScr[BPLUS]);
		DrawOn(&MyWatchScr[BMINUS]);
		DrawOn(&MyWatchScr[TTSBKG]);
		if (m == ALARMMODE) {
			DrawOn(&MyWatchScr[BALARM]);
			DrawOn(&MyWatchScr[SECBKG]);
		} else {
			DrawOn(&MyWatchScr[BTIMER]);
		}
		break;
	default:
		break;
	}
}

void strencode2digit(char *str, int digit)
{
	str[2]=0;
	str[0]=digit/10+'0';
	str[1]=digit%10+'0';
}

static void strencode1digit(char *str, int digit)
{
	str[1] = 0;
	str[0] = digit + '0';
}

TASK(TaskFSM)
{
unsigned char i;
static int oldmode=8;
static int oldswatchrun = 10;
static int oldwatchset = 2;
static int oldalarm = 2;
static int oldtimer = 2;
static unsigned char oh=99, om=99, os=99, ot=99;
char tstr[3];
Signal s;

	s = TICK;

	SWatchFSMdispatch(&watch, s);

	s = ABSENT;

	if (IsEvent(WATCHBPRESS))	s = watch_b;
	if (IsEvent(SWATCHBPRESS))	s = swatch_b;
	if (IsEvent(ALARMBPRESS))	s = alarm_b;
	if (IsEvent(TIMERBPRESS))	s = timer_b;
	if (IsEvent(PLUSBPRESS))	s = plus_b;
	if (IsEvent(MINUSBPRESS))	s = minus_b;
	if (IsEvent(STARTBPRESS))	s = start_b;
	if (IsEvent(STOPBPRESS))	s = stop_b;

	SWatchFSMdispatch(&watch, s);

	ClearEvents();

	if (display_time.hours!=oh) {
		strencode2digit(tstr, (int)display_time.hours);
		DrawOn(&MyWatchScr[HRSBKG]);
		WPrint(&MyWatchScr[HRSSTR], tstr);
		oh=display_time.hours;
	}
	if (display_time.minutes!=om) {
		strencode2digit(tstr, (int)display_time.minutes);
		DrawOn(&MyWatchScr[MINBKG]);
		WPrint(&MyWatchScr[MINSTR], tstr);
		om=display_time.minutes;
	}
	if (display_time.seconds!= os) {
		strencode2digit(tstr, (int)display_time.seconds);
		DrawOn(&MyWatchScr[SECBKG]);
		WPrint(&MyWatchScr[SECSTR], tstr);
		os=display_time.seconds;
	}
	if ((display_time.tenths != ot && mode == SWATCHMODE) || (oldmode != SWATCHMODE && mode == SWATCHMODE)) {
		strencode1digit(tstr, (int)display_time.tenths);
		DrawOn(&MyWatchScr[TTSBKG]);
		WPrint(&MyWatchScr[TTSSEP], ".");
		WPrint(&MyWatchScr[TTSSTR], tstr);
		ot=display_time.tenths;
	}

	if (oldalarm != alarm_exp) {
		if (alarm_exp == 1) {
			DrawOn(&MyWatchScr[ALARMEXP]);
		} else {
			DrawOff(&MyWatchScr[ALARMEXP]);
		}
		oldalarm = alarm_exp;
	}

	if (oldtimer != timer_exp) {
			if (timer_exp == 1) {
				DrawOn(&MyWatchScr[TIMEREXP]);
			} else {
				DrawOff(&MyWatchScr[TIMEREXP]);
			}
			oldtimer = timer_exp;
		}

	if (oldmode != mode || oldswatchrun != swatchrun || oldwatchset != watchset) {
	updateScreen(oldmode, mode);
	oldmode = mode;
	oldswatchrun = swatchrun;
	oldwatchset = watchset;
	}
}

/*
 * MAIN TASK
 */
int main(void)
{

	SystemInit();
	/*Initializes Erika related stuffs*/
	EE_system_init();

	/* Initialize state machine */

	SWatchFSMinit(&watch);

	/*Initialize systick */
	EE_systick_set_period(MILLISECONDS_TO_TICKS(1, SystemCoreClock));
	EE_systick_enable_int();
	EE_systick_start();

	/* Initializes LCD and touchscreen */
	IOE_Config();
	/* Initialize the LCD */
	STM32f4_Discovery_LCD_Init();

	InitTouch(-0.0853, 0.0665, -331, 15);

	/* Draw the background */
	DrawInit(MyWatchScr);

	/* Program cyclic alarms which will fire after an initial offset,
	 * and after that periodically
	 * */
	SetRelAlarm(AlarmTaskLCD, 10, 100);
	SetRelAlarm(AlarmTaskFSM, 10, 100);
	SetRelAlarm(AlarmTaskWatch, 10, 100);

  /* Forever loop: background activities (if any) should go here */
	for (;;) { }
}


