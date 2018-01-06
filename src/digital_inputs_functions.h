/*
 * digital_inputs_functions.h
 *
 *  Created on: Jan 6, 2018
 *      Author: mniendicker
 */

#ifndef SRC_DIGITAL_INPUTS_FUNCTIONS_H_
#define SRC_DIGITAL_INPUTS_FUNCTIONS_H_

/*------------------------------------------------------------------
 * 						HEADERS
 * ----------------------------------------------------------------*/
#include <stdint.h>

/*------------------------------------------------------------------
 * 					GLOBAL CONSTANTS
 * ----------------------------------------------------------------*/
//--------------DIGITAL INPUT FUNCTIONS LIST MASK-------------------
//Status of bits for the status of circuit breakers
const uint8_t circuit_breaker_opened= 0x00;
const uint8_t circuit_breaker_closed= 0x01;

//Status of bits for status of power limit
const uint8_t power_limit_enabled= 0x00;
const uint8_t power_limit_disabled= 0x01;

/*------------------------------------------------------------------
 * 					GLOBAL VARIABLES
 * ----------------------------------------------------------------*/
/**
 * Each function is bit wise and the set/reset must be performed
 * using the correct constants (depending on function group)
 */
struct{
	//---------- Circuit breakers status
	//GCB status
	uint8_t dif_gcb_status: 1;
	//MGCB status
	uint8_t dif_mgcb_status: 1;
	//MCB status
	uint8_t dif_mcb_status: 1;

	//---------- Logic management functions
	//Disable the power limitation for PV system
	uint8_t dif_disable_power_limit: 1;
} di_functions;


/*------------------------------------------------------------------
 * 					PROTOTYPES
 * ----------------------------------------------------------------*/
/*------------------------------------------------------------------
 * Initialize the functions states
 * ----------------------------------------------------------------*/
void di_functions_init();


 /*------------------------------------------------------------------
 * 					FUNCTIONS DEFINITION
 * ----------------------------------------------------------------*/
/*------------------------------------------------------------------
 * Initialize the functions states
 * ----------------------------------------------------------------*/
void di_functions_init(){
	//All circuit breakers opened
	di_functions.dif_gcb_status= circuit_breaker_opened;
	di_functions.dif_mgcb_status= circuit_breaker_opened;
	di_functions.dif_mcb_status= circuit_breaker_opened;

	//Power limit enabled
	di_functions.dif_disable_power_limit= power_limit_enabled;
}


#endif /* SRC_DIGITAL_INPUTS_FUNCTIONS_H_ */
