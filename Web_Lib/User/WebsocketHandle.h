/*
 * WebsocketHandle.h
 *
 *  Created on: Apr 22, 2025
 *      Author: RAYA_4
 */

#ifndef USER_WEBSOCKETHANDLE_H_
#define USER_WEBSOCKETHANDLE_H_


#include "main.h"
#include "mongoose.h"


typedef enum
{
    //WS Spec
    WebsocketHandle_DataType_WsSpec,

    //Stream 1
    WebsocketHandle_DataType_St1TotalAndFlow,
    WebsocketHandle_DataType_St1Average,
    WebsocketHandle_DataType_St1TotalPrevious,

    //Stream 2
    WebsocketHandle_DataType_St2TotalAndFlow,
    WebsocketHandle_DataType_St2Average,
    WebsocketHandle_DataType_St2TotalPrevious,

    //Stream 3
    WebsocketHandle_DataType_St3TotalAndFlow,
    WebsocketHandle_DataType_St3Average,
    WebsocketHandle_DataType_St3TotalPrevious,

    //System
}WebsocketHandle_DataType_e;



void websoketHandle_init();
int websocketHandle_addNewWebsocketConnection(struct mg_connection *c);
int websocketHandle_removeWebsocketConnection(struct mg_connection *c);
int websocketHandle_checkNewData(struct mg_connection *c, WebsocketHandle_DataType_e NewDataType);

int websocketHandle_getWsSendStageStep(struct mg_connection *c);
int websocketHandle_setWsSendStageStep(struct mg_connection *c, uint8_t stepValue);


#endif /* USER_WEBSOCKETHANDLE_H_ */
