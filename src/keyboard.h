/*
 * keyboard.h
 *
 *  Created on: Dec 14, 2017
 *      Author: mniendicker
 */

#ifndef KEYBOARD_H_
#define KEYBOARD_H_

/*------------------------------------------------------------------
 * 						HEADERS
 * ----------------------------------------------------------------*/
#include "hal/board.h"


/*------------------------------------------------------------------
 * 					GLOBAL CONSTANTS
 * ----------------------------------------------------------------*/
const bool button_pressed= 0;  //Low = Pressed
const bool button_released= 1; //High= Released


static const uint8_t no_key_pressed		= 0x00;
static const uint8_t left_button_mask	= 0x01;
static const uint8_t right_button_mask	= 0x02;
static const uint8_t down_button_mask	= 0x04;
static const uint8_t up_button_mask		= 0x08;
static const uint8_t enter_button_mask	= 0x10;
static const uint8_t esc_button_mask	= 0x20;


/*------------------------------------------------------------------
 * 					GLOBAL VARIABLES
 * ----------------------------------------------------------------*/
volatile bool keyboard_flag_sync; 	//Flag to indicate that some key was pressed/released
									//Store key states (pressed/released)


/*------------------------------------------------------------------
 * 					PROTOTYPES
 * ----------------------------------------------------------------*/
/*------------------------------------------------------------------
 * Initialize the hardware
 * ----------------------------------------------------------------*/
void init_keyboard();

/*------------------------------------------------------------------
 * ISR - LEFT button pressed
 * ----------------------------------------------------------------*/
void left_changed();

/*------------------------------------------------------------------
 * ISR - RIGHT button pressed
 * ----------------------------------------------------------------*/
void right_changed();

/*------------------------------------------------------------------
 * ISR - DOWN button pressed
 * ----------------------------------------------------------------*/
void down_changed();

/*------------------------------------------------------------------
 * ISR - UP button pressed
 * ----------------------------------------------------------------*/
void up_changed();

/*------------------------------------------------------------------
 * ISR - ENTER button pressed
 * ----------------------------------------------------------------*/
void enter_changed();

/*------------------------------------------------------------------
 * ISR - ENTER button pressed
 * ----------------------------------------------------------------*/
void esc_changed();

/*------------------------------------------------------------------
 * Verify the key pressed
 * ----------------------------------------------------------------*/
void manage_keyboard();


 /*------------------------------------------------------------------
 * 					FUNCTIONS DEFINITION
 * ----------------------------------------------------------------*/
/*------------------------------------------------------------------
 * Initialize the hardware
 * ----------------------------------------------------------------*/
void init_keyboard(){
	pinMode(button_left, INPUT_PULLUP);
	pinMode(button_right, INPUT_PULLUP);
	pinMode(button_down, INPUT_PULLUP);
	pinMode(button_up, INPUT_PULLUP);
	pinMode(button_enter, INPUT_PULLUP);
	pinMode(button_esc, INPUT_PULLUP);

	//Initialize variables to manage the keyboard
	keyboard_flag_sync&= no_key_pressed;

	//Interrupts
	//<LEFT>
	attachInterrupt(digitalPinToInterrupt(button_left), left_changed, FALLING);
	//<RIGHT>
	attachInterrupt(digitalPinToInterrupt(button_right), right_changed, FALLING);
	//<DOWN>
	attachInterrupt(digitalPinToInterrupt(button_down), down_changed, FALLING);
	//<UP>
	attachInterrupt(digitalPinToInterrupt(button_up), up_changed, FALLING);
	//<ENTER>
	attachInterrupt(digitalPinToInterrupt(button_enter), enter_changed, FALLING);
	//<ESC>
	attachInterrupt(digitalPinToInterrupt(button_esc), esc_changed, FALLING);
}

/*------------------------------------------------------------------
 * ISR - LEFT button pressed
 * ----------------------------------------------------------------*/
void left_changed(){
	if(digitalRead(button_left) == button_pressed)
		keyboard_flag_sync|= left_button_mask;
	else
		keyboard_flag_sync&= ~left_button_mask;
}

/*------------------------------------------------------------------
 * ISR - RIGHT button pressed
 * ----------------------------------------------------------------*/
void right_changed(){
	keyboard_flag_sync= 1;

	if(digitalRead(button_right) == button_pressed)
		keyboard_flag_sync|= right_button_mask;
	else
		keyboard_flag_sync&= ~right_button_mask;
}

/*------------------------------------------------------------------
 * ISR - DOWN button pressed
 * ----------------------------------------------------------------*/
void down_changed(){
	if(digitalRead(button_down) == button_pressed)
		keyboard_flag_sync|= down_button_mask;
	else
		keyboard_flag_sync&= ~down_button_mask;
}

/*------------------------------------------------------------------
 * ISR - UP button pressed
 * ----------------------------------------------------------------*/
void up_changed(){
	if(digitalRead(button_up) == button_pressed)
		keyboard_flag_sync|= up_button_mask;
	else
		keyboard_flag_sync&= ~up_button_mask;
}

/*------------------------------------------------------------------
 * ISR - ENTER button pressed
 * ----------------------------------------------------------------*/
void enter_changed(){
	if(digitalRead(button_enter) == button_pressed)
		keyboard_flag_sync|= enter_button_mask;
	else
		keyboard_flag_sync&= ~enter_button_mask;
}

/*------------------------------------------------------------------
 * ISR - ESC button pressed
 * ----------------------------------------------------------------*/
void esc_changed(){
	if(digitalRead(button_esc) == button_pressed)
		keyboard_flag_sync|= esc_button_mask;
	else
		keyboard_flag_sync&= ~esc_button_mask;
}


/*------------------------------------------------------------------
 * Verify the key pressed
 * ----------------------------------------------------------------*/
void manage_keyboard(){
	keyboard_flag_sync= 0;

	Serial.println(keyboard_flag_sync);
}


#endif /* KEYBOARD_H_ */
