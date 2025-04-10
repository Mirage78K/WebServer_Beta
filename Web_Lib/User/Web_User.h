/*
 * Web_User.h
 *
 *  Created on: Mar 25, 2025
 *      Author: mojkh
 */

#ifndef USER_WEB_USER_H_
#define USER_WEB_USER_H_


#include "main.h"
#include "mongoose.h"

typedef struct
{
	struct
	{
		uint8_t macAddress[6];
		uint8_t ipV4[4];
		uint8_t maskV4[4];
		uint8_t gwV4[4];
	}Driver;
}WebServerConfig_t;


typedef struct
{
	WebServerConfig_t Config;
}gWebServer_t;



void webUser_initialBeforeTask();
void webUser_taskHandler_webServer(void *arg);


/*add in thirdparty*/
int webUser_myFuncInHttpHandler(struct mg_connection *c, int ev, void *ev_data);


#endif /* USER_WEB_USER_H_ */
