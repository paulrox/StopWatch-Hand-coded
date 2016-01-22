/**
 ******************************************************************************
 * @file types.h
 * @author Paolo Sassi
 * @date 22 January 2016
 * @brief Type definitions.
 ******************************************************************************
 */

#ifndef TYPES_H_
#define TYPES_H_

/**
 * @addtogroup types Types
 * @{
 */
typedef				char	char_t;
typedef signed		char	int8_t;
typedef signed		short	int16_t;
typedef unsigned 	char	uint8_t;
typedef unsigned 	short	uint16_t;
typedef 			float	float32_t;
typedef 			double 	float64_t;
typedef long		double 	float128_t;

/**
 * @brief Data structure containing timing information.
 */
typedef struct time_ {
	uint8_t hours;
	uint8_t minutes;
	uint8_t seconds;
	uint8_t	tenths;
} time;

/**
 * @}
 */

#endif /* TYPES_H_ */
