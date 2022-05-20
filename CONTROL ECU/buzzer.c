/*
 * buzzer.c
 *
 *  Created on: Mar 1, 2022
 *      Author: abdelrahman
 */
#include"buzzer.h"
#include"gpio.h"
/*******************************************************************************
 *                      Functions Definitions                                  *
 *******************************************************************************/
/*
 * Description: Set the direction of Buzzer pin (Zero At Start).
 */
void Buzzer_init(void)
{
	/* Set Buzzer Pin as output*/
	GPIO_setupPinDirection(BUZZER_PORT, BUZZER_PIN , PIN_OUTPUT);
	/* write 0 (turnoff) Buzzer Pin at begining*/
	GPIO_writePin(BUZZER_PORT, BUZZER_PIN , LOGIC_LOW);
}
/*
 * Description: Function To Turn Buzzer On.
 */
void Buzzer_On(void)
{
	GPIO_writePin(BUZZER_PORT, BUZZER_PIN , LOGIC_HIGH);
}
/*
 * Description: Function To Turn Buzzer Off.
 */
void Buzzer_OFF(void)
{
	GPIO_writePin(BUZZER_PORT, BUZZER_PIN , LOGIC_LOW);

}


