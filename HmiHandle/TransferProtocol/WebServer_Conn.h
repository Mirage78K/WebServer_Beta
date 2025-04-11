/*
 * WebServer_Conn.h
 *
 *  Created on: Apr 11, 2025
 *      Author: mojkh
 */


/**Just Use in HMI_Connection File**/

#ifndef TRANSFERPROTOCOL_WEBSERVER_CONN_H_
#define TRANSFERPROTOCOL_WEBSERVER_CONN_H_


#include "main.h"
#include <stdint.h>
#include "WebServer_Conn_Param.h"


typedef enum
{
    WS_CONN_PACKETCMD_HMI_DATA,
    WS_CONN_PACKETCMD_HMI_SETTING,
}WsConn_SendPacketFromHmiCmd_e;

typedef enum
{
    WS_CONN_PACKETCMD_WS_DATA,
    WS_CONN_PACKETCMD_WS_SETTING,
}WsConn_SendPacketFromWebSrvCmd_e;



/*header and footer*/
typedef struct
{
    uint32_t header;
    uint16_t packetLength;
    WsConn_SendPacketFromHmiCmd_e respCmd;
    uint8_t version;
}WsConn_PacketSend_Header_t;


typedef struct
{
    uint8_t checksum;
    uint32_t footer;
}WsConn_PacketSend_Footer_t;





/*Web Server Board Packet*/
typedef struct
{
    uint32_t packetRecState :1;     //0 == OK , 1 == Error
    uint32_t needSetting :1;        //0 == No Need , 1 == Need
    uint32_t dummy: 30;
}WsConn_SendFromWebSrv_State_t;


typedef struct
{
    WsConn_PacketSend_Header_t Header;
    WsConn_SendFromWebSrv_State_t statePacket;


    WsConn_PacketSend_Footer_t Footer;
}WsConn_SendFromWebSrv_All_t;




/*HMI Packet*/

/*Data CMD*/
typedef struct
{
    //Nothing
}WsConn_SendDataFromHmi_State_t;


typedef struct
{
    WsConn_PacketSend_Header_t Header;
    WsConn_SendDataFromHmi_State_t statePacket;

    WsConn_Param_Data_Flow_t AllDataFlow;
    WsConn_Param_Data_ClockAndData_t ClockAndData;

    WsConn_PacketSend_Footer_t Footer;
}WsConn_SendDataFromHmi_All_t;

/*************************************************/


/*Setting CMD*/
typedef struct
{
    //Nothing
}WsConn_SendSettingFromHmi_State_t;


typedef struct
{
    WsConn_PacketSend_Header_t Header;
    WsConn_SendSettingFromHmi_State_t statePacket;


    WsConn_PacketSend_Footer_t Footer;
}WsConn_SendSettingFromHmi_All_t;

/*************************************************/

#endif /* TRANSFERPROTOCOL_WEBSERVER_CONN_H_ */
