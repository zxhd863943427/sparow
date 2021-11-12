#ifndef _INCLUDE_UNICODEUTF8_H
#define _INCLUDE_UNICODEUTF8_H
#include "common.h"
uint32_t getByteNumOfEncodeUtf8(int value);
uint32_t getByteNumOfDecodeUtf8(uint8_t byte);
uint8_t encodeUtf8(uint8_t* buf, int value);
int decodeUtf8(const uint8_t* buf, uint32_t length);
#endif