/*
 * buzzer.h
 *
 *  Created on: Mar 1, 2022
 *      Author: abdelrahman
 */

#ifndef BUZZER_H_
#define BUZZER_H_

#include"std_types.h"
/*******************************************************************************
 *                                Definitions                                  *
 *******************************************************************************/
#define BUZZER_PORT  PORTC_ID
#define BUZZER_PIN   PIN4_ID
/*******************************************************************************
 *                      Functions Prototypes                                   *
 *******************************************************************************/
/*
 * Description: Set the direction of Buzzer pin (Zero At Start).
 */
void Buzzer_init(void);
/*
 * Description: Function To Turn Buzzer On.
 */
void Buzzer_On(void);
/*
 * Description: Function To Turn Buzzer Off.
 */
void Buzzer_OFF(void);

#endif /* BUZZER_H_ */
