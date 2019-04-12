#pragma once

#include "cl_common.h"

//计算crc16
uint16_t CalcCRC16(const uint8_t* data, int count);

//根据之前的crc,继续计算crc16
uint16_t CalcCRC16Ex(uint16_t crc, const uint8_t* data, int count);

//计算crc32
uint32_t CalcCRC32(const uint8_t* data, int count);

//根据之前的值,继续计算crc32
uint32_t CalcCRC32Ex(uint32_t crc, const uint8_t* data, int count);


//uint32_t CRC32_LittleEndian(uint32_t crc, uint32_t* data, uint32_t length);



