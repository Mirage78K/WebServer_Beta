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
#include "parameter.h"


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
        uint8_t hashData_wsSepc[32];
        uint8_t hashData_st1TotalAndFlowAndErrorAndOutCalc[32];
        uint8_t hashData_st1Average[32];
        uint8_t hashData_st1TotalPrevious[32];
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
static void hashDataType_wsSpec(uint8_t *outputHash);
static void hashDataType_st1TotalAndFlow(uint8_t *outputHash);
static void hashDataType_st1Average(uint8_t *outputHash);
static void hashDataType_st1TotalPrevious(uint8_t *outputHash);




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

    //WS Spec
    case WebsocketHandle_DataType_WsSpec:
        hashDataType_wsSpec(newSha256);
        if(memcmp(newSha256, WebsocketHandle.WebSocketClient[clientIndex].LastBuffer.hashData_wsSepc, sizeof(newSha256)) != 0)
        {
            outputRes = 1;
            memcpy(WebsocketHandle.WebSocketClient[clientIndex].LastBuffer.hashData_wsSepc, newSha256, sizeof(newSha256));
        }
        else
        {
            outputRes = 0;
        }
        break;


    //Stream 1
    case WebsocketHandle_DataType_St1TotalAndFlow:
        hashDataType_st1TotalAndFlow(newSha256);
        if(memcmp(newSha256, WebsocketHandle.WebSocketClient[clientIndex].LastBuffer.hashData_st1TotalAndFlowAndErrorAndOutCalc, sizeof(newSha256)) != 0)
        {
            outputRes = 1;
            memcpy(WebsocketHandle.WebSocketClient[clientIndex].LastBuffer.hashData_st1TotalAndFlowAndErrorAndOutCalc, newSha256, sizeof(newSha256));
        }
        else
        {
            outputRes = 0;
        }
        break;

    case WebsocketHandle_DataType_St1Average:
        hashDataType_st1Average(newSha256);
        if(memcmp(newSha256, WebsocketHandle.WebSocketClient[clientIndex].LastBuffer.hashData_st1Average, sizeof(newSha256)) != 0)
        {
            outputRes = 1;
            memcpy(WebsocketHandle.WebSocketClient[clientIndex].LastBuffer.hashData_st1Average, newSha256, sizeof(newSha256));
        }
        else
        {
            outputRes = 0;
        }
        break;

    case WebsocketHandle_DataType_St1TotalPrevious:
        hashDataType_st1TotalPrevious(newSha256);
        if(memcmp(newSha256, WebsocketHandle.WebSocketClient[clientIndex].LastBuffer.hashData_st1TotalPrevious, sizeof(newSha256)) != 0)
        {
            outputRes = 1;
            memcpy(WebsocketHandle.WebSocketClient[clientIndex].LastBuffer.hashData_st1TotalPrevious, newSha256, sizeof(newSha256));
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




/*hash functions*/
static void hashDataType_wsSpec(uint8_t *outputHash)    //check for all streams
{
	sha256_init(&Sha256Ctx);

    for (uint8_t i = 0; i < WSCONN_PARAM_STREAM_NUM; i++)
    {
        sha256_update(&Sha256Ctx, (const uint8_t *)&gParamFromHmi.Setting.SetupStream[i].flowMeterType, sizeof(gParamFromHmi.Setting.SetupStream[i].flowMeterType));
        sha256_update(&Sha256Ctx, (const uint8_t *)&gParamFromHmi.Setting.SetupStream[i].Fluid.fluid, sizeof(gParamFromHmi.Setting.SetupStream[i].Fluid.fluid));
        sha256_update(&Sha256Ctx, (const uint8_t *)&gParamFromHmi.Setting.SetupStream[i].Fluid.fluid_type, sizeof(gParamFromHmi.Setting.SetupStream[i].Fluid.fluid_type));
    }

	sha256_final(&Sha256Ctx, outputHash);
}


static void hashDataType_st1TotalAndFlow(uint8_t *outputHash)
{
	sha256_init(&Sha256Ctx);

	sha256_update(&Sha256Ctx, (const uint8_t *)&gParamFromHmi.Data.AllDataFlow.Now[0], sizeof(gParamFromHmi.Data.AllDataFlow.Now[0]));

    if(gParamFromHmi.Setting.SetupStream[0].Fluid.fluid == Stream_Gasses)
    {
        sha256_update(&Sha256Ctx, (const uint8_t *)&gParamFromHmi.Data.AllDataFlow.Now[0].conversion_c_factor, sizeof(gParamFromHmi.Data.AllDataFlow.Now[0].conversion_c_factor));
        sha256_update(&Sha256Ctx, (const uint8_t *)&gParamFromHmi.Data.AllOutputCalc.NaturalGas[0].zFactor, sizeof(gParamFromHmi.Data.AllOutputCalc.NaturalGas[0].zFactor));
        sha256_update(&Sha256Ctx, (const uint8_t *)&gParamFromHmi.Data.AllDataFlow.Now[0].density, sizeof(gParamFromHmi.Data.AllDataFlow.Now[0].density));
        sha256_update(&Sha256Ctx, (const uint8_t *)&gParamFromHmi.Data.AllOutputCalc.General[0].pb, sizeof(gParamFromHmi.Data.AllOutputCalc.General[0].pb));
    }
    else if(gParamFromHmi.Setting.SetupStream[0].Fluid.fluid == Stream_Liquid)
    {
        sha256_update(&Sha256Ctx, (const uint8_t *)&gParamFromHmi.Data.AllOutputCalc.Liquid[0].cpl, sizeof(gParamFromHmi.Data.AllOutputCalc.Liquid[0].cpl));
        sha256_update(&Sha256Ctx, (const uint8_t *)&gParamFromHmi.Data.AllOutputCalc.Liquid[0].ctl, sizeof(gParamFromHmi.Data.AllOutputCalc.Liquid[0].ctl));
        sha256_update(&Sha256Ctx, (const uint8_t *)&gParamFromHmi.Data.AllDataFlow.Now[0].density, sizeof(gParamFromHmi.Data.AllDataFlow.Now[0].density));
        sha256_update(&Sha256Ctx, (const uint8_t *)&gParamFromHmi.Data.AllOutputCalc.General[0].pb, sizeof(gParamFromHmi.Data.AllOutputCalc.General[0].pb));
    }

	sha256_final(&Sha256Ctx, outputHash);
}


static void hashDataType_st1Average(uint8_t *outputHash)
{
	sha256_init(&Sha256Ctx);

	sha256_update(&Sha256Ctx, (const uint8_t *)&gParamFromHmi.Data.AllDataFlow.AverageLastMin[0], sizeof(gParamFromHmi.Data.AllDataFlow.AverageLastMin[0]));
    sha256_update(&Sha256Ctx, (const uint8_t *)&gParamFromHmi.Data.AllDataFlow.AverageLastHour[0], sizeof(gParamFromHmi.Data.AllDataFlow.AverageLastHour[0]));
    sha256_update(&Sha256Ctx, (const uint8_t *)&gParamFromHmi.Data.AllDataFlow.AverageLastDay[0], sizeof(gParamFromHmi.Data.AllDataFlow.AverageLastDay[0]));
    sha256_update(&Sha256Ctx, (const uint8_t *)&gParamFromHmi.Data.AllDataFlow.AverageLastMonth[0], sizeof(gParamFromHmi.Data.AllDataFlow.AverageLastMonth[0]));

	sha256_final(&Sha256Ctx, outputHash);
}


static void hashDataType_st1TotalPrevious(uint8_t *outputHash)
{
	sha256_init(&Sha256Ctx);

	sha256_update(&Sha256Ctx, (const uint8_t *)&gParamFromHmi.Data.AllDataFlow.TotalPreviousDay[0], sizeof(gParamFromHmi.Data.AllDataFlow.TotalPreviousDay[0]));
    sha256_update(&Sha256Ctx, (const uint8_t *)&gParamFromHmi.Data.AllDataFlow.TotalPreviousMonth[0], sizeof(gParamFromHmi.Data.AllDataFlow.TotalPreviousMonth[0]));

	sha256_final(&Sha256Ctx, outputHash);
}


