/*
 * Callback_ISR.c
 *
 *  Created on: Apr 12, 2025
 *      Author: RAYA_4
 */

#include "main.h"
#include "HMI_Connextion.h"



extern UART_HandleTypeDef HMICONN_UART_HANDLER;



void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size)
{
	if(huart->Instance == HMICONN_UART_HANDLER.Instance)
	{
		hmiConn_UartExRxDmaISR(Size);
	}
}
