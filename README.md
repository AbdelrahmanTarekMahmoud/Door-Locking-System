# Door-Locking-System
Embedded System Project using AVR

Door locking system with two microcontrollers of ATmega16 version.

First Microcontroller: HMI ECU that is responsible for interacting with LCD, keypad, displaying the status of door and alerts and sending keypad pressed keys to Control ECU using UART.

Second Microcontroller: Control ECU that is responsible for password comparison received from HMI ECU using UART, saving passwords in an external EEPROM using I2C and activating Buzzer and motor to open and close door.
# I Used in This Project The below :
# 1-Drivers :
           1-GPIO DRIVER
           2-LCD DRIVER
           4-ICU DRIVER
           5-UART DRIVER
           6-BUZZER DRIVER
           7-DC MOTOR DRIVER
           8-TIMER DRIVER
           9-TWI DRIVER
           10-KEYBAD DRIVER
           
# 2-TOOLS  :
           1-PROTUES
           2-ECLIPSE
# Tested With 
           Atmega16 Running at 8Mhz
# AUTHOR 
           Abdelrahman Tarek Mahmoud - Gmail : AbdelrhmanTarekMahmoud@gmail.com

