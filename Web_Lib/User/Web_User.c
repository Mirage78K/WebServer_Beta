/*
 * Web_User.c
 *
 *  Created on: Mar 25, 2025
 *      Author: mojkh
 */


#include "Web_User.h"
#include "cmsis_os.h"
#include "mongoose.h"
#include "DebugLog.h"


#define ETH_PHY_ADDRESS						0

#define LAN8720_BASICSTATUS_REG				0x01U
#define LAN8720_SYMBOLERRORCOUNTER_REG		0x1AU



/*extern variable*/


/*Global Variable*/
TaskHandle_t WebServer_TaskHandler;


/*Local Variable*/
gWebServer_t gWebServer;


//third pary
struct mg_mgr mgr;
struct mg_tcpip_if mif;
struct mg_tcpip_driver_stm32f_data driver_data;




static void webServerCallBack(struct mg_connection *cn, int ev ,void *ev_data);

static void userInit();
static void thirdPartyInit();
static void checkPhyHandle();

static uint16_t readPhy(uint8_t addr, uint8_t reg);
static void mylog(char ch, void *param);



void webUser_taskHandler_webServer(void *arg)
{
	//initial
	userInit();
	thirdPartyInit();

	while(1)
	{
		checkPhyHandle();
		mg_mgr_poll(&mgr, 100);
	}
}



void webServerCallBack(struct mg_connection *cn, int ev ,void *ev_data)
{
	if(ev == MG_EV_HTTP_MSG)
	{
	     struct mg_http_message *hm = ev_data;  // Parsed HTTP request
	     struct mg_http_serve_opts opts = {.root_dir = "/web_root", .fs = &mg_fs_packed};
		 mg_http_serve_dir(cn, hm, &opts);
	}
}



void webUser_initialBeforeTask()
{
	//create mutex

	//create sema

	//create queue

	//create task
}


void webUser_taskPoll()
{

}


static void userInit()
{
	gWebServer.Config.Driver.macAddress[0] = 0x11;
	gWebServer.Config.Driver.macAddress[1] = 0x22;
	gWebServer.Config.Driver.macAddress[2] = 0x44;
	gWebServer.Config.Driver.macAddress[3] = 0x55;
	gWebServer.Config.Driver.macAddress[4] = 0xAA;
	gWebServer.Config.Driver.macAddress[5] = 0xBB;

	gWebServer.Config.Driver.ipV4[0] = 192;
	gWebServer.Config.Driver.ipV4[1] = 168;
	gWebServer.Config.Driver.ipV4[2] = 88;
	gWebServer.Config.Driver.ipV4[3] = 160;

	gWebServer.Config.Driver.maskV4[0] = 255;
	gWebServer.Config.Driver.maskV4[1] = 255;
	gWebServer.Config.Driver.maskV4[2] = 255;
	gWebServer.Config.Driver.maskV4[3] = 0;

	gWebServer.Config.Driver.gwV4[0] = 192;
	gWebServer.Config.Driver.gwV4[1] = 168;
	gWebServer.Config.Driver.gwV4[2] = 88;
	gWebServer.Config.Driver.gwV4[3] = 1;
}


//fisrt call userInit then call this function
static void thirdPartyInit()
{

	memset(&driver_data, 0, sizeof(driver_data));
	memset(&mif, 0, sizeof(mif));

	mg_log_set_fn(mylog, NULL);
	mg_log_set(MG_LL_DEBUG);
	mg_mgr_init(&mgr);


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

	NVIC_EnableIRQ(ETH_IRQn);
    mg_tcpip_init(&mgr, &mif);


    //Initial HTTP App
    mg_http_listen(&mgr, "http://0.0.0.0:80", webServerCallBack, NULL);
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
