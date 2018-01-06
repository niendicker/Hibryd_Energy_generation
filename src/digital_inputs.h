/*
 * digital_inputs.h
 *
 *  Created on: Dec 15, 2017
 *      Author: mniendicker
 */

#ifndef DIGITAL_INPUTS_H_
#define DIGITAL_INPUTS_H_

/*------------------------------------------------------------------
 * 						HEADERS
 * ----------------------------------------------------------------*/
#include "hal/board.h"
#include "digital_inputs_functions.h"


/*------------------------------------------------------------------
 * 					GLOBAL CONSTANTS
 * ----------------------------------------------------------------*/
/**
 * Mask used to manage the digital inputs bits
 */
const uint16_t di_01= 0x0001; //Digital input 01= bit 01
const uint16_t di_02= 0x0002; //Digital input 02= bit 02
const uint16_t di_03= 0x0004; //Digital input 03= bit 03
const uint16_t di_04= 0x0008; //Digital input 04= bit 04

/**
 * Define the logic levels used for digital inputs
 */
const uint8_t di_activated= LOW;
const uint8_t di_deactivated= HIGH;

/**
 * Software filter for digital inputs.
 * Minimum time between to interrupts.
 * This time define the MINIMUM time that some DI need to be activated to
 * activate the respective function.
 */
const uint16_t di_filter_time= 100; //100ms

//Used as default value to signal that no synchronization is necessary
const uint16_t di_sync_none= 0x0000;

/*------------------------------------------------------------------
 * 					GLOBAL VARIABLES
 * ----------------------------------------------------------------*/
/**
 * Save the logical status of each digital input.
 *-->Changed by associated digital input
 *-->The logical state is the status used for program logic
 *-->When some logical state change, the flag for management is set
 */
volatile uint16_t di_logical_states;

//Save the hardware status (after filter) of each digital input
volatile uint16_t di_physical_states;

//Save the logic used to set the logic status of each digital input function
//Bit= 0; Normal logic, logical state = physical state
//Bit= 1; Reverse logic, logical state = ~physical state
uint16_t di_logic_selection;

//Synchronization flag
volatile uint16_t digital_inputs_sync_flag;

/*------------------------------------------------------------------
 * 					PROTOTYPES
 * ----------------------------------------------------------------*/
/*------------------------------------------------------------------
 * Initialize the hardware and control variables
 * ----------------------------------------------------------------*/
void init_digital_inputs();

/*------------------------------------------------------------------
 * Manage the digital inputs logic and physical status.
 * Function called from main loop when the synchronization flag is set
 * ----------------------------------------------------------------*/
void manage_digital_inputs();

/*------------------------------------------------------------------
 * Manage the physical and logical status of each digital input
 * @digital_input is the hardware pin number used as digital input
 * ----------------------------------------------------------------*/
void di_change_detect(uint8_t digital_input);

/*------------------------------------------------------------------
 * ISR - Digital input 1 activated
 * ----------------------------------------------------------------*/
void di_1_activated();

/*------------------------------------------------------------------
 * ISR - Digital input 2 activated
 * ----------------------------------------------------------------*/
void di_2_activated();

/*------------------------------------------------------------------
 * ISR - Digital input 3 activated
 * ----------------------------------------------------------------*/
void di_3_activated();

/*------------------------------------------------------------------
 * ISR - Digital input 4 activated
 * ----------------------------------------------------------------*/
void di_4_activated();


 /*------------------------------------------------------------------
 * 					FUNCTIONS DEFINITION
 * ----------------------------------------------------------------*/
/*------------------------------------------------------------------
 * Initialize the hardware
 * ----------------------------------------------------------------*/
void init_digital_inputs(){
	pinMode(digital_input1, INPUT);
	attachInterrupt(digitalPinToInterrupt(digital_input1), di_1_activated, CHANGE);
	pinMode(digital_input2, INPUT);
	attachInterrupt(digitalPinToInterrupt(digital_input2), di_2_activated, CHANGE);
	pinMode(digital_input3, INPUT);
	attachInterrupt(digitalPinToInterrupt(digital_input3), di_3_activated, CHANGE);
	pinMode(digital_input4, INPUT);
	attachInterrupt(digitalPinToInterrupt(digital_input1), di_4_activated, CHANGE);

	//All deactivated
	di_physical_states= 0x0000;
	di_logical_states= 0x0000;
	//Normal logic
	di_logic_selection= 0x0000;

	//Reset synchronization flag
	digital_inputs_sync_flag= di_sync_none;
}

/*------------------------------------------------------------------
 * Manage the digital inputs logic and physical status.
 * Function called from main loop when the synchronization flag is set
 * ----------------------------------------------------------------*/
void manage_digital_inputs(){
	//Verifies the physical states
	if(digital_inputs_sync_flag & di_01){
		di_change_detect(digital_input1);
	}
	if(digital_inputs_sync_flag & di_02){
		di_change_detect(digital_input2);
	}
	if(digital_inputs_sync_flag & di_03){
		di_change_detect(digital_input3);
	}
	if(digital_inputs_sync_flag & di_04){
		di_change_detect(digital_input4);
	}

	//Save previous states
	uint16_t prev_logical_states= di_logical_states;

	//Verifies the logical state based on physical state and logic selection.
	//XOR operation do exactly the logic used to set the logic states based on physical states.
	di_logical_states= di_physical_states ^ di_logic_selection;

	//Manage the functions associated with the digital inputs
	if(prev_logical_states != di_logical_states){

	}

}

/*------------------------------------------------------------------
 * Manage the physical status of each digital input
 * @digital_input is the hardware pin number used as digital input
 * ----------------------------------------------------------------*/
void di_change_detect(uint8_t digital_input){
	//Read the state of digital input
	uint8_t level= digitalRead(digital_input);

	//Verifies the physical state
	switch(digital_input){
		//Set reset physical states
		case digital_input1:
			level == di_activated ?	di_physical_states|= di_01 : di_physical_states&= ~di_01;
		break;
		case digital_input2:
			level == di_activated ? di_physical_states|= di_02 : di_physical_states&= ~di_02;
		break;
		case digital_input3:
			level == di_activated ? di_physical_states|= di_03 : di_physical_states&= ~di_03;
		break;
		case digital_input4:
			level == di_activated ? di_physical_states|= di_04 : di_physical_states&= ~di_04;
		break;
	}
}

/*------------------------------------------------------------------
 * ISR - Digital input 1 activated
 * Digital inputs with software filter
 * ----------------------------------------------------------------*/
void di_1_activated(){
	static uint32_t last_interrupt_time= 0;
	uint32_t interrupt_time= millis();

	//Filter - the signal on input must be stable at least
	//di_filter_time milliseconds to trigger the function.
	if((interrupt_time - last_interrupt_time) >= di_filter_time){
		//Set status to manage the digital input 1
		digital_inputs_sync_flag|= di_01;

		//Reset time to next trigger
		last_interrupt_time= millis();
	}
}

/*------------------------------------------------------------------
 * ISR - Digital input 2 activated
 * Digital inputs with software filter
 * ----------------------------------------------------------------*/
void di_2_activated(){
	static uint32_t last_interrupt_time= 0;
	uint32_t interrupt_time= millis();

	//Filter - the signal on input must be stable at least
	//di_filter_time milliseconds to trigger the function.
	if((interrupt_time - last_interrupt_time) >= di_filter_time){
		//Set status to manage the digital input 2
		digital_inputs_sync_flag|= di_02;

		//Reset time to next trigger
		last_interrupt_time= millis();
	}
}

/*------------------------------------------------------------------
 * ISR - Digital input 3 activated
 * Digital inputs with software filter
 * ----------------------------------------------------------------*/
void di_3_activated(){
	static uint32_t last_interrupt_time= 0;
	uint32_t interrupt_time= millis();

	//Filter - the signal on input must be stable at least
	//di_filter_time milliseconds to trigger the function.
	if((interrupt_time - last_interrupt_time) >= di_filter_time){
		//Set status to manage the digital input 3
		digital_inputs_sync_flag|= di_03;

		//Reset time to next trigger
		last_interrupt_time= millis();
	}
}

/*------------------------------------------------------------------
 * ISR - Digital input 4 activated
 * Digital inputs with software filter
 * ----------------------------------------------------------------*/
void di_4_activated(){
	static uint32_t last_interrupt_time= 0;
	uint32_t interrupt_time= millis();

	//Filter - the signal on input must be stable at least
	//di_filter_time milliseconds to trigger the function.
	if((interrupt_time - last_interrupt_time) >= di_filter_time){
		//Set status to manage the digital input 4
		digital_inputs_sync_flag|= di_04;

		//Reset time to next trigger
		last_interrupt_time= millis();
	}
}





#endif /* DIGITAL_INPUTS_H_ */
