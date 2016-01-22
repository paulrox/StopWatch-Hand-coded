/**
 ******************************************************************************
 * @file code.c
 * @author Paolo Sassi
 * @date 21 January 2016
 * @brief Contains the body of all tasks and the global
 *  variables defined.
 ******************************************************************************
 * @attention
 *
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
 ******************************************************************************
 */

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
#include "Touch.h"
#include "Event.h"
#include "lcd_add.h"
#include "fonts.h"
#include "types.h"
#include "SWatchFSM.h"

/**
 * @brief Application mode.
 */
uint8_t mode = 0;

/**
 * @brief Alarm status.
 * 0: Alarm not set yet.
 * 1: Alarm set.
 * 2: Alarm expired.
 */
uint8_t alarm_status = 0;

/**
 * 1 if the timer is expired, 0 otherwise.
 */
uint8_t timer_exp = 0;
uint8_t swatchrun = 0;
uint8_t watchset = 0;
uint8_t alarm_cycle = 200; /* 20 seconds */

/* time data structures */
time display_time;
time watch_time;
time swatch_time;
time alarm_time;
time timer_time;

static SWatchFSM watch;

/** @defgroup utility Utility
 * @{
 */

/**
 * @brief Converts a one digit integer into a string
 * @param str: pointer to the returning string.
 * @param digit: integer digit to be converted.
 * @retval None
 */
static void strencode1digit(char *str, int digit)
{
	str[1] = 0;
	str[0] = digit + '0';
}

/**
 * @brief Converts a two digits integer into a string
 * @param str: pointer to the returning string.
 * @param digit: integer digits to be converted.
 * @retval None
 */
static void strencode2digit(char *str, int digit)
{
	str[2]=0;
	str[0]=digit/10+'0';
	str[1]=digit%10+'0';
}

/**
 * @brief Activates the Stopwatch task.
 * @param None
 * @retval None
 */
void activateSwatch()
{
	SetRelAlarm(AlarmTaskSwatch, 10, 100);
}

/**
 * @brief Activates the Alarm task.
 * @param None
 * @retval None
 */
void activateAlarm()
{
	SetRelAlarm(AlarmTaskAlarm, 10, 100);
}

/**
 * @brief Activates the Timer task.
 * @param None
 * @retval None
 */
void activateTimer()
{
	SetRelAlarm(AlarmTaskTimer, 10, 100);
}

/**
 * @brief Terminates the Alarm task.
 * @param None
 * @retval None
 */
void disableAlarm()
{
	CancelAlarm(AlarmTaskAlarm);
}

/**
 * @brief Terminates the Timer task.
 * @param None
 * @retval None
 */
void disableTimer()
{
	CancelAlarm(AlarmTaskTimer);
}

/**
 * @brief Terminates the Stopwatch task.
 * @param None
 * @retval None
 *
 */
void disableSwatch()
{
	CancelAlarm(AlarmTaskSwatch);
}

/**
 * @brief Updates the time on the screen.
 * @param oh: Old hours.
 * @param om: Old minutes.
 * @param os: Old seconds.
 * @param ot: Old tenths.
 * @param oldmode: Old application mode.
 * @retval None
 */
static void updateTime(uint8_t *oh, uint8_t *om, uint8_t *os, uint8_t *ot,
		uint8_t oldmode)
{
char tstr[3];

	if (display_time.hours != *oh) {
		strencode2digit(tstr, (int)display_time.hours);
		DrawOn(&MyWatchScr[HRSBKG]);
		WPrint(&MyWatchScr[HRSSTR], tstr);
		*oh=display_time.hours;
	}
	if (display_time.minutes != *om) {
		strencode2digit(tstr, (int)display_time.minutes);
		DrawOn(&MyWatchScr[MINBKG]);
		WPrint(&MyWatchScr[MINSTR], tstr);
		*om=display_time.minutes;
	}
	if (display_time.seconds != *os) {
		strencode2digit(tstr, (int)display_time.seconds);
		DrawOn(&MyWatchScr[SECBKG]);
		WPrint(&MyWatchScr[SECSTR], tstr);
		*os=display_time.seconds;
	}
	if ((display_time.tenths != *ot && mode == SWATCHMODE) ||
		(oldmode != SWATCHMODE && mode == SWATCHMODE)) {
		strencode1digit(tstr, (int)display_time.tenths);
		DrawOn(&MyWatchScr[TTSBKG]);
		WPrint(&MyWatchScr[TTSSEP], ".");
		WPrint(&MyWatchScr[TTSSTR], tstr);
		*ot=display_time.tenths;
	}
}

/**
 * @brief Updates the screen widgets.
 * @param om: Old application mode.
 * @param m: New application mode.
 * @retval None
 */
void updateScreen(uint8_t om, uint8_t m)
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
/**
 * @}
 */

/**
 * @defgroup isr Interrupt Handler
 * @{
 */
/**
 * @brief System Tick interrupt handler
 */
ISR2(systick_handler)
{
	/* count the interrupts, waking up expired alarms */
	CounterTick(myCounter);
}

/**
 * @}
 */

/** @defgroup Tasks
 *  @{
 */
/**
 * 	@brief LDC task body.
 *
 * 	This task is periodically activated in order to
 * 	get the touch events.
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

/**
 * 	@brief Implements the watch mode.
 *
 */
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

/**
 * @brief Implements the Stopwatch mode.
 *
 * This task is activated by the FSM when the Stopwatch
 * is started.
 */
TASK(TaskSwatch)
{
time t;
	/* If the Stopwatch is in the running state */
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

/**
 * @brief Implements the Alarm mode.
 *
 * This task is activated by the FSM when the alarm
 * time is set.
 */
TASK(TaskAlarm)
{
	if ((watch_time.hours == alarm_time.hours) &&
		(watch_time.minutes == alarm_time.minutes)) {
		if (alarm_cycle != 0) {
			/* In order to make the alarm icon blink when it
			 * expires, we change the alarm_status value from 1 to 2
			 * and vice versa for 200 tenths
			 */
			alarm_cycle--;
			alarm_status = (alarm_status == 1) ? 2: 1;
		} else {
			alarm_status = 0;
			CancelAlarm(AlarmTaskAlarm);
		}
	}
}

/**
 * @brief Implements the Timer mode.
 *
 * This task is activated by the FSM when the timer
 * is started.
 */
TASK(TaskTimer)
{
time t;
int tenths_tot;

	t.hours = 0;
	t.minutes = 0;
	t.seconds = 0;
	t.tenths = 0;
	tenths_tot = (timer_time.hours * 60 * 60 * 10) + (timer_time.minutes
			* 60 * 10) + (timer_time.seconds * 10) + timer_time.tenths;
	tenths_tot--;

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
	timer_time = t;
}

/**
 * @brief Implements the State Machine of the application
 *
 * This task checks whether an event has occurred and dispatches
 * the right signal to the FSM.
 */
TASK(TaskFSM)
{
static uint8_t oldmode=8;
static uint8_t oldswatchrun = 10;
static uint8_t oldwatchset = 2;
static uint8_t oldalarm = 3;
static uint8_t oldtimer = 2;
static uint8_t oh=99, om=99, os=99, ot=99;
Signal s;

	/* We always dispatch the TICK event in order to
	 * update the screen digits
	 */
	s = TICK;
	SWatchFSMdispatch(&watch, s);

	/* If no button presses are detected, we dispatch
	 * and empty event.
	 */
	s = ABSENT;

	/* Checks the button presses */
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

	/* Updates the displayed time */
	updateTime(&oh, &om, &os, &ot, oldmode);

	/* Updates alarm and timer status */
	if (oldalarm != alarm_status) {
		if (alarm_status == 1) {
			DrawOn(&MyWatchScr[ALARMEXP]);
		} else {
			DrawOff(&MyWatchScr[ALARMEXP]);
		}
		oldalarm = alarm_status;
	}
	if (oldtimer != timer_exp) {
			if (timer_exp == 1) {
				DrawOn(&MyWatchScr[TIMEREXP]);
			} else {
				DrawOff(&MyWatchScr[TIMEREXP]);
			}
			oldtimer = timer_exp;
	}

	/* Checks if the application mode has changed or not */
	if (oldmode != mode || oldswatchrun != swatchrun ||
			oldwatchset != watchset) {
		updateScreen(oldmode, mode);
		oldmode = mode;
		oldswatchrun = swatchrun;
		oldwatchset = watchset;
	}
}

/**
 * @brief Main task of the application
 * @param None
 * @retval None This function should never return.
 */
int main(void)
{
	/* Initializes the system */
	SystemInit();
	EE_system_init();
	EE_systick_set_period(MILLISECONDS_TO_TICKS(1, SystemCoreClock));
	EE_systick_enable_int();
	EE_systick_start();

	/* Initializes LCD and touchscreen */
	IOE_Config();
	STM32f4_Discovery_LCD_Init();
	InitTouch(-0.0853, 0.0665, -331, 15);
	DrawInit(MyWatchScr);

	/* Initializes the FSM */
	SWatchFSMinit(&watch);

	/* Initial task set */
	SetRelAlarm(AlarmTaskLCD, 10, 100);
	SetRelAlarm(AlarmTaskFSM, 10, 100);
	SetRelAlarm(AlarmTaskWatch, 10, 100);

	/* Forever loop: background activities (if any) should go here */
	for (;;) { }
}
/**
 * @}
 */
