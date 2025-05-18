/*
 * Callback_ISR.c
 *
 *  Created on: Apr 12, 2025
 *      Author: RAYA_4
 */

#include "main.h"
#include "HMI_Connextion.h"



extern UART_HandleTypeDef HMICONN_UART_HANDLER;



void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	if(huart->Instance == HMICONN_UART_HANDLER.Instance)
	{
		hmiConn_UartRxDmaISR();
	}
}
