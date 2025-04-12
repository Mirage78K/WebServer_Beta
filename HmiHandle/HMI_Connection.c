/*
 * HMI_Connection.c
 *
 *  Created on: Apr 10, 2025
 *      Author: RAYA_4
 */


#include "HMI_Connextion.h"
#include <string.h>
#include "WebServer_Conn.h"
#include "cmsis_os.h"



typedef struct
{
    struct
    {
        SemaphoreHandle_t SemaHmiDmaRx;
    }RtosApi;
    

    struct
    {
        uint8_t dataReceiveBuffer[HMICONN_MAX_PACKET_RECEIVE_LENGH];
        uint8_t dataSendBuffer[HMICONN_PACKET_SEND_LENGTH];
        WsConn_SendDataFromHmi_All_t HmiRecvData;
        WsConn_SendSettingFromHmi_All_t HmiRecvSetting;
        uint16_t recvDataSize;
    }HmiHandle;
    
    
}HmiConnLocal_t;


/*extern variable*/
extern UART_HandleTypeDef HMICONN_UART_HANDLER;



/*Global Variable*/
TaskHandle_t hmiConn_TaskHandle;

gParamFromHmi_t gParamFromHmi;



/*Local Variable*/
WsConn_SendDataFromHmi_All_t RecDataFromHmi;
WsConn_SendSettingFromHmi_All_t RecSettingFromHmi;

HmiConnLocal_t HmiConnLocal;


/*Local Function Prototype*/
static void getDataFromHmiRs232Poll();

static uint8_t calChecksum(const uint8_t *data,uint16_t dataSize);



/*Function*/
void hmiConn_taskHandler_connection(void *arg)
{
    /*Initial*/
    memset(&gParamFromHmi, 0, sizeof(gParamFromHmi_t));
    memset(&HmiConnLocal, 0, sizeof(HmiConnLocal_t));

    //Initial RTOS API
    HmiConnLocal.RtosApi.SemaHmiDmaRx = xSemaphoreCreateBinary();


    vTaskDelay(pdMS_TO_TICKS(500));    //Wait to System Startup TODO (must be 8000ms)

    HAL_UARTEx_ReceiveToIdle_DMA(&HMICONN_UART_HANDLER, HmiConnLocal.HmiHandle.dataReceiveBuffer, sizeof(HmiConnLocal.HmiHandle.dataReceiveBuffer));

    while(1)
    {
        getDataFromHmiRs232Poll();
    }
}

void hmiConn_initialBeforeTask()
{
    //Nothing
}


int hmiConn_GetAllParam(gParamFromHmi_t *AllParam)
{
    //Check Flags if you want


    AllParam = &gParamFromHmi;

    return 0;
}


void hmiConn_UartExRxDmaISR(uint16_t size)
{
    HmiConnLocal.HmiHandle.recvDataSize = size;
    xSemaphoreGiveFromISR(HmiConnLocal.RtosApi.SemaHmiDmaRx, NULL);
}


/*Local Function*/
static void getDataFromHmiRs232Poll()
{
    /**
     * 1- wait for get Data from HMI
     * 2- active dma receive
     * 3- check header and footer
     * 4- check crc
     * 5- check version
     * 6- check data or setting cmd
     * 7- send Ack
     * 8- copy To gParamFromHmi
     * 9- set ready param flag
    */

    uint8_t crcValue;

    if(xSemaphoreTake(HmiConnLocal.RtosApi.SemaHmiDmaRx, pdMS_TO_TICKS(5000)) == pdTRUE)    //Use time out if you want
    {
        HAL_UARTEx_ReceiveToIdle_DMA(&HMICONN_UART_HANDLER, HmiConnLocal.HmiHandle.dataReceiveBuffer, sizeof(HmiConnLocal.HmiHandle.dataReceiveBuffer));

        if((HmiConnLocal.HmiHandle.dataReceiveBuffer[0] == WS_HMI_TRANSFER_HEADER_BYTE_LOW) &&
            (HmiConnLocal.HmiHandle.dataReceiveBuffer[1] == WS_HMI_TRANSFER_HEADER_BYTE_LOW) &&
                HmiConnLocal.HmiHandle.dataReceiveBuffer[HmiConnLocal.HmiHandle.recvDataSize - (2 + HMICONN_CRC_SIZE)] == WS_HMI_TRANSFER_FOOTER_BYTE_LOW &&
                    HmiConnLocal.HmiHandle.dataReceiveBuffer[HmiConnLocal.HmiHandle.recvDataSize - (1 + HMICONN_CRC_SIZE)] == WS_HMI_TRANSFER_FOOTER_BYTE_HIGH)
        {
            crcValue = calChecksum(HmiConnLocal.HmiHandle.dataReceiveBuffer, (HmiConnLocal.HmiHandle.recvDataSize - 1));   //-1 because remove CRC
            
            if(crcValue == HmiConnLocal.HmiHandle.dataReceiveBuffer[HmiConnLocal.HmiHandle.recvDataSize - 1])
            {
                HAL_GPIO_TogglePin(LED_BOARD_GPIO_Port, LED_BOARD_Pin);
            }
        }

        //clear buffer if you want
    }
}


static uint8_t calChecksum(const uint8_t *data,uint16_t dataSize)
{
	uint8_t res = 0;	//0^anything = anything
	for (uint32_t i=0 ; i<dataSize ; i++) {
		res ^= *(data+i);
	}
	return res;
}


