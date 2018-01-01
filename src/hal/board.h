/*
 * board.h
 *
 *  Created on: Dec 12, 2017
 *      Author: mniendicker
 *
 *     HAL: Board definition
 */

#ifndef BOARD_H_
#define BOARD_H_

#define ARDUINO_DUE

//Default baud rate used for RS485 serial ports
static const uint16_t default_baud_rate= 115200;

//=========================RS485 - PV SYSTEM SERIAL COMMUNICATION=========================//
//19 (RX) -  18 (TX)
#define pv_serial_port Serial1
static const uint8_t pv_serial_port_re=	22;
static const uint8_t pv_serial_port_de=	23;
//=========================RS485 - PV SYSTEM SERIAL COMMUNICATION=========================//

//=========================RS485 - GENSET SERIAL COMMUNICATION============================//
//17 (RX) - 16 (TX)
#define genset_serial_port Serial2
static const uint8_t genset_serial_port_re=	24;
static const uint8_t genset_serial_port_de=	25;
//=========================RS485 - GENSET SERIAL COMMUNICATION============================//

//======================================DIGITAL INPUTS===================================//
static const uint8_t digital_input1= 26;
static const uint8_t digital_input2= 27;
static const uint8_t digital_input3= 28;
static const uint8_t digital_input4= 29;
//======================================DIGITAL INPUTS===================================//

//=====================================DIGITAL OUTPUTS===================================//
static const uint8_t digital_output1= 30;
static const uint8_t digital_output2= 31;
static const uint8_t digital_output3= 32;
static const uint8_t digital_output4= 33;
//=====================================DIGITAL OUTPUTS===================================//

//=====================================ANALOGUE INPUTS===================================//
static const uint8_t analogue_input1= A0;
static const uint8_t analogue_input2= A1;
static const uint8_t analogue_input3= A2;
static const uint8_t analogue_input4= A3;
//=====================================ANALOGUE INPUTS===================================//

//=======================================KEY BUTTONS=====================================//
static const uint8_t button_left  =	34;
static const uint8_t button_right =	35;
static const uint8_t button_down  =	36;
static const uint8_t button_up    =	37;
static const uint8_t button_enter =	38;
static const uint8_t button_esc   =	39;
//=======================================KEY BUTTONS=====================================//

#endif /* BOARD_H_ */
