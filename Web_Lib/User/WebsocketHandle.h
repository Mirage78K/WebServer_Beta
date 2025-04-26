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
    //Stream 1
    WebsocketHandle_DataType_St1TotalAndFlow,

    //Stream 2
    WebsocketHandle_DataType_St2TotalAndFlow,

    //Stream 3
    WebsocketHandle_DataType_St3TotalAndFlow,

    //System
}WebsocketHandle_DataType_e;



void websoketHandle_init();
int websocketHandle_addNewWebsocketConnection(struct mg_connection *c);
int websocketHandle_removeWebsocketConnection(struct mg_connection *c);
int websocketHandle_checkNewData(struct mg_connection *c, WebsocketHandle_DataType_e NewDataType);



#endif /* USER_WEBSOCKETHANDLE_H_ */
