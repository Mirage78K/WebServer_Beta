/*
 * WebServer_Conn.h
 *
 *  Created on: Apr 11, 2025
 *      Author: mojkh
 */


/**Just Use in HMI_Connection.c File**/

#ifndef TRANSFERPROTOCOL_WEBSERVER_CONN_H_
#define TRANSFERPROTOCOL_WEBSERVER_CONN_H_


#include "main.h"
#include <stdint.h>
#include "WebServer_Conn_Param.h"


#define WS_HMI_TRANSFER_FROM_HMI_HEADER                  0xA4B8
#define WS_HMI_TRANSFER_FROM_HMI_HEADER_BYTE_LOW         0xB8
#define WS_HMI_TRANSFER_FROM_HMI_HEADER_BYTE_HIGH        0xA4

#define WS_HMI_TRANSFER_FROM_HMI_FOOTER                  0xC4D2
#define WS_HMI_TRANSFER_FROM_HMI_FOOTER_BYTE_LOW         0xD2
#define WS_HMI_TRANSFER_FROM_HMI_FOOTER_BYTE_HIGH        0xC4


#define WS_HMI_TRANSFER_FROM_WS_HEADER                  0x87AD
#define WS_HMI_TRANSFER_FROM_WS_HEADER_BYTE_LOW         0xAD
#define WS_HMI_TRANSFER_FROM_WS_HEADER_BYTE_HIGH        0x87

#define WS_HMI_TRANSFER_FROM_WS_FOOTER                  0xEB12
#define WS_HMI_TRANSFER_FROM_WS_FOOTER_BYTE_LOW         0x12
#define WS_HMI_TRANSFER_FROM_WS_FOOTER_BYTE_HIGH        0xEB



typedef enum
{
    WS_CONN_PACKETCMD_HMI_DATA,
    WS_CONN_PACKETCMD_HMI_SETTING,
}WsConn_SendPacketFromHmiCmd_e;

typedef enum
{
    WS_CONN_PACKETCMD_WS_ALL,
}WsConn_SendPacketFromWebSrvCmd_e;



/*header*/
typedef struct
{
    uint16_t header;
    uint16_t packetLength;
    WsConn_SendPacketFromHmiCmd_e respCmd;
    uint8_t version;
}WsConn_PacketSendFromHMI_Header_t;

typedef struct
{
    uint16_t header;
    uint16_t packetLength;
    WsConn_SendPacketFromWebSrvCmd_e respCmd;
    uint8_t version;
}WsConn_PacketSendFromWebSrv_Header_t;



/*Web Server Board Packet*/
typedef struct
{
    uint32_t versionErr :1;            //0 == OK , 1 == Error
    uint32_t needSetting :1;           //0 == No Need , 1 == Need
    uint32_t dummy: 30;
}WsConn_SendFromWebSrv_State_t;


typedef struct
{
    WsConn_PacketSendFromWebSrv_Header_t HeaderSt;
    WsConn_SendFromWebSrv_State_t statePacket;


    uint16_t footer;
}WsConn_SendFromWebSrv_All_t;




/*HMI Packet*/

/*Data CMD*/
typedef struct
{
    //Nothing
}WsConn_SendDataFromHmi_State_t;


typedef struct
{
    WsConn_PacketSendFromHMI_Header_t HeaderSt;
    WsConn_SendDataFromHmi_State_t statePacket;

    WsConn_Param_Data_Flow_t AllDataFlow;
    WsConn_Param_Data_ClockAndData_t ClockAndData;

    uint16_t footer;
}WsConn_SendDataFromHmi_All_t;

/*************************************************/


/*Setting CMD*/
typedef struct
{
    //Nothing
}WsConn_SendSettingFromHmi_State_t;


typedef struct
{
    WsConn_PacketSendFromHMI_Header_t HeaderSt;
    WsConn_SendSettingFromHmi_State_t statePacket;


    uint16_t footer;
}WsConn_SendSettingFromHmi_All_t;

/*************************************************/

#endif /* TRANSFERPROTOCOL_WEBSERVER_CONN_H_ */
