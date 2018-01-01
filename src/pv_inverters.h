/*
 * pv_inverters.h
 *
 *  Created on: Dec 13, 2017
 *      Author: mniendicker
 *
 *      List of PV inverter suported
 */

#ifndef PV_INVERTERS_H_
#define PV_INVERTERS_H_

/*------------------------------------------------------------------
 * 					GLOBAL CONSTANTS
 * ----------------------------------------------------------------*/
//Max. PV inverters supported
static const uint8_t pv_max_nodes= 20;

/*------------------------------------------------------------------
 * 					SUNGROW
 * -----------------------------------------------------------------
 * SG33KTL-M, SG40KTL-M, SG49K5J, SG50KTL-M, SG60KU, SG60KU-M,
 * SG60KTL, SG60KTL-M, SG36KTL-M, SG8KTL-M,SG10KTL-M,SG12KTL-M,
 * SG80KTL, SG80KTL-M, SG80HV, SG125HV
 * ----------------------------------------------------------------*/
class Sungrow{
public:
	//READ - INPUT REGISTERS (FUNCTION 0x04)
	static const uint16_t nominal_power= 5001; 			//Register address - Nominal power of inverter
	static const uint8_t nominal_power_nr= 1; 			//Number of registers
	static const float nominal_power_scale;				//Scale for conversion (value= received * scale)
	typedef uint16_t PV_NOMINAL_POWER_DATA;				//Data type

	static const uint16_t active_power= 5031;			//Register address - Actual active power generated
	static const uint8_t active_power_nr= 2;			//Number of registers
	static const float active_power_scale;				//Scale for conversion (value= received * scale)
	typedef uint32_t PV_TOTAL_ACTIVE_POWER_DATA;		//Data type

	//WRITE - HOLDING REGISTERS (FUNCTION 0x06)
	static const uint16_t enable_power_limit= 5007;		//Register address - Enable the power limitation of inverter
														//Enable= 0xAA; Disable= 0x55
	static const uint8_t enable_power_limit_nr= 1;		//Number of registers
	typedef uint16_t PV_ENABLE_POWER_LIMIT_DATA; 		//Data type
	//No scale

	static const uint16_t power_limit_percent= 5008;	//Register address - Value to limit the power in % of nominal_power
	static const uint8_t power_limit_percent_nr= 1;		//Number of registers
	static const float power_limit_percent_scale;		//Scale for conversion (value_to_send= value_limitation / scale)
	typedef uint16_t PV_POWER_LIMIT_PERCENT_DATA; 		//Data type

	static const uint16_t power_limit_kw= 5039;			//Register address - Value to limit the active power in KW
	static const uint8_t power_limit_kw_nr= 2;			//Number of registers
	static const float power_limit_kw_scale;			//Scale for conversion (value_to_send= value_limitation / scale)
	typedef uint16_t PV_POWER_LIMIT_KW_DATA;			//Data type
};
//Init the float members
const float Sungrow::nominal_power_scale= 0.1;
const float Sungrow::active_power_scale= 1.0;
const float Sungrow::power_limit_percent_scale= 0.1;
const float Sungrow::power_limit_kw_scale= 0.1;

/*------------------------------------------------------------------
 * 					ABB
 * -----------------------------------------------------------------
 *
 * ----------------------------------------------------------------*/

/*------------------------------------------------------------------
 * 					FRONIUS
 * -----------------------------------------------------------------
 *
 * ----------------------------------------------------------------*/


/*------------------------------------------------------------------
 * Inverters supported - Used as reference to select the inverter models
 * ----------------------------------------------------------------*/
enum inverters {
	NoInverter,	//No inverter for this node
	Sungrow,	//This node is Sungrow inverter
	ABB,		//This node is ABB inverter
	Fronius		//This node is Fronius inverter
};
#endif /* PV_INVERTERS_H_ */
