#pragma once

#include "stdint.h"

typedef struct
{
    char* buff;
    uint16_t buffSize;
    uint16_t strLen;
} JsonString;

#define JSON_STR_DEF(name, size, modifier) \
static char name##_jbuff[size]; \
modifier JsonString name = {name##_jbuff, size, 0};

uint8_t JsonStart(JsonString* jStr);
uint8_t JsonAddUint32(JsonString* jStr, const char* key, const uint32_t value);
uint8_t JsonAddUint8Array(JsonString* jStr, const char* key, const uint8_t* array, const uint16_t arrayLen);
uint8_t JsonEnd(JsonString* jStr);
