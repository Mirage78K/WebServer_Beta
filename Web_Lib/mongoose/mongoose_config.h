/*
 * mongoose_config.h
 *
 *  Created on: Mar 25, 2025
 *      Author: mojkh
 */

#ifndef MONGOOSE_MONGOOSE_CONFIG_H_
#define MONGOOSE_MONGOOSE_CONFIG_H_


// See https://mongoose.ws/documentation/#build-options
#define MG_ARCH 						MG_ARCH_FREERTOS

#define MG_ENABLE_TCPIP 				1
#define MG_ENABLE_DRIVER_STM32F 		1
// #define MG_ENABLE_TCPIP_DRIVER_INIT		0

// #define MG_TCPIP_IP MG_IPV4(192, 168, 88, 160)
// #define MG_TCPIP_MASK MG_IPV4(255, 255, 255, 0)
// #define MG_TCPIP_GW MG_IPV4(192, 168, 88, 1)

//For Embedded File System
#define MG_ENABLE_PACKED_FS 			1
#define MG_ENABLE_POSIX_FS				0

#define MG_IO_SIZE						256		//default



#endif /* MONGOOSE_MONGOOSE_CONFIG_H_ */
