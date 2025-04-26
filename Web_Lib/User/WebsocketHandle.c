/*
 * WebsocketHandle.c
 *
 *  Created on: Apr 22, 2025
 *      Author: RAYA_4
 */


#include "WebsocketHandle.h"
#include "Web_User.h"
#include "SHA256/MySHA256.h"
#include "HMI_Connextion.h"


#define LIST_CLIENT_MAX_CLIENT		(WEB_USER_MAX_CLIENT - 2)



typedef struct
{
    bool clientIsSet;

    struct
    {
        uint8_t ipV4[4];
        uint16_t portClient;
    }ConnectionInfo;

    struct
    {
        uint8_t hashData_st1TotalAndFlow[32];
        uint8_t hashData_st1TotalAndFlowError[32];
    }LastBuffer;
    
}WebSocketClient_t;



typedef struct
{
    WebSocketClient_t WebSocketClient[LIST_CLIENT_MAX_CLIENT];
}WebsocketHandle_t;



extern gParamFromHmi_t gParamFromHmi;


WebsocketHandle_t WebsocketHandle;
SHA256_CTX Sha256Ctx;




/*Function Prototypes*/
static void hashDataType_st1TotalAndFlow(uint8_t *outputHash);




void websoketHandle_init()
{
    memset(WebsocketHandle.WebSocketClient, 0, sizeof(WebsocketHandle.WebSocketClient));
}


int websocketHandle_addNewWebsocketConnection(struct mg_connection *c)
{
    /**
     * 1- find empty list if not find return -1
     * 2- set ip and port and clear hash buffers
    */

    uint8_t clientIndex = 0;
    bool emptyIndexFound = false;

    for (uint8_t i = 0; i < LIST_CLIENT_MAX_CLIENT; i++)
    {
        if(WebsocketHandle.WebSocketClient[i].clientIsSet == false)
        {
            emptyIndexFound = true;
            clientIndex = i;
            break; 
        }
    }

    if(emptyIndexFound == false)
    {
        return -1;
    }
    


    //Set Client Info
    WebsocketHandle.WebSocketClient[clientIndex].ConnectionInfo.ipV4[0] = c->rem.ip[0];
    WebsocketHandle.WebSocketClient[clientIndex].ConnectionInfo.ipV4[1] = c->rem.ip[1];
    WebsocketHandle.WebSocketClient[clientIndex].ConnectionInfo.ipV4[2] = c->rem.ip[2];
    WebsocketHandle.WebSocketClient[clientIndex].ConnectionInfo.ipV4[3] = c->rem.ip[3];

    WebsocketHandle.WebSocketClient[clientIndex].ConnectionInfo.portClient = c->rem.port;

    memset(&WebsocketHandle.WebSocketClient[clientIndex].LastBuffer, 0, sizeof(WebsocketHandle.WebSocketClient[clientIndex].LastBuffer));

    WebsocketHandle.WebSocketClient[clientIndex].clientIsSet = true;


    return 0;
}


int websocketHandle_removeWebsocketConnection(struct mg_connection *c)
{
    uint8_t tIpV4[4];
    uint16_t tPort;
    bool clientFound = false;
    uint8_t clientIndex = 0;

    tIpV4[0] = c->rem.ip[0];
    tIpV4[1] = c->rem.ip[1];
    tIpV4[2] = c->rem.ip[2];
    tIpV4[3] = c->rem.ip[3];

    tPort = c->rem.port;

    for (uint8_t i = 0; i < LIST_CLIENT_MAX_CLIENT; i++)
    {
        if(tPort == WebsocketHandle.WebSocketClient[i].ConnectionInfo.portClient)
        {
            if(memcmp(tIpV4, WebsocketHandle.WebSocketClient[i].ConnectionInfo.ipV4, sizeof(tIpV4)) == 0)
            {
                clientFound = true;
                clientIndex = i;
                break;
            }
        }
    }

    if(clientFound == false)
    {
        return -1;
    }

    //clear list
    memset(&WebsocketHandle.WebSocketClient[clientIndex], 0, sizeof(WebsocketHandle.WebSocketClient[clientIndex]));

    return 0;
}


int websocketHandle_checkNewData(struct mg_connection *c, WebsocketHandle_DataType_e NewDataType)
{
    /**
     * 1- find client and if not found return -1.
     * 2- hash new Data
     * 3- check hash new with hash old
     * 4- if hash is diffrent return 1 else return 0
     * 5- set new hash to old hash
    */

    int outputRes;
    uint8_t tIpV4[4];
    uint16_t tPort;
    bool clientFound = false;
    uint8_t clientIndex = 0;
    uint8_t newSha256[32];

    tIpV4[0] = c->rem.ip[0];
    tIpV4[1] = c->rem.ip[1];
    tIpV4[2] = c->rem.ip[2];
    tIpV4[3] = c->rem.ip[3];

    tPort = c->rem.port;

    for (uint8_t i = 0; i < LIST_CLIENT_MAX_CLIENT; i++)
    {
        if(tPort == WebsocketHandle.WebSocketClient[i].ConnectionInfo.portClient)
        {
            if(memcmp(tIpV4, WebsocketHandle.WebSocketClient[i].ConnectionInfo.ipV4, sizeof(tIpV4)) == 0)
            {
                clientFound = true;
                clientIndex = i;
                break;
            }
        }
    }

    if(clientFound == false)
    {
        return -1;
    }


    switch (NewDataType)
    {
    //Stream 1
    case WebsocketHandle_DataType_St1TotalAndFlow:
        hashDataType_st1TotalAndFlow(newSha256);
        if(memcmp(newSha256, WebsocketHandle.WebSocketClient[clientIndex].LastBuffer.hashData_st1TotalAndFlow, sizeof(newSha256)) != 0)
        {
            outputRes = 1;
            memcpy(WebsocketHandle.WebSocketClient[clientIndex].LastBuffer.hashData_st1TotalAndFlow, newSha256, sizeof(newSha256));
        }
        else
        {
            outputRes = 0;
        }
        break;


    //Stream 2
    case WebsocketHandle_DataType_St2TotalAndFlow:
        NULL;
        break;


    //Stream 3
    case WebsocketHandle_DataType_St3TotalAndFlow:
        NULL;
        break;


    //System
    }


    return outputRes;
}




/*hash functions Type of Datas*/
static void hashDataType_st1TotalAndFlow(uint8_t *outputHash)
{
	sha256_init(&Sha256Ctx);

	sha256_update(&Sha256Ctx, (const uint8_t *)&gParamFromHmi.Data.AllDataFlow.Now[0], sizeof(gParamFromHmi.Data.AllDataFlow.Now[0]));

	sha256_final(&Sha256Ctx, outputHash);
}




