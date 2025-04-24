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
    WebsocketHandle_DataType_TotalAndFlow,
    WebsocketHandle_DataType_TotalAndFlowError,
}WebsocketHandle_DataType_e;



void websoketHandle_init();
int websocketHandle_addNewWebsocketConnection(struct mg_connection *c);
int websocketHandle_removeWebsocketConnection(struct mg_connection *c);
int websocketHandle_checkNewData(struct mg_connection *c, WebsocketHandle_DataType_e NewDataType, uint8_t *dataIn, uint32_t dataInLen);



#endif /* USER_WEBSOCKETHANDLE_H_ */
