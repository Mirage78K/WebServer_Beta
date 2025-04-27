/*
 * parameter.h
 *
 *  Created on: Feb 22, 2024
 *      Author: Ahar
 */

#ifndef PARAMETER_H_
#define PARAMETER_H_

#include <stdint.h>

#define Max_number_stream 				 	3
#define Max_number_station 					18
#define Max_number_channel_input 			30
#define Max_number_Com 						3
#define Max_number_channel_master_modbus 	18
#define Max_number_modbus_user_define 		200
#define Max_number_digital_input	 		4
#define Max_number_digital_output	 		4
#define Max_number_analog_output	 		4
#define Max_number_analog_input		 		6
#define Max_number_hart_input		 		6


#define Max_min_cutoff_flowrate_histre		5 //m3/h

#define Max_try_connection_analog_card		5
#define Max_try_connection_hmi				20
#define Max_Reconnect_Try					6
#define Max_stop_error                      16

//*******************************************************************************************//
//*******************************************************************************************//
//*******************************************************************************************//
///////////////////////////////////////   stream   ////////////////////////////////////////////
//*******************************************************************************************//
//*******************************************************************************************//
//*******************************************************************************************//

#define Volume_m3 							0
#define Volume_Km3 							1
#define Volume_Mm3 							2
#define Volume_L 							3
#define Volume_mL 							4

#define Mass_g 								0
#define Mass_Kg 							1
#define Mass_T 								2

#define Energy_MJ							0
#define Energy_GJ							1
#define Energy_kWh							2
#define Energy_MWh							3

#define Volume_Flowrate_m3_h				0
#define Volume_Flowrate_Km3_h				1
#define Volume_Flowrate_Mm3_h				2
#define Volume_Flowrate_L_h					3
#define Volume_Flowrate_mL_h				4
#define Volume_Flowrate_m3_s				5

#define Volume_Mass_g_h						0
#define Volume_Mass_Kg_h					1
#define Volume_Mass_t_h						2
#define Volume_Mass_g_s						3

#define Volume_Energy_MJ_h					0
#define Volume_Energy_GJ_h					1
#define Volume_Energy_kWh_h					2
#define Volume_Energy_MWh_h					3
#define Volume_Energy_MJ_s 		 			4

#define Keypad_Value 						0
#define Min_Archive							1
#define Hour_Archive						2
#define Day_Archive							3

#define Stream_Inactive						0
#define Stream_Pulse_Meter					1
#define Stream_Diff_Pressure_meter			2
#define Stream_Ultrasonic					3
#define Stream_Current_input 				4

#define Stream_Uvol_flowrate 				0
#define Stream_Cvol_flowrate 				1
#define Stream_Mass_flowrate 				2


#define Stream_Up_stream_pressure			0
#define Stream_Down_stream_pressure			1

#define Stream_Keypad_Value					0
#define Stream_Din1							1
#define Stream_Din2							2
#define Stream_Din3							3
#define Stream_Din4							4

#define Stream_AGA3_2017					0
#define Stream_ISO_5167						1
#define Stream_AGA3_1992					2
#define Stream_AGA3_1988					3

#define Stream_Corner_Taps					0
#define Stream_Flange_Taps					1
#define Stream_D_D_2_Taps					2

#define Stream_Steel_304					0
#define Stream_Steel_316					1
#define Stream_Steel_304_316				2
#define Stream_Monol						3
#define Stream_Carbon						4

#define Stream_Com1							0
#define Stream_Com2							1
#define Stream_Com3							2

#define Stream_Keypad_Value					0
#define Stream_Channel						1
#define Stream_Standard						2

#define Stream_Keypad_Value					0
#define Stream_Channel						1
#define Stream_Station						2

#define Stream_Gasses						0
#define Stream_Liquid						1
#define Stream_Water						2

#define Stream_Natural_gas					0
#define Stream_Gasoil						0


#define Stream_Detail          				0
#define Stream_Gross						1

#define Stream_Keypad_Value					0
#define Stream_GC							1

// typedef struct
// {
// 	//total
// 	uint8_t volume;
// 	uint8_t mass;
// 	uint8_t energy;
// 	//flowrate
// 	uint8_t volume_flowrate;
// 	uint8_t mass_flowrate;
// 	uint8_t energy_flowrate;
// }Unit_total;
// typedef struct
// {
// 	uint8_t pulse_channel;
// 	float keypad_value;
// 	float k_factor;
// 	float m_factor;
// }Pulse_meter;
// typedef struct
// {
// 	uint8_t diff_pressure_channel;
// 	uint8_t pressure_upstream;
// 	float keypad_value;
// 	uint8_t calc_standard;
// 	uint8_t tap_config;
// 	uint8_t orifice_material;
// 	uint8_t pipe_material;
// 	float constricted_diameter;
// 	float diameter_ratio;
// 	float pipe_reference_temp;
// 	float orifice_reference_temp;
// }Diff_pressure_meter;
// typedef struct
// {
// 	uint8_t select_comport;
// 	uint8_t select_flowmeter;
// }Ultrasonic_meter;
// typedef struct
// {
// 	uint8_t type_of_input;
// 	uint8_t flowrate_channel;
// 	float keypad_value;
// }Current_input;
// typedef struct
// {
// 	Pulse_meter pulse_meter;
// 	Diff_pressure_meter diff_pressure_meter;
// 	Ultrasonic_meter ultrasonic_meter;
// 	Current_input current_input;
// }Flowmeter;
// typedef struct
// {
// 	uint8_t Z_input;
// 	uint8_t Z_base_input;
// 	float keypad_Z_input;
// 	float keypad_Z_base_input;
// 	uint8_t gas_composition;
// 	uint8_t gas_composition_input;
// 	double _x[22];
// 	//     0 - _______
// 	//     1 - Methane
// 	//     2 - Nitrogen
// 	//     3 - Carbon dioxide
// 	//     4 - Ethane
// 	//     5 - Propane
// 	//     6 - Isobutane
// 	//     7 - n-Butane
// 	//     8 - Isopentane
// 	//     9 - n-Pentane
// 	//    10 - n-Hexane
// 	//    11 - n-Heptane
// 	//    12 - n-Octane
// 	//    13 - n-Nonane
// 	//    14 - n-Decane
// 	//    15 - Hydrogen
// 	//    16 - Oxygen
// 	//    17 - Carbon monoxide
// 	//    18 - Water
// 	//    19 - Hydrogen sulfide
// 	//    20 - Helium
// 	//    21 - Argon
// 	uint8_t detail_gc_select_com;
// 	uint8_t select_N2;
// 	uint8_t select_H2;
// 	uint8_t select_CO2;
// 	float keypad_N2;
// 	float keypad_H2;
// 	float keypad_CO2;
// }Natural_Gas;
// typedef struct
// {
// 	uint8_t CPL;
// 	uint8_t CTL;
// 	uint8_t standard;
// 	float CSW;
// }Liquid_fluid;
// typedef struct
// {
// 	float pressure_base;
// 	float temperature_base;
// 	uint8_t fluid;
// 	uint8_t fluid_type;
// 	uint8_t convertion_method;
// 	uint8_t convertion_c_factor;
// 	uint8_t heting_vol_input;
// 	uint8_t heting_mass_input;
// 	uint8_t dansity_base_input;
// 	uint8_t dansity_measure_input;
// 	float keypad_convertion_c_factor;
// 	float keypad_heting_vol_input;
// 	float keypad_heting_mass_input;
// 	float keypad_dansity_base_input;
// 	float keypad_dansity_measure_input;
// 	Natural_Gas natural_gas;
// 	Liquid_fluid liquid;
// }Fluid;
// typedef struct
// {
// 	float max_flowrate;
// 	float min_flowrate;
// 	float air_pressure;
// 	uint8_t pressure_channel;
// 	uint8_t temperature_channel;
// 	uint8_t substitute_pressure_mode;
// 	uint8_t substitute_temperature_mode;
// 	uint8_t pressure_gauge;
// 	uint8_t minor_input;
// 	uint8_t maxor_input;
// 	float minor_keypad;
// 	float maxor_keypad;
// 	float pressure_keypad_value;
// 	float temperature_keypad_value;
// 	float dinamic_viscosity;
// 	float pipe_diameter;
// }General;
// typedef struct
// {
// 	uint8_t flow_meter;
// 	Unit_total unit_total;
// 	General general;
// 	Fluid fluid;
// 	Flowmeter flowmeter;
// }Setup_Stream;
// typedef struct
// {
// 	double Z; //compressibility factor
// 	double Z_b;
// 	double D; //molar_density[mol/l]
// 	double dPdD; //d(P)/d(rho) [kPa/(mol/l)
// 	double dPdD2; //d^2(P)/d(rho)^2 [kPa/(mol/l)^2
// 	double d2PdTD; //differential pressure per temp & dens
// 	double dPdT; //d(P)/d(T) [kPa/K]
// 	double U; //Energy [J/mol]
// 	double MW; //Molar mass [g/mol]
// 	double H; //Enthalpy [J/mol]
// 	double S; //Entropy [J/mol-K]
// 	double Cv; //Isochoric heat capacity [J/mol-K]
// 	double Cp; //Isobaric heat capacity [J/mol-K]
// 	double W; //Speed of sound [m/s]
// 	double G; //Gibbs energy [J/mol]
// 	double JT; //Joule-Thomson coefficient [K/kPa]
// 	double Kappa; //Isentropic exponent
// 	double Y; //expansion factor
// 	double C;	//orifice co-efficient[nan]
// 	double V;	//speed of fluid
// 	double Gr;//relatine density
// }Parameter_Natural_gas;
// typedef struct
// {
// 	double CTL;
// 	double CPL;
// 	double RHo_b;
// 	double NSV;

// }Parameter_Liquid;
// typedef struct
// {
// 	Parameter_Natural_gas natural_gas;
// 	Parameter_Liquid liquid;
// 	double heting_volue;
// 	double heting_mass;
// 	double mass_density_b;
// 	double reynolds;
// }Other;
// typedef struct
// {
// 	//Input
// 	double signal_flowmeter;
// 	double pressure;
// 	double temperature;
// 	//output calc
// 	double conversion_c_factor;
// 	double density;
// 	//flowrate
// 	double uvol_flowrate;
// 	double cvol_flowrate;
// 	double mass_flowrate;
// 	double energy_flowrate;
// 	//total flow
// 	double total_uvol;
// 	double total_cvol;
// 	double total_mass;
// 	double total_energy;
// 	double total_uvol_error;
// 	double total_cvol_error;
// 	double total_mass_error;
// 	double total_energy_error;
// }Summary;
// typedef struct
// {
// 	Summary summery;
// 	Other other;

// 	struct {
// 		uint32_t alarm_stream :1;				//0
// 		uint32_t calc_flow_in_error :1;			//1
// 		uint32_t warning_stream :1;				//2
// 		uint32_t flow_signal_fault :1;			//3  //input signal have problem need check it(fault)
// 		uint32_t pressure_fault :1;				//4  //pressure sensor or input have problem need check it(fault)
// 												//handle : input pressure = bias pressure setting
// 		uint32_t temp_fault:1;					//5  //temperature sensor or input have problem need check it(fault)
// 		   	   	   	   	   	   	   	   	   	    //handle : input temperature = bias temperature setting
// 		uint32_t flow_signal_warning :1;		//6  //differential pressure sensor or input have warning need check it(warning)
// 		uint32_t pressure_warning :1;			//7  //pressure sensor or input have warning need check it(warning)
// 		uint32_t temp_warning :1;				//8  //temperature sensor or input have warning need check it(warning)

// 	//	uint32_t pressure_calc_low :1;  		//9  //pressure is low
// 											   	//handle : Y=1

// 		uint32_t ALU_error :1;          		//10 //input parameter make problem in calc flow signal
// 		uint32_t calc_detail_error :1;  		//11 //P or T or gas component have problem in calc Density and Z
// 		uint32_t calc_detail_error_base :1; 	//12 //P_base or T_base or gas component have problem in calc Density and Z
// 											  	  //handle : Qv=0 , Qm=0

// 		uint32_t cutoff_flow_rate :1;	      	//13 //flow rate = 0
// 		uint32_t max_flow_rate :1;      		//14 //flow rate > max flow rate
// 		uint32_t min_flow_rate :1;      		//15 //flow rate < min flow rate
// 		uint32_t maxor :1;	      				//16 //flow rate = 0

// 	}error;
// }Output;
// typedef struct
// {
// 	//Input
// 	float signal_flowmeter;
// 	float pressure;
// 	float temperature;
// 	//output calc
// 	float mass_density;
// 	//flowrate
// 	float uvol_flowrate;
// 	float cvol_flowrate;
// 	float mass_flowrate;
// 	float energy_flowrate;
// }Average;
// typedef struct
// {
// 	double uvol;
// 	double cvol;
// 	double mass;
// 	double energy;
// 	double uvol_error;
// 	double cvol_error;
// 	double mass_error;
// 	double energy_error;
// }Total_previous;
// typedef struct
// {
// 	Average last_min;
// 	Average last_hour;
// 	Average last_day;
// 	Average last_month;
// 	Total_previous previous_day;
// 	Total_previous previous_month;
// }Archive;
// //typedef struct
// //{
// //	uint8_t measured_value_meter_correction;
// //	uint8_t corrected_value_meter_correction;
// //	float flow_measured[10];
// //	float deviation_measured[10];
// //	float flow_corrected[10];
// //	float deviation_corrected[10];
// //}Correction_measurement;
// typedef struct
// {
// 	Archive archive;
// 	Output output;
// 	Setup_Stream setup;
// //	Correction_measurement correction;
// }Stream;
//*******************************************************************************************//
//*******************************************************************************************//
//*******************************************************************************************//
///////////////////////////////////////   station   ///////////////////////////////////////////
//*******************************************************************************************//
//*******************************************************************************************//
//*******************************************************************************************//
//type_parameter1
#define Station_Keypad_Value	 		0
#define Station_Virtual_Stream 			1
#define Station_Channel					2
#define Station_Cvol					3
#define Station_Uvol					4
#define Station_Mass					5
#define Station_Energy					6
#define Station_Cvol_fail				7
#define Station_Uvol_fail				8
#define Station_Mass_fail				9
#define Station_Energy_fail				10
#define Station_Cvol_flowrate			11
#define Station_Uvol_flowrate			12
#define Station_Mass_flowrate			13
#define Station_Energy_flowrate			14
#define Station_Cvol_pre_day			15
#define Station_Uvol_pre_day			16
#define Station_Mass_pre_day			17
#define Station_Energy_pre_day			18
#define Station_Cvol_pre_month			19
#define Station_Uvol_pre_month			20
#define Station_Mass_pre_month			21
#define Station_Energy_pre_month		22

#define Station_Disable					0
#define Station_Plus					1
#define Station_Mins					2
#define Station_Multiply				3
#define Station_Devide					4
#define Station_Equal					5
#define Station_Not_equal				6
#define Station_Geater					7
#define Station_Less					8
#define Station_And						9
#define Station_Or						10

#define Station_Output_Disable			0
#define Station_Output_Flowmeter		1
#define Station_Output_Fluid			2
#define Station_Output_Fluid_Type		3

// typedef struct
// {
// 	float value;
// 	uint8_t type_parameter1;
// 	uint8_t type_parameter2;
// 	float value_parameter1;
// 	float value_parameter2;
// 	uint16_t Operation;
// 	uint8_t output_station;
// 	uint8_t number_output_station;			//Note: Start at one and carefull for use in array
// }Station;
//*******************************************************************************************//
//*******************************************************************************************//
//*******************************************************************************************//
/////////////////////////////////////   setup I/O   ///////////////////////////////////////////
//*******************************************************************************************//
//*******************************************************************************************//
//*******************************************************************************************//
//state digital input
#define Din_Inactive					0
#define Din_Digital_Input 				1
#define Din_Pulse_Input					2
//type_contact
#define Din_Normal_open					0
#define Din_Normal_close 				1
//en_log
//en_even
#define Din_Inactive 					0
#define Din_Active 						1

#define Din_DigitalInput_Control_Totalizer_Inactive		0
#define Din_DigitalInput_Control_Stream1				1
#define Din_DigitalInput_Control_Stream2				2
#define Din_DigitalInput_Control_Stream3				3

//Digital Input Value
#define Din_DigitalInput_Value_Inactive	0
#define Din_DigitalInput_Value_Active	1

#define Din_Rissing_up 					0
#define Din_Falling_up 					1

#define Din_Sense_10mv 					0
#define Din_Sense_100mv 				1

//type_contact
#define Dout_Normal_open				0
#define Dout_Normal_close 				1
//Initial
#define Dout_Inactive 					0
#define Dout_Active 					1

//Control Output
#define Dout_Dout_Manual				0
#define Dout_Dout_Station				1

#define Dout_Inactive 					0
#define Dout_Pulse_flowrate 			1
#define Dout_Alarm_output 				2
#define Dout_Pulse_time_base 			3
#define Dout_Digital_output 			4
#define Dout_ODOR 						5

#define Dout_Cvol 						0
#define Dout_Uvol 		 				1
#define Dout_Mass 						2
#define Dout_Energy		 				3
#define Dout_Cvol_fail 		 			4
#define Dout_Uvol_fail 					5
#define Dout_Mass_fail 				 	6
#define Dout_Energy_fail 				7
#define Dout_Virtual_stream 			8

#define Dout_Automat 					0
#define Dout_Manual 					1

#define Dout_Cvol1 						0
#define Dout_Cvol2 						1
#define Dout_Cvol3 						2
#define Dout_Uvol1 						3
#define Dout_Uvol2 						4
#define Dout_Uvol3 						5

// typedef struct
// {
// 	uint8_t state;
// 	//digital input
// 	uint8_t digitalInput_ControlTotalizer;
// 	uint8_t type_contact;
// 	uint8_t en_log;
// 	uint8_t en_event;
// 	//pulse input
// 	uint8_t type_trigger;
// 	uint8_t sensevity_pikup;
// 	float frqency;
// 	uint32_t counter_pulse;
// }Din;
// typedef struct
// {
// 	uint8_t state;
// 	uint8_t type_contact;
// 	uint8_t output_type;
// 	uint8_t number;
// 	uint16_t pulse_wide;
// 	//pulse time base
// 	uint16_t period_time;
// 	//pulse flowrate
// 	float pulse_value;
// 	//Digital output
// 	uint8_t initial_state;
// 	uint8_t control_output;
// 	//alarm output
// 	uint8_t alarm_type;
// 	uint16_t delay_to_active;
// 	//ODOR
// 	uint8_t output_driver;
// 	float stroke_value;
// 	float stroke_cycle;
// }Dout;

#define Ain_Inactive 					 0
#define Ain_Pressure 					 1
#define Ain_Tempeture 					 2
#define Ain_Diff_Pressure 				 3
#define Ain_flowrate 					 4
#define Ain_Dansity       				 5
#define Ain_other 						 6

#define Ain_Pressure_bar 				 0
#define Ain_Pressure_mbar 				 1
#define Ain_Pressure_KPa 				 2
#define Ain_Pressure_MPa 				 3
#define Ain_Pressure_mmH2O 				 4
#define Ain_Pressure_psi 				 5
#define Ain_Pressure_atm  				 6
#define Ain_Pressure_sig				 7

#define Ain_Tempeture_C 				 0
#define Ain_Tempeture_F 				 1
#define Ain_Tempeture_K 				 2

#define Ain_Diff_Pressure_mbar 			 0
#define Ain_Diff_Pressure_Pa 			 1
#define Ain_Diff_Pressure_KPa 			 2
#define Ain_Diff_Pressure_mmH2O 		 3
#define Ain_Diff_Pressure_inH2O 		 4
#define Ain_Diff_Pressure_bar	 		 5

#define Ain_Flowrate_m_3_h 				 0
#define Ain_Flowrate_Km_3_h 			 1
#define Ain_Flowrate_Mm_3_h 			 2
#define Ain_Flowrate_m_3_s 				 3
#define Ain_Flowrate_cm_3_h				 4
#define Ain_Flowrate_cm_3_s				 5
//#define Ain_Flowrate_MVA 				 6

#define Ain_Dansity_Kg_m3 				 0
#define Ain_Dansity_g_cm3 				 1
#define Ain_Gauge						 0
#define Ain_Absolute					 1

// typedef struct {
// 	uint8_t disconnected:1;
// 	uint8_t over_current:1;
// 	uint8_t under_current:1;
// 	uint8_t high_trishhold:1;
// 	uint8_t low_trishhold:1;
// }Analog_in_error;
// typedef struct {
// 	uint8_t disconnected:1;
// 	uint8_t non_range_current:1;
// 	uint8_t high_trishhold:1;
// 	uint8_t low_trishhold:1;
// 	uint8_t hart_time_out:1;
// 	uint8_t hart_data_unexpected:1;
// 	uint8_t hart_checksum_error:1;
// 	uint8_t pv_index_data_isnt_exist:1;
// }Hart_in_error;


// typedef struct
// {
// 	uint8_t type_input;
// 	uint8_t unit_input;
// 	uint8_t pressure_transmitter_type;
// 	Analog_in_error error;
// 	float low_scale;
// 	float high_scale;
// 	float low_limit;
// 	float high_limit;
// 	double current_input;
// 	double Gain;
// 	double offset;
// }Analog_input;
// typedef struct
// {
// 	uint8_t type_input;
// 	uint8_t unit_input;
// 	uint8_t number_terminal;
// 	uint8_t pressure_transmitter_type;
// 	uint8_t pooling_address;
// 	uint8_t pv_index;
// 	Hart_in_error error;
// 	float low_scale;
// 	float high_scale;
// 	float low_limit;
// 	float high_limit;
// 	float pvLoopCurrent;
// }Hart_input;

#define Aout_Inactive			 		0
#define Aout_Voltage					1
#define Aout_Current					2

#define Aout_Keypad_Value 				0
#define Aout_station					1
#define Aout_channel					2
#define Aout_Cvol_flowrate 				3
#define Aout_Uvol_flowrate 				4
#define Aout_Mass_flowrate 				5
#define Aout_Energy_flowrate 			6
#define Aout_Desity			 			7

// typedef struct
// {
// 	uint8_t type_output;
// 	uint8_t output;
// 	uint8_t number;
// 	float low_scale;
// 	float high_scale;
// 	float keypad_value;
// 	double current_output;
// 	double Gain;
// 	double offset;
// }Analog_output;

// typedef struct {
// 	uint16_t ADC_IC1_problem:1;
// 	uint16_t ADC_IC2_problem:1;
// 	uint16_t refrence_ADC_IC1_problem:1;
// 	uint16_t refrence_ADC_IC2_problem:1;
// }I_O_error;

// typedef struct
// {
// 	I_O_error error;
// 	Din d_in[Max_number_digital_input];
// 	Dout d_out[Max_number_digital_output];
// 	Analog_input analog_in[Max_number_analog_input];
// 	Hart_input hart[Max_number_hart_input];
// 	Analog_output analog_out[Max_number_analog_output];
// 	double refrence_adc;
// }I_O;

//*******************************************************************************************//
//*******************************************************************************************//
//*******************************************************************************************//
//////////////////////////////////   setup system   ///////////////////////////////////////////
//*******************************************************************************************//
//*******************************************************************************************//
//*******************************************************************************************//
#define Set_clock_Inactive  			0
#define Set_clock_Set  					1

#define Swap_display_Inactive  			0
#define Swap_display_Active  			1

// typedef struct
// {
// 	uint32_t alarm_system:1;
// 	uint32_t warning_system:1;
// 	uint32_t reserved:1;
// 	uint32_t High_Temp_Box:1;
// 	uint32_t defult_setting:1;
// 	uint32_t defult_total_flow:1;
// 	uint32_t save_setting:1;
// 	uint32_t save_total_flow:1;
// 	uint32_t eth_not_init_phy:1;
// 	uint32_t connect_to_eth:1;
// 	uint32_t eth_100M:1;
// 	uint32_t eth_10M:1;
// 	uint32_t modbus_init:1;
// 	uint32_t connection_hmi:1;
// 	uint32_t validation_hmi:1;
// 	uint32_t validation_main_board_hmi:1;
// 	uint32_t sd_card_detection:1;
// 	uint32_t nextion_connection:1;
// 	uint32_t rtc_problem:1;
// 	uint32_t hmi_spi_flash_problem:1;
// 	uint32_t connection_Analog_board:1;
// 	uint32_t validation_Analog_board:1;
// 	uint32_t validation_main_board_Analog_board:1;
// 	uint32_t spi_flash_problem:1;

// }setting_error;

// typedef struct
// {
// 	uint8_t set_clock;
// 	uint8_t year;
// 	uint8_t month;
// 	uint8_t day;
// 	uint8_t hour;
// 	uint8_t minute;
// 	uint8_t second;
// }Clock;
// typedef struct
// {
// 	uint8_t _1;
// 	uint8_t	_2;
// 	uint8_t _3;
// 	uint8_t _4;
// }Setup_Address;
// typedef struct
// {
// 	//ID & Version Flowcomputer
// 	uint16_t id_1;
// 	uint16_t id_2;
// 	uint16_t version;
// 	//Set Alarm temp
// 	uint16_t case_temperature;
// 	//Display
// 	uint16_t main_display_swap;
// 	uint16_t time_main_display_swap;
// 	//set clock
// 	Clock set_clock;
// 	//Set Time Perevious month
// 	Clock time_perevious_month;
// 	//Set Time Perevious day
// 	Clock time_perevious_day;
// 	//Ethernet
// 	Setup_Address ip;
// 	Setup_Address netmask;
// 	Setup_Address getway;
// 	uint8_t macAddress[6];
// 	//Password
// 	uint16_t password[8];
// 	setting_error error;
// 	int16_t boxTemp;
// }Setup_System;

//*******************************************************************************************//
//*******************************************************************************************//
//*******************************************************************************************//
/////////////////////////////////////   Modbus   //////////////////////////////////////////////
//*******************************************************************************************//
//*******************************************************************************************//
//*******************************************************************************************//

#define Modbus_Inactive 				0
#define Modbus_slave					1
#define Modbus_USM						2
#define Modbus_GC						3
#define Modbus_master					4

#define Modbus_B1200					0
#define Modbus_B2400					1
#define Modbus_B4800					2
#define Modbus_B9600					3
#define Modbus_B19200					4
#define Modbus_B38400					5
#define Modbus_B57600					6
#define Modbus_B115200					7

#define Modbus_RS485					0
#define Modbus_RS422					1

#define Modbus_RTU						0
#define Modbus_ASCI						1

#define Modbus_Inactive					0
#define Modbus_Com1						1
#define Modbus_Com2						2
#define Modbus_Com3						3

#define Modbus_Read_Coil				0
#define Modbus_Read_Discrete_Input		1
#define Modbus_Read_Holding_Registers 	2
#define Modbus_Read_Input_Registers 	3

#define Modbus_Coil								0
#define Modbus_int_16							1
#define Modbus_uint_16							2
#define Modbus_float_BigEndian					3
#define Modbus_float_LittleEndian				4
#define Modbus_float_BigEndianByteSwap			5
#define Modbus_float_LittleEndianByteSwap		6
#define Modbus_int_32_BigEndian					7
#define Modbus_int_32_LittleEndian  	 		8
#define Modbus_int_32_BigEndianByteSwap			9
#define Modbus_int_32_LittleEndianByteSwap		10
#define Modbus_uint_32_BigEndian				11
#define Modbus_uint_32_LittleEndian  	 		12
#define Modbus_uint_32_BigEndianByteSwap		13
#define Modbus_uint_32_LittleEndianByteSwap		14
#define Modbus_double_BigEndian					15
#define Modbus_double_LittleEndian				16
#define Modbus_double_BigEndianByteSwap			17
#define Modbus_double_LittleEndianByteSwap		18
#define Modbus_int_64_BigEndian					19
#define Modbus_int_64_LittleEndian  	 		20
#define Modbus_int_64_BigEndianByteSwap			21
#define Modbus_int_64_LittleEndianByteSwap		22
#define Modbus_uint_64_BigEndian				23
#define Modbus_uint_64_LittleEndian  	 		24
#define Modbus_uint_64_BigEndianByteSwap		25
#define Modbus_uint_64_LittleEndianByteSwap		26

#define Modbus_RespnseStatus_None				0
#define Modbus_RespnseStatus_Success			1
#define Modbus_RespnseStatus_Timeout			2
#define Modbus_RespnseStatus_ModbusProblem		3


// typedef struct
// {
// 	uint8_t function;
// 	uint8_t boudrate;
// 	uint8_t data_config;
// 	uint8_t drive_mode;
// 	uint8_t modbus_type;
// 	//modbus slave
// 	uint8_t slave_address;
// 	uint16_t delay_to_response;
// 	//USM & GC & Master
// 	uint8_t device_type;
// 	uint8_t device_address;
// 	uint16_t scanRateAllReq;
// 	uint16_t delayBetweenPoll;
// 	uint16_t time_out;
// }COM;

// typedef struct
// {
// 	uint16_t startAddress;
// }ModbusUserDefineSetting_t;

// typedef struct
// {
// 	uint8_t select_com;
// 	uint8_t device_address;
// 	uint8_t function_read;
// 	uint8_t type_of_data;
// 	uint16_t address_read;
// }Channel_modbus_master_config;

// typedef struct
// {
// 	float data;
// 	uint8_t respStatus;
// 	uint8_t flagNewData;
// }Channel_modbus_master_data;

// typedef struct {
// 	uint8_t timeout:1;
// 	uint8_t packetErr:1;
// }ModbusMaster_Channel_Error;

// typedef struct
// {
// 	COM com[Max_number_Com];
// 	Channel_modbus_master_config channelConfig[Max_number_channel_master_modbus];
// 	uint16_t user_def_address[Max_number_modbus_user_define];
// 	ModbusUserDefineSetting_t ModbusUserDefineSetting;
// 	ModbusMaster_Channel_Error modbusChannelErr[Max_number_channel_master_modbus];
// }Modbus;
//*******************************************************************************************//
//*******************************************************************************************//
//*******************************************************************************************//
//////////////////////////////////////   clock   //////////////////////////////////////////////
//*******************************************************************************************//
//*******************************************************************************************//
//*******************************************************************************************//
// typedef enum{
// 	SAT=6,
// 	SUN=7,
// 	MON=1,
// 	TUE=2,
// 	WED=3,
// 	THU=4,
// 	FRI=5
// }Day_Week;

// typedef struct {
// 	uint16_t Year;
// 	uint8_t Month;
// 	uint8_t Day;
// 	Day_Week DayOfWeek;

// 	uint8_t Hour;
// 	uint8_t Minute;
// 	uint8_t Second;

// 	int8_t TemperatureInteger;
// 	uint8_t TemperatureFraction;
// }Time_RTC;

//*******************************************************************************************//
//*******************************************************************************************//
//*******************************************************************************************//
/////////////////////////////////////   flag new setting   ////////////////////////////////////
//*******************************************************************************************//
//*******************************************************************************************//
//*******************************************************************************************//
// typedef struct {
// 	uint8_t din;
// 	uint8_t dout;
// 	uint8_t ain;
// 	uint8_t hart;
// 	uint8_t aout;
// 	uint8_t calib_ain;
// 	uint8_t calib_aout;
// 	uint8_t stream[Max_number_stream];
// 	struct {
// 		uint32_t cvol:1;
// 		uint32_t uvol:1;
// 		uint32_t mass:1;
// 		uint32_t energy:1;
// 		uint32_t cvol_error:1;
// 		uint32_t uvol_error:1;
// 		uint32_t mass_error:1;
// 		uint32_t energy_error:1;
// 	}total[Max_number_stream];
// 	uint8_t station;
// 	uint8_t station_value[2];
// 	uint8_t system;
// 	uint8_t com[Max_number_Com];
// 	uint8_t modbus_master;
// 	uint8_t modbus_user_define;

// 	struct
// 	{
// 		uint8_t odor[Max_number_digital_output];
// 		uint8_t gasComposition[Max_number_stream];
// 	}ConfigFromHmi;
// }flag_new_setting;

/*****************************************************************************/
/*****************************************************************************/
/*******************************By Mojtaba**********************************/
/*****************************************************************************/
/*****************************************************************************/

#define MAX_STREAM_NUM  Max_number_stream


#endif /* PARAMETER_H_ */
