/*
 * DCMOTOR.h
 *
 *  Created on: Feb 23, 2022
 *      Author: abdelrahman
 */

#ifndef DCMOTOR_H_
#define DCMOTOR_H_

#include"std_types.h"

/*******************************************************************************
 *                                Definitions                                  *
 *******************************************************************************/
#define DC_PORT    PORTC_ID
#define DC_PIN1    PIN2_ID
#define DC_PIN2    PIN3_ID

/*******************************************************************************
 *                               Types Declaration                             *
 *******************************************************************************/
typedef enum {
	STOP, CLOCK_WISE, ANTI_CLOCK_WISE
} DcMotor_State;
/*******************************************************************************
 *                              Functions Prototypes                           *
 *******************************************************************************/
/*
 * Description :
 The Function responsible for setup the direction for the two
 motor pins through the GPIO driver
 */
void DcMotor_init(void);
/*
 * Description :
 he function responsible for rotate the DC Motor CW/ or A-CW or
 stop the motor based on the state input state value.
 */
void DcMotor_Rotate(DcMotor_State state);

#endif /* DCMOTOR_H_ */
