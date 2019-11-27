#ifndef DEBUG_UTILS_H
#define DEBUG_UTILS_H

#pragma once

#ifdef DEBUG_JATBOT
#define DEBUG_PRINT(...) Serial.print(__VA_ARGS__);
#define DEBUG_PRINTLN(...) Serial.println(__VA_ARGS__);
#define DEBUG_PRINTJSON(...)                                                   \
    serializeJsonPretty(__VA_ARGS__, Serial);                                  \
    Serial.println();
#else
#define DEBUG_PRINT(...)
#define DEBUG_PRINTLN(...)
#define DEBUG_PRINTJSON(...)
#endif

#endif
