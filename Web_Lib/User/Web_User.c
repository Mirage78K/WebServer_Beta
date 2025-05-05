/*
 * Web_User.c
 *
 *  Created on: Mar 25, 2025
 *      Author: mojkh
 */


#include "Web_User.h"
#include <string.h>
#include "cmsis_os.h"
#include "mongoose.h"
#include "mongoose_glue.h"
#include "DebugLog.h"
#include "HMI_Connextion.h"
#include "WebsocketHandle.h"
#include "WebUi_String.h"


#define ETH_PHY_ADDRESS						0

#define LAN8720_BASICSTATUS_REG				0x01U
#define LAN8720_SYMBOLERRORCOUNTER_REG		0x1AU








extern void http_ev_handler(struct mg_connection *c, int ev, void *ev_data);
extern void send_websocket_data(void);

/*extern variable*/
extern gParamFromHmi_t gParamFromHmi;

/*Global Variable*/
TaskHandle_t WebServer_TaskHandler;


/*Local Variable*/
static char jsonSendBuffer[4096];
static char tempBuff[64];
gWebServer_t gWebServer;


//third pary
extern struct mg_mgr g_mgr;
struct mg_tcpip_if mif;
struct mg_tcpip_driver_stm32f_data driver_data;


//websocket
static void ws_1000(struct mg_connection *c);



static void userInit();
static void thirdPartyInit();
static void checkPhyHandle();

static uint16_t readPhy(uint8_t addr, uint8_t reg);
static void mylog(char ch, void *param);

static inline int numconns(struct mg_mgr *mgr);

static void setJson_wsSpecAndStreamStatus(char *buffer);
static void setJson_totalAndFlowAndInputSignalAndOutputcalcAndTotalErrorAndFlowError(uint8_t streamNum, char *buffer);
static void setJson_average(uint8_t streamNum, char *buffer);
static void setJson_totalPrevious(uint8_t streamNum, char *buffer);
static void setJson_paramVar(uint8_t streamNum, char *buffer);
static void setJson_paramConst(uint8_t streamNum, char *buffer);
static void setJson_paramGas21x(uint8_t streamNum, char *buffer);

static int wsSend_stage0(struct mg_connection *c);
static int wsSend_stage1(struct mg_connection *c);
static int wsSend_stage2(struct mg_connection *c);
static int wsSend_stage3(struct mg_connection *c);
static int wsSend_stage4(struct mg_connection *c);
static int wsSend_stage5(struct mg_connection *c);




void webUser_taskHandler_webServer(void *arg)
{
	//initial
	userInit();
	thirdPartyInit();

	while(1)
	{

		//Thirdparty
		checkPhyHandle();
		mg_mgr_poll(&g_mgr, 100);
		send_websocket_data();
	}
}


void webUser_initialBeforeTask()
{
	//Nothing
}


static void userInit()
{
	gWebServer.Config.Driver.macAddress[0] = 0x02;
	gWebServer.Config.Driver.macAddress[1] = 0x22;
	gWebServer.Config.Driver.macAddress[2] = 0x44;
	gWebServer.Config.Driver.macAddress[3] = 0x55;
	gWebServer.Config.Driver.macAddress[4] = 0xAA;
	gWebServer.Config.Driver.macAddress[5] = 0xBB;

	gWebServer.Config.Driver.ipV4[0] = 192;
	gWebServer.Config.Driver.ipV4[1] = 168;
	gWebServer.Config.Driver.ipV4[2] = 88;
	gWebServer.Config.Driver.ipV4[3] = 170;

	gWebServer.Config.Driver.maskV4[0] = 255;
	gWebServer.Config.Driver.maskV4[1] = 255;
	gWebServer.Config.Driver.maskV4[2] = 255;
	gWebServer.Config.Driver.maskV4[3] = 0;

	gWebServer.Config.Driver.gwV4[0] = 192;
	gWebServer.Config.Driver.gwV4[1] = 168;
	gWebServer.Config.Driver.gwV4[2] = 88;
	gWebServer.Config.Driver.gwV4[3] = 1;

	websoketHandle_init();
}



/*Websocket handle functions*/
static void ws_1000(struct mg_connection *c)
{
	int wsSendStepStage;


	wsSendStepStage = websocketHandle_getWsSendStageStep(c);
	if(wsSendStepStage == (-1))
	{
		//Never happen in noraml
		return;
	}

	//send ws spec
	if(websocketHandle_checkNewData(c, WebsocketHandle_DataType_WsSpec) == 1)
	{
		setJson_wsSpecAndStreamStatus(jsonSendBuffer);
		mg_ws_send(c, jsonSendBuffer, strlen(jsonSendBuffer), WEBSOCKET_OP_TEXT);
	}


	while (1)
	{
		if(wsSendStepStage == 0)
		{
			wsSendStepStage = 1;
			wsSend_stage0(c);	//break anyway
			break;
		}

		if(wsSendStepStage == 1)
		{
			wsSendStepStage = 2;
			if(wsSend_stage1(c) == 1)
			{
				break;
			}
		}

		if(wsSendStepStage == 2)
		{
			wsSendStepStage = 3;
			if(wsSend_stage2(c) == 1)
			{
				break;
			}
		}

		if(wsSendStepStage == 3)
		{
			wsSendStepStage = 4;
			if(wsSend_stage3(c) == 1)
			{
				break;
			}
		}

		if(wsSendStepStage == 4)
		{
			wsSendStepStage = 5;
			if(wsSend_stage4(c) == 1)
			{
				break;
			}
		}

		if(wsSendStepStage == 5)
		{
			wsSendStepStage = 0;
			if(wsSend_stage5(c) == 1)
			{
				break;
			}
		}
	}

	websocketHandle_setWsSendStageStep(c, (uint8_t)wsSendStepStage);
}


static void ws_500(struct mg_connection *c)
{
	NULL;
}


/*Stages*/
static int wsSend_stage0(struct mg_connection *c)
{
	/**
	 * 1- stream 1 , 2 , 3 total and flow and output calc and Pram Var
	*/

	bool dataChange = false;

	//total and flow and output calc
	if(websocketHandle_checkNewData(c, WebsocketHandle_DataType_St1TotalAndFlow) == 1)
	{
		dataChange = true;
		setJson_totalAndFlowAndInputSignalAndOutputcalcAndTotalErrorAndFlowError(1 ,jsonSendBuffer);
		mg_ws_send(c, jsonSendBuffer, strlen(jsonSendBuffer), WEBSOCKET_OP_TEXT);
	}

	if(websocketHandle_checkNewData(c, WebsocketHandle_DataType_St2TotalAndFlow) == 1)
	{
		dataChange = true;
		setJson_totalAndFlowAndInputSignalAndOutputcalcAndTotalErrorAndFlowError(2 ,jsonSendBuffer);
		mg_ws_send(c, jsonSendBuffer, strlen(jsonSendBuffer), WEBSOCKET_OP_TEXT);
	}

	if(websocketHandle_checkNewData(c, WebsocketHandle_DataType_St3TotalAndFlow) == 1)
	{
		dataChange = true;
		setJson_totalAndFlowAndInputSignalAndOutputcalcAndTotalErrorAndFlowError(3 ,jsonSendBuffer);
		mg_ws_send(c, jsonSendBuffer, strlen(jsonSendBuffer), WEBSOCKET_OP_TEXT);
	}


	//Pram Var stream 1 , 2 , 3
	if(websocketHandle_checkNewData(c, WebsocketHandle_DataType_St1ParamVar) == 1)
	{
		dataChange = true;
		setJson_paramVar(1 ,jsonSendBuffer);
		mg_ws_send(c, jsonSendBuffer, strlen(jsonSendBuffer), WEBSOCKET_OP_TEXT);
	}

	if(websocketHandle_checkNewData(c, WebsocketHandle_DataType_St2ParamVar) == 1)
	{
		dataChange = true;
		setJson_paramVar(2 ,jsonSendBuffer);
		mg_ws_send(c, jsonSendBuffer, strlen(jsonSendBuffer), WEBSOCKET_OP_TEXT);
	}

	if(websocketHandle_checkNewData(c, WebsocketHandle_DataType_St3ParamVar) == 1)
	{
		dataChange = true;
		setJson_paramVar(3 ,jsonSendBuffer);
		mg_ws_send(c, jsonSendBuffer, strlen(jsonSendBuffer), WEBSOCKET_OP_TEXT);
	}
	
	if(dataChange == true)
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

static int wsSend_stage1(struct mg_connection *c)
{
	/**
	 * 1- stream 1 , 2 , 3 average
	*/

	bool dataChange = false;

	if(websocketHandle_checkNewData(c, WebsocketHandle_DataType_St1Average) == 1)
	{
		dataChange = true;
		setJson_average(1 ,jsonSendBuffer);
		mg_ws_send(c, jsonSendBuffer, strlen(jsonSendBuffer), WEBSOCKET_OP_TEXT);
	}

	if(websocketHandle_checkNewData(c, WebsocketHandle_DataType_St2Average) == 1)
	{
		dataChange = true;
		setJson_average(2 ,jsonSendBuffer);
		mg_ws_send(c, jsonSendBuffer, strlen(jsonSendBuffer), WEBSOCKET_OP_TEXT);
	}

	if(websocketHandle_checkNewData(c, WebsocketHandle_DataType_St3Average) == 1)
	{
		dataChange = true;
		setJson_average(3 ,jsonSendBuffer);
		mg_ws_send(c, jsonSendBuffer, strlen(jsonSendBuffer), WEBSOCKET_OP_TEXT);
	}

	if(dataChange == true)
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

static int wsSend_stage2(struct mg_connection *c)
{
	/**
	 * 1- stream 1 , 2 , 3 total previous
	*/

	bool dataChange = false;

	if(websocketHandle_checkNewData(c, WebsocketHandle_DataType_St1TotalPrevious) == 1)
	{
		dataChange = true;
		setJson_totalPrevious(1 ,jsonSendBuffer);
		mg_ws_send(c, jsonSendBuffer, strlen(jsonSendBuffer), WEBSOCKET_OP_TEXT);
	}

	if(websocketHandle_checkNewData(c, WebsocketHandle_DataType_St2TotalPrevious) == 1)
	{
		dataChange = true;
		setJson_totalPrevious(2 ,jsonSendBuffer);
		mg_ws_send(c, jsonSendBuffer, strlen(jsonSendBuffer), WEBSOCKET_OP_TEXT);
	}

	if(websocketHandle_checkNewData(c, WebsocketHandle_DataType_St3TotalPrevious) == 1)
	{
		dataChange = true;
		setJson_totalPrevious(3 ,jsonSendBuffer);
		mg_ws_send(c, jsonSendBuffer, strlen(jsonSendBuffer), WEBSOCKET_OP_TEXT);
	}

	if(dataChange == true)
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

static int wsSend_stage3(struct mg_connection *c)
{
	/**
	 * 1- param stream 1 and 2 and 3
	*/

	bool dataChange = false;

	if(websocketHandle_checkNewData(c, WebsocketHandle_DataType_St1ParamConst) == 1)
	{
		dataChange = true;
		setJson_paramConst(1 ,jsonSendBuffer);
		mg_ws_send(c, jsonSendBuffer, strlen(jsonSendBuffer), WEBSOCKET_OP_TEXT);
	}

	if(websocketHandle_checkNewData(c, WebsocketHandle_DataType_St2ParamConst) == 1)
	{
		dataChange = true;
		setJson_paramConst(2 ,jsonSendBuffer);
		mg_ws_send(c, jsonSendBuffer, strlen(jsonSendBuffer), WEBSOCKET_OP_TEXT);
	}

	if(websocketHandle_checkNewData(c, WebsocketHandle_DataType_St3ParamConst) == 1)
	{
		dataChange = true;
		setJson_paramConst(3 ,jsonSendBuffer);
		mg_ws_send(c, jsonSendBuffer, strlen(jsonSendBuffer), WEBSOCKET_OP_TEXT);
	}

	if(dataChange == true)
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

static int wsSend_stage4(struct mg_connection *c)
{
	/**
	 * 1- 21 gas parameter
	*/

	return 0;
}

static int wsSend_stage5(struct mg_connection *c)
{
	/**
	 * 1- 21 gas parameter
	*/

	bool dataChange = false;

	if(websocketHandle_checkNewData(c, WebsocketHandle_DataType_St1ParamGas21X) == 1)
	{
		dataChange = true;
		setJson_paramGas21x(1 ,jsonSendBuffer);
		mg_ws_send(c, jsonSendBuffer, strlen(jsonSendBuffer), WEBSOCKET_OP_TEXT);
	}

	if(websocketHandle_checkNewData(c, WebsocketHandle_DataType_St2ParamGas21X) == 1)
	{
		dataChange = true;
		setJson_paramGas21x(2 ,jsonSendBuffer);
		mg_ws_send(c, jsonSendBuffer, strlen(jsonSendBuffer), WEBSOCKET_OP_TEXT);
	}

	if(websocketHandle_checkNewData(c, WebsocketHandle_DataType_St3ParamGas21X) == 1)
	{
		dataChange = true;
		setJson_paramGas21x(3 ,jsonSendBuffer);
		mg_ws_send(c, jsonSendBuffer, strlen(jsonSendBuffer), WEBSOCKET_OP_TEXT);
	}

	if(dataChange == true)
	{
		return 1;
	}
	else
	{
		return 0;
	}
}


int connNumberTest = 0;
/*add thirdparty function*/
int webUser_myFuncInHttpHandler(struct mg_connection *c, int ev, void *ev_data)		//if return == 0 then countinue
{
	if(ev == MG_EV_ACCEPT)
	{
		if (numconns(c->mgr) > WEB_USER_MAX_CLIENT)
		{
		      c->is_draining = 1;
		}
	}

	if(ev == MG_EV_WS_OPEN)
	{
		websocketHandle_addNewWebsocketConnection(c);
	}

	if(ev == MG_EV_CLOSE)
	{
		websocketHandle_removeWebsocketConnection(c);
	}

	return 0;
}

/*Connection Liit*/
static inline int numconns(struct mg_mgr *mgr)
{
  int n = 0;
  for (struct mg_connection *t = mgr->conns; t != NULL; t = t->next) n++;
  return n;
}

//fisrt call userInit then call this function
static void thirdPartyInit()
{

	memset(&driver_data, 0, sizeof(driver_data));
	memset(&mif, 0, sizeof(mif));

	mg_log_set_fn(mylog, NULL);
	mg_log_set(MG_LL_DEBUG);
	mg_mgr_init(&g_mgr);


	driver_data.mdc_cr = 4;
	driver_data.phy_addr = ETH_PHY_ADDRESS;



	mif.ip = MG_IPV4(gWebServer.Config.Driver.ipV4[0],
				gWebServer.Config.Driver.ipV4[1],
					gWebServer.Config.Driver.ipV4[2],
						gWebServer.Config.Driver.ipV4[3]);

	mif.mask = MG_IPV4(gWebServer.Config.Driver.maskV4[0],
				gWebServer.Config.Driver.maskV4[1],
					gWebServer.Config.Driver.maskV4[2],
						gWebServer.Config.Driver.maskV4[3]);

	mif.gw = MG_IPV4(gWebServer.Config.Driver.gwV4[0],
				gWebServer.Config.Driver.gwV4[1],
					gWebServer.Config.Driver.gwV4[2],
						gWebServer.Config.Driver.gwV4[3]);

	mif.driver = &mg_tcpip_driver_stm32f;
	mif.driver_data = &driver_data;

	mif.mac[0] = gWebServer.Config.Driver.macAddress[0];
	mif.mac[1] = gWebServer.Config.Driver.macAddress[1];
	mif.mac[2] = gWebServer.Config.Driver.macAddress[2];
	mif.mac[3] = gWebServer.Config.Driver.macAddress[3];
	mif.mac[4] = gWebServer.Config.Driver.macAddress[4];
	mif.mac[5] = gWebServer.Config.Driver.macAddress[5];

	MG_SET_MAC_ADDRESS(mif.mac);

	NVIC_EnableIRQ(ETH_IRQn);
	mg_tcpip_init(&g_mgr, &mif);

    //Initial HTTP App
    mg_http_listen(&g_mgr, "http://0.0.0.0:80", http_ev_handler, NULL);

    //Web Socket
    mongoose_add_ws_handler(1000, ws_1000);
    mongoose_add_ws_handler(500, ws_500);
}


static void checkPhyHandle()
{
	enum PhyCheckStep_e
	{
		PhyCheck_Step_ReadPhy,
		PhyCheck_Step_SetPinLow,
		PhyCheck_Step_WaitLowPin,
		PhyCheck_Step_SetPinHigh,
		PhyCheck_Step_WaitHighPin,
		PhyCheck_Step_End
	};

	static enum PhyCheckStep_e stepCheck = 0;
	static uint32_t timerCheck = 0;

	static uint16_t symbolErrorCntRegLastValue = 0;
	static uint8_t errorCnt = 0;

	uint16_t phyBasicStautRegData;
	uint16_t phySymbolErrorCounterRegData;


	switch(stepCheck)
	{
	case PhyCheck_Step_ReadPhy:
		if((HAL_GetTick() - timerCheck) > 200)
		{
			timerCheck = HAL_GetTick();

			phyBasicStautRegData = readPhy(ETH_PHY_ADDRESS, LAN8720_BASICSTATUS_REG);
			phySymbolErrorCounterRegData = readPhy(ETH_PHY_ADDRESS, LAN8720_SYMBOLERRORCOUNTER_REG);

			//For Symbol Error Counter check
			if(phySymbolErrorCounterRegData != symbolErrorCntRegLastValue)
				errorCnt++;
			else
				errorCnt = 0;

			symbolErrorCntRegLastValue = phySymbolErrorCounterRegData;

			if((phyBasicStautRegData == 0xFFFF) || (errorCnt >= 5))
			{
				errorCnt = 0;
				stepCheck = PhyCheck_Step_SetPinLow;
			}
		}
		break;

	case PhyCheck_Step_SetPinLow:
		HAL_GPIO_WritePin(LAN_RESET_GPIO_Port, LAN_RESET_Pin, GPIO_PIN_RESET);
		timerCheck = HAL_GetTick();
		stepCheck = PhyCheck_Step_WaitLowPin;
		break;

	case PhyCheck_Step_WaitLowPin:
		if((HAL_GetTick() - timerCheck) > 250)
		{
			stepCheck = PhyCheck_Step_SetPinHigh;
		}
		break;

	case PhyCheck_Step_SetPinHigh:
		HAL_GPIO_WritePin(LAN_RESET_GPIO_Port, LAN_RESET_Pin, GPIO_PIN_SET);
		timerCheck = HAL_GetTick();
		stepCheck = PhyCheck_Step_WaitHighPin;
		break;

	case PhyCheck_Step_WaitHighPin:
		if((HAL_GetTick() - timerCheck) > 250)
		{
			stepCheck = PhyCheck_Step_SetPinHigh;
		}
		break;

	case PhyCheck_Step_End:
		timerCheck = HAL_GetTick();
		stepCheck = PhyCheck_Step_ReadPhy;
		break;
	}

}


static uint16_t readPhy(uint8_t addr, uint8_t reg)	//copy from mongoose.c
{
  ETH->MACMIIAR &= (7 << 2);
  ETH->MACMIIAR |= ((uint32_t) addr << 11) | ((uint32_t) reg << 6);
  ETH->MACMIIAR |= MG_BIT(0);
  while (ETH->MACMIIAR & MG_BIT(0)) (void) 0;
  return ETH->MACMIIDR & 0xffff;
}


static void mylog(char ch, void *param)
{
  static char buf[256];
  static uint16_t len = 0;

  buf[len++] = ch;
  if (ch == '\n' || len >= sizeof(buf)) {
    debugLog_printLogWithMutex(buf, len);
    len = 0;
  }

  (void) param;
}



/*Set JSON Functions*/
//WS Specs
static void setJson_wsSpecAndStreamStatus(char *buffer)
{
	//Start
	sprintf(buffer,"{");

	for (uint8_t i = 0; i < WSCONN_PARAM_STREAM_NUM; i++)
	{
		//Flometer Type
		if(gParamFromHmi.Setting.SetupStream[i].flowMeterType == Stream_Inactive)
		{
			sprintf(tempBuff,"\"wsSpec_flometerType\":\"inactive\",");
			strcat(buffer, tempBuff);	

			sprintf(tempBuff,"\"st%u_status\":\"Inactive\",", i+1);
			strcat(buffer, tempBuff);
		}
		else if(gParamFromHmi.Setting.SetupStream[i].flowMeterType == Stream_Pulse_Meter)
		{
			sprintf(tempBuff,"\"wsSpec_flometerType\":\"pulseMeter\",");
			strcat(buffer, tempBuff);	

			sprintf(tempBuff,"\"st%u_status\":\"Enable (Turbine)\",", i+1);
			strcat(buffer, tempBuff);
		}
		else if(gParamFromHmi.Setting.SetupStream[i].flowMeterType == Stream_Diff_Pressure_meter)
		{
			sprintf(tempBuff,"\"wsSpec_flometerType\":\"DiffPressMeter\",");
			strcat(buffer, tempBuff);	

			sprintf(tempBuff,"\"st%u_status\":\"Enable (Orifice)\",", i+1);
			strcat(buffer, tempBuff);
		}
		else if(gParamFromHmi.Setting.SetupStream[i].flowMeterType == Stream_Ultrasonic)
		{
			sprintf(tempBuff,"\"wsSpec_flometerType\":\"ultrasonic\",");
			strcat(buffer, tempBuff);	

			sprintf(tempBuff,"\"st%u_status\":\"Enable (Ultrasonic)\",", i+1);
			strcat(buffer, tempBuff);
		}
		else if(gParamFromHmi.Setting.SetupStream[i].flowMeterType == Stream_Current_input)
		{
			sprintf(tempBuff,"\"wsSpec_flometerType\":\"currentInput\",");
			strcat(buffer, tempBuff);	

			sprintf(tempBuff,"\"st%u_status\":\"Enable (Current Input)\",", i+1);
			strcat(buffer, tempBuff);
		}
		

		//Fluid And Fluid Type
		if(gParamFromHmi.Setting.SetupStream[i].Fluid.fluid == Stream_Gasses)
		{
			sprintf(tempBuff,"\"wsSpec_fluid\":\"gasses\",");
			strcat(buffer, tempBuff);

			sprintf(tempBuff,"\"wsSpec_fluidType\":\"naturalGas\",");
			strcat(buffer, tempBuff);
		}
		else if(gParamFromHmi.Setting.SetupStream[i].flowMeterType == Stream_Liquid)
		{
			sprintf(tempBuff,"\"wsSpec_fluid\":\"liquid\",");
			strcat(buffer, tempBuff);

			sprintf(tempBuff,"\"wsSpec_fluidType\":\"gasoil\",");
			strcat(buffer, tempBuff);
		}
		else if(gParamFromHmi.Setting.SetupStream[i].flowMeterType == Stream_Water)
		{
			sprintf(tempBuff,"\"wsSpec_fluid\":\"water\",");
			strcat(buffer, tempBuff);

			sprintf(tempBuff,"\"wsSpec_fluidType\":\"none\",");
			strcat(buffer, tempBuff);
		}
	}
	
	//Remove Last , in json
	size_t jsonSize = strlen(buffer);
	if(jsonSize > 1)
	{
		buffer[jsonSize-1] = '\0';
	}

	//End
	strcat(buffer,"}");
}

//Data stream Total and flow
static void setJson_totalAndFlowAndInputSignalAndOutputcalcAndTotalErrorAndFlowError(uint8_t streamNum, char *buffer)	//streamNum at one
{
	uint8_t streamNumAtZero = streamNum - 1;

	//Start
	sprintf(buffer,"{");

	//total
	sprintf(tempBuff,"\"st%u_uvol\":\"%.2lf\",",streamNum,gParamFromHmi.Data.AllDataFlow.Now[streamNumAtZero].total_uvol);
	strcat(buffer, tempBuff);

	sprintf(tempBuff,"\"st%u_cvol\":\"%.2lf\",",streamNum,gParamFromHmi.Data.AllDataFlow.Now[streamNumAtZero].total_cvol);
	strcat(buffer, tempBuff);

	sprintf(tempBuff,"\"st%u_energy\":\"%.2lf\",",streamNum,gParamFromHmi.Data.AllDataFlow.Now[streamNumAtZero].total_energy);
	strcat(buffer, tempBuff);

	sprintf(tempBuff,"\"st%u_mass\":\"%.2lf\",",streamNum,gParamFromHmi.Data.AllDataFlow.Now[streamNumAtZero].total_mass);
	strcat(buffer, tempBuff);

	//flowrate
	sprintf(tempBuff,"\"st%u_uvolFlowrate\":\"%.4lf\",",streamNum,gParamFromHmi.Data.AllDataFlow.Now[streamNumAtZero].uvol_flowrate);
	strcat(buffer, tempBuff);

	sprintf(tempBuff,"\"st%u_cvolFlowrate\":\"%.4lf\",",streamNum,gParamFromHmi.Data.AllDataFlow.Now[streamNumAtZero].cvol_flowrate);
	strcat(buffer, tempBuff);

	sprintf(tempBuff,"\"st%u_energyFlowrate\":\"%.4lf\",",streamNum,gParamFromHmi.Data.AllDataFlow.Now[streamNumAtZero].energy_flowrate);
	strcat(buffer, tempBuff);

	sprintf(tempBuff,"\"st%u_massFlowrate\":\"%.4lf\",",streamNum,gParamFromHmi.Data.AllDataFlow.Now[streamNumAtZero].mass_flowrate);
	strcat(buffer, tempBuff);

	//Input Signal
	sprintf(tempBuff,"\"st%u_signalFlowMeter\":\"%.4lf\",",streamNum,gParamFromHmi.Data.AllDataFlow.Now[streamNumAtZero].signal_flowmeter);
	strcat(buffer, tempBuff);

	sprintf(tempBuff,"\"st%u_pressure\":\"%.4lf\",",streamNum,gParamFromHmi.Data.AllDataFlow.Now[streamNumAtZero].pressure);
	strcat(buffer, tempBuff);

	sprintf(tempBuff,"\"st%u_temperature\":\"%.4lf\",",streamNum,gParamFromHmi.Data.AllDataFlow.Now[streamNumAtZero].temperature);
	strcat(buffer, tempBuff);

	//Output Calculation
	if(gParamFromHmi.Setting.SetupStream[streamNumAtZero].Fluid.fluid == Stream_Gasses)
	{
		sprintf(tempBuff,"\"st%u_conversionFactor\":\"%.4lf\",",streamNum,gParamFromHmi.Data.AllDataFlow.Now[streamNumAtZero].conversion_c_factor);
		strcat(buffer, tempBuff);

		sprintf(tempBuff,"\"st%u_zFactor\":\"%.4lf\",",streamNum,gParamFromHmi.Data.AllOutputCalc.NaturalGas[streamNumAtZero].zFactor);
		strcat(buffer, tempBuff);
	
		sprintf(tempBuff,"\"st%u_density\":\"%.4lf\",",streamNum,gParamFromHmi.Data.AllDataFlow.Now[streamNumAtZero].density);
		strcat(buffer, tempBuff);

		sprintf(tempBuff,"\"st%u_densityBase\":\"%.4lf\",",streamNum,gParamFromHmi.Data.AllOutputCalc.General[streamNumAtZero].pb);
		strcat(buffer, tempBuff);
	}
	else if(gParamFromHmi.Setting.SetupStream[streamNumAtZero].Fluid.fluid == Stream_Liquid)
	{
		sprintf(tempBuff,"\"st%u_cpl\":\"%.4lf\",",streamNum,gParamFromHmi.Data.AllOutputCalc.Liquid[streamNumAtZero].cpl);
		strcat(buffer, tempBuff);

		sprintf(tempBuff,"\"st%u_ctl\":\"%.4lf\",",streamNum,gParamFromHmi.Data.AllOutputCalc.Liquid[streamNumAtZero].cpl);
		strcat(buffer, tempBuff);
	
		sprintf(tempBuff,"\"st%u_density\":\"%.4lf\",",streamNum,gParamFromHmi.Data.AllDataFlow.Now[streamNumAtZero].density);
		strcat(buffer, tempBuff);

		sprintf(tempBuff,"\"st%u_densityBase\":\"%.4lf\",",streamNum,gParamFromHmi.Data.AllOutputCalc.General[streamNumAtZero].pb);
		strcat(buffer, tempBuff);
	}

	//total Error
	sprintf(tempBuff,"\"st%u_uvolError\":\"%.2lf\",",streamNum,gParamFromHmi.Data.AllDataFlow.Now[streamNumAtZero].total_uvol_error);
	strcat(buffer, tempBuff);

	sprintf(tempBuff,"\"st%u_cvolError\":\"%.2lf\",",streamNum,gParamFromHmi.Data.AllDataFlow.Now[streamNumAtZero].total_cvol_error);
	strcat(buffer, tempBuff);

	sprintf(tempBuff,"\"st%u_energyError\":\"%.2lf\",",streamNum,gParamFromHmi.Data.AllDataFlow.Now[streamNumAtZero].total_energy_error);
	strcat(buffer, tempBuff);

	sprintf(tempBuff,"\"st%u_massError\":\"%.2lf\",",streamNum,gParamFromHmi.Data.AllDataFlow.Now[streamNumAtZero].total_mass_error);
	strcat(buffer, tempBuff);

	//Remove Last , in json
	size_t jsonSize = strlen(buffer);
	if(jsonSize > 1)
	{
		buffer[jsonSize-1] = '\0';
	}

	//End
	strcat(buffer,"}");
}


static void setJson_average(uint8_t streamNum, char *buffer)	//streamNum at one
{
	uint8_t streamNumAtZero = streamNum - 1;


	//Start
	sprintf(buffer,"{");

	//Average Minutes
	sprintf(tempBuff,"\"st%u_avrMin_uvol\":\"%.4lf\",",streamNum,gParamFromHmi.Data.AllDataFlow.AverageLastMin[streamNumAtZero].uvol_flowrate);
	strcat(buffer, tempBuff);

	sprintf(tempBuff,"\"st%u_avrMin_cvol\":\"%.4lf\",",streamNum,gParamFromHmi.Data.AllDataFlow.AverageLastMin[streamNumAtZero].cvol_flowrate);
	strcat(buffer, tempBuff);

	sprintf(tempBuff,"\"st%u_avrMin_energy\":\"%.4lf\",",streamNum,gParamFromHmi.Data.AllDataFlow.AverageLastMin[streamNumAtZero].energy_flowrate);
	strcat(buffer, tempBuff);

	sprintf(tempBuff,"\"st%u_avrMin_mass\":\"%.4lf\",",streamNum,gParamFromHmi.Data.AllDataFlow.AverageLastMin[streamNumAtZero].mass_flowrate);
	strcat(buffer, tempBuff);

	sprintf(tempBuff,"\"st%u_avrMin_pressure\":\"%.4lf\",",streamNum,gParamFromHmi.Data.AllDataFlow.AverageLastMin[streamNumAtZero].pressure);
	strcat(buffer, tempBuff);

	sprintf(tempBuff,"\"st%u_avrMin_temperature\":\"%.4lf\",",streamNum,gParamFromHmi.Data.AllDataFlow.AverageLastMin[streamNumAtZero].temperature);
	strcat(buffer, tempBuff);

	sprintf(tempBuff,"\"st%u_avrMin_signalFlowMeter\":\"%.4lf\",",streamNum,gParamFromHmi.Data.AllDataFlow.AverageLastMin[streamNumAtZero].signal_flowmeter);
	strcat(buffer, tempBuff);

	sprintf(tempBuff,"\"st%u_avrMin_density\":\"%.4lf\",",streamNum,gParamFromHmi.Data.AllDataFlow.AverageLastMin[streamNumAtZero].mass_density);
	strcat(buffer, tempBuff);


	//Average Hour
	sprintf(tempBuff,"\"st%u_avrHour_uvol\":\"%.4lf\",",streamNum,gParamFromHmi.Data.AllDataFlow.AverageLastHour[streamNumAtZero].uvol_flowrate);
	strcat(buffer, tempBuff);

	sprintf(tempBuff,"\"st%u_avrHour_cvol\":\"%.4lf\",",streamNum,gParamFromHmi.Data.AllDataFlow.AverageLastHour[streamNumAtZero].cvol_flowrate);
	strcat(buffer, tempBuff);

	sprintf(tempBuff,"\"st%u_avrHour_energy\":\"%.4lf\",",streamNum,gParamFromHmi.Data.AllDataFlow.AverageLastHour[streamNumAtZero].energy_flowrate);
	strcat(buffer, tempBuff);

	sprintf(tempBuff,"\"st%u_avrHour_mass\":\"%.4lf\",",streamNum,gParamFromHmi.Data.AllDataFlow.AverageLastHour[streamNumAtZero].mass_flowrate);
	strcat(buffer, tempBuff);

	sprintf(tempBuff,"\"st%u_avrHour_pressure\":\"%.4lf\",",streamNum,gParamFromHmi.Data.AllDataFlow.AverageLastHour[streamNumAtZero].pressure);
	strcat(buffer, tempBuff);

	sprintf(tempBuff,"\"st%u_avrHour_temperature\":\"%.4lf\",",streamNum,gParamFromHmi.Data.AllDataFlow.AverageLastHour[streamNumAtZero].temperature);
	strcat(buffer, tempBuff);

	sprintf(tempBuff,"\"st%u_avrHour_signalFlowMeter\":\"%.4lf\",",streamNum,gParamFromHmi.Data.AllDataFlow.AverageLastHour[streamNumAtZero].signal_flowmeter);
	strcat(buffer, tempBuff);

	sprintf(tempBuff,"\"st%u_avrHour_density\":\"%.4lf\",",streamNum,gParamFromHmi.Data.AllDataFlow.AverageLastHour[streamNumAtZero].mass_density);
	strcat(buffer, tempBuff);


	//Average Daily
	sprintf(tempBuff,"\"st%u_avrDay_uvol\":\"%.4lf\",",streamNum,gParamFromHmi.Data.AllDataFlow.AverageLastDay[streamNumAtZero].uvol_flowrate);
	strcat(buffer, tempBuff);

	sprintf(tempBuff,"\"st%u_avrDay_cvol\":\"%.4lf\",",streamNum,gParamFromHmi.Data.AllDataFlow.AverageLastDay[streamNumAtZero].cvol_flowrate);
	strcat(buffer, tempBuff);

	sprintf(tempBuff,"\"st%u_avrDay_energy\":\"%.4lf\",",streamNum,gParamFromHmi.Data.AllDataFlow.AverageLastDay[streamNumAtZero].energy_flowrate);
	strcat(buffer, tempBuff);

	sprintf(tempBuff,"\"st%u_avrDay_mass\":\"%.4lf\",",streamNum,gParamFromHmi.Data.AllDataFlow.AverageLastDay[streamNumAtZero].mass_flowrate);
	strcat(buffer, tempBuff);

	sprintf(tempBuff,"\"st%u_avrDay_pressure\":\"%.4lf\",",streamNum,gParamFromHmi.Data.AllDataFlow.AverageLastDay[streamNumAtZero].pressure);
	strcat(buffer, tempBuff);

	sprintf(tempBuff,"\"st%u_avrDay_temperature\":\"%.4lf\",",streamNum,gParamFromHmi.Data.AllDataFlow.AverageLastDay[streamNumAtZero].temperature);
	strcat(buffer, tempBuff);

	sprintf(tempBuff,"\"st%u_avrDay_signalFlowMeter\":\"%.4lf\",",streamNum,gParamFromHmi.Data.AllDataFlow.AverageLastDay[streamNumAtZero].signal_flowmeter);
	strcat(buffer, tempBuff);

	sprintf(tempBuff,"\"st%u_avrDay_density\":\"%.4lf\",",streamNum,gParamFromHmi.Data.AllDataFlow.AverageLastDay[streamNumAtZero].mass_density);
	strcat(buffer, tempBuff);


	//Average Month
	sprintf(tempBuff,"\"st%u_avrMonth_uvol\":\"%.4lf\",",streamNum,gParamFromHmi.Data.AllDataFlow.AverageLastMonth[streamNumAtZero].uvol_flowrate);
	strcat(buffer, tempBuff);

	sprintf(tempBuff,"\"st%u_avrMonth_cvol\":\"%.4lf\",",streamNum,gParamFromHmi.Data.AllDataFlow.AverageLastMonth[streamNumAtZero].cvol_flowrate);
	strcat(buffer, tempBuff);

	sprintf(tempBuff,"\"st%u_avrMonth_energy\":\"%.4lf\",",streamNum,gParamFromHmi.Data.AllDataFlow.AverageLastMonth[streamNumAtZero].energy_flowrate);
	strcat(buffer, tempBuff);

	sprintf(tempBuff,"\"st%u_avrMonth_mass\":\"%.4lf\",",streamNum,gParamFromHmi.Data.AllDataFlow.AverageLastMonth[streamNumAtZero].mass_flowrate);
	strcat(buffer, tempBuff);

	sprintf(tempBuff,"\"st%u_avrMonth_pressure\":\"%.4lf\",",streamNum,gParamFromHmi.Data.AllDataFlow.AverageLastMonth[streamNumAtZero].pressure);
	strcat(buffer, tempBuff);

	sprintf(tempBuff,"\"st%u_avrMonth_temperature\":\"%.4lf\",",streamNum,gParamFromHmi.Data.AllDataFlow.AverageLastMonth[streamNumAtZero].temperature);
	strcat(buffer, tempBuff);

	sprintf(tempBuff,"\"st%u_avrMonth_signalFlowMeter\":\"%.4lf\",",streamNum,gParamFromHmi.Data.AllDataFlow.AverageLastMonth[streamNumAtZero].signal_flowmeter);
	strcat(buffer, tempBuff);

	sprintf(tempBuff,"\"st%u_avrMonth_density\":\"%.4lf\",",streamNum,gParamFromHmi.Data.AllDataFlow.AverageLastMonth[streamNumAtZero].mass_density);
	strcat(buffer, tempBuff);


	//Remove Last , in json
	size_t jsonSize = strlen(buffer);
	if(jsonSize > 1)
	{
		buffer[jsonSize-1] = '\0';
	}

	//End
	strcat(buffer,"}");
}


static void setJson_totalPrevious(uint8_t streamNum, char *buffer)	//streamNum at one
{
	uint8_t streamNumAtZero = streamNum - 1;


	//Start
	sprintf(buffer,"{");

	//Total Previous Day
	sprintf(tempBuff,"\"st%u_totalPrevDay_uvol\":\"%.4lf\",",streamNum,gParamFromHmi.Data.AllDataFlow.TotalPreviousDay[streamNumAtZero].uvol);
	strcat(buffer, tempBuff);

	sprintf(tempBuff,"\"st%u_totalPrevDay_cvol\":\"%.4lf\",",streamNum,gParamFromHmi.Data.AllDataFlow.TotalPreviousDay[streamNumAtZero].cvol);
	strcat(buffer, tempBuff);

	sprintf(tempBuff,"\"st%u_totalPrevDay_energy\":\"%.4lf\",",streamNum,gParamFromHmi.Data.AllDataFlow.TotalPreviousDay[streamNumAtZero].energy);
	strcat(buffer, tempBuff);

	sprintf(tempBuff,"\"st%u_totalPrevDay_mass\":\"%.4lf\",",streamNum,gParamFromHmi.Data.AllDataFlow.TotalPreviousDay[streamNumAtZero].mass);
	strcat(buffer, tempBuff);

	sprintf(tempBuff,"\"st%u_totalPrevErrDay_uvol\":\"%.4lf\",",streamNum,gParamFromHmi.Data.AllDataFlow.TotalPreviousDay[streamNumAtZero].uvol_error);
	strcat(buffer, tempBuff);

	sprintf(tempBuff,"\"st%u_totalPrevErrDay_cvol\":\"%.4lf\",",streamNum,gParamFromHmi.Data.AllDataFlow.TotalPreviousDay[streamNumAtZero].cvol_error);
	strcat(buffer, tempBuff);

	sprintf(tempBuff,"\"st%u_totalPrevErrDay_energy\":\"%.4lf\",",streamNum,gParamFromHmi.Data.AllDataFlow.TotalPreviousDay[streamNumAtZero].energy_error);
	strcat(buffer, tempBuff);

	sprintf(tempBuff,"\"st%u_totalPrevErrDay_mass\":\"%.4lf\",",streamNum,gParamFromHmi.Data.AllDataFlow.TotalPreviousDay[streamNumAtZero].mass_error);
	strcat(buffer, tempBuff);


	//Total Previous Month
	sprintf(tempBuff,"\"st%u_totalPrevMonth_uvol\":\"%.4lf\",",streamNum,gParamFromHmi.Data.AllDataFlow.TotalPreviousMonth[streamNumAtZero].uvol);
	strcat(buffer, tempBuff);

	sprintf(tempBuff,"\"st%u_totalPrevMonth_cvol\":\"%.4lf\",",streamNum,gParamFromHmi.Data.AllDataFlow.TotalPreviousMonth[streamNumAtZero].cvol);
	strcat(buffer, tempBuff);

	sprintf(tempBuff,"\"st%u_totalPrevMonth_energy\":\"%.4lf\",",streamNum,gParamFromHmi.Data.AllDataFlow.TotalPreviousMonth[streamNumAtZero].energy);
	strcat(buffer, tempBuff);

	sprintf(tempBuff,"\"st%u_totalPrevMonth_mass\":\"%.4lf\",",streamNum,gParamFromHmi.Data.AllDataFlow.TotalPreviousMonth[streamNumAtZero].mass);
	strcat(buffer, tempBuff);

	sprintf(tempBuff,"\"st%u_totalPrevErrMonth_uvol\":\"%.4lf\",",streamNum,gParamFromHmi.Data.AllDataFlow.TotalPreviousMonth[streamNumAtZero].uvol_error);
	strcat(buffer, tempBuff);

	sprintf(tempBuff,"\"st%u_totalPrevErrMonth_cvol\":\"%.4lf\",",streamNum,gParamFromHmi.Data.AllDataFlow.TotalPreviousMonth[streamNumAtZero].cvol_error);
	strcat(buffer, tempBuff);

	sprintf(tempBuff,"\"st%u_totalPrevErrMonth_energy\":\"%.4lf\",",streamNum,gParamFromHmi.Data.AllDataFlow.TotalPreviousMonth[streamNumAtZero].energy_error);
	strcat(buffer, tempBuff);

	sprintf(tempBuff,"\"st%u_totalPrevErrMonth_mass\":\"%.4lf\",",streamNum,gParamFromHmi.Data.AllDataFlow.TotalPreviousMonth[streamNumAtZero].mass_error);
	strcat(buffer, tempBuff);


	//Remove Last , in json
	size_t jsonSize = strlen(buffer);
	if(jsonSize > 1)
	{
		buffer[jsonSize-1] = '\0';
	}

	//End
	strcat(buffer,"}");
}


static void setJson_paramVar(uint8_t streamNum, char *buffer)	//streamNum at one
{
	uint8_t streamNumAtZero = streamNum - 1;


	//Start
	sprintf(buffer,"{");

	//General
	//Nothing

	
	if(gParamFromHmi.Setting.SetupStream[streamNumAtZero].Fluid.fluid == Stream_Gasses) //Fluid Gas
	{
		sprintf(tempBuff,"\"st%u_paramGa_zFactorBase\":\"%.4f\",",streamNum,gParamFromHmi.Data.AllOutputCalc.NaturalGas[streamNumAtZero].zFactorBase);
		strcat(buffer, tempBuff);

		sprintf(tempBuff,"\"st%u_paramGa_mW\":\"%.4f\",",streamNum,gParamFromHmi.Data.AllOutputCalc.NaturalGas[streamNumAtZero].molarMass);
		strcat(buffer, tempBuff);
		
		sprintf(tempBuff,"\"st%u_paramGa_velosity\":\"%.4f\",",streamNum,gParamFromHmi.Data.AllOutputCalc.NaturalGas[streamNumAtZero].speedOfFluid);
		strcat(buffer, tempBuff);

		sprintf(tempBuff,"\"st%u_paramGa_sos\":\"%.4f\",",streamNum,gParamFromHmi.Data.AllOutputCalc.NaturalGas[streamNumAtZero].speedOfSound);
		strcat(buffer, tempBuff);
		
		sprintf(tempBuff,"\"st%u_paramGa_kappa\":\"%.4f\",",streamNum,gParamFromHmi.Data.AllOutputCalc.NaturalGas[streamNumAtZero].isentropicExponent);
		strcat(buffer, tempBuff);

		sprintf(tempBuff,"\"st%u_paramGa_reynold\":\"%.4f\",",streamNum,gParamFromHmi.Data.AllOutputCalc.General[streamNumAtZero].reynoldNumber);
		strcat(buffer, tempBuff);
	}
	else if(gParamFromHmi.Setting.SetupStream[streamNumAtZero].Fluid.fluid == Stream_Liquid)    //fluid gasoil
    {
        //Nothing
    }

	//flowmeter
	if(gParamFromHmi.Setting.SetupStream[streamNumAtZero].flowMeterType == Stream_Diff_Pressure_meter)
    {
		sprintf(tempBuff,"\"st%u_paramOr_expansionFactor\":\"%.4f\",",streamNum,gParamFromHmi.Data.AllOutputCalc.NaturalGas[streamNumAtZero].epansionFactor);
		strcat(buffer, tempBuff);

		sprintf(tempBuff,"\"st%u_paramOr_orificeMeterCoefficient\":\"%.4f\",",streamNum,gParamFromHmi.Data.AllOutputCalc.NaturalGas[streamNumAtZero].orificeCoefficeint);
		strcat(buffer, tempBuff);
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

	//Remove Last , in json
	size_t jsonSize = strlen(buffer);
	if(jsonSize > 1)
	{
		buffer[jsonSize-1] = '\0';
	}
	
	//End
	strcat(buffer,"}");
}


static void setJson_paramConst(uint8_t streamNum, char *buffer)	//streamNum at one
{
	uint8_t streamNumAtZero = streamNum - 1;


	//Start
	sprintf(buffer,"{");

	//General
	sprintf(tempBuff,"\"st%u_paramGe_fluidName\":\"%s\",",streamNum,WebUiStringHmi.Parameter.fluidName[gParamFromHmi.Setting.SetupStream[streamNumAtZero].Fluid.fluid]);
	strcat(buffer, tempBuff);

	sprintf(tempBuff,"\"st%u_paramGe_unitVolume\":\"%s\",",streamNum,WebUiStringHmi.Parameter.unitVolume[gParamFromHmi.Setting.SetupStream[streamNumAtZero].UnitTotal.volume]);
	strcat(buffer, tempBuff);

	sprintf(tempBuff,"\"st%u_paramGe_flowmeterType\":\"%s\",",streamNum,WebUiStringHmi.Parameter.floweterType[gParamFromHmi.Setting.SetupStream[streamNumAtZero].flowMeterType]);
	strcat(buffer, tempBuff);

	sprintf(tempBuff,"\"st%u_paramGe_unitFlowrate\":\"%s\",",streamNum,WebUiStringHmi.Parameter.unitFlowrate[gParamFromHmi.Setting.SetupStream[streamNumAtZero].UnitTotal.volume_flowrate]);
	strcat(buffer, tempBuff);

	sprintf(tempBuff,"\"st%u_paramGe_minFlow\":\"%.4f\",",streamNum,gParamFromHmi.Setting.SetupStream[streamNumAtZero].General.min_flowrate);
	strcat(buffer, tempBuff);

	sprintf(tempBuff,"\"st%u_paramGe_maxFlow\":\"%.4f\",",streamNum,gParamFromHmi.Setting.SetupStream[streamNumAtZero].General.max_flowrate);
	strcat(buffer, tempBuff);

	sprintf(tempBuff,"\"st%u_paramGe_minor\":\"%.4f\",",streamNum,gParamFromHmi.Setting.SetupStream[streamNumAtZero].General.minor_keypad);
	strcat(buffer, tempBuff);

	sprintf(tempBuff,"\"st%u_paramGe_major\":\"%.4f\",",streamNum,gParamFromHmi.Setting.SetupStream[streamNumAtZero].General.maxor_keypad);
	strcat(buffer, tempBuff);

	sprintf(tempBuff,"\"st%u_paramGe_airPressure\":\"%.4f\",",streamNum,gParamFromHmi.Setting.SetupStream[streamNumAtZero].General.air_pressure);
	strcat(buffer, tempBuff);


	//Fluid
	if(gParamFromHmi.Setting.SetupStream[streamNumAtZero].Fluid.fluid == Stream_Gasses) //Fluid Gas
	{
		sprintf(tempBuff,"\"st%u_paramGa_pressureBase\":\"%.4f\",",streamNum,gParamFromHmi.Setting.SetupStream[streamNumAtZero].Fluid.pressure_base);
		strcat(buffer, tempBuff);

		sprintf(tempBuff,"\"st%u_paramGa_temperatureBase\":\"%.4f\",",streamNum,gParamFromHmi.Setting.SetupStream[streamNumAtZero].Fluid.temperature_base);
		strcat(buffer, tempBuff);
	}
	else if(gParamFromHmi.Setting.SetupStream[streamNumAtZero].Fluid.fluid == Stream_Liquid)    //fluid gasoil
    {
        //Nothing
    }


	//Flowmeter
	if(gParamFromHmi.Setting.SetupStream[streamNumAtZero].flowMeterType == Stream_Diff_Pressure_meter)
	{
		sprintf(tempBuff,"\"st%u_paramOr_pipeDiameter\":\"%.4f\",",streamNum,gParamFromHmi.Setting.SetupStream[streamNumAtZero].General.pipe_diameter);
		strcat(buffer, tempBuff);

		sprintf(tempBuff,"\"st%u_paramOr_orificeDiameter\":\"%.4f\",",streamNum,gParamFromHmi.Setting.SetupStream[streamNumAtZero].Flowmeter.diff_pressure_meter.constricted_diameter);
		strcat(buffer, tempBuff);

		sprintf(tempBuff,"\"st%u_paramOr_beta\":\"%.4f\",",streamNum,gParamFromHmi.Setting.SetupStream[streamNumAtZero].Flowmeter.diff_pressure_meter.diameter_ratio);
		strcat(buffer, tempBuff);

		sprintf(tempBuff,"\"st%u_paramOr_dynamicViscosity\":\"%.4f\",",streamNum,gParamFromHmi.Setting.SetupStream[streamNumAtZero].General.dinamic_viscosity);
		strcat(buffer, tempBuff);

		sprintf(tempBuff,"\"st%u_paramOr_temperatureReferencePipe\":\"%.4f\",",streamNum,gParamFromHmi.Setting.SetupStream[streamNumAtZero].Flowmeter.diff_pressure_meter.pipe_reference_temp);
		strcat(buffer, tempBuff);

		sprintf(tempBuff,"\"st%u_paramOr_temperatureReferenceOrifice\":\"%.4f\",",streamNum,gParamFromHmi.Setting.SetupStream[streamNumAtZero].Flowmeter.diff_pressure_meter.orifice_reference_temp);
		strcat(buffer, tempBuff);

		sprintf(tempBuff,"\"st%u_paramOr_pipeMaterial\":\"%s\",",streamNum,WebUiStringHmi.Orifice.pipeMaterial[gParamFromHmi.Setting.SetupStream[streamNumAtZero].Flowmeter.diff_pressure_meter.pipe_material]);
		strcat(buffer, tempBuff);

		sprintf(tempBuff,"\"st%u_paramOr_orificeMaterial\":\"%s\",",streamNum,WebUiStringHmi.Orifice.orificeMaterial[gParamFromHmi.Setting.SetupStream[streamNumAtZero].Flowmeter.diff_pressure_meter.orifice_material]);
		strcat(buffer, tempBuff);

		sprintf(tempBuff,"\"st%u_paramOr_tapConfigure\":\"%s\",",streamNum,WebUiStringHmi.Orifice.tapConfig[gParamFromHmi.Setting.SetupStream[streamNumAtZero].Flowmeter.diff_pressure_meter.tap_config]);
		strcat(buffer, tempBuff);

		sprintf(tempBuff,"\"st%u_paramOr_pressureTap\":\"%s\",",streamNum,WebUiStringHmi.Orifice.pressureTap[gParamFromHmi.Setting.SetupStream[streamNumAtZero].Flowmeter.diff_pressure_meter.pressure_upstream]);
		strcat(buffer, tempBuff);
	}
	else if(gParamFromHmi.Setting.SetupStream[streamNumAtZero].flowMeterType == Stream_Pulse_Meter)
	{
		sprintf(tempBuff,"\"st%u_paramTu_pulseInput\":\"%u\",",streamNum,gParamFromHmi.Setting.SetupStream[streamNumAtZero].Flowmeter.pulse_meter.pulse_channel);
		strcat(buffer, tempBuff);

		sprintf(tempBuff,"\"st%u_paramTu_pipeDiameter\":\"%.4f\",",streamNum,gParamFromHmi.Setting.SetupStream[streamNumAtZero].General.pipe_diameter);
		strcat(buffer, tempBuff);

		sprintf(tempBuff,"\"st%u_paramTu_kFactor\":\"%.4f\",",streamNum,gParamFromHmi.Setting.SetupStream[streamNumAtZero].Flowmeter.pulse_meter.k_factor);
		strcat(buffer, tempBuff);

		sprintf(tempBuff,"\"st%u_paramTu_mFactor\":\"%.4f\",",streamNum,gParamFromHmi.Setting.SetupStream[streamNumAtZero].Flowmeter.pulse_meter.m_factor);
		strcat(buffer, tempBuff);
	}
	else if(gParamFromHmi.Setting.SetupStream[streamNumAtZero].flowMeterType == Stream_Current_input)
	{
		sprintf(tempBuff,"\"st%u_paramCu_typeInput\":\"%s\",",streamNum,WebUiStringHmi.CurrentFlowMeter.typeInput[gParamFromHmi.Setting.SetupStream[streamNumAtZero].Flowmeter.current_input.type_of_input]);
		strcat(buffer, tempBuff);

		sprintf(tempBuff,"\"st%u_paramCu_flowrateChannel\":\"%u\",",streamNum,gParamFromHmi.Setting.SetupStream[streamNumAtZero].Flowmeter.current_input.flowrate_channel);
		strcat(buffer, tempBuff);

		sprintf(tempBuff,"\"st%u_paramCu_flowrateKeypadValue\":\"%.4f\",",streamNum,gParamFromHmi.Setting.SetupStream[streamNumAtZero].Flowmeter.current_input.keypad_value);
		strcat(buffer, tempBuff);
	}
	else if(gParamFromHmi.Setting.SetupStream[streamNumAtZero].flowMeterType == Stream_Ultrasonic)
	{
		//Nothing
	}


	//Remove Last , in json
	size_t jsonSize = strlen(buffer);
	if(jsonSize > 1)
	{
		buffer[jsonSize-1] = '\0';
	}
	
	//End
	strcat(buffer,"}");
}


static void setJson_paramGas21x(uint8_t streamNum, char *buffer)	//streamNum at one
{
	uint8_t streamNumAtZero = streamNum - 1;

	double pIndex[22];


	//Start
	sprintf(buffer,"{");

	//General
	//Nothing

	pIndex[0] = gParamFromHmi.Setting.SetupStream[streamNumAtZero].Fluid.natural_gas._x[1] * 100;
	pIndex[1] = gParamFromHmi.Setting.SetupStream[streamNumAtZero].Fluid.natural_gas._x[4] * 100;
	pIndex[2] = gParamFromHmi.Setting.SetupStream[streamNumAtZero].Fluid.natural_gas._x[5] * 100;
	pIndex[3] = gParamFromHmi.Setting.SetupStream[streamNumAtZero].Fluid.natural_gas._x[6] * 100;
	pIndex[4] = gParamFromHmi.Setting.SetupStream[streamNumAtZero].Fluid.natural_gas._x[7] * 100;
	pIndex[5] = gParamFromHmi.Setting.SetupStream[streamNumAtZero].Fluid.natural_gas._x[8] * 100;
	pIndex[6] = gParamFromHmi.Setting.SetupStream[streamNumAtZero].Fluid.natural_gas._x[9] * 100;
	pIndex[7] = gParamFromHmi.Setting.SetupStream[streamNumAtZero].Fluid.natural_gas._x[10] * 100;
	pIndex[8] = gParamFromHmi.Setting.SetupStream[streamNumAtZero].Fluid.natural_gas._x[11] * 100;
	pIndex[9] = gParamFromHmi.Setting.SetupStream[streamNumAtZero].Fluid.natural_gas._x[12] * 100;
	pIndex[10] = gParamFromHmi.Setting.SetupStream[streamNumAtZero].Fluid.natural_gas._x[13] * 100;
	pIndex[11] = gParamFromHmi.Setting.SetupStream[streamNumAtZero].Fluid.natural_gas._x[2] * 100;
	pIndex[12] = gParamFromHmi.Setting.SetupStream[streamNumAtZero].Fluid.natural_gas._x[3] * 100;
	pIndex[13] = gParamFromHmi.Setting.SetupStream[streamNumAtZero].Fluid.natural_gas._x[15] * 100;
	pIndex[14] = gParamFromHmi.Setting.SetupStream[streamNumAtZero].Fluid.natural_gas._x[16] * 100;
	pIndex[15] = gParamFromHmi.Setting.SetupStream[streamNumAtZero].Fluid.natural_gas._x[17] * 100;
	pIndex[16] = gParamFromHmi.Setting.SetupStream[streamNumAtZero].Fluid.natural_gas._x[18] * 100;
	pIndex[17] = gParamFromHmi.Setting.SetupStream[streamNumAtZero].Fluid.natural_gas._x[19] * 100;
	pIndex[18] = gParamFromHmi.Setting.SetupStream[streamNumAtZero].Fluid.natural_gas._x[21] * 100;
	pIndex[19] = gParamFromHmi.Setting.SetupStream[streamNumAtZero].Fluid.natural_gas._x[20] * 100;
	pIndex[20] = gParamFromHmi.Setting.SetupStream[streamNumAtZero].Fluid.natural_gas._x[14] * 100;

	//calculate Total
	pIndex[21] = 0;
	for (uint8_t i = 0; i < 21; i++)
	{
		pIndex[21] += pIndex[i]; 
	}

	for(uint8_t i = 0; i < 22; i++)
	{
		sprintf(tempBuff,"\"st%u_paramGas21x_%u\":\"%.4f\",",streamNum,i+1,pIndex[i]);
		strcat(buffer, tempBuff);
	}

	//Remove Last , in json
	size_t jsonSize = strlen(buffer);
	if(jsonSize > 1)
	{
		buffer[jsonSize-1] = '\0';
	}
	
	//End
	strcat(buffer,"}");	
}

