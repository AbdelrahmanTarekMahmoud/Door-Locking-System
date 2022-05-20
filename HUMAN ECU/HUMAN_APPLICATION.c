/*
 * HUMAN_APPLICATION.c
 *
 *  Created on: Mar 1, 2022
 *      Author: Abdelrahman Tarek
 */

#include"lcd.h"
#include"keypad.h"
#include"timer.h"
#include"uart.h"
#include"util/delay.h"
#include "avr/interrupt.h"

/*******************************************************************************
 *                                Definitions                                  *
 *******************************************************************************/

#define READY              0x01
/*
 * Calculations :
 * We work at 8Mhz And 1024 Prescaler
 * Resolution = 1/(Frequency/Prescaler)
 * Resolution = 1024/8Mhz =1.28*10^(-4)
 * So T = Resolution * Ticks =  1.28*10^(-4) * 256 =32ms
 */
#define OPEN_TIME              469     /*15 Seconds*/
#define CLOSE_TIME             469     /*15 Seconds*/
#define THE_DOOR_STILL_OPEN    94      /*3 Seconds */
#define WARNING_TIME            1875    /*60 Seconds*/
#define PASSWORD_LENGTH         5
#define WARNING_VALUE           3

/*******************************************************************************
 *                      Global Variables                                       *
 *******************************************************************************/
uint32 g_ticks = 0; /*number of times which Timer makes interrupt */
uint8 WARNING_COUNTER = 0; /* A counter To count number of unmatch cases */

uint8 HUMAN_PASSWORD[PASSWORD_LENGTH];

/*******************************************************************************
 *                      Functions Definitions                                  *
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
 * Initiate The LCD
 * Initiate The UART
 * Initiate The Timer
 */
void inits() {
	LCD_init();
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
 * Function to Show The main options To user To choose
 */
void MAIN_OPTIONS(void) {
	LCD_clearScreen();
	LCD_displayStringRowColumn(0, 1, "+ : Change Pass");
	LCD_displayStringRowColumn(1, 1, "- : Open Door");
}
/*
 * Description :
 * Function to Make The User to enter password
 * And Display (*) to LCD
 */
void Write_Password(uint8 *Password) {
	LCD_moveCursor(1, 0);
	uint8 key;
	for (uint8 i = 0; i < PASSWORD_LENGTH; i++) {
		key = KEYPAD_getPressedKey();
		if ((key >= 0) && (key <= 9)) {
			LCD_displayCharacter('*');
			Password[i] = key;
		} else {

			i--;
		}
		_delay_ms(500);
	}
	while (KEYPAD_getPressedKey() != 13)
		;
}
/*
 * Description :
 * Function to Send The given Password To CONTROL ECU
 * By looping Bytes of Entered password
 */
void Send_Password(uint8 *Password) {
	for (uint8 i = 0; i < PASSWORD_LENGTH; i++) {
		UART_sendByte(Password[i]);
		_delay_ms(100);
	}
}
/*
 * Description :
 * Function to simulate The state of door with time
 * ._________________________________.______.___________________________________.
 * 0                                15      18                                 32
 *           (OPEN_TIME)             (IS_OPEN)              (CLOSING_TIME)
 */
void OPEN_DOOR() {
	g_ticks = 0;
	LCD_clearScreen();
	LCD_displayStringRowColumn(0, 1, "THE DOOR IS OPENING");
	while (g_ticks < OPEN_TIME) {
	}
	LCD_clearScreen();
	LCD_displayStringRowColumn(0, 1, "THE DOOR IS OPEN");
	while (g_ticks < (OPEN_TIME) + (THE_DOOR_STILL_OPEN)) {
	};
	LCD_clearScreen();
	LCD_displayStringRowColumn(0, 1, "THE DOOR IS CLOSING");
	while (g_ticks < (OPEN_TIME) + (THE_DOOR_STILL_OPEN) + (CLOSE_TIME)) {
	};
}
/*
 * Description :
 * Function to display warning messages on Screen
 */
void WARINING(void) {
	LCD_clearScreen();
	g_ticks = 0;
	LCD_displayStringRowColumn(0, 0, "WARNING!");
	LCD_displayStringRowColumn(0, 10, "WARNING!");
	LCD_displayStringRowColumn(1, 0, "WARNING!");
	LCD_displayStringRowColumn(1, 10, "WARNING!");
	while (g_ticks < WARNING_TIME) {
	};
}
/*
 * Description :
 * Function to Display only in first time and it does the following :
 * 1-Display "ENTER NEW PASSWORD"
 * 2-Let User to write the password he needs
 * 3-Send The Password To control ECU
 * 4-Let User to Re_Enter The Password
 * 5-Send the Password to Control ECU To compare with the first one
 * 6-if both are equal it will display "PASSWORD IS MATCH"
 * 7-if both are not equal it will display "PASSWORD NOT MATCH"
 */
void FIRST_TIME_DISPLAY() {
	uint8 condition = FALSE;
	uint8 PASSWORD1[PASSWORD_LENGTH];
	uint8 PASSWORD2[PASSWORD_LENGTH];
	while (condition == FALSE) {
		LCD_clearScreen();
		LCD_displayStringRowColumn(0, 0, "Enter New Password");
		Write_Password(PASSWORD1);

		Send_Password(PASSWORD1);
		LCD_clearScreen();
		LCD_displayStringRowColumn(0, 0, "Re_Enter Password ");
		Write_Password(PASSWORD2);

		Send_Password(PASSWORD2);

		condition = UART_recieveByte();
		if (condition == TRUE) {
			LCD_clearScreen();
			LCD_displayStringRowColumn(0, 1, " PASSWORD IS MATCH    ");
			_delay_ms(1000);
			LCD_displayStringRowColumn(0, 1, "  WELCOME TO HMI   ");
			_delay_ms(1000);
		} else if (condition == FALSE) {
			LCD_clearScreen();
			LCD_displayStringRowColumn(0, 1, "PASSWORD NOT_MATCH");
			_delay_ms(1500);
		}
	}
	LCD_clearScreen();
}

int main() {
	/*Variable to store the value recieved from Control ECU (Compare output)*/
	uint8 Match_State;
	/*Enable interrupt because of timer*/
	SREG |= (1 << 7);
	/*initiate all modules*/
	inits();
	/*
	 *To Achieve Synch between HUMAN_ECU & CONTROL_ECU
	 */
	UART_sendByte(READY);
	/*
	 * Call function in main to display in fisrt time only to let user set password
	 */
	FIRST_TIME_DISPLAY();
	while (1) {
		/*
		 * Display HMI to user to make his choice
		 */
		MAIN_OPTIONS();
		/*
		 * Dont enter the program until user press + or -
		 */
		while (KEYPAD_getPressedKey() != '+' && KEYPAD_getPressedKey() != '-')
			;
		/*
		 * IN CASE THE USER CHOSE TO CHANGE PASSWORD
		 */
		if ((KEYPAD_getPressedKey()) == '+') {
			LCD_clearScreen();
			LCD_displayStringRowColumn(0, 0, "Enter the Password");
			/*
			 * let user to enter password so he can acess the program
			 */
			Write_Password(HUMAN_PASSWORD);
			/*
			 *To Achieve Synch between HUMAN_ECU & CONTROL_ECU
			 */
			UART_sendByte(READY);
			/*
			 * send Password to Control ECU to make a compare by the one in EEPROM
			 */
			Send_Password(HUMAN_PASSWORD);
			UART_sendByte('+');
			/*
			 * recieve the compare output from Control ECU
			 */
			Match_State = UART_recieveByte();
			/*
			 * if both passwords match it will return true
			 * it will display message on screen
			 * And make the counter =0
			 * And let the user change password
			 */
			if (Match_State == TRUE) {
				LCD_clearScreen();
				LCD_displayStringRowColumn(0, 0, "  Correct Password  ");
				WARNING_COUNTER = 0;
				_delay_ms(1000);
				FIRST_TIME_DISPLAY();
			}
			/*
			 * if both passwords dont match it will return False
			 * it will make the user to try again only for 2 times
			 * And it will increase the counter
			 * And if counter equal to warning value
			 * it will display warning message on screen for 60seconds
			 */
			else if (Match_State == FALSE) {
				WARNING_COUNTER++;
				if (WARNING_COUNTER != WARNING_VALUE) {
					LCD_clearScreen();
					LCD_displayStringRowColumn(0, 0, "In-Correct Try again ");
					_delay_ms(1000);
				} else if (WARNING_COUNTER == WARNING_VALUE) {
					WARINING();
				}
			}
		}
		/*
		 * IN CASE THE USER CHOSE TO OPEN DOOR
		 */
		else if (KEYPAD_getPressedKey() == '-') {
			LCD_clearScreen();
			LCD_displayStringRowColumn(0, 0, "Enter the Password");
			/*
			 * Let the user to enter the password first
			 */
			Write_Password(HUMAN_PASSWORD);
			/*
			 *To Achieve Synch between HUMAN_ECU & CONTROL_ECU
			 */
			UART_sendByte(READY);
			/*
			 * send Password to Control ECU to make a compare by the one in EEPROM
			 */
			Send_Password(HUMAN_PASSWORD);
			UART_sendByte('-');
			/*
			 * recieve the compare output from Control ECU
			 */
			Match_State = UART_recieveByte();
			if (Match_State == TRUE) {
				OPEN_DOOR();
			}
			/*
			 * if both passwords dont match it will return False
			 * it will make the user to try again only for 2 times
			 * And it will increase the counter
			 * And if counter equal to warning value
			 * it will display warning message on screen for 60seconds
			 */
			else if (Match_State == FALSE) {
				WARNING_COUNTER++;
				if (WARNING_COUNTER != WARNING_VALUE) {
					LCD_clearScreen();
					LCD_displayStringRowColumn(0, 0, "In-Correct Try again ");
					_delay_ms(1000);
				} else if (WARNING_COUNTER == WARNING_VALUE) {
					WARINING();
				}
			}

		}

	}
}

