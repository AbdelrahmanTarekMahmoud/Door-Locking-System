/*
 * CONTROL_APPLICATION.c
 *
 *  Created on: Mar 1, 2022
 *      Author: Abdelrahman Tarek
 */
#include"DCMOTOR.h"
#include"buzzer.h"
#include"uart.h"
#include"external_eeprom.h"
#include"timer.h"
#include"twi.h"
#include "avr/interrupt.h"
#include"util/delay.h"


/*******************************************************************************
 *                                Definitions                                  *
 *******************************************************************************/
/*
 * Calculations :
 * We work at 8Mhz And 1024 Prescaler
 * Resolution = 1/(Frequency/Prescaler)
 * Resolution = 1024/8Mhz =1.28*10^(-4)
 * So T = Resolution * Ticks =  1.28*10^(-4) * 256 =32ms
 */
#define READY 0x01
#define OPEN_TIME              469     /*15 Seconds*/
#define CLOSE_TIME             469     /*15 Seconds*/
#define THE_DOOR_STILL_OPEN    94      /*3 Seconds */
#define WARNING_TIME                1875    /*60 Seconds*/
#define PASSWORD_LENGTH         5
#define WARNING_VALUE            3

uint32 g_ticks = 0; /*number of times which Timer makes interrupt */
uint8 CONTROL_PASSWORD[PASSWORD_LENGTH];
uint8 MAIN_PASSWORD[PASSWORD_LENGTH];
uint8 WARNING_COUNTER =0;

/*******************************************************************************
 *                      Functions Definitions                                  *
 *
 *******************************************************************************/
/*
 * Description :
 * Function to count how many times timer reaches its compare Value
 * in our case its every nearly equal to 32ms
 */
void Callback_Function() {
	g_ticks++;
}
/*
 * Description :
 * Initiate The BUZZER
 * Initiate The UART
 * Initiate The TIMER
 * Initiate The DC_MOTOR
 * Initiate The TWI
 */
void inits() {
	Buzzer_init();
	DcMotor_init();
	EEPROM_init();

	/*
	 * Timer Config Which we will work on
	 * Compare Mode,1024 PRESCALER, Zero initial Value , 250 Compare value
	 */
	Timer_ConfigType Timer_Config_Type = { CTC, CLK_1024, 0, 250 };
	timer0_SetCallBack(Callback_Function);
	Timer_init(&Timer_Config_Type);

	/*
	 * UART Config Which we will work on
	 * Eight bits mode,Disable Parity,One Stop bit , Baud rate=9600
	 */
	UART_ConfigType UART_Config_Type = { EIGHT, DISABLE, ONE_BIT, 9600 };
	UART_init(&UART_Config_Type);
}
/*
 * Description :
 * Function to Recieve The given Password From HUMAN ECU
 * By looping Bytes of Entered password
 */
void Recieve_Password(uint8 *Password) {
	for (uint8 i = 0; i < PASSWORD_LENGTH; i++) {
		Password[i] = UART_recieveByte();
		_delay_ms(100);
	}
}
/*
 * Description :
 * Function to simulate The state of door with time
 * By controlling The State of DC_MOTOR
 * ._________________________________.______.___________________________________.
 * 0                                15      18                                 32
 *           (OPEN_TIME)             (IS_OPEN)              (CLOSING_TIME)
 */
void OPEN_DOOR() {
	g_ticks = 0;
	DcMotor_Rotate(CLOCK_WISE);
	while (g_ticks < OPEN_TIME) {
	};
	DcMotor_Rotate(STOP);
	while (g_ticks < ((OPEN_TIME) + (THE_DOOR_STILL_OPEN))) {
	};
	DcMotor_Rotate(ANTI_CLOCK_WISE);
	while (g_ticks < ((OPEN_TIME) + (THE_DOOR_STILL_OPEN) + (CLOSE_TIME))) {
	};
	DcMotor_Rotate(STOP);
}
/*
 * Description :
 * Function to on buzzer on Warning case
 * for 60 Seconds
 * then turn it off
 */
void WARINING() {
	g_ticks = 0;
	Buzzer_On();
	while (g_ticks < WARNING_TIME){};
	Buzzer_OFF();

}
/*
 * Description :
 * Function Do Save The first recieved Password to EEPROM only if it matched The second Password
 */
void Saving_In_EEPROM(uint8 *Password) {
	for (uint8 i = 0; i < PASSWORD_LENGTH; i++) {
		EEPROM_writeByte((0x0311 + i), Password[i]);
		MAIN_PASSWORD[i]=Password[i];
		_delay_ms(100);
	}
}
/*
 * Description :
 * Function To compare The 2 Recieved Password From User
 * By : Comparing Counter to Password Length
 * And Return True if both are equal
 * And Retrun False if Not
 */
uint8 COMPARE(uint8* P1, uint8* P2) {
	uint8 MatchCounter = 0;
	for (uint8 i = 0; i < PASSWORD_LENGTH; i++) {
		if (P1[i] == P2[i]) {
			MatchCounter++;
		}
	}
	if (MatchCounter == PASSWORD_LENGTH)
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}
/*
 * Description :
 * Function to Display only in first time and it does the following :
 * 1-Recieve First Password from HUMAN ECU
 * 2-Recieve Second Password from HUMAN ECU
 * 3-Compare Both passwords
 * 4-if both equal it will save PASSWORD1 to EEPROM
 * 5-if not it user will retry
 */
void FIRST_TIME_DISPLAY()
{
	uint8 PASSWORD1[PASSWORD_LENGTH];
	uint8 PASSWORD2[PASSWORD_LENGTH];
	uint8 condition=FALSE;
	while(condition == FALSE)
	{
   //while (UART_recieveByte() != READY);
	//UART_sendByte(READY);//5
	Recieve_Password(PASSWORD1);
	//while (UART_recieveByte() != READY);
	//UART_sendByte(READY);
	Recieve_Password(PASSWORD2);
	condition = COMPARE(PASSWORD1, PASSWORD2);
	//UART_sendByte(READY);
    UART_sendByte(condition);
    if(condition == TRUE)
    {
    	Saving_In_EEPROM(PASSWORD1);
    }
	}
}



int main() {
	/*Variable to store the value recieved from Human ECU (+) OR (-)*/
	uint8 condition;
	/*Variable To store the output of Compare function*/
	uint8 Match_State;
	/*Enable interrupt because of timer*/
	SREG |= (1 << 7);
	/*initiate all modules*/
	inits();
	/*
	 *To Achieve Synch between HUMAN_ECU & CONTROL_ECU
	 */
	while (UART_recieveByte() != READY);
	/*
     * Call function in main to access only at first time to recieve the 2 password from user
     * Then Compare them and save the first to EEPROM
	 */
	FIRST_TIME_DISPLAY();
	while (1) {
		/*
		 *To Achieve Synch between HUMAN_ECU & CONTROL_ECU
		 */
		while (UART_recieveByte() != READY);
		/*
		 *Recieve Password From HUMAN ECU to compare it to Main_Password
		 */
		Recieve_Password(CONTROL_PASSWORD);
		/*
		 *to detect the use choice :
		 *1-CHANGE PASSWORD
		 *2-OPEN DOOR
		 */
        condition=UART_recieveByte();
		/*
		 * IN CASE THE USER CHOSE TO CHANGE PASSWORD
		 */
		if(condition == '+')
		{
			/*
			 * Compare :
			 * 1-recieved Password
			 * 2-Main Password
			 */
			Match_State=COMPARE(MAIN_PASSWORD,CONTROL_PASSWORD);
			/*
			 * if both passwords match
			 * it will send TRUE to HUMAN_ECU
			 * And make the counter =0
			 * And let the user change password
			 */
			if (Match_State == TRUE)
			{
				UART_sendByte(TRUE);
				WARNING_COUNTER =0;
				FIRST_TIME_DISPLAY();
			}
			/*
			 * if both passwords dont match
			 * it will send FALSE to HUMAN_ECU
			 * And it will increase the counter
			 * And if counter equal to warning value
			 * it will turn BUZZER ON for 60seconds
			 */
			else if(Match_State == FALSE)
			{
				WARNING_COUNTER++;
				UART_sendByte(FALSE);
				if(WARNING_COUNTER == WARNING_VALUE)
				{
					WARINING();
				}
			}

		}
		/*
		 * IN CASE THE USER CHOSE TO OPEN DOOR
		 */
		else if(condition == '-')
		{
			/*
			 * Compare :
			 * 1-recieved Password
			 * 2-Main Password
			 */
			Match_State=COMPARE(MAIN_PASSWORD,CONTROL_PASSWORD);
			/*
			 * if both passwords match
			 * it will send TRUE to HUMAN_ECU
			 * And make the counter =0
			 * And it will rotate the Motor depend on number of ticks in timer
			 */
			if(Match_State == TRUE)
			{
				WARNING_COUNTER =0;
				UART_sendByte(TRUE);//4
				OPEN_DOOR();
			}
			/*
			 * if both passwords dont match
			 * it will send FALSE to HUMAN_ECU
			 * And it will increase the counter
			 * And if counter equal to warning value
			 * it will turn BUZZER ON for 60seconds
			 */
			else if(Match_State == FALSE)
						{
							WARNING_COUNTER++;
							UART_sendByte(FALSE);//5
							if(WARNING_COUNTER == WARNING_VALUE)
							{
								WARINING();
							}
						}


		}


}
}

