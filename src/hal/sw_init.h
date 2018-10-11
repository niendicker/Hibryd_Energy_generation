/*
 * sw_init.h
 *
 *  Created on: Dec 12, 2017
 *      Author: mniendicker
 *
 *      HAL: init software configuration
 */

#ifndef HAL_SW_INIT_H_
#define HAL_SW_INIT_H_

#include "../lib/modbus_master.h"
#include "../pv_modbus.h"
#include "../genset_modbus.h"
#include "../digital_inputs_functions.h"

/*------------------------------------------------------------------
 * 						HEADERS
 * ----------------------------------------------------------------*/


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
 * 					FUNCTIONS DEFINITION
 * ----------------------------------------------------------------*/

void sw_init(){
	//Init PV system modbus interface
	pv_init_modbus(pv_default_slave_addr);

	//Init Genset modbus interface
	genset_init_modbus(genset_default_slave_addr);

	//Init digital inputs functions
	di_functions_init();

	//Debug port
	Serial.begin(115200);
	Serial.println("--------------- SETUP -------------------");

	//On-board LED
	pinMode(LED_BUILTIN, OUTPUT);

	//TODO: REMOVE ////////////////////////////////////////////
	for(int i=0; i < pv_max_nodes; i++)
		pv_set_node_type(i, Sungrow);

	for (int i= 0; i < genset_max_nodes; i++)
		genset_set_node_type(i, Sices);
	//TODO: REMOVE ////////////////////////////////////////////
}


#endif /* HAL_SW_INIT_H_ */
