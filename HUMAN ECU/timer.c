/*
 * timer.c
 *
 *  Created on: Mar 1, 2022
 *      Author: abdelrahman
 */

#include"timer.h"
#include"common_macros.h"
#include "avr/io.h"
#include "avr/interrupt.h"
/*******************************************************************************
 *                           Global Variables                                  *
 *******************************************************************************/

static volatile void (*g_timer0CallBackPtr)(void) = NULL_PTR;

/*******************************************************************************
 *                       Interrupt Service Routines                            *
 *******************************************************************************/

ISR(TIMER0_OVF_vect) {
	if (g_timer0CallBackPtr != NULL_PTR) {

		(*g_timer0CallBackPtr)(); /* another method to call the function using pointer to function g_callBackPtr(); */
	}
}
ISR(TIMER0_COMP_vect) {
	if (g_timer0CallBackPtr != NULL_PTR) {

		(*g_timer0CallBackPtr)(); /* another method to call the function using pointer to function g_callBackPtr(); */
	}
}

/*******************************************************************************
 *                      Functions Definitions                                  *
 *******************************************************************************/

/*
 * Description : Function to initialize the Timer driver
 * 	1. Set the required Mode.
 * 	2. Set the required Prescaler.
 * 	3. Set the required initial value.	TCCR1A = (1<<FOC1A) | (1<<FOC1B);
 *
 * 	4. Set the required compare value.
 * 	4. Initialize Timer0,1 Registers
 *
 */
void Timer_init(const Timer_ConfigType *Config_Ptr) {
	SET_BIT(TCCR0, FOC0); /* Set because its Non_PWM mode */
	TCNT0 = (Config_Ptr->initial_value);
	TCCR0 = (TCCR0 & 0xF8) | (Config_Ptr->prescaler);
	if (Config_Ptr->mode == NORMAL) {
		/*
		 * WGM01:0 = 0
		 * COM01:00 = 0
		 * OCR0 =0
		 */
		TIMSK = (1 << TOIE0);
		TCCR0 &= ~(1 << WGM01) & ~(1 << WGM00);
	} else if (Config_Ptr->mode == CTC) {
		/*
		 * WGM00 = 0
		 * COM01:00 = 0
		 */
		TCCR0 |= (1 << WGM01);
		OCR0 = (Config_Ptr->compare_value);
		TIMSK = (1 << OCIE0);
	}
}

/*
 * Description: Function to set the Call Back function address (TIMER0) CASE.
 */
void timer0_SetCallBack(void (*a_ptr)(void)) {
	g_timer0CallBackPtr = a_ptr;
}

/*
 * Description: Function to disable the Timer
 */
void Timer_deinit(void) {
	TCCR0 = 0;
	OCR0 = 0;
	TIMSK &= ~(1 << OCIE0);
	TIMSK &= ~(1 << TOIE0);
}

