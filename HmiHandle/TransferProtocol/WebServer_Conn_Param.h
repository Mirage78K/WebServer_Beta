/*
 * WebServer_Conn_Param.h
 *
 *  Created on: Apr 11, 2025
 *      Author: mojkh
 */

#ifndef TRANSFERPROTOCOL_WEBSERVER_CONN_PARAM_H_
#define TRANSFERPROTOCOL_WEBSERVER_CONN_PARAM_H_


#include "main.h"


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
	uint16_t Year;
	uint8_t Month;
	uint8_t Day;

	uint8_t Hour;
	uint8_t Minute;
	uint8_t Second;
}WsConn_Param_Data_ClockAndData_t;


/*For Setting CMD*/
typedef struct
{

}WsConn_Param_Setting__t;



#endif /* TRANSFERPROTOCOL_WEBSERVER_CONN_PARAM_H_ */
