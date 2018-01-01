/*
 * digital_outputs.h
 *
 *  Created on: Dec 14, 2017
 *      Author: mniendicker
 */

#ifndef DIGITAL_OUTPUTS_H_
#define DIGITAL_OUTPUTS_H_

/*------------------------------------------------------------------
 * 						HEADERS
 * ----------------------------------------------------------------*/
#include "hal/board.h"


/*------------------------------------------------------------------
 * 					GLOBAL CONSTANTS
 * ----------------------------------------------------------------*/


/*------------------------------------------------------------------
 * 					GLOBAL VARIABLES
 * ----------------------------------------------------------------*/


/*------------------------------------------------------------------
 * 					PROTOTYPES
 * ----------------------------------------------------------------*/
/*------------------------------------------------------------------
 * Initialize the hardware
 * ----------------------------------------------------------------*/
void init_digital_outputs();


 /*------------------------------------------------------------------
 * 					FUNCTIONS DEFINITION
 * ----------------------------------------------------------------*/
/*------------------------------------------------------------------
 * Initialize the hardware
 * ----------------------------------------------------------------*/
void init_digital_outputs(){
	pinMode(digital_output1, OUTPUT);
	digitalWrite(digital_output1, 0);
	pinMode(digital_output2, OUTPUT);
	digitalWrite(digital_output2, 0);
	pinMode(digital_output3, OUTPUT);
	digitalWrite(digital_output3, 0);
	pinMode(digital_output4, OUTPUT);
	digitalWrite(digital_output4, 0);
}


#endif /* DIGITAL_OUTPUTS_H_ */
