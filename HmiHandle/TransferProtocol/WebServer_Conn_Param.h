/*
 * WebServer_Conn_Param.h
 *
 *  Created on: Apr 11, 2025
 *      Author: mojkh
 */

#ifndef TRANSFERPROTOCOL_WEBSERVER_CONN_PARAM_H_
#define TRANSFERPROTOCOL_WEBSERVER_CONN_PARAM_H_


#include "main.h"
#include "parameter.h"

#define WSCONN_PARAM_STREAM_NUM			3



/*For Data CMD*/
struct WsConn_Param_Data_Flow_Now_t
{
	double signal_flowmeter;
	double pressure;
	double temperature;
	//output calc
	double conversion_c_factor;
	double density;
	//flowrate
	double uvol_flowrate;
	double cvol_flowrate;
	double mass_flowrate;
	double energy_flowrate;
	//total flow
	double total_uvol;
	double total_cvol;
	double total_mass;
	double total_energy;
	double total_uvol_error;
	double total_cvol_error;
	double total_mass_error;
	double total_energy_error;
};
struct WsConn_Param_Data_Flow_Average_t
{
	//Input
	float signal_flowmeter;
	float pressure;
	float temperature;
	//output calc
	float mass_density;
	//flowrate
	float uvol_flowrate;
	float cvol_flowrate;
	float mass_flowrate;
	float energy_flowrate;
};
struct WsConn_Param_Data_Flow_TotalPrevious_t
{
	double uvol;
	double cvol;
	double mass;
	double energy;
	double uvol_error;
	double cvol_error;
	double mass_error;
	double energy_error;
};
typedef struct
{

	struct WsConn_Param_Data_Flow_Now_t Now[WSCONN_PARAM_STREAM_NUM];

	struct WsConn_Param_Data_Flow_Average_t AverageLastMin[WSCONN_PARAM_STREAM_NUM];
	struct WsConn_Param_Data_Flow_Average_t AverageLastHour[WSCONN_PARAM_STREAM_NUM];
	struct WsConn_Param_Data_Flow_Average_t AverageLastDay[WSCONN_PARAM_STREAM_NUM];
	struct WsConn_Param_Data_Flow_Average_t AverageLastMonth[WSCONN_PARAM_STREAM_NUM];

	struct WsConn_Param_Data_Flow_TotalPrevious_t TotalPreviousDay[WSCONN_PARAM_STREAM_NUM];
	struct WsConn_Param_Data_Flow_TotalPrevious_t TotalPreviousMonth[WSCONN_PARAM_STREAM_NUM];
	
}WsConn_Param_Data_Flow_t;



typedef struct
{
	float pressureBase;
	float temperatureBase;
	// float Hvol;
	// float Hmass;
	float pb;
	float reynoldNumber;
}WsConn_Param_Data_OutputCalc_General_t;
typedef struct
{
	float zFactor;
	float zFactorBase;
	float molarMass;
	// float internalEnergy;
	// float entalpy;
	// float entropy;
	// float isochoricHeatCapacity;
	// float isobaricHeatCapacity;
	// float gibbsEnergy;
	float isentropicExponent;
	// float jouleThomsonCoefficient;
	// float d2pPerdRho2;
	// float d2pPerdt;
	// float speceficGravity;
	// float energy;
	// float molarDensity;
	// float orificeCoefficeint;
	float epansionFactor;
	float speedOfFluid;
	float speedOfSound;
}WsConn_Param_Data_OutputCalc_NaturalGas_t;
typedef struct
{
	float cpl;
	float ctl;
	// float rho_b;
	// float nsv;
}WsConn_Param_Data_OutputCalc_Liquid_t;
typedef struct
{
	WsConn_Param_Data_OutputCalc_General_t General[WSCONN_PARAM_STREAM_NUM];
	WsConn_Param_Data_OutputCalc_NaturalGas_t NaturalGas[WSCONN_PARAM_STREAM_NUM];
	WsConn_Param_Data_OutputCalc_Liquid_t Liquid[WSCONN_PARAM_STREAM_NUM];
}WsConn_Param_Data_OutputCalc_t;


typedef struct
{
	uint16_t year;
	uint8_t month;
	uint8_t day;

	uint8_t hour;
	uint8_t minute;
	uint8_t second;
}WsConn_Param_Data_ClockAndData_t;


/*For Setting CMD*/
typedef struct
{
	//total
	uint8_t volume;
	uint8_t mass;
	uint8_t energy;
	//flowrate
	uint8_t volume_flowrate;
	uint8_t mass_flowrate;
	uint8_t energy_flowrate;	
}WsConn_Param_Setting_SetupStream_UnitTotal_t;

typedef struct
{
	float max_flowrate;
	float min_flowrate;
	float air_pressure;
	uint8_t pressure_channel;
	uint8_t temperature_channel;
	uint8_t substitute_pressure_mode;
	uint8_t substitute_temperature_mode;
	uint8_t pressure_gauge;
	uint8_t minor_input;
	uint8_t maxor_input;
	float minor_keypad;
	float maxor_keypad;
	float pressure_keypad_value;
	float temperature_keypad_value;
	float dinamic_viscosity;
	float pipe_diameter;
}WsConn_Param_Setting_SetupStream_General_t;

typedef struct
{
	uint8_t Z_input;
	uint8_t Z_base_input;
	float keypad_Z_input;
	float keypad_Z_base_input;
	uint8_t gas_composition;
	uint8_t gas_composition_input;
	double _x[22];
	//     0 - _______
	//     1 - Methane
	//     2 - Nitrogen
	//     3 - Carbon dioxide
	//     4 - Ethane
	//     5 - Propane
	//     6 - Isobutane
	//     7 - n-Butane
	//     8 - Isopentane
	//     9 - n-Pentane
	//    10 - n-Hexane
	//    11 - n-Heptane
	//    12 - n-Octane
	//    13 - n-Nonane
	//    14 - n-Decane
	//    15 - Hydrogen
	//    16 - Oxygen
	//    17 - Carbon monoxide
	//    18 - Water
	//    19 - Hydrogen sulfide
	//    20 - Helium
	//    21 - Argon
	uint8_t detail_gc_select_com;
	uint8_t select_N2;
	uint8_t select_H2;
	uint8_t select_CO2;
	float keypad_N2;
	float keypad_H2;
	float keypad_CO2;	
}WsConn_Param_Setting_SetupStream_Fluid_NaturalGas_t;

typedef struct
{
	uint8_t CPL;
	uint8_t CTL;
	uint8_t standard;
	float CSW;	
}WsConn_Param_Setting_SetupStream_Fluid_Liquid_t;

typedef struct
{
	float pressure_base;
	float temperature_base;
	uint8_t fluid;
	uint8_t fluid_type;
	uint8_t convertion_method;
	uint8_t convertion_c_factor;
	uint8_t heting_vol_input;
	uint8_t heting_mass_input;
	uint8_t dansity_base_input;
	uint8_t dansity_measure_input;
	float keypad_convertion_c_factor;
	float keypad_heting_vol_input;
	float keypad_heting_mass_input;
	float keypad_dansity_base_input;
	float keypad_dansity_measure_input;
	WsConn_Param_Setting_SetupStream_Fluid_NaturalGas_t natural_gas;
	WsConn_Param_Setting_SetupStream_Fluid_Liquid_t liquid;	
}WsConn_Param_Setting_SetupStream_Fluid_t;

typedef struct
{
	uint8_t pulse_channel;
	float keypad_value;
	float k_factor;
	float m_factor;
}WsConn_Param_Setting_SetupStream_Flowmeter_PulseMeter_t;

typedef struct
{
	uint8_t diff_pressure_channel;
	uint8_t pressure_upstream;
	float keypad_value;
	uint8_t calc_standard;
	uint8_t tap_config;
	uint8_t orifice_material;
	uint8_t pipe_material;
	float constricted_diameter;
	float diameter_ratio;
	float pipe_reference_temp;
	float orifice_reference_temp;
}WsConn_Param_Setting_SetupStream_Flowmeter_DiffPressureMeter_t;

typedef struct
{
	uint8_t select_comport;
	uint8_t select_flowmeter;
}WsConn_Param_Setting_SetupStream_Flowmeter_UltrasonicMeter_t;

typedef struct
{
	uint8_t type_of_input;
	uint8_t flowrate_channel;
	float keypad_value;
}WsConn_Param_Setting_SetupStream_Flowmeter_CurrentInput_t;

typedef struct
{
	WsConn_Param_Setting_SetupStream_Flowmeter_PulseMeter_t pulse_meter;
	WsConn_Param_Setting_SetupStream_Flowmeter_DiffPressureMeter_t diff_pressure_meter;
	WsConn_Param_Setting_SetupStream_Flowmeter_UltrasonicMeter_t ultrasonic_meter;
	WsConn_Param_Setting_SetupStream_Flowmeter_CurrentInput_t current_input;	
}WsConn_Param_Setting_SetupStream_Flowmeter_t;

typedef struct
{
	uint8_t flowMeterType;
	WsConn_Param_Setting_SetupStream_UnitTotal_t UnitTotal;
	WsConn_Param_Setting_SetupStream_General_t General;
	WsConn_Param_Setting_SetupStream_Fluid_t Fluid;
	WsConn_Param_Setting_SetupStream_Flowmeter_t Flowmeter;
}WsConn_Param_Setting_SetupStream_t;



#endif /* TRANSFERPROTOCOL_WEBSERVER_CONN_PARAM_H_ */
