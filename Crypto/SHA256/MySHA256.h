#ifndef __MY_SHA256_H__
#define __MY_SHA256_H__

#include "stdint.h"

#define MY_SHA256_BLOCK_SIZE 32

int mySha256_calculate(const uint8_t *inputData, uint32_t inputLen, uint8_t *outputData);


#endif
