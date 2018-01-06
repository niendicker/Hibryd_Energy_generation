
/*------------------------------------------------------------------
 * Headers
 * ----------------------------------------------------------------*/
#include <Arduino.h>
#include "main.h"
#include "pv_modbus.h"
#include "genset_modbus.h"
#include "keyboard.h"

/*------------------------------------------------------------------
 * External global synchronization variables
 * ----------------------------------------------------------------*/
//keyboard.h
extern volatile bool keyboard_flag_sync; //Keyboard some key was pressed synchronization flag
//pv_modbus.h
extern uint16_t pv_flag_sync; 			//Modbus new data available synchronization flag
//genset_modbus.h
extern uint16_t genset_flag_sync;		//Modbus new data available synchronization flag
//digital_inputs.h
extern volatile uint16_t digital_inputs_sync_flag;

/*------------------------------------------------------------------
 * Global variables
 * ----------------------------------------------------------------*/


/*------------------------------------------------------------------
 * SETUP
 * ----------------------------------------------------------------*/
void setup() {
	//Initialize all hardware
	hw_init();

	//Init PV system modbus interface
	pv_init_modbus(pv_default_slave_addr);

	//Init Genset modbus interface
	genset_init_modbus(genset_default_slave_addr);

	//Debug port
	Serial.begin(115200);
	Serial.println("--------------- SETUP -------------------");
	//On-board LED
	pinMode(LED_BUILTIN, OUTPUT);

	//REMOVE ////////////////////////////////////////////
	for(int i=0; i < pv_max_nodes; i++)
		pv_set_node_type(i, Sungrow);

	for (int i= 0; i < genset_max_nodes; i++)
		genset_set_node_type(i, Sices);
	/////////////////////////////////////////////////////
}

/*------------------------------------------------------------------
 * LOOP
 * ----------------------------------------------------------------*/
void loop() {
	//Resource management interval control
	static unsigned long prev_millis_1   = 0;
	static unsigned long prev_millis_10  = 0;
	static unsigned long prev_millis_100 = 0;
	static unsigned long prev_millis_200 = 0;
	static unsigned long prev_millis_500 = 0;

	//Main loop time
	static unsigned long main_loop_time_average_sum = 0;
	static unsigned long main_loop_time_average = 0;
	static unsigned long main_loop_iteraction_count = 0;
	static unsigned long prev_micros_main_loop = 0;
	unsigned long main_loop_time_us = 0;

//------------------- TIME LAPSE COMPUTATION -----------------------
	//Time lapse computation
	unsigned long currentMillis= millis();

	//Main loop - time lapse computation Microseconds
	unsigned long currentMicros= micros();
	main_loop_time_us= (unsigned long)(currentMicros - prev_micros_main_loop);
	prev_micros_main_loop= micros();

	//Main loop - time lapse average computation
	main_loop_time_average_sum+= main_loop_time_us;
	main_loop_iteraction_count++;

//------------------ RESOURCE MANAGEMENT 1ms ---------------------
	if ((unsigned long)(currentMillis - prev_millis_1) >= 1) {
		prev_millis_1= millis(); //Update before code can increase the accuracy?

		//Scheduler for modbus variables reading
		const uint8_t scheduler_read_genset= 	0x00;    //Read genset controller node
		const uint8_t scheduler_read_pv= 		0x01;    //Read PV inverter node

		static uint8_t scheduler= scheduler_read_genset; //Scheduler for node modbus read
		static uint8_t pv_node_read= 		0; 			 //Set pv node index to read
		static uint8_t genset_node_read= 	0;			 //Set genset node index to read
		switch(scheduler){
			case scheduler_read_genset:
				//Actual node modbus variables transactions was finished
				if(genset_read_modbus_variables(genset_node_read)){
					if(++genset_node_read >= genset_max_nodes) genset_node_read= 0;
					//Read next pv inverter
					scheduler= scheduler_read_pv;
				}
				break;
			case scheduler_read_pv:
				//Actual node modbus variables transactions was finished
				if(pv_read_modbus_variables(pv_node_read)){
					if(++pv_node_read >= pv_max_nodes) pv_node_read= 0;
					//Read next genset controller
					scheduler= scheduler_read_genset;
				}
				break;
		}

		//Manage digital inputs status
		//Keep this pooling time as low as possible
		if(digital_inputs_sync_flag){
			manage_digital_inputs();
		}
	}

//------------------ RESOURCE MANAGEMENT 10ms ---------------------
	if ((unsigned long)(currentMillis - prev_millis_10) >= 10) {
		prev_millis_10= millis(); //Update before code can increase the accuracy?

		//GENSETS NEW MODBUS VALUES
		if(genset_flag_sync){
			Serial.println("manage_genset_system()");
			manage_genset_system();
		}

		//PV SYSTEM NEW MODBUS VALUES
		if(pv_flag_sync){
			Serial.println("manage_pv_system()");
			manage_pv_system();
		}
}

//------------------ RESOURCE MANAGEMENT 100ms ---------------------
	if ((unsigned long)(currentMillis - prev_millis_100) >= 100) {
		prev_millis_100= millis(); //Update before code can increase the accuracy?

		//KEYBOARD
		if(keyboard_flag_sync){
			Serial.println("manage_keyboard()");
			manage_keyboard();
		}

	}

//------------------ RESOURCE MANAGEMENT 200ms ---------------------
	if ((unsigned long)(currentMillis - prev_millis_200) >= 200) {
		prev_millis_200= millis(); //Update before code can increase the accuracy?

	}

//------------------ RESOURCE MANAGEMENT 500ms ---------------------
	if ((unsigned long)(currentMillis - prev_millis_500) >= 500) {
		prev_millis_500= millis(); //Update before code can increase the accuracy?

		//LED run indication
		digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));

		//Main loop time average calculation
		main_loop_time_average= (main_loop_time_average_sum / main_loop_iteraction_count);
		main_loop_time_average_sum= 0;
		main_loop_iteraction_count= 0;
		//Serial.println(main_loop_time_average);
	}

}
