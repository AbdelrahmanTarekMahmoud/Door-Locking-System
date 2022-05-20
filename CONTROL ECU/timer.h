/*
 * timer.h
 *
 *  Created on: Mar 1, 2022
 *      Author: abdelrahman
 */

#ifndef TIMER_H_
#define TIMER_H_

#include "std_types.h"

#define TIMER0 /* in this case we use Timer0 we can use Timer1 only by changing the Define*/
/*******************************************************************************
 *                         Types Declaration                                   *
 *******************************************************************************/
typedef enum {
	NORMAL, CTC
} Timer_Mode;
typedef enum {
	TIMER_STOP,
	NO_PRESCALER,
	CLK_8,
	CLK_64,
	CLK_256,
	CLK_1024,
	EXTERNAL_CLK_FALLING,
	EXTERNAL_CLK_RISING
} Timer_Prescaler;
typedef struct {
	Timer_Mode mode;
	Timer_Prescaler prescaler;
	uint16 initial_value;
	uint16 compare_value;
} Timer_ConfigType;
/*******************************************************************************
 *                      Functions Prototypes                                   *
 *******************************************************************************/
/*
 * Description : Function to initialize the Timer driver
 * 	1. Set the required Mode.
 * 	2. Set the required Prescaler.
 * 	3. Set the required initial value.
 * 	4. Set the required compare value.
 * 	4. Initialize Timer0,1 Registers
 */
void Timer_init(const Timer_ConfigType *Config_Ptr);
/*
 * Description: Function to set the Call Back function address (TIMER0) CASE.
 */
void timer0_SetCallBack(void (*a_ptr)(void));

/*
 * Description: Function to disable the Timer
 */
void Timer_deinit(void);

#endif /* TIMER_H_ */
