/*
 * SWatchFSM.h
 *
 *  Created on: Jan 20, 2016
 *      Author: paolosassi
 */

#ifndef SWATCHFSM_H_
#define SWATCHFSM_H_

#include "Cplus.h"
#include "stm32f4xx.h"

typedef struct time_ {
	uint8_t hours;
	uint8_t minutes;
	uint8_t seconds;
	uint8_t	tenths;
} time;

typedef enum {
	watch_b, swatch_b, alarm_b, timer_b, plus_b, minus_b, start_b, stop_b, ENTRY, EXIT, INIT, TICK, ABSENT
}Signal;

typedef enum {
	watch_showtime, watch_sethours, watch_setminutes, swatch_stop, swatch_running,
	swatch_pause, alarm_sethours, alarm_setminutes, alarm_running, timer_sethours,
	timer_setminutes, timer_setseconds, timer_running
}State;

CLASS(SWatchFSM)
	State state_;
	State swatchHistory_;
	State alarmHistory_;
	State timerHistory_;
METHODS
	void SWatchFSMinit(SWatchFSM *me);
	void SWatchFSMdispatch(SWatchFSM *me, Signal sig);
END_CLASS

#endif /* SWATCHFSM_H_ */
