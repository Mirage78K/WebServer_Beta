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


#define LIST_CLIENT_MAX_CLIENT		(WEB_USER_MAX_CLIENT+2) //+2 for insure



typedef struct
{
    bool clientIsSet;
    uint8_t wsSendStageStep;

    struct
    {
        uint8_t ipV4[4];
        uint16_t portClient;
    }ConnectionInfo;

    struct
    {
        uint8_t hashData_wsSepc[32];

        //Stream 1
        uint8_t hashData_st1TotalAndFlowAndErrorAndOutCalc[32];
        uint8_t hashData_st1Average[32];
        uint8_t hashData_st1TotalPrevious[32];
        uint8_t hashData_st1Param_var[32];
        uint8_t hashData_st1Param_const[32];
        uint8_t hashData_st1Param_gas21x[32];

        //Stream 2
        uint8_t hashData_st2TotalAndFlowAndErrorAndOutCalc[32];
        uint8_t hashData_st2Average[32];
        uint8_t hashData_st2TotalPrevious[32];
        uint8_t hashData_st2Param_var[32];
        uint8_t hashData_st2Param_const[32];
        uint8_t hashData_st2Param_gas21x[32];

        //Stream 3
        uint8_t hashData_st3TotalAndFlowAndErrorAndOutCalc[32];
        uint8_t hashData_st3Average[32];
        uint8_t hashData_st3TotalPrevious[32];
        uint8_t hashData_st3Param_var[32];
        uint8_t hashData_st3Param_const[32];
        uint8_t hashData_st3Param_gas21x[32];
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
static void hashDataType_stTotalAndFlow(uint8_t *outputHash, uint8_t streamNumAtZero);
static void hashDataType_stAverage(uint8_t *outputHash, uint8_t streamNumAtZero);
static void hashDataType_stTotalPrevious(uint8_t *outputHash, uint8_t streamNumAtZero);
static void hashDataType_stParamConst(uint8_t *outputHash, uint8_t streamNumAtZero);
static void hashDataType_stParamVar(uint8_t *outputHash, uint8_t streamNumAtZero);
static void hashDataType_stParamGas21x(uint8_t *outputHash, uint8_t streamNumAtZero);



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

    WebsocketHandle.WebSocketClient[clientIndex].wsSendStageStep = 0;

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
        hashDataType_stTotalAndFlow(newSha256, 1-1);
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
        hashDataType_stAverage(newSha256, 1-1);
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
        hashDataType_stTotalPrevious(newSha256, 1-1);
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

    case WebsocketHandle_DataType_St1ParamVar:
        hashDataType_stParamVar(newSha256, 1-1);
        if(memcmp(newSha256, WebsocketHandle.WebSocketClient[clientIndex].LastBuffer.hashData_st1Param_var, sizeof(newSha256)) != 0)
        {
            outputRes = 1;
            memcpy(WebsocketHandle.WebSocketClient[clientIndex].LastBuffer.hashData_st1Param_var, newSha256, sizeof(newSha256));
        }
        else
        {
            outputRes = 0;
        }
        break;

    case WebsocketHandle_DataType_St1ParamConst:
        hashDataType_stParamConst(newSha256, 1-1);
        if(memcmp(newSha256, WebsocketHandle.WebSocketClient[clientIndex].LastBuffer.hashData_st1Param_const, sizeof(newSha256)) != 0)
        {
            outputRes = 1;
            memcpy(WebsocketHandle.WebSocketClient[clientIndex].LastBuffer.hashData_st1Param_const, newSha256, sizeof(newSha256));
        }
        else
        {
            outputRes = 0;
        }
        break;

    case WebsocketHandle_DataType_St1ParamGas21X:
        hashDataType_stParamGas21x(newSha256, 1-1);
        if(memcmp(newSha256, WebsocketHandle.WebSocketClient[clientIndex].LastBuffer.hashData_st1Param_gas21x, sizeof(newSha256)) != 0)
        {
            outputRes = 1;
            memcpy(WebsocketHandle.WebSocketClient[clientIndex].LastBuffer.hashData_st1Param_gas21x, newSha256, sizeof(newSha256));
        }
        else
        {
            outputRes = 0;
        }
        break;


    //Stream 2
    case WebsocketHandle_DataType_St2TotalAndFlow:
        hashDataType_stTotalAndFlow(newSha256, 2-1);
        if(memcmp(newSha256, WebsocketHandle.WebSocketClient[clientIndex].LastBuffer.hashData_st2TotalAndFlowAndErrorAndOutCalc, sizeof(newSha256)) != 0)
        {
            outputRes = 1;
            memcpy(WebsocketHandle.WebSocketClient[clientIndex].LastBuffer.hashData_st2TotalAndFlowAndErrorAndOutCalc, newSha256, sizeof(newSha256));
        }
        else
        {
            outputRes = 0;
        }
        break;

    case WebsocketHandle_DataType_St2Average:
        hashDataType_stAverage(newSha256, 2-1);
        if(memcmp(newSha256, WebsocketHandle.WebSocketClient[clientIndex].LastBuffer.hashData_st2Average, sizeof(newSha256)) != 0)
        {
            outputRes = 1;
            memcpy(WebsocketHandle.WebSocketClient[clientIndex].LastBuffer.hashData_st2Average, newSha256, sizeof(newSha256));
        }
        else
        {
            outputRes = 0;
        }
        break;

    case WebsocketHandle_DataType_St2TotalPrevious:
        hashDataType_stTotalPrevious(newSha256, 2-1);
        if(memcmp(newSha256, WebsocketHandle.WebSocketClient[clientIndex].LastBuffer.hashData_st2TotalPrevious, sizeof(newSha256)) != 0)
        {
            outputRes = 1;
            memcpy(WebsocketHandle.WebSocketClient[clientIndex].LastBuffer.hashData_st2TotalPrevious, newSha256, sizeof(newSha256));
        }
        else
        {
            outputRes = 0;
        }
        break;

    case WebsocketHandle_DataType_St2ParamVar:
        hashDataType_stParamVar(newSha256, 2-1);
        if(memcmp(newSha256, WebsocketHandle.WebSocketClient[clientIndex].LastBuffer.hashData_st2Param_var, sizeof(newSha256)) != 0)
        {
            outputRes = 1;
            memcpy(WebsocketHandle.WebSocketClient[clientIndex].LastBuffer.hashData_st2Param_var, newSha256, sizeof(newSha256));
        }
        else
        {
            outputRes = 0;
        }
        break;

    case WebsocketHandle_DataType_St2ParamConst:
        hashDataType_stParamConst(newSha256, 2-1);
        if(memcmp(newSha256, WebsocketHandle.WebSocketClient[clientIndex].LastBuffer.hashData_st2Param_const, sizeof(newSha256)) != 0)
        {
            outputRes = 1;
            memcpy(WebsocketHandle.WebSocketClient[clientIndex].LastBuffer.hashData_st2Param_const, newSha256, sizeof(newSha256));
        }
        else
        {
            outputRes = 0;
        }
        break;

    case WebsocketHandle_DataType_St2ParamGas21X:
        hashDataType_stParamGas21x(newSha256, 2-1);
        if(memcmp(newSha256, WebsocketHandle.WebSocketClient[clientIndex].LastBuffer.hashData_st2Param_gas21x, sizeof(newSha256)) != 0)
        {
            outputRes = 1;
            memcpy(WebsocketHandle.WebSocketClient[clientIndex].LastBuffer.hashData_st2Param_gas21x, newSha256, sizeof(newSha256));
        }
        else
        {
            outputRes = 0;
        }
        break;


    //Stream 3
    case WebsocketHandle_DataType_St3TotalAndFlow:
        hashDataType_stTotalAndFlow(newSha256, 3-1);
        if(memcmp(newSha256, WebsocketHandle.WebSocketClient[clientIndex].LastBuffer.hashData_st3TotalAndFlowAndErrorAndOutCalc, sizeof(newSha256)) != 0)
        {
            outputRes = 1;
            memcpy(WebsocketHandle.WebSocketClient[clientIndex].LastBuffer.hashData_st3TotalAndFlowAndErrorAndOutCalc, newSha256, sizeof(newSha256));
        }
        else
        {
            outputRes = 0;
        }
        break;

    case WebsocketHandle_DataType_St3Average:
        hashDataType_stAverage(newSha256, 3-1);
        if(memcmp(newSha256, WebsocketHandle.WebSocketClient[clientIndex].LastBuffer.hashData_st3Average, sizeof(newSha256)) != 0)
        {
            outputRes = 1;
            memcpy(WebsocketHandle.WebSocketClient[clientIndex].LastBuffer.hashData_st3Average, newSha256, sizeof(newSha256));
        }
        else
        {
            outputRes = 0;
        }
        break;

    case WebsocketHandle_DataType_St3TotalPrevious:
        hashDataType_stTotalPrevious(newSha256, 3-1);
        if(memcmp(newSha256, WebsocketHandle.WebSocketClient[clientIndex].LastBuffer.hashData_st3TotalPrevious, sizeof(newSha256)) != 0)
        {
            outputRes = 1;
            memcpy(WebsocketHandle.WebSocketClient[clientIndex].LastBuffer.hashData_st3TotalPrevious, newSha256, sizeof(newSha256));
        }
        else
        {
            outputRes = 0;
        }
        break;

        case WebsocketHandle_DataType_St3ParamVar:
        hashDataType_stParamVar(newSha256, 3-1);
        if(memcmp(newSha256, WebsocketHandle.WebSocketClient[clientIndex].LastBuffer.hashData_st3Param_var, sizeof(newSha256)) != 0)
        {
            outputRes = 1;
            memcpy(WebsocketHandle.WebSocketClient[clientIndex].LastBuffer.hashData_st3Param_var, newSha256, sizeof(newSha256));
        }
        else
        {
            outputRes = 0;
        }
        break;

    case WebsocketHandle_DataType_St3ParamConst:
        hashDataType_stParamConst(newSha256, 3-1);
        if(memcmp(newSha256, WebsocketHandle.WebSocketClient[clientIndex].LastBuffer.hashData_st3Param_const, sizeof(newSha256)) != 0)
        {
            outputRes = 1;
            memcpy(WebsocketHandle.WebSocketClient[clientIndex].LastBuffer.hashData_st3Param_const, newSha256, sizeof(newSha256));
        }
        else
        {
            outputRes = 0;
        }
        break;

    case WebsocketHandle_DataType_St3ParamGas21X:
        hashDataType_stParamGas21x(newSha256, 3-1);
        if(memcmp(newSha256, WebsocketHandle.WebSocketClient[clientIndex].LastBuffer.hashData_st3Param_gas21x, sizeof(newSha256)) != 0)
        {
            outputRes = 1;
            memcpy(WebsocketHandle.WebSocketClient[clientIndex].LastBuffer.hashData_st3Param_gas21x, newSha256, sizeof(newSha256));
        }
        else
        {
            outputRes = 0;
        }
        break;


    //System
    }


    return outputRes;
}


int websocketHandle_getWsSendStageStep(struct mg_connection *c)     //if error return negative value
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


    if(clientFound == true)
    {
        return  (int)WebsocketHandle.WebSocketClient[clientIndex].wsSendStageStep;
    }
    else
    {
        return -1;
    }
}

int websocketHandle_setWsSendStageStep(struct mg_connection *c, uint8_t stepValue)
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


    if(clientFound == true)
    {
        WebsocketHandle.WebSocketClient[clientIndex].wsSendStageStep = stepValue;
        return  0;
    }
    else
    {
        return -1;
    }
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


static void hashDataType_stTotalAndFlow(uint8_t *outputHash, uint8_t streamNumAtZero)
{
	sha256_init(&Sha256Ctx);

	sha256_update(&Sha256Ctx, (const uint8_t *)&gParamFromHmi.Data.AllDataFlow.Now[streamNumAtZero], sizeof(gParamFromHmi.Data.AllDataFlow.Now[streamNumAtZero]));

    if(gParamFromHmi.Setting.SetupStream[streamNumAtZero].Fluid.fluid == Stream_Gasses)
    {
        sha256_update(&Sha256Ctx, (const uint8_t *)&gParamFromHmi.Data.AllDataFlow.Now[streamNumAtZero].conversion_c_factor, sizeof(gParamFromHmi.Data.AllDataFlow.Now[streamNumAtZero].conversion_c_factor));
        sha256_update(&Sha256Ctx, (const uint8_t *)&gParamFromHmi.Data.AllOutputCalc.NaturalGas[streamNumAtZero].zFactor, sizeof(gParamFromHmi.Data.AllOutputCalc.NaturalGas[streamNumAtZero].zFactor));
        sha256_update(&Sha256Ctx, (const uint8_t *)&gParamFromHmi.Data.AllDataFlow.Now[streamNumAtZero].density, sizeof(gParamFromHmi.Data.AllDataFlow.Now[streamNumAtZero].density));
        sha256_update(&Sha256Ctx, (const uint8_t *)&gParamFromHmi.Data.AllOutputCalc.General[streamNumAtZero].pb, sizeof(gParamFromHmi.Data.AllOutputCalc.General[streamNumAtZero].pb));
    }
    else if(gParamFromHmi.Setting.SetupStream[streamNumAtZero].Fluid.fluid == Stream_Liquid)
    {
        sha256_update(&Sha256Ctx, (const uint8_t *)&gParamFromHmi.Data.AllOutputCalc.Liquid[streamNumAtZero].cpl, sizeof(gParamFromHmi.Data.AllOutputCalc.Liquid[streamNumAtZero].cpl));
        sha256_update(&Sha256Ctx, (const uint8_t *)&gParamFromHmi.Data.AllOutputCalc.Liquid[streamNumAtZero].ctl, sizeof(gParamFromHmi.Data.AllOutputCalc.Liquid[streamNumAtZero].ctl));
        sha256_update(&Sha256Ctx, (const uint8_t *)&gParamFromHmi.Data.AllDataFlow.Now[streamNumAtZero].density, sizeof(gParamFromHmi.Data.AllDataFlow.Now[streamNumAtZero].density));
        sha256_update(&Sha256Ctx, (const uint8_t *)&gParamFromHmi.Data.AllOutputCalc.General[streamNumAtZero].pb, sizeof(gParamFromHmi.Data.AllOutputCalc.General[streamNumAtZero].pb));
    }

	sha256_final(&Sha256Ctx, outputHash);
}


static void hashDataType_stAverage(uint8_t *outputHash, uint8_t streamNumAtZero)
{
	sha256_init(&Sha256Ctx);

	sha256_update(&Sha256Ctx, (const uint8_t *)&gParamFromHmi.Data.AllDataFlow.AverageLastMin[streamNumAtZero], sizeof(gParamFromHmi.Data.AllDataFlow.AverageLastMin[streamNumAtZero]));
    sha256_update(&Sha256Ctx, (const uint8_t *)&gParamFromHmi.Data.AllDataFlow.AverageLastHour[streamNumAtZero], sizeof(gParamFromHmi.Data.AllDataFlow.AverageLastHour[streamNumAtZero]));
    sha256_update(&Sha256Ctx, (const uint8_t *)&gParamFromHmi.Data.AllDataFlow.AverageLastDay[streamNumAtZero], sizeof(gParamFromHmi.Data.AllDataFlow.AverageLastDay[streamNumAtZero]));
    sha256_update(&Sha256Ctx, (const uint8_t *)&gParamFromHmi.Data.AllDataFlow.AverageLastMonth[streamNumAtZero], sizeof(gParamFromHmi.Data.AllDataFlow.AverageLastMonth[streamNumAtZero]));

	sha256_final(&Sha256Ctx, outputHash);
}


static void hashDataType_stTotalPrevious(uint8_t *outputHash, uint8_t streamNumAtZero)
{
	sha256_init(&Sha256Ctx);

	sha256_update(&Sha256Ctx, (const uint8_t *)&gParamFromHmi.Data.AllDataFlow.TotalPreviousDay[streamNumAtZero], sizeof(gParamFromHmi.Data.AllDataFlow.TotalPreviousDay[streamNumAtZero]));
    sha256_update(&Sha256Ctx, (const uint8_t *)&gParamFromHmi.Data.AllDataFlow.TotalPreviousMonth[streamNumAtZero], sizeof(gParamFromHmi.Data.AllDataFlow.TotalPreviousMonth[streamNumAtZero]));

	sha256_final(&Sha256Ctx, outputHash);
}


static void hashDataType_stParamConst(uint8_t *outputHash, uint8_t streamNumAtZero)
{
	sha256_init(&Sha256Ctx);


    //General
    sha256_update(&Sha256Ctx, (const uint8_t *)&gParamFromHmi.Setting.SetupStream[streamNumAtZero].Fluid.fluid, sizeof(gParamFromHmi.Setting.SetupStream[streamNumAtZero].Fluid.fluid));
    sha256_update(&Sha256Ctx, (const uint8_t *)&gParamFromHmi.Setting.SetupStream[streamNumAtZero].Fluid.fluid_type, sizeof(gParamFromHmi.Setting.SetupStream[streamNumAtZero].Fluid.fluid_type));
    sha256_update(&Sha256Ctx, (const uint8_t *)&gParamFromHmi.Setting.SetupStream[streamNumAtZero].UnitTotal.volume, sizeof(gParamFromHmi.Setting.SetupStream[streamNumAtZero].UnitTotal.volume));
    sha256_update(&Sha256Ctx, (const uint8_t *)&gParamFromHmi.Setting.SetupStream[streamNumAtZero].flowMeterType, sizeof(gParamFromHmi.Setting.SetupStream[streamNumAtZero].flowMeterType));
    sha256_update(&Sha256Ctx, (const uint8_t *)&gParamFromHmi.Setting.SetupStream[streamNumAtZero].UnitTotal.volume_flowrate, sizeof(gParamFromHmi.Setting.SetupStream[streamNumAtZero].UnitTotal.volume_flowrate));
    sha256_update(&Sha256Ctx, (const uint8_t *)&gParamFromHmi.Setting.SetupStream[streamNumAtZero].General.min_flowrate, sizeof(gParamFromHmi.Setting.SetupStream[streamNumAtZero].General.min_flowrate));
    sha256_update(&Sha256Ctx, (const uint8_t *)&gParamFromHmi.Setting.SetupStream[streamNumAtZero].General.max_flowrate, sizeof(gParamFromHmi.Setting.SetupStream[streamNumAtZero].General.max_flowrate));
    sha256_update(&Sha256Ctx, (const uint8_t *)&gParamFromHmi.Setting.SetupStream[streamNumAtZero].General.minor_keypad, sizeof(gParamFromHmi.Setting.SetupStream[streamNumAtZero].General.minor_keypad));
    sha256_update(&Sha256Ctx, (const uint8_t *)&gParamFromHmi.Setting.SetupStream[streamNumAtZero].General.maxor_keypad, sizeof(gParamFromHmi.Setting.SetupStream[streamNumAtZero].General.maxor_keypad));
    sha256_update(&Sha256Ctx, (const uint8_t *)&gParamFromHmi.Setting.SetupStream[streamNumAtZero].General.air_pressure, sizeof(gParamFromHmi.Setting.SetupStream[streamNumAtZero].General.air_pressure));


    if(gParamFromHmi.Setting.SetupStream[streamNumAtZero].Fluid.fluid == Stream_Gasses) //Fluid Gas
    {
        //Density and Density base and compressibility and compressibility base exist in output calc

        sha256_update(&Sha256Ctx, (const uint8_t *)&gParamFromHmi.Setting.SetupStream[streamNumAtZero].Fluid.pressure_base, sizeof(gParamFromHmi.Setting.SetupStream[streamNumAtZero].Fluid.pressure_base));
        sha256_update(&Sha256Ctx, (const uint8_t *)&gParamFromHmi.Setting.SetupStream[streamNumAtZero].Fluid.temperature_base, sizeof(gParamFromHmi.Setting.SetupStream[streamNumAtZero].Fluid.temperature_base));
    }
	else if(gParamFromHmi.Setting.SetupStream[streamNumAtZero].Fluid.fluid == Stream_Liquid)    //fluid gasoil
    {
        //Nothing
    }

    //flowmeter
	if(gParamFromHmi.Setting.SetupStream[streamNumAtZero].flowMeterType == Stream_Diff_Pressure_meter)
    {
        sha256_update(&Sha256Ctx, (const uint8_t *)&gParamFromHmi.Setting.SetupStream[streamNumAtZero].General.pipe_diameter, sizeof(gParamFromHmi.Setting.SetupStream[streamNumAtZero].General.pipe_diameter));
        sha256_update(&Sha256Ctx, (const uint8_t *)&gParamFromHmi.Setting.SetupStream[streamNumAtZero].General.dinamic_viscosity, sizeof(gParamFromHmi.Setting.SetupStream[streamNumAtZero].General.dinamic_viscosity));
        sha256_update(&Sha256Ctx, (const uint8_t *)&gParamFromHmi.Setting.SetupStream[streamNumAtZero].Flowmeter.diff_pressure_meter, sizeof(gParamFromHmi.Setting.SetupStream[streamNumAtZero].Flowmeter.diff_pressure_meter));
    }
    else if(gParamFromHmi.Setting.SetupStream[streamNumAtZero].flowMeterType == Stream_Pulse_Meter)
    {
        sha256_update(&Sha256Ctx, (const uint8_t *)&gParamFromHmi.Setting.SetupStream[streamNumAtZero].General.pipe_diameter, sizeof(gParamFromHmi.Setting.SetupStream[streamNumAtZero].General.pipe_diameter));
        sha256_update(&Sha256Ctx, (const uint8_t *)&gParamFromHmi.Setting.SetupStream[streamNumAtZero].Flowmeter.pulse_meter, sizeof(gParamFromHmi.Setting.SetupStream[streamNumAtZero].Flowmeter.pulse_meter));
    }
    else if(gParamFromHmi.Setting.SetupStream[streamNumAtZero].flowMeterType == Stream_Current_input)
    {
        sha256_update(&Sha256Ctx, (const uint8_t *)&gParamFromHmi.Setting.SetupStream[streamNumAtZero].Flowmeter.current_input, sizeof(gParamFromHmi.Setting.SetupStream[streamNumAtZero].Flowmeter.current_input));
    }
    else if(gParamFromHmi.Setting.SetupStream[streamNumAtZero].flowMeterType == Stream_Ultrasonic)
    {
        //Nothing
    }

	sha256_final(&Sha256Ctx, outputHash);
}


static void hashDataType_stParamVar(uint8_t *outputHash, uint8_t streamNumAtZero)
{
	sha256_init(&Sha256Ctx);


    if(gParamFromHmi.Setting.SetupStream[streamNumAtZero].Fluid.fluid == Stream_Gasses) //Fluid Gas
    {
        //Density and Density Base and compressibility exist in output calc

        sha256_update(&Sha256Ctx, (const uint8_t *)&gParamFromHmi.Data.AllOutputCalc.NaturalGas[streamNumAtZero].zFactorBase, sizeof(gParamFromHmi.Data.AllOutputCalc.NaturalGas[streamNumAtZero].zFactorBase));
        sha256_update(&Sha256Ctx, (const uint8_t *)&gParamFromHmi.Data.AllOutputCalc.NaturalGas[streamNumAtZero].molarMass, sizeof(gParamFromHmi.Data.AllOutputCalc.NaturalGas[streamNumAtZero].molarMass));
        sha256_update(&Sha256Ctx, (const uint8_t *)&gParamFromHmi.Data.AllOutputCalc.NaturalGas[streamNumAtZero].speedOfFluid, sizeof(gParamFromHmi.Data.AllOutputCalc.NaturalGas[streamNumAtZero].speedOfFluid));
        sha256_update(&Sha256Ctx, (const uint8_t *)&gParamFromHmi.Data.AllOutputCalc.NaturalGas[streamNumAtZero].speedOfSound, sizeof(gParamFromHmi.Data.AllOutputCalc.NaturalGas[streamNumAtZero].speedOfSound));
        sha256_update(&Sha256Ctx, (const uint8_t *)&gParamFromHmi.Data.AllOutputCalc.NaturalGas[streamNumAtZero].isentropicExponent, sizeof(gParamFromHmi.Data.AllOutputCalc.NaturalGas[streamNumAtZero].isentropicExponent));
        sha256_update(&Sha256Ctx, (const uint8_t *)&gParamFromHmi.Data.AllOutputCalc.General[streamNumAtZero].reynoldNumber, sizeof(gParamFromHmi.Data.AllOutputCalc.General[streamNumAtZero].reynoldNumber));
    }
	else if(gParamFromHmi.Setting.SetupStream[streamNumAtZero].Fluid.fluid == Stream_Liquid)    //fluid gasoil
    {
        //cpl and ctl and density exist in output calc
    }

    //flowmeter
	if(gParamFromHmi.Setting.SetupStream[streamNumAtZero].flowMeterType == Stream_Diff_Pressure_meter)
    {
        sha256_update(&Sha256Ctx, (const uint8_t *)&gParamFromHmi.Data.AllOutputCalc.NaturalGas[streamNumAtZero].epansionFactor, sizeof(gParamFromHmi.Data.AllOutputCalc.NaturalGas[streamNumAtZero].epansionFactor));
        sha256_update(&Sha256Ctx, (const uint8_t *)&gParamFromHmi.Data.AllOutputCalc.NaturalGas[streamNumAtZero].orificeCoefficeint, sizeof(gParamFromHmi.Data.AllOutputCalc.NaturalGas[streamNumAtZero].orificeCoefficeint));
    }
    else if(gParamFromHmi.Setting.SetupStream[streamNumAtZero].flowMeterType == Stream_Pulse_Meter)
    {
        //Nothing
    }
    else if(gParamFromHmi.Setting.SetupStream[streamNumAtZero].flowMeterType == Stream_Current_input)
    {
        //Nothing
    }
    else if(gParamFromHmi.Setting.SetupStream[streamNumAtZero].flowMeterType == Stream_Ultrasonic)
    {
        //Nothing
    }

	sha256_final(&Sha256Ctx, outputHash);
}


static void hashDataType_stParamGas21x(uint8_t *outputHash, uint8_t streamNumAtZero)
{
    if(gParamFromHmi.Setting.SetupStream[streamNumAtZero].Fluid.fluid == Stream_Gasses) //Fluid Gas
    {
        sha256_init(&Sha256Ctx);

        sha256_update(&Sha256Ctx, (const uint8_t *)&gParamFromHmi.Setting.SetupStream[streamNumAtZero].Fluid.natural_gas._x, sizeof(gParamFromHmi.Setting.SetupStream[streamNumAtZero].Fluid.natural_gas._x));

        sha256_final(&Sha256Ctx, outputHash);
    }
    else
    {
        memset(outputHash, 0, 32 * sizeof(uint8_t));
    }
}


