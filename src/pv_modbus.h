/*
 * modbus.h
 *
 *  Created on: Dec 12, 2017
 *      Author: mniendicker
 */

#ifndef PV_MODBUS_H_
#define PV_MODBUS_H_


/*------------------------------------------------------------------
 * 						HEADERS
 * ----------------------------------------------------------------*/
#include "lib/modbus_master.h"
#include "hal/sw_init.h"
#include "pv_inverters.h"
#include "hal/board.h"
#include "rs485.h"


/*------------------------------------------------------------------
 * 					GLOBAL CONSTANTS
 * ----------------------------------------------------------------*/
//Synchronization constants used for system control management
static const uint16_t pv_sync_none  		= 0x0000; //No comunication ongoing
static const uint16_t pv_sync_active_power  = 0x0001; //New active power read from any node
static const uint16_t pv_sync_nominal_power = 0x0002; //New nominal power read from any node
static const uint16_t pv_sync_comm_status   = 0x0004; //New communication status from any node

//Node communication status control
static const uint8_t pv_min_comm_errors= 0x00; //Pass from timeout to connected
static const uint8_t pv_max_comm_errors= 0x03; //Pass from timeout to disconnected


/*------------------------------------------------------------------
 * 					GLOBAL VARIABLES
 * ----------------------------------------------------------------*/
//Modbus master interface - PV System
ModbusMaster pv_node;

//Modbus variables for each node
typedef struct{
	volatile uint32_t active_power;  //Actual deliverable power (ADP)
	volatile uint32_t nominal_power; //Deliverable power (DP)
}_pv_node_modbus_data;

//Each node information
typedef struct{
	//Define the node model - Sungrow, ABB, etc
	inverters node_type;
	//Node Modbus address
	uint8_t node_addr;

	//Communication status
	comm_status node_communication_status;
	uint8_t node_comm_error_counter;

	_pv_node_modbus_data node_modbus_variables;

}_pv_modbus_node;

//All nodes access
_pv_modbus_node pv_nodes[pv_max_nodes];

//PV system total calculation
uint32_t pv_active_power_total; //Actual deliverable power (ADPt) - Sum of all inverters
uint32_t pv_nominal_power_total; //Deliverable power total (DPt) - Sum of all inverters

//Modbus new data available synchronization flag
uint16_t pv_flag_sync;

//Modbus transactions synchronization
bool pv_comm_ongoing;			//Modbus transaction ongoing with some node
uint8_t pv_global_node_index; 	//Used for update node communication status
uint16_t pv_variable_modbus; 	//Used to identify the modbus transaction for successful transaction
								//callback function and for main loop scheduler


/*------------------------------------------------------------------
 * 					PROTOTYPES
 * ----------------------------------------------------------------*/
/*------------------------------------------------------------------
 * Initialize PV system modbus interface
 * If @addr= 0 the default slave address (1) is loaded
 * ----------------------------------------------------------------*/
void pv_init_modbus(uint8_t addr);

/*------------------------------------------------------------------
 * Set @addr of @node_index
 * ----------------------------------------------------------------*/
void pv_set_node_addr(uint8_t node_index, uint8_t addr);

/*------------------------------------------------------------------
 * Set node type (ABB, Fronius, etc) of @node_index
 * ----------------------------------------------------------------*/
void pv_set_node_type(uint8_t node_index, inverters type);

/*------------------------------------------------------------------
 *Set the timeout for all Modbus transactions
 *----------------------------------------------------------------*/
void pv_set_timeout	(uint16_t new_timeout);

/*------------------------------------------------------------------
 *Read modbus variables from PV system
 *Return true if transaction is finished (with success or not)
 *----------------------------------------------------------------*/
bool pv_read_modbus_variables(uint8_t pv_node_read);

/*------------------------------------------------------------------
 *Read modbus variables from PV systemS
 *----------------------------------------------------------------*/
void pv_read_modbus_variables();

/*------------------------------------------------------------------
 *Manage modbus variables for PV system - called from main loop
 *----------------------------------------------------------------*/
void manage_pv_system();

/*------------------------------------------------------------------
 *Callback function for all successful modbus transactions
 *----------------------------------------------------------------*/
void pv_successful_transaction();

/*------------------------------------------------------------------
 *Callback function for all timeout modbus transactions
 *----------------------------------------------------------------*/
void pv_timeout_transaction();

/*-----------------------------------------------------------------
 * Read active power from specified node
 * ----------------------------------------------------------------*/
uint8_t pv_read_active_power(uint8_t node_index);

/*-----------------------------------------------------------------
 * Read nominal power from specified node
 * Return the transaction status:
 * - Idle
 * - Receveing
 * - Timeout
 * ----------------------------------------------------------------*/
uint8_t pv_read_nominal_power(uint8_t node_index);

/*------------------------------------------------------------------
 *Update node communication status
 *@sucess define if the last modbus transaction was successful
 *----------------------------------------------------------------*/
void pv_update_communication_status(uint8_t node_index, bool sucess);




/*------------------------------------------------------------------
* 					FUNCTIONS DEFINITION
* ----------------------------------------------------------------*/
/*------------------------------------------------------------------
 * Initialize PV system modbus interface
 * If @addr= 0 the default slave address (1) is loaded
 * ----------------------------------------------------------------*/
void pv_init_modbus(uint8_t addr){
	if(addr == 0) //Default
		pv_node.begin(pv_default_slave_addr, pv_serial_port);
	else
		pv_node.begin(addr, pv_serial_port);

	//Called before any Modbus query - Set the driver in TX mode
	pv_node.preTransmission(pre_tx_rs485_pv);
	//Called after any Modbus query - Set the driver in RX mode
	pv_node.postTransmission(post_tx_rs485_pv);
	//Called on successful transaction
	pv_node.querySuccess(pv_successful_transaction);
	//Called on transaction timeout
	pv_node.queryTimeout(pv_timeout_transaction);

	//Init nodes information
	for(int i= 0; i < pv_max_nodes; i++){
		pv_set_node_addr(i, (i + 1));
		pv_set_node_type(i, NoInverter);
		pv_nodes[i].node_communication_status= disconnected;
		pv_nodes[i].node_comm_error_counter= 0;
		pv_nodes[i].node_modbus_variables.active_power= 0x0000;
		pv_nodes[i].node_modbus_variables.nominal_power= 0x0000;
	}

	//Synchronization variables
	pv_comm_ongoing= false;
	pv_global_node_index= 0;
	pv_variable_modbus= 0;

	//PV system total calculation
	pv_active_power_total= 0;  //Actual deliverable power (ADPt) - Sum of all inverters
	pv_nominal_power_total= 0; //Deliverable power total (DPt) - Sum of all inverters

	//Modbus new data available synchronization flag
	pv_flag_sync= 0;
}

/*------------------------------------------------------------------
 * Set @addr of @node_index
 * ----------------------------------------------------------------*/
void pv_set_node_addr(uint8_t node_index, uint8_t addr){
	if(node_index >= pv_max_nodes)
		return;

	pv_nodes[node_index].node_addr= addr;
}

/*------------------------------------------------------------------
 * Set node type (ABB, Fronius, etc) of @node_index
 * ----------------------------------------------------------------*/
void pv_set_node_type(uint8_t node_index, inverters type){
	if(node_index >= pv_max_nodes)
		return;

	pv_nodes[node_index].node_type= type;
}

/*------------------------------------------------------------------
 *Set the timeout for Modbus answer
 *----------------------------------------------------------------*/
void pv_set_timeout(uint16_t new_timeout){
	pv_node.setTimeout(new_timeout);
}

/*------------------------------------------------------------------
 *Read modbus variables from PV system
 *Return true if transaction is finished (with success or not)
 *----------------------------------------------------------------*/
bool pv_read_modbus_variables(uint8_t pv_node_read){
	const uint8_t scheduler_active_power= 	0x02; //Read active power from node [PV or Genset]
	const uint8_t scheduler_nominal_power= 	0x04; //Read nominal power from node [PV or Genset]

	static uint8_t function_scheduler= 	scheduler_active_power; //Scheduler for each node variable read

	if(function_scheduler == scheduler_active_power){
		uint8_t result= pv_read_active_power(pv_node_read);
		//Previous read_active_power transaction has finished
		if((result == transaction_idle) || (result == transaction_timeout)){
			//Read next variable
			function_scheduler= scheduler_nominal_power;
		}
	}
	else if(function_scheduler == scheduler_nominal_power){
		uint8_t result= pv_read_nominal_power(pv_node_read);
		if((result == transaction_idle) || (result == transaction_timeout)){
			//Next time read active power
			function_scheduler= scheduler_active_power;
			//Variables read
			return(true);
		}
	}

	return(false);
}

/*------------------------------------------------------------------
 *Manage modbus variables for PV system - called from main loop
 *----------------------------------------------------------------*/
void manage_pv_system(){
	//New nominal power - recalculate total nominal power (DPt)
	if(pv_flag_sync & pv_sync_nominal_power){
		pv_nominal_power_total= 0x00000000;
		for(uint8_t i= 0; i < pv_max_nodes; i++){
			pv_nominal_power_total+= pv_nodes[i].node_modbus_variables.nominal_power;
		}
		pv_flag_sync&= ~pv_sync_nominal_power; //Reset flag
	}
	//New active power - recalculate total active power (ADPt)
	if(pv_flag_sync & pv_sync_active_power){
		pv_active_power_total= 0x00000000;
		for(uint8_t i= 0; i < pv_max_nodes; i++){
			pv_active_power_total+= pv_nodes[i].node_modbus_variables.active_power;
		}
		pv_flag_sync&= ~pv_sync_active_power; //Reset flag
	}
	//New communication status - some node has the communication status changed
	if(pv_flag_sync & pv_sync_comm_status){
		//Call some function
		pv_flag_sync&= ~pv_sync_comm_status; //Reset flag
	}
}

/*------------------------------------------------------------------
 *Callback function for all successful modbus transactions
 *----------------------------------------------------------------*/
void pv_successful_transaction(){
	if(pv_variable_modbus == pv_sync_active_power){
		uint32_t active_power= 0x00000000;
		uint16_t register_low= pv_node.getResponseBuffer(0x00);
		uint16_t register_high= pv_node.getResponseBuffer(0x01);

		//Recovery the entire value
		active_power|= register_high;
		active_power<<= 8;
		active_power|= register_low;

		//Update Modbus variable
		pv_nodes[pv_global_node_index].node_modbus_variables.active_power= active_power;

		//Update communication status - transaction success
		pv_update_communication_status(pv_global_node_index, true);

		//Indicate that there are new active power for some node
		pv_flag_sync|= pv_sync_active_power;

		//Reset flag
		pv_variable_modbus&= ~pv_sync_active_power;
	}
	else if(pv_variable_modbus == pv_sync_nominal_power){
		uint32_t nominal_power= 0x00000000;
		uint16_t register_low= pv_node.getResponseBuffer(0x00);
		uint16_t register_high= pv_node.getResponseBuffer(0x01);

		//Recovery the entire value
		nominal_power|= register_high;
		nominal_power<<= 8;
		nominal_power|= register_low;

		//Update Modbus variable
		pv_nodes[pv_global_node_index].node_modbus_variables.nominal_power= nominal_power;

		//Update communication status - transaction success
		pv_update_communication_status(pv_global_node_index, true);

		//Indicate that there are new nominal power for some node
		pv_flag_sync|= pv_sync_nominal_power;

		//Reset flag
		pv_variable_modbus&= ~pv_sync_nominal_power;
	}

	//Enable new transaction
	pv_comm_ongoing= false;
}

/*------------------------------------------------------------------
 *Callback function for all timeout modbus transactions
 *----------------------------------------------------------------*/
void pv_timeout_transaction(){
	//Update communication status - transaction fail
	pv_update_communication_status(pv_global_node_index, false);

	Serial.println("------------ PV Timeout -----------------");
	Serial.println(pv_global_node_index);

	if(pv_variable_modbus == pv_sync_active_power){
		//Reset flag
		pv_variable_modbus&= ~pv_sync_active_power;
	}
	else if(pv_variable_modbus == pv_sync_nominal_power){
		//Reset flag
		pv_variable_modbus&= ~pv_sync_nominal_power;
	}

	//Enable new transaction
	pv_comm_ongoing= false;
}

/*-----------------------------------------------------------------
 * Read active power from specified node
 * ----------------------------------------------------------------*/
uint8_t pv_read_active_power(uint8_t node_index){
	//Verifies the index
	if(node_index >= pv_max_nodes)
			return(transaction_timeout);

	uint16_t register_to_read= 0;
	uint8_t number_of_registers= 0;

	//Set the destination node address
	pv_node.setSlaveAddr(pv_nodes[node_index].node_addr);

	//Set destination Modbus registers and number of registers to be read
	switch (pv_nodes[node_index].node_type) {
		case NoInverter:
				return(transaction_timeout);
			break;

		case Sungrow:
				register_to_read= Sungrow::active_power;
				number_of_registers= Sungrow::active_power_nr;
			break;
		case ABB:
				return(transaction_timeout);
			break;
		case Fronius:
				return(transaction_timeout);
			break;
		default:
				return(transaction_timeout);
			break;
	}

	pv_comm_ongoing= true;
	pv_global_node_index= node_index; 		//Signaling the node that is communicating
	pv_variable_modbus|= pv_sync_active_power; 	//Signaling the variable that is being read

	//Non-blocking function
	return(pv_node.readInputRegisters(register_to_read, number_of_registers));


}

/*-----------------------------------------------------------------
 * Read nominal power from specified node
 * ----------------------------------------------------------------*/
uint8_t pv_read_nominal_power(uint8_t node_index){
	//Verifies the index
	if(node_index >= pv_max_nodes)
			return(transaction_timeout);

	uint16_t register_to_read= 0;
	uint8_t number_of_registers= 0;

	//Set the destination node address
	pv_node.setSlaveAddr(pv_nodes[node_index].node_addr);

	//Set destination Modbus registers and number of registers to be read
	switch (pv_nodes[node_index].node_type) {
		case NoInverter:
				return(transaction_timeout);
			break;

		case Sungrow:
				register_to_read= Sungrow::nominal_power;
				number_of_registers= Sungrow::nominal_power_nr;
			break;
		case ABB:
				return(transaction_timeout);
			break;
		case Fronius:
				return(transaction_timeout);
			break;
		default:
				return(transaction_timeout);
			break;
	}

	pv_comm_ongoing= true;
	pv_global_node_index= node_index; 		//Signaling the node that is communicating
	pv_variable_modbus|= pv_sync_nominal_power; //Signaling the variable that is being read

	return(pv_node.readInputRegisters(register_to_read, number_of_registers));
}

/*------------------------------------------------------------------
 *Update node communication status
 *@sucess define if the last modbus transaction was successful
 *----------------------------------------------------------------*/
void pv_update_communication_status(uint8_t node_index, bool sucess){
	switch (pv_nodes[node_index].node_communication_status) {
		case connected:
			if(!sucess){
				pv_nodes[node_index].node_comm_error_counter++;
				pv_nodes[node_index].node_communication_status= timeout;
				pv_flag_sync|= pv_sync_comm_status;
			}
			break;
		case timeout:
			if(sucess){
				if(--pv_nodes[node_index].node_comm_error_counter == pv_min_comm_errors){
					pv_nodes[node_index].node_communication_status= connected;
					pv_flag_sync|= pv_sync_comm_status;
				}
			}
			else{
				if(++pv_nodes[node_index].node_comm_error_counter == pv_max_comm_errors){
					pv_nodes[node_index].node_communication_status= disconnected;
					pv_flag_sync|= pv_sync_comm_status;
				}
			}
			break;
		case disconnected:
				if(sucess){
					pv_nodes[node_index].node_comm_error_counter--;
					pv_nodes[node_index].node_communication_status= timeout;
					pv_flag_sync|= pv_sync_comm_status;
				}
			break;
	}
}

#endif /* PV_MODBUS_H_ */
