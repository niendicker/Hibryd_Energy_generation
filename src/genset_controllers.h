/*
 * genset_controllers.h
 *
 *  Created on: Dec 13, 2017
 *      Author: mniendicker
 */

#ifndef GENSET_CONTROLLERS_H_
#define GENSET_CONTROLLERS_H_

/*------------------------------------------------------------------
 * 					GLOBAL CONSTANTS
 * ----------------------------------------------------------------*/
static const uint8_t genset_max_nodes = 32;

/*------------------------------------------------------------------
 * 					SICES SRL CONTROLLER
 * -----------------------------------------------------------------
 * 					GC600 - GC600Mains
 * ----------------------------------------------------------------*/
class Sices{
public:
	//READ - INPUT REGISTERS (FUNCTION 0x04)
	static const uint16_t nominal_power= 13018; 		//Register address - Nominal power of genset
	static const uint8_t nominal_power_nr= 1; 			//Number of registers
	static const float nominal_power_scale;				//Scale for conversion (value= received * scale)
	typedef uint16_t GENSET_NOMINAL_POWER_DATA;			//Data type

	static const uint16_t active_power= 61;				//Register address - Actual active power generated
	static const uint8_t active_power_nr= 2;			//Number of registers
	static const float active_power_scale;				//Scale for conversion (value= received * scale)
	typedef int32_t GENSET_TOTAL_ACTIVE_POWER_DATA;		//Data type

	static const uint16_t gcb_status= 138;				//Register address - GCB status of genset
	static const uint8_t gcb_status_nr= 1;				//Number of register
	static const uint16_t gcb_status_mask= 0x0001;		//Bit 00 - GCB status

	static const uint16_t mcb_status= 138;				//Register address - MCB status of genset
	static const uint8_t mcb_status_nr= 1;				//Number of register
	static const uint16_t mcb_status_mask= 0x0002;		//Bit 01 - MCB status

	static const uint16_t mgcb_status= 138;				//Register address - MGCB status of genset
	static const uint8_t mgcb_status_nr= 1;				//Number of register
	static const uint16_t mgcb_status_mask= 0x0004;		//Bit 03 - MGCB status

	#define GENSET_CIRCUIT_BRAKERS_DATA uint16_t		//Data type for circuit brakers

};
//Init the float members
const float Sices::nominal_power_scale= 1.0;
const float Sices::active_power_scale= 0.00390625;//(1/256)

/*------------------------------------------------------------------
 * Genset controllers supported - Used as reference to select the inverter models
 * ----------------------------------------------------------------*/
enum genset_controllers{
	NoGenset,
	Sices
};

#endif /* GENSET_CONTROLLERS_H_ */
