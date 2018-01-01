/*
 * rs485.h
 *
 *  Created on: Dec 15, 2017
 *      Author: mniendicker
 */

#ifndef RS485_H_
#define RS485_H_
/*------------------------------------------------------------------
 * 						HEADERS
 * ----------------------------------------------------------------*/
#include "hal/board.h"


/*------------------------------------------------------------------
 * 					GLOBAL CONSTANTS
 * ----------------------------------------------------------------*/
/*------------------------------------------------------------------
 * RS-485 Driver Enable and Receiver Enable control
 * ----------------------------------------------------------------*/
static const uint8_t rs485_tx= 1; //RE= 1 and DE= 1
static const uint8_t rs485_rx= 0; //RE= 0 and DE= 0

//Default node address for PV system inverters
static const uint8_t pv_default_slave_addr= 1;
////Default node address for the genset controllers
static const uint8_t genset_default_slave_addr= 1;

//Communication status of each node
enum comm_status{
	disconnected,
	connected,
	timeout
};


/*------------------------------------------------------------------
 * 					GLOBAL VARIABLES
 * ----------------------------------------------------------------*/


/*------------------------------------------------------------------
 * 					PROTOTYPES
 * ----------------------------------------------------------------*/
/*------------------------------------------------------------------
 * Initialize the hardware - PV system
 * ----------------------------------------------------------------*/
void init_rs485_pv();

/*------------------------------------------------------------------
 * Setup the hardware before transmission - called before any Modbus
 * request.
 * Set the RS-485 driver in transmission mode
 * ----------------------------------------------------------------*/
void pre_tx_rs485_pv();

/*------------------------------------------------------------------
 * Setup the hardware after transmission - called after any Modbus
 * request.
 * Set the RS-485 driver in receive mode
 * ----------------------------------------------------------------*/
void post_tx_rs485_pv();

/*------------------------------------------------------------------
 * Initialize the hardware - Gensets
 * ----------------------------------------------------------------*/
void init_rs485_genset();

/*------------------------------------------------------------------
 * Setup the hardware before transmission - called before any Modbus
 * request.
 * Set the RS-485 driver in transmission mode
 * ----------------------------------------------------------------*/
void pre_tx_rs485_genset();

/*------------------------------------------------------------------
 * Setup the hardware after transmission - called after any Modbus
 * request.
 * Set the RS-485 driver in receive mode
 * ----------------------------------------------------------------*/
void post_tx_rs485_genset();


 /*------------------------------------------------------------------
 * 					FUNCTIONS DEFINITION
 * ----------------------------------------------------------------*/
/*------------------------------------------------------------------
 * Initialize the hardware - PV system
 * ----------------------------------------------------------------*/
void init_rs485_pv(){
	//PV system communication
	pv_serial_port.begin(default_baud_rate);
	pinMode(pv_serial_port_de, OUTPUT);
	pinMode(pv_serial_port_re, OUTPUT);
	//Init in receive mode
	digitalWrite(pv_serial_port_de, rs485_rx);
	digitalWrite(pv_serial_port_re, rs485_rx);
}

/*------------------------------------------------------------------
 * Setup the hardware before transmission - called before any Modbus
 * request.
 * Set the RS-485 driver in transmission mode
 * ----------------------------------------------------------------*/
void pre_tx_rs485_pv(){
	digitalWrite(pv_serial_port_de, rs485_tx);
	digitalWrite(pv_serial_port_re, rs485_tx);
}

/*------------------------------------------------------------------
 * Setup the hardware after transmission - called after any Modbus
 * request.
 * Set the RS-485 driver in receive mode
 * ----------------------------------------------------------------*/
void post_tx_rs485_pv(){
	digitalWrite(pv_serial_port_de, rs485_rx);
	digitalWrite(pv_serial_port_re, rs485_rx);
}



/*------------------------------------------------------------------
 * Initialize the hardware - Gensets
 * ----------------------------------------------------------------*/
void init_rs485_genset(){
	//Gensets communication
	genset_serial_port.begin(default_baud_rate);
	pinMode(genset_serial_port_de, OUTPUT);
	pinMode(genset_serial_port_re, OUTPUT);
	//Init in receive mode
	digitalWrite(genset_serial_port_de, 0);
	digitalWrite(genset_serial_port_re, 0);
}

/*------------------------------------------------------------------
 * Setup the hardware before transmission - called before any Modbus
 * request.
 * Set the RS-485 driver in transmission mode
 * ----------------------------------------------------------------*/
void pre_tx_rs485_genset(){
	digitalWrite(genset_serial_port_de, rs485_tx);
	digitalWrite(genset_serial_port_re, rs485_tx);
}

/*------------------------------------------------------------------
 * Setup the hardware after transmission - called after any Modbus
 * request.
 * Set the RS-485 driver in receive mode
 * ----------------------------------------------------------------*/
void post_tx_rs485_genset(){
	digitalWrite(genset_serial_port_de, rs485_rx);
	digitalWrite(genset_serial_port_re, rs485_rx);
}


#endif /* RS485_H_ */
