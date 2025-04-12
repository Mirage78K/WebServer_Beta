/*
 * DebugLog.c
 *
 *  Created on: Mar 27, 2025
 *      Author: mojkh
 */


#include "main.h"
#include "cmsis_os.h"
#include <string.h>


extern UART_HandleTypeDef DEBUG_UART_HANDLER;


SemaphoreHandle_t sendLogUartMutexHandle;


void debugLog_initBeforeTask()
{
    //Create Mutex
	sendLogUartMutexHandle = xSemaphoreCreateMutex();
}



/**
 * if len == 0 then use strlen for find length of string
 */
void debugLog_printLogWithMutex(char *data, uint16_t len)
{
	uint16_t dataLen;

	if(len == 0)
	{
		dataLen = strlen(data);
	}
	else
	{
		dataLen = len;
	}

	if(xTaskGetSchedulerState() == taskSCHEDULER_NOT_STARTED)
	{
//		HAL_UART_Transmit(&huart1, (uint8_t *)data, dataLen, 100);
	}
	else
	{
		if(xSemaphoreTake(sendLogUartMutexHandle, pdMS_TO_TICKS(200)) == pdTRUE)
		{
//			HAL_UART_Transmit(&huart1, (uint8_t *)data, dataLen, 100);
			xSemaphoreGive(sendLogUartMutexHandle);
		}
	}
}
