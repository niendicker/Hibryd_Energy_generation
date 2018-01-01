/*
 * init.h
 *
 *  Created on: Dec 12, 2017
 *      Author: Marcelo Niendicker
 *
 *      HAL: Init hardware configuration
 */

#ifndef HW_INIT_H_
#define HW_INIT_H_

#include "board.h"
#include "../rs485.h"
#include "../digital_inputs.h"
#include "../digital_outputs.h"
#include "../analog_inputs.h"
#include "../keyboard.h"

/*------------------------------------------------------------------
 * Initialize all board hardware configuration
 * ----------------------------------------------------------------*/
void hw_init(){
//===================================SERIAL COMMUNICATION================================//
	init_rs485_pv();
	init_rs485_genset();
//===================================SERIAL COMMUNICATION================================//

//======================================DIGITAL INPUTS===================================//
	init_digital_inputs();
//======================================DIGITAL INPUTS===================================//

//=====================================DIGITAL OUTPUTS===================================//
	init_digital_outputs();
//=====================================DIGITAL OUTPUTS===================================//

//=====================================ANALOGUE INPUTS===================================//
	init_analog_inputs();
//=====================================ANALOGUE INPUTS===================================//

//=======================================KEY BUTTONS=====================================//
	init_keyboard();
//=======================================KEY BUTTONS=====================================//
}

#endif /* HW_INIT_H_ */
