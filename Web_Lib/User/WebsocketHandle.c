/*
 * WebsocketHandle.c
 *
 *  Created on: Apr 22, 2025
 *      Author: RAYA_4
 */


#include "WebsocketHandle.h"
#include "Web_User.h"
#include "mongoose.h"
#include "SHA256/MySHA256.h"



#define LIST_CLIENT_MAX_CLIENT		(WEB_USER_MAX_CLIENT - 2)



typedef struct
{
    bool clientIsSet;

    struct
    {
        uint8_t ipV4[4];
        int portClient;
    }ConnectionInfo;

    struct
    {
        uint8_t hashData_totalAndFlow[32];
        uint8_t hashData_totalAndFlowError[32];
    }LastBuffer;
    
}WebSocketClient_t;



typedef struct
{
    WebSocketClient_t WebSocketClient[LIST_CLIENT_MAX_CLIENT];
}WebsocketHandle_t;




WebsocketHandle_t WebsocketHandle;




void websoketHandle_init()
{
    memset(WebsocketHandle.WebSocketClient, 0, sizeof(WebsocketHandle.WebSocketClient));
}


int websocketHandle_addNewConnection(struct mg_connection *c)
{
    /**
     * 1- find empty list if not find return 1
     * 2- set ip and port and clear hash buffers
    */

    uint8_t clientIndex = 0;
    bool emptyIndexFound = false;

    for (uint8_t i = 0; i < LIST_CLIENT_MAX_CLIENT; i++)
    {
        if(WebsocketHandle.WebSocketClient[i].clientIsSet == false)
        {
            emptyIndexFound = true;
            break; 
        }
        clientIndex++;
    }

    if(emptyIndexFound == false)
    {
        return 1;
    }
    


    //Set Client Info
    WebsocketHandle.WebSocketClient[clientIndex].ConnectionInfo.ipV4[0] = c.

    WebsocketHandle.WebSocketClient[clientIndex].clientIsSet = true;
}


void websocketHandle_removeConnection(struct mg_connection *c)
{

}


void websocketHandle_checkNewData(struct mg_connection *c)
{

}




