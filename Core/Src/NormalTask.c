/*
 * NormalTask.c
 *
 *  Created on: Apr 19, 2025
 *      Author: RAYA_4
 */


#include "NormalTask.h"
#include "cmsis_os.h"
#include "File_Handling.h"


TaskHandle_t NormalTask_TaskHandler;




void normalTask_taskHandler(void *arg)
{

	while(1)
	{
		HAL_GPIO_TogglePin(LED_BOARD_GPIO_Port, LED_BOARD_Pin);
		vTaskDelay(350);
	}
}


void normalTask_initialBeforeRtosApi()
{
	while(HAL_GPIO_ReadPin(SD_DTC_GPIO_Port, SD_DTC_Pin) == GPIO_PIN_SET);

	HAL_Delay(1000);

	Mount_SD("/");
}
