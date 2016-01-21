/*
 * SWatchFSM.c
 *
 *  Created on: Jan 20, 2016
 *      Author: paolosassi
 */

#include"Cplus.h"
#include "SWatchFSM.h"
#include "ee.h"

extern void activateAlarm();
extern void activateSwatch();
extern void activateTimer();
extern void disableTimer();
extern void disableAlarm();
extern void disableSwatch();

extern uint8_t watchset, mode, swatchrun, alarm_exp, timer_exp;
extern time display_time, watch_time, swatch_time, alarm_time, timer_time;

void SWatchFSMinit(SWatchFSM *me) {
	me->state_ = watch_showtime;
	watchset = 0;
	swatchrun = 0;
	mode = 0;
	SWatchFSMdispatch(me, ENTRY);
	me->swatchHistory_ = swatch_stop;
	me->alarmHistory_ = alarm_sethours;
	me->timerHistory_ = timer_sethours;
}

static void tran_(SWatchFSM *me, State dest) {
	SWatchFSMdispatch(me, EXIT);
	me->state_ = dest;
	SWatchFSMdispatch(me, ENTRY);
}

void SWatchFSMdispatch(SWatchFSM *me, Signal sig) {
	switch(me->state_) {
/* WATCH_MODE */
	/* WATCH_SHOWTIME */
	case watch_showtime:
		switch(sig) {
		case ENTRY: mode = 0; break;
		case TICK: display_time = watch_time; break;
		case watch_b:
			watchset = 1;
			tran_(me, watch_sethours);
			break;
		case swatch_b: tran_(me, me->swatchHistory_); break;
		case alarm_b: tran_(me, me->alarmHistory_); break;
		case timer_b: tran_(me, me->timerHistory_); break;
		default: break;
		}
		break;
	/* WATCH_SETTIME */
	case watch_sethours:
		switch(sig) {
		case ENTRY: display_time.hours = watch_time.hours; break;
		case plus_b:
			watch_time.hours = (watch_time.hours + 1) % 24;
			tran_(me, watch_sethours);
			break;
		case minus_b:
			watch_time.hours = (watch_time.hours - 1) % 24;
			tran_(me, watch_sethours);
			break;
		case watch_b: tran_(me, watch_setminutes); break;
		case swatch_b: tran_(me, me->swatchHistory_); break;
		case alarm_b: tran_(me, me->alarmHistory_); break;
		case timer_b: tran_(me, me->timerHistory_); break;
		default: break;
		}
		break;
	case watch_setminutes:
		switch(sig) {
		case ENTRY: display_time.minutes = watch_time.minutes; break;
		case plus_b:
			watch_time.minutes = (watch_time.minutes + 1) % 60;
			tran_(me, watch_setminutes);
			break;
		case minus_b:
			watch_time.minutes = (watch_time.minutes - 1) % 60;
			tran_(me, watch_setminutes);
			break;
		case watch_b:
			watch_time.seconds = 0;
			watch_time.tenths = 0;
			watchset = 0;
			tran_(me, watch_showtime);
			break;
		case swatch_b: tran_(me, me->swatchHistory_); break;
		case alarm_b: tran_(me, me->alarmHistory_); break;
		case timer_b: tran_(me, me->timerHistory_); break;
		default: break;
		}
		break;
/* SWATCH_MODE */
	case swatch_stop:
		switch(sig) {
		case ENTRY:
			mode = 1;
			display_time = swatch_time;
			break;
		case start_b:
			activateSwatch();
			swatchrun = 1;
			tran_(me, swatch_running);
			break;
		case watch_b:
			me->swatchHistory_ = swatch_stop;
			tran_(me, watch_showtime);
			break;
		case alarm_b:
			me->swatchHistory_ = swatch_stop;
			tran_(me, me->alarmHistory_);
			break;
		case timer_b:
			me->swatchHistory_ = swatch_stop;
			tran_(me, me->timerHistory_);
			break;
		default: break;
		}
		break;
	case swatch_running:
		switch(sig) {
		case ENTRY:
			mode = 1;
			display_time = swatch_time;
			break;
		case TICK:
			display_time = swatch_time;
			break;
		case stop_b:
			swatchrun = 2;
			tran_(me, swatch_pause);
			break;
		case watch_b:
			me->swatchHistory_ = swatch_running;
			tran_(me, watch_showtime);
			break;
		case alarm_b:
			me->swatchHistory_ = swatch_running;
			tran_(me, me->alarmHistory_);
			break;
		case timer_b:
			me->swatchHistory_ = swatch_running;
			tran_(me, me->timerHistory_);
			break;
		default: break;
		}
		break;
	case swatch_pause:
		switch(sig) {
		case ENTRY:
			mode = 1;
			display_time = swatch_time;
			break;
		case start_b:
			swatchrun = 1;
			tran_(me, swatch_running);
			break;
		case stop_b:
			swatchrun = 0;
			disableSwatch();
			swatch_time.hours = 0;
			swatch_time.minutes = 0;
			swatch_time.seconds = 0;
			swatch_time.tenths = 0;
			tran_(me, swatch_stop);
			break;
		case watch_b:
			me->swatchHistory_ = swatch_pause;
			tran_(me, watch_showtime);
			break;
		case alarm_b:
			me->swatchHistory_ = swatch_pause;
			tran_(me, me->alarmHistory_);
			break;
		case timer_b:
			me->swatchHistory_ = swatch_pause;
			tran_(me, me->timerHistory_);
			break;
		default: break;
		}
		break;
/* ALARM_MODE */
	/* ALARM_SET */
	case alarm_sethours:
		switch(sig) {
		case ENTRY:
			display_time.hours = alarm_time.hours;
			display_time.minutes = alarm_time.minutes;
			mode = 2;
			break;
		case alarm_b: tran_(me, alarm_setminutes); break;
		case start_b:
			if ((alarm_time.hours > watch_time.hours) ||
					((alarm_time.hours == watch_time.hours) &&
					(alarm_time.minutes > watch_time.minutes))) {
				activateAlarm();
				tran_(me, alarm_running);
			}
			break;
		case plus_b:
			alarm_time.hours = (alarm_time.hours + 1) % 24;
			tran_(me, alarm_sethours);
			break;
		case minus_b:
			alarm_time.hours = (alarm_time.hours == 0) ? 23 :
								alarm_time.hours - 1;
			tran_(me, alarm_sethours);
			break;
		case watch_b:
			me->alarmHistory_ = alarm_sethours;
			tran_(me, watch_showtime);
			break;
		case swatch_b:
			me->alarmHistory_ = alarm_sethours;
			tran_(me, me->swatchHistory_);
			break;
		case timer_b:
			me->alarmHistory_ = alarm_sethours;
			tran_(me, me->timerHistory_);
			break;
		default: break;
		}
		break;
	case alarm_setminutes:
		switch(sig) {
		case ENTRY:
			mode = 2;
			display_time.hours = alarm_time.hours;
			display_time.minutes = alarm_time.minutes;
			break;
		case alarm_b: tran_(me, alarm_sethours); break;
		case start_b:
			if ((alarm_time.hours > watch_time.hours) ||
					((alarm_time.hours == watch_time.hours) &&
					(alarm_time.minutes > watch_time.minutes))) {
				activateAlarm();
				tran_(me, alarm_running);
			}
			break;
		case plus_b:
			alarm_time.minutes = (alarm_time.minutes + 1) % 60;
			tran_(me, alarm_setminutes);
			break;
		case minus_b:
			alarm_time.minutes = (alarm_time.minutes == 0) ? 59 :
								  alarm_time.minutes - 1;
			tran_(me, alarm_setminutes);
			break;
		case watch_b:
			me->alarmHistory_ = alarm_setminutes;
			tran_(me, watch_showtime);
			break;
		case swatch_b:
			me->alarmHistory_ = alarm_setminutes;
			tran_(me, me->swatchHistory_);
			break;
		case timer_b:
			me->alarmHistory_ = alarm_setminutes;
			tran_(me, me->timerHistory_);
			break;
		default: break;
		}
		break;
	/* ALARM_RUNNIG */
	case alarm_running:
		switch(sig) {
		case ENTRY:
			mode = 2;
			display_time.hours = alarm_time.hours;
			display_time.minutes = alarm_time.minutes;
			break;
		case stop_b:
			alarm_time.hours = 0;
			alarm_time.minutes = 0;
			alarm_exp = 0;
			disableAlarm();
			tran_(me, alarm_sethours);
			break;
		case watch_b:
			me->alarmHistory_ = alarm_running;
			tran_(me, watch_showtime);
			break;
		case swatch_b:
			me->alarmHistory_ = alarm_running;
			tran_(me, me->swatchHistory_);
			break;
		case timer_b:
			me->alarmHistory_ = alarm_running;
			tran_(me, me->timerHistory_);
			break;
		}
		break;
/* TIMER_MODE */
	/* TIMER_SET */
	case timer_sethours:
		switch(sig) {
		case ENTRY:
			mode = 3;
			display_time.hours = timer_time.hours;
			display_time.minutes = timer_time.minutes;
			display_time.seconds = timer_time.seconds;
			break;
		case stop_b:
			timer_time.hours = 0;
			timer_time.minutes = 0;
			timer_time.seconds = 0;
			timer_time.tenths = 0;
			tran_(me, timer_sethours);
			break;
		case start_b:
			if ((timer_time.hours * 24 + timer_time.minutes * 60 +
					timer_time.seconds) > 0) {
				tran_(me, timer_running);
				activateTimer();
			}
			break;
		case plus_b:
			timer_time.hours = (timer_time.hours + 1) % 24;
			tran_(me, timer_sethours);
			break;
		case minus_b:
			timer_time.hours = (timer_time.hours - 1) % 24;
			tran_(me, timer_sethours);
			break;
		case timer_b:
			tran_(me, timer_setminutes);
			break;
		case watch_b:
			me->timerHistory_ = timer_sethours;
			tran_(me, watch_showtime);
			break;
		case swatch_b:
			me->timerHistory_ = timer_sethours;
			tran_(me, me->swatchHistory_);
			break;
		case alarm_b:
			me->timerHistory_ = timer_sethours;
			tran_(me, me->alarmHistory_);
			break;
		default: break;
		}
		break;
	case timer_setminutes:
		switch(sig) {
		case ENTRY:
			mode = 3;
			display_time.hours = timer_time.hours;
			display_time.minutes = timer_time.minutes;
			display_time.seconds = timer_time.seconds;
			break;
		case stop_b:
			timer_time.hours = 0;
			timer_time.minutes = 0;
			timer_time.seconds = 0;
			timer_time.tenths = 0;
			tran_(me, timer_sethours);
			break;
		case start_b:
			if ((timer_time.hours * 24 + timer_time.minutes * 60 +
					timer_time.seconds) > 0) {
				tran_(me, timer_running);
				activateTimer();
			}
			break;
		case plus_b:
			timer_time.minutes = (timer_time.minutes + 1) % 60;
			tran_(me, timer_setminutes);
			break;
		case minus_b:
			timer_time.minutes = (timer_time.minutes == 0) ? 59:
								 (timer_time.minutes - 1) % 60;
			tran_(me, timer_setminutes);
			break;
		case timer_b:
			tran_(me, timer_setseconds);
			break;
		case watch_b:
			me->timerHistory_ = timer_sethours;
			tran_(me, watch_showtime);
			break;
		case swatch_b:
			me->timerHistory_ = timer_sethours;
			tran_(me, me->swatchHistory_);
			break;
		case alarm_b:
			me->timerHistory_ = timer_sethours;
			tran_(me, me->alarmHistory_);
			break;
		default: break;
		}
		break;
	case timer_setseconds:
		switch(sig) {
		case ENTRY:
			mode = 3;
			display_time.hours = timer_time.hours;
			display_time.minutes = timer_time.minutes;
			display_time.seconds = timer_time.seconds;
			break;
		case stop_b:
			timer_time.hours = 0;
			timer_time.minutes = 0;
			timer_time.seconds = 0;
			timer_time.tenths = 0;
			tran_(me, timer_sethours);
			break;
		case start_b:
			if ((timer_time.hours * 24 + timer_time.minutes * 60 +
					timer_time.seconds) > 0) {
				activateTimer();
				tran_(me, timer_running);
			}
			break;
		case plus_b:
			timer_time.seconds = (timer_time.seconds + 1) % 60;
			tran_(me, timer_setseconds);
			break;
		case minus_b:
			timer_time.seconds = (timer_time.seconds == 0) ? 59:
					 	 	 	 timer_time.seconds - 1;
			tran_(me, timer_setseconds);
			break;
		case timer_b:
			tran_(me, timer_sethours);
			break;
		case watch_b:
			me->timerHistory_ = timer_sethours;
			tran_(me, watch_showtime);
			break;
		case swatch_b:
			me->timerHistory_ = timer_sethours;
			tran_(me, me->swatchHistory_);
			break;
		case alarm_b:
			me->timerHistory_ = timer_sethours;
			tran_(me, me->alarmHistory_);
			break;
		default: break;
		}
		break;
	/* TIMER_RUNNING */
	case timer_running:
		switch(sig) {
		case ENTRY:
			mode = 3;
			display_time.hours = timer_time.hours;
			display_time.minutes = timer_time.minutes;
			display_time.seconds = timer_time.seconds;
			break;
		case TICK:
			display_time.hours = timer_time.hours;
			display_time.minutes = timer_time.minutes;
			display_time.seconds = timer_time.seconds;
			break;
		case stop_b:
			disableTimer();
			timer_time.hours = 0;
			timer_time.minutes = 0;
			timer_time.seconds = 0;
			timer_time.tenths = 0;
			timer_exp = 0;
			break;
		}
		break;
	default: break;
	}
}


