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


/*extern variable*/


/*Global Variable*/
TaskHandle_t hmiConn_TaskHandle;

gParamFromHmi_t gParamFromHmi;



/*Local Variable*/
WsConn_SendDataFromHmi_All_t RecDataFromHmi;
WsConn_SendSettingFromHmi_All_t RecSettingFromHmi;



/*Local Function Prototype*/
static void getDataFromHmiRs232Poll();



/*Function*/
void hmiConn_taskHandler_connection(void *arg)
{
    /*Initial*/
    memset(&gParamFromHmi, 0, sizeof(gParamFromHmi));

    vTaskDelay(pdMS_TO_TICKS(500));    //Wait to System Startup TODO (must be 8000ms)

    while(1)
    {
        getDataFromHmiRs232Poll();
    }
}


int hmiConn_GetAllParam(gParamFromHmi_t *AllParam)
{
    //Check Flags if you want


    AllParam = &gParamFromHmi;

    return 0;
}



/*Local Function*/
static void getDataFromHmiRs232Poll()
{
    /**
     * 1- wait for get Data from HMI
     * 2- check header and footer
     * 3- check crc
     * 4- check version
     * 5- send Ack
     * 6- copy To gParamFromHmi
     * 7- set ready param flag
    */
}


