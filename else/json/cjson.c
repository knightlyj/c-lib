#include "cjson.h"

static inline uint8_t FormatHelper(JsonString* jStr, const char* format, ...)
{
    va_list valist;
    va_start(valist, format);

    size_t remSize = jStr->buffSize - jStr->strLen;
    int len = vsnprintf(jStr->buff + jStr->strLen, remSize, format, valist);
    va_end(valist);

    if (len >= remSize)
        return 0;

    jStr->strLen += len;
    return 1;
}

uint8_t JsonStart(JsonString* jStr)
{
    if (jStr->buff == 0 || jStr->strLen >= jStr->buffSize)
        return 0;

    jStr->strLen = 0;
    return FormatHelper(jStr, "{\n");
}

static uint8_t JsonAddKey(JsonString* jStr, const char* key)
{
    if (jStr->buff == 0 || jStr->strLen >= jStr->buffSize)
        return 0;

    const char format[] = "\"%s\":";
    return FormatHelper(jStr, format, key);
}

uint8_t JsonAddUint32(JsonString* jStr, const char* key, const uint32_t value)
{
    if (JsonAddKey(jStr, key) == 0)
        return 0;

    const char format[] = "\"%d\",\n";
    return FormatHelper(jStr, format, value);
}

uint8_t JsonAddUint8Array(JsonString* jStr, const char* key, const uint8_t* array, const uint16_t arrayLen)
{
    if (JsonAddKey(jStr, key) == 0)
        return 0;

    //加"
    if (FormatHelper(jStr, "\"") == 0)
        return 0;

    //加数据
    for (int i = 0; i < arrayLen; i++)
    {
        if (i == 0)
        {
            if (FormatHelper(jStr, "%d", array[i]) == 0)
                return 0;
        }
        else
        {
            if (FormatHelper(jStr, ",%d", array[i]) == 0)
                return 0;
        }
    }

    //加",
    return FormatHelper(jStr, "\",\n");
}

uint8_t JsonEnd(JsonString* jStr)
{
    if (jStr->buff == 0 || jStr->strLen >= jStr->buffSize)
        return 0;

    return FormatHelper(jStr, "}");
}
