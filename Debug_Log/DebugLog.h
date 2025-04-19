/*
 * DebugLog.h
 *
 *  Created on: Mar 27, 2025
 *      Author: mojkh
 */

#ifndef DEBUGLOG_H_
#define DEBUGLOG_H_


#include "main.h"


#define DEBUG_UART_HANDLER		huart1


void debugLog_initAfterRtosApi();
void debugLog_printLogWithMutex(char *data, uint16_t len);


#endif /* DEBUGLOG_H_ */
