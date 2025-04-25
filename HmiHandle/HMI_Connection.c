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



typedef enum
{
    HMI_REC_PACKET_STATE_OK,
    HMI_REC_PACKET_STATE_HEADER_FOOTER_ERR,
    HMI_REC_PACKET_STATE_CRC_ERR,
    HMI_REC_PACKET_STATE_VERSION_ERR,
    HMI_REC_PACKET_STATE_CMD_ERR,
    HMI_REC_PACKET_STATE_LENGTH_ERR,
}Hmi_ReceivePacket_State_t;



typedef struct
{
    struct
    {
        SemaphoreHandle_t SemaHmiDmaRx;
    }RtosApi;
    

    struct
    {
        uint8_t packetReceiveBuffer[HMICONN_MAX_PACKET_RECEIVE_LENGH];
        uint8_t dataSendBuffer[HMICONN_PACKET_SEND_LENGTH];
        WsConn_SendDataFromHmi_All_t HmiRecvData;
        WsConn_SendSettingFromHmi_All_t HmiRecvSetting;
        WsConn_SendFromWebSrv_All_t SendFromWebserver;
        uint16_t recvPacketSize;

        Hmi_ReceivePacket_State_t packetRecvSatate;
    }HmiHandle;
    
    
}HmiConnLocal_t;


/*extern variable*/
extern UART_HandleTypeDef HMICONN_UART_HANDLER;



/*Global Variable*/
TaskHandle_t hmiConn_TaskHandle;

gParamFromHmi_t gParamFromHmi;



/*Local Variable*/
HmiConnLocal_t HmiConnLocal;


/*Local Function Prototype*/
static void getDataFromHmiRs232Poll();
static int analyzeAndGetDataFromHmi();
static int analyzeAndGetSettingFromHmi();
static void preparePacketSendToHmi();
static uint8_t getVersionPacketFromWebSrv();
static void sendPacketToHmi();
static int checkPacketFromHmiVersion(WsConn_SendPacketFromHmiCmd_e cmdType);
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

    HAL_UARTEx_ReceiveToIdle_DMA(&HMICONN_UART_HANDLER, HmiConnLocal.HmiHandle.packetReceiveBuffer, sizeof(HmiConnLocal.HmiHandle.packetReceiveBuffer));

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
    HmiConnLocal.HmiHandle.recvPacketSize = size;
    xSemaphoreGiveFromISR(HmiConnLocal.RtosApi.SemaHmiDmaRx, NULL);
}


/*Local Function*/
static void getDataFromHmiRs232Poll()
{
    /**
     * 1- wait for get Data from HMI
     * 3- check header and footer
     * 4- check crc
     * 5- check cmd
     * 6- check version
     * 7- send Ack
     * 8- copy To gParamFromHmi
     * 9- set ready param flag
     * 9- clear buffers if you want
     * 2- active dma receive
    */

    uint8_t crcValue;
    int res;

    if(xSemaphoreTake(HmiConnLocal.RtosApi.SemaHmiDmaRx, pdMS_TO_TICKS(5000)) == pdTRUE)    //Use time out if you want
    {

        if((HmiConnLocal.HmiHandle.packetReceiveBuffer[0] == WS_HMI_TRANSFER_FROM_HMI_HEADER_BYTE_LOW) &&
            (HmiConnLocal.HmiHandle.packetReceiveBuffer[1] == WS_HMI_TRANSFER_FROM_HMI_HEADER_BYTE_HIGH))
        {
            crcValue = calChecksum(HmiConnLocal.HmiHandle.packetReceiveBuffer, (HmiConnLocal.HmiHandle.recvPacketSize - 1));   //-1 because remove CRC
            
            if(crcValue == HmiConnLocal.HmiHandle.packetReceiveBuffer[HmiConnLocal.HmiHandle.recvPacketSize - 1])
            {
                
                WsConn_PacketSendFromHMI_Header_t HeaderPacket;
                memcpy(&HeaderPacket, HmiConnLocal.HmiHandle.packetReceiveBuffer, sizeof(WsConn_PacketSendFromHMI_Header_t));

                switch (HeaderPacket.respCmd)
                {
                case WS_CONN_PACKETCMD_HMI_DATA:
                    res = analyzeAndGetDataFromHmi();
                    if(res == 0)
                    {
                        HmiConnLocal.HmiHandle.packetRecvSatate = HMI_REC_PACKET_STATE_OK;
                        gParamFromHmi.StateFlag.isDataReceived = true;
                    }
                    else if(res == 1)
                    {
                        HmiConnLocal.HmiHandle.packetRecvSatate = HMI_REC_PACKET_STATE_HEADER_FOOTER_ERR;
                    }
                    else if(res == 2)
                    {
                        HmiConnLocal.HmiHandle.packetRecvSatate = HMI_REC_PACKET_STATE_LENGTH_ERR;
                    }
                    else if(res == 3)
                    {
                        HmiConnLocal.HmiHandle.packetRecvSatate = HMI_REC_PACKET_STATE_VERSION_ERR;
                    }
                    break;

                case WS_CONN_PACKETCMD_HMI_SETTING:
                    res = analyzeAndGetSettingFromHmi();
                    if(res == 0)
                    {
                        HmiConnLocal.HmiHandle.packetRecvSatate = HMI_REC_PACKET_STATE_OK;
                        gParamFromHmi.StateFlag.isSettingReceived = true;
                    }
                    else if(res == 1)
                    {
                        HmiConnLocal.HmiHandle.packetRecvSatate = HMI_REC_PACKET_STATE_HEADER_FOOTER_ERR;
                    }
                    else if(res == 2)
                    {
                        HmiConnLocal.HmiHandle.packetRecvSatate = HMI_REC_PACKET_STATE_LENGTH_ERR;
                    }
                    else if(res == 3)
                    {
                        HmiConnLocal.HmiHandle.packetRecvSatate = HMI_REC_PACKET_STATE_VERSION_ERR;
                    }
                    break;
                
                default:
                    HmiConnLocal.HmiHandle.packetRecvSatate = HMI_REC_PACKET_STATE_CMD_ERR;
                    break;
                }

                
                if((HmiConnLocal.HmiHandle.packetRecvSatate == HMI_REC_PACKET_STATE_OK) ||
                    (HmiConnLocal.HmiHandle.packetRecvSatate == HMI_REC_PACKET_STATE_VERSION_ERR))
                {
                    preparePacketSendToHmi();
                    sendPacketToHmi();
                }
            }
            else
            {
                HmiConnLocal.HmiHandle.packetRecvSatate = HMI_REC_PACKET_STATE_CRC_ERR;
            }
        }
        else
        {
            HmiConnLocal.HmiHandle.packetRecvSatate = HMI_REC_PACKET_STATE_HEADER_FOOTER_ERR;
        }

        //TODO clear buffer if you want


        HAL_UARTEx_ReceiveToIdle_DMA(&HMICONN_UART_HANDLER, HmiConnLocal.HmiHandle.packetReceiveBuffer, sizeof(HmiConnLocal.HmiHandle.packetReceiveBuffer));
    }
}


static int analyzeAndGetDataFromHmi()     //out = 0(OK), 1(header or footer error), 2(Packet Length error), 3(version error)
{
    memcpy(&HmiConnLocal.HmiHandle.HmiRecvData, HmiConnLocal.HmiHandle.packetReceiveBuffer, HmiConnLocal.HmiHandle.recvPacketSize - 1);    //-1 for crc
    
    if((HmiConnLocal.HmiHandle.HmiRecvData.HeaderSt.header != WS_HMI_TRANSFER_FROM_HMI_HEADER) || (HmiConnLocal.HmiHandle.HmiRecvData.footer != WS_HMI_TRANSFER_FROM_HMI_FOOTER))
    {
        return 1;
    }
    else if(HmiConnLocal.HmiHandle.HmiRecvData.HeaderSt.packetLength != HmiConnLocal.HmiHandle.recvPacketSize)
    {
        return 2;
    }
    else if(checkPacketFromHmiVersion(WS_CONN_PACKETCMD_HMI_DATA) != 0)
    {
        return 3;
    }

    //check range (if needed)

    //Get Data
    memcpy(&gParamFromHmi.Data.AllDataFlow, &HmiConnLocal.HmiHandle.HmiRecvData.AllDataFlow, sizeof(gParamFromHmi.Data.AllDataFlow));
    memcpy(&gParamFromHmi.Data.ClockAndData, &HmiConnLocal.HmiHandle.HmiRecvData.ClockAndData, sizeof(gParamFromHmi.Data.ClockAndData));

    return 0;
}


static int analyzeAndGetSettingFromHmi()  //out = 0(OK), 1(header or footer error), 2(Packet Length error), 3(version error)
{
    memcpy(&HmiConnLocal.HmiHandle.HmiRecvSetting, HmiConnLocal.HmiHandle.packetReceiveBuffer, HmiConnLocal.HmiHandle.recvPacketSize - 1);    //-1 for crc
    
    if((HmiConnLocal.HmiHandle.HmiRecvSetting.HeaderSt.header != WS_HMI_TRANSFER_FROM_HMI_HEADER) || (HmiConnLocal.HmiHandle.HmiRecvSetting.footer != WS_HMI_TRANSFER_FROM_HMI_FOOTER))
    {
        return 1;
    }
    else if(HmiConnLocal.HmiHandle.HmiRecvSetting.HeaderSt.packetLength != HmiConnLocal.HmiHandle.recvPacketSize)
    {
        return 2;
    }
    else if(checkPacketFromHmiVersion(WS_CONN_PACKETCMD_HMI_SETTING) != 0)
    {
        return 3;
    }

    //check range (if needed)

    //Get Data
    //Nothing yet

    return 0;
}


static void preparePacketSendToHmi()
{
    /**
     * (prepare HmiConnLocal.HmiHandle.SendFromWebserver Structure)
     * 1- clear structure
     * 2- header and footer
     * 3- prepare state
     * 4- prepare data
    */

    memset(&HmiConnLocal.HmiHandle.SendFromWebserver, 0, sizeof(HmiConnLocal.HmiHandle.SendFromWebserver));

    //prepare header and footer
    HmiConnLocal.HmiHandle.SendFromWebserver.HeaderSt.header = WS_HMI_TRANSFER_FROM_WS_HEADER;
    HmiConnLocal.HmiHandle.SendFromWebserver.HeaderSt.respCmd = WS_CONN_PACKETCMD_WS_ALL;
    HmiConnLocal.HmiHandle.SendFromWebserver.HeaderSt.version = getVersionPacketFromWebSrv();
    HmiConnLocal.HmiHandle.SendFromWebserver.HeaderSt.packetLength = HMICONN_PACKET_SEND_LENGTH;
    HmiConnLocal.HmiHandle.SendFromWebserver.footer = WS_HMI_TRANSFER_FROM_WS_FOOTER;


    //prepare state
    if(gParamFromHmi.StateFlag.isSettingReceived == false)
    {
        HmiConnLocal.HmiHandle.SendFromWebserver.statePacket.needSetting = 1;
    }

    if(HmiConnLocal.HmiHandle.packetRecvSatate == HMI_REC_PACKET_STATE_VERSION_ERR)
    {
        HmiConnLocal.HmiHandle.SendFromWebserver.statePacket.versionErr = 1;
    }
    

    //prepare data

}


static uint8_t getVersionPacketFromWebSrv()
{
    return 0;
}


static void sendPacketToHmi()
{
    /**
     * 1- calculate CRC
     * 2- put structure and crc in bufferSend
     * 3- send to HMI
    */

    uint8_t crcSend = calChecksum((uint8_t *)&HmiConnLocal.HmiHandle.SendFromWebserver, sizeof(HmiConnLocal.HmiHandle.SendFromWebserver));

    memcpy(HmiConnLocal.HmiHandle.dataSendBuffer, &HmiConnLocal.HmiHandle.SendFromWebserver, sizeof(HmiConnLocal.HmiHandle.SendFromWebserver));
    HmiConnLocal.HmiHandle.dataSendBuffer[sizeof(HmiConnLocal.HmiHandle.SendFromWebserver)] = crcSend;  //Last byte

    HAL_UART_Transmit_DMA(&HMICONN_UART_HANDLER, HmiConnLocal.HmiHandle.dataSendBuffer, HMICONN_PACKET_SEND_LENGTH);

//    HAL_GPIO_TogglePin(LED_BOARD_GPIO_Port, LED_BOARD_Pin);
}


static int checkPacketFromHmiVersion(WsConn_SendPacketFromHmiCmd_e cmdType)
{
    return 0;
}


static uint8_t calChecksum(const uint8_t *data,uint16_t dataSize)
{
	uint8_t res = 0;	//0^anything = anything
	for (uint32_t i=0 ; i<dataSize ; i++) {
		res ^= *(data+i);
	}
	return res;
}


