/*
 * HMI_Connextion.h
 *
 *  Created on: Apr 10, 2025
 *      Author: RAYA_4
 */

#ifndef HMI_CONNEXTION_H_
#define HMI_CONNEXTION_H_


#include "main.h"
#include "WebServer_Conn_Param.h"



#define HMICONN_UART_HANDLER		                    huart1

#define HMICONN_CRC_SIZE                                1       //CRC is last byte in buffer

#define HMICONN_MAX_PACKET_RECEIVE_LENGH                3072        //Max Packet Receive
#define HMICONN_PACKET_SEND_LENGTH                      (sizeof(WsConn_SendFromWebSrv_All_t) + HMICONN_CRC_SIZE)



//All Modules can use this Structure
struct ParamFromHmi_Data_t
{
	WsConn_Param_Data_Flow_t AllDataFlow;
    WsConn_Param_Data_ClockAndData_t ClockAndData;
};

struct ParamFromHmi_Setting_t
{
    uint8_t dummy;
};

struct PramFromHmi_StateFlags_t
{
    bool isSettingReceived;
    bool isDataReceived;
};


typedef struct
{

    struct ParamFromHmi_Data_t Data;
    struct ParamFromHmi_Setting_t Setting;
    struct PramFromHmi_StateFlags_t StateFlag;
    
}gParamFromHmi_t;




void hmiConn_taskHandler_connection(void *arg);
void hmiConn_initialAfterRtosApi();

//ISR
void hmiConn_UartExRxDmaISR(uint16_t size);

//we can use getter but do not change datas
int hmiConn_GetAllParam(gParamFromHmi_t *AllParam);



#endif /* HMI_CONNEXTION_H_ */
