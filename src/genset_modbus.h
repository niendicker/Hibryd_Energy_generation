/*
 * genset_modbus.h
 *
 *  Created on: Dec 12, 2017
 *      Author: mniendicker
 */

#ifndef GENSET_MODBUS_H_
#define GENSET_MODBUS_H_

/*------------------------------------------------------------------
 * 						HEADERS
 * ----------------------------------------------------------------*/
#include "genset_controllers.h"
#include "lib/modbus_master.h"
#include "hal/board.h"
#include "rs485.h"

/*------------------------------------------------------------------
 *					GLOBAL CONSTANTS
 * ----------------------------------------------------------------*/
//Synchronization constants used for system control management
static const uint16_t genset_sync_none  		= 0x0000; //No comunication ongoing
static const uint16_t genset_sync_active_power  = 0x0001; //New active power read from any node
static const uint16_t genset_sync_nominal_power = 0x0002; //New nominal power read from any node
static const uint16_t genset_sync_comm_status   = 0x0004; //New communication status from any node

//Node communication status control
static const uint8_t genset_min_comm_errors= 0x00; //Pass from timeout to connected
static const uint8_t genset_max_comm_errors= 0x03; //Pass from timeout to disconnected


/*------------------------------------------------------------------
 * 					GLOBAL VARIABLES
 * ----------------------------------------------------------------*/
//Modbus Genset interface
ModbusMaster genset_node;

//Modbus variables for each node
typedef struct{
	volatile uint32_t active_power;  //Actual deliverable power (ADP)
	volatile uint32_t nominal_power; //Deliverable power (DP)
}_genset_node_modbus_data;

//Each node information
typedef struct{
	//Define the node model - Sices
	genset_controllers node_type;
	//Node Modbus address
	uint8_t node_addr;

	//Communication status
	comm_status node_communication_status;
	uint8_t node_comm_error_counter;

	_genset_node_modbus_data node_modbus_variables;

}_genset_modbus_node;

//All nodes access
_genset_modbus_node genset_nodes[genset_max_nodes];

//Gensets total calculation
uint32_t genset_active_power_total; //Actual deliverable power (ADPt) - Sum of all gensets
uint32_t genset_nominal_power_total; //Deliverable power total (DPt) - Sum of all gensets

//Modbus new data available synchronization flag
uint16_t genset_flag_sync;

//Modbus transactions synchronization
uint8_t genset_global_node_index; 	//Used for update node communication status
uint16_t genset_variable_modbus;  	//Identify the modbus transaction for successful transaction
									//callback function and for main loop scheduler


/*------------------------------------------------------------------
 * 					PROTOTYPES
 * ----------------------------------------------------------------*/
/*------------------------------------------------------------------
 * Initialize Genset modbus interface
 * If @addr= 0 the default slave address (1) is loaded
 *----------------------------------------------------------------*/
void genset_init_modbus(uint8_t addr);

/*------------------------------------------------------------------
 * Set @addr of @node_index
 * ----------------------------------------------------------------*/
void genset_set_node_addr(uint8_t node_index, uint8_t addr);

/*------------------------------------------------------------------
 * Set node type (Sices, etc) of @node_index
 * ----------------------------------------------------------------*/
void genset_set_node_type(uint8_t node_index, genset_controllers type);

/*------------------------------------------------------------------
 * Set the slave address for next query
 *----------------------------------------------------------------*/
void genset_set_slave_addr(uint8_t addr);

/*------------------------------------------------------------------
 *Set the timeout for Modbus answer
 *----------------------------------------------------------------*/
void genset_set_timeout(uint16_t new_timeout);

/*------------------------------------------------------------------
 *Callback function for all successful modbus transactions
 *----------------------------------------------------------------*/
void genset_successful_transaction();

/*------------------------------------------------------------------
 *Callback function for all timeout modbus transactions
 *----------------------------------------------------------------*/
void genset_timeout_transaction();

/*-----------------------------------------------------------------
 * Read active power from slave addr
 * ----------------------------------------------------------------*/
uint8_t genset_read_active_power(uint8_t addr);

/*-----------------------------------------------------------------
 * Read nominal power from specified node
 * ----------------------------------------------------------------*/
uint8_t genset_read_nominal_power(uint8_t node_index);

/*------------------------------------------------------------------
 *Update node communication status
 *@sucess define if the last modbus transaction was successful
 *----------------------------------------------------------------*/
void genset_update_communication_status(uint8_t node_index, bool sucess);

/*------------------------------------------------------------------
 *Read modbus variables from gensets controllers
 *Return true if transaction is finished (with success or not)
 *----------------------------------------------------------------*/
bool genset_read_modbus_variables(uint8_t genset_node_read);

/*------------------------------------------------------------------
 *Manage modbus variables for gensets system - called from main loop
 *----------------------------------------------------------------*/
void manage_genset_system();


/*------------------------------------------------------------------
* 					FUNCTIONS DEFINITION
* ----------------------------------------------------------------*/
/*------------------------------------------------------------------
 * Initialize Genset modbus interface
 * If @addr= 0 the default slave address (1) is loaded
 *----------------------------------------------------------------*/
void genset_init_modbus(uint8_t addr){
	if(addr == 0) //Default
		genset_node.begin(genset_default_slave_addr, genset_serial_port);
	else
		genset_node.begin(addr, genset_serial_port);

	//Called before any Modbus query - Set the driver in TX mode
	genset_node.preTransmission(pre_tx_rs485_genset);
	//Called after any Modbus query - Set the driver in RX mode
	genset_node.postTransmission(post_tx_rs485_genset);
	//Called on successful transaction
	genset_node.querySuccess(genset_successful_transaction);
	//Called on transaction timeout
	genset_node.queryTimeout(genset_timeout_transaction);

	//Init nodes information
	for(int i= 0; i < genset_max_nodes; i++){
		genset_set_node_addr(i, (i + 1));
		genset_set_node_type(i, NoGenset);
		genset_nodes[i].node_communication_status= disconnected;
		genset_nodes[i].node_comm_error_counter= 0;
		genset_nodes[i].node_modbus_variables.active_power= 0x0000;
		genset_nodes[i].node_modbus_variables.nominal_power= 0x0000;
	}

	//Synchronization variables
	genset_global_node_index= 0;
	genset_variable_modbus= genset_sync_none;

	//Gensets total calculation
	genset_active_power_total= 0; //Actual deliverable power (ADPt) - Sum of all gensets
	genset_nominal_power_total= 0; //Deliverable power total (DPt) - Sum of all gensets

	//Modbus new data available synchronization flag
	genset_flag_sync= genset_sync_none;
}

/*------------------------------------------------------------------
 * Set @addr of @node_index
 * ----------------------------------------------------------------*/
void genset_set_node_addr(uint8_t node_index, uint8_t addr){
	if(node_index >= genset_max_nodes)
		return;

	genset_nodes[node_index].node_addr= addr;
}

/*------------------------------------------------------------------
 * Set node type (Sices, etc) of @node_index
 * ----------------------------------------------------------------*/
void genset_set_node_type(uint8_t node_index, genset_controllers type){
	if(node_index >= genset_max_nodes)
		return;

	genset_nodes[node_index].node_type= type;
}

/*------------------------------------------------------------------
 * Set the slave address for next query
 *----------------------------------------------------------------*/
void genset_set_slave_addr(uint8_t addr){
	if(addr == 0) //Default
		genset_node.setSlaveAddr(genset_default_slave_addr);
	else
		genset_node.setSlaveAddr(addr);
}

/*------------------------------------------------------------------
 *Set the timeout for Modbus answer
 *----------------------------------------------------------------*/
void genset_set_timeout(uint16_t new_timeout){
	genset_node.setTimeout(new_timeout);
}

/*------------------------------------------------------------------
 *Read modbus variables from gensets controllers
 *Return true if transaction is finished (with success or not)
 *----------------------------------------------------------------*/
bool genset_read_modbus_variables(uint8_t genset_node_read){
	const uint8_t scheduler_active_power= 	0x02; //Read active power from node [PV or Genset]
	const uint8_t scheduler_nominal_power= 	0x04; //Read nominal power from node [PV or Genset]

	static uint8_t function_scheduler= 	scheduler_active_power; //Scheduler for each node variable read

	if(function_scheduler == scheduler_active_power){
		uint8_t result= genset_read_active_power(genset_node_read);
		//Previous read_active_power transaction has finished
		if((result == transaction_idle) || (result == transaction_timeout)){
			//Read next variable
			function_scheduler= scheduler_nominal_power;
		}
	}
	else if(function_scheduler == scheduler_nominal_power){
		uint8_t result= genset_read_nominal_power(genset_node_read);
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
 *Manage modbus variables for gensets system - called from main loop
 *----------------------------------------------------------------*/
void manage_genset_system(){
	//New nominal power - recalculate total nominal power (DPt)
	if(genset_flag_sync & genset_sync_nominal_power){
		genset_nominal_power_total= 0x00000000;
		for(uint8_t i= 0; i < genset_max_nodes; i++){
			genset_nominal_power_total+= genset_nodes[i].node_modbus_variables.nominal_power;
		}
		genset_flag_sync&= ~genset_sync_nominal_power; //Reset flag
	}
	//New active power - recalculate total active power (ADPt)
	if(genset_flag_sync & genset_sync_active_power){
		genset_active_power_total= 0x00000000;
		for(uint8_t i= 0; i < genset_max_nodes; i++){
			genset_active_power_total+= genset_nodes[i].node_modbus_variables.active_power;
		}
		genset_flag_sync&= ~genset_sync_active_power; //Reset flag
	}
	//New communication status - some node has the communication status changed
	if(genset_flag_sync & genset_sync_comm_status){
		//Call some function
		genset_flag_sync&= ~genset_sync_comm_status; //Reset flag
	}
}

/*------------------------------------------------------------------
 *Callback function for all successful modbus transactions
 *----------------------------------------------------------------*/
void genset_successful_transaction(){
	if(genset_variable_modbus == genset_sync_active_power){
		uint32_t active_power= 0x00000000;
		uint16_t register_low= genset_node.getResponseBuffer(0x00);
		uint16_t register_high= genset_node.getResponseBuffer(0x01);

		//Recovery the entire value
		active_power|= register_high;
		active_power<<= 8;
		active_power|= register_low;

		//Update Modbus variable
		genset_nodes[genset_global_node_index].node_modbus_variables.active_power= active_power;

		//Update communication status - transaction success
		genset_update_communication_status(genset_global_node_index, true);

		//Indicate that there are new active power for some node
		genset_flag_sync|= genset_sync_active_power;

		//Reset flag
		genset_variable_modbus&= ~genset_sync_active_power;
	}
	else if(genset_variable_modbus == genset_sync_nominal_power){
		uint32_t nominal_power= 0x00000000;
		uint16_t register_low= genset_node.getResponseBuffer(0x00);
		uint16_t register_high= genset_node.getResponseBuffer(0x01);

		//Recovery the entire value
		nominal_power|= register_high;
		nominal_power<<= 8;
		nominal_power|= register_low;

		//Update Modbus variable
		genset_nodes[genset_global_node_index].node_modbus_variables.nominal_power= nominal_power;

		//Update communication status - transaction success
		genset_update_communication_status(genset_global_node_index, true);

		//Indicate that there are new nominal power for some node
		genset_flag_sync|= genset_sync_nominal_power;

		//Reset flag
		genset_variable_modbus&= ~genset_sync_nominal_power;
	}
}

/*------------------------------------------------------------------
 *Callback function for all timeout modbus transactions
 *----------------------------------------------------------------*/
void genset_timeout_transaction(){
	//Update communication status - transaction fail
	genset_update_communication_status(genset_global_node_index, false);

	Serial.println("------------ Genset Timeout -----------------");
	Serial.println(genset_global_node_index);

	if(genset_variable_modbus == genset_sync_active_power){
		//Reset flag
		genset_variable_modbus&= ~genset_sync_active_power;
	}
	else if(genset_variable_modbus == genset_sync_nominal_power){
		//Reset flag
		genset_variable_modbus&= ~genset_sync_nominal_power;
	}
}

/*-----------------------------------------------------------------
 * Read active power from slave addr
 * ----------------------------------------------------------------*/
uint8_t genset_read_active_power(uint8_t node_index){
	//Verifies the index
	if(node_index >= genset_max_nodes)
			return(transaction_timeout);

	uint16_t register_to_read= 0;
	uint8_t number_of_registers= 0;

	//Set the destination node address
	genset_node.setSlaveAddr(genset_nodes[node_index].node_addr);

	//Set destination Modbus registers and number of registers to be read
	switch (genset_nodes[node_index].node_type) {
		case NoGenset:
				return(transaction_timeout);
			break;
		case Sices:
				register_to_read= Sices::active_power;
				number_of_registers= Sices::active_power_nr;
			break;
		default:
				return(transaction_timeout);
			break;
	}

	genset_global_node_index= node_index; 				//Signaling the node that is communicating
	genset_variable_modbus|= genset_sync_active_power; 	//Signaling the variable that is being read
	//Non-blocking function
	return (genset_node.readInputRegisters(register_to_read, number_of_registers));
}

/*-----------------------------------------------------------------
 * Read nominal power from specified node
 * ----------------------------------------------------------------*/
uint8_t genset_read_nominal_power(uint8_t node_index){
	//Verifies the index
	if(node_index >= genset_max_nodes)
			return(transaction_timeout);

	uint16_t register_to_read= 0;
	uint8_t number_of_registers= 0;

	//Set the destination node address
	genset_node.setSlaveAddr(genset_nodes[node_index].node_addr);

	//Set Modbus start register and number of registers to be read
	switch (genset_nodes[node_index].node_type) {
		case NoInverter:
				return(transaction_timeout);
			break;
		case Sices:
				register_to_read= Sices::nominal_power;
				number_of_registers= Sices::nominal_power_nr;
			break;
		default:
				return(transaction_timeout);
			break;
	}

	genset_global_node_index= node_index; 				//Save the node that is communicating
	genset_variable_modbus|= genset_sync_nominal_power; //Set the variable that is being read
	//Non-blocking function
	return(genset_node.readInputRegisters(register_to_read, number_of_registers));
}


/*------------------------------------------------------------------
 *Update node communication status
 *@sucess define if the last modbus transaction was successful
 *----------------------------------------------------------------*/
void genset_update_communication_status(uint8_t node_index, bool sucess){
	switch (genset_nodes[node_index].node_communication_status) {
		case connected:
			if(!sucess){
				genset_nodes[node_index].node_comm_error_counter++;
				genset_nodes[node_index].node_communication_status= timeout;
				genset_flag_sync|= genset_sync_comm_status;
			}
			break;
		case timeout:
			if(sucess){
				if(--genset_nodes[node_index].node_comm_error_counter == genset_min_comm_errors){
					genset_nodes[node_index].node_communication_status= connected;
					genset_flag_sync|= genset_sync_comm_status;
				}
			}
			else{
				if(++genset_nodes[node_index].node_comm_error_counter == genset_max_comm_errors){
					genset_nodes[node_index].node_communication_status= disconnected;
					genset_flag_sync|= genset_sync_comm_status;
				}
			}
			break;
		case disconnected:
				if(sucess){
					genset_nodes[node_index].node_comm_error_counter--;
					genset_nodes[node_index].node_communication_status= timeout;
					genset_flag_sync|= genset_sync_comm_status;
				}
			break;
	}
}


#endif /* GENSET_MODBUS_H_ */
