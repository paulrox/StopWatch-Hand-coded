/**
 ******************************************************************************
 * @file Event.h
 * @author Paolo Sassi
 * @date 22 January 2016
 * @brief Contains the macros used to handle the event masks.
 ******************************************************************************
 */

#ifndef EVENT_H_
#define EVENT_H_

/**
 * @addtogroup events
 * @{
 */

typedef unsigned char Event;

typedef unsigned char Events;

/**
 * @brief Sets an event in the event mask
 * @param Event: The event to be set.
 */
#define SetEvt(Event) (evts |= Event)

/**
 * @brief Resets an event in the event mask
 * @param Event: The event to be reset.
 */
#define ClearEvt(Event) (evts &= !Event)

/**
 * @brief Resets the event mask.
 */
#define ClearEvents() (evts = 0)

/**
 * @brief Checks if an event has been set.
 * @param Event: The event to be checked in the event mask.
 */
#define IsEvent(Event) ((unsigned char)(evts & Event))

/**
 * @}
 */

extern Events evts;

#endif /* EVENT_H_ */
