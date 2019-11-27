#ifndef JATBOT_COMMON_H
#define JATBOT_COMMON_H
#pragma once

#include "DebugUtils.h"

#include <Arduino.h>

#include "JATBotError.h"

enum LogLevel { ERROR, WARN, INFO };

static const char strP_error[] PROGMEM = "error";
static const char strP_warn[] PROGMEM = "warn";
static const char strP_info[] PROGMEM = "info";
static const char strP_unknown[] PROGMEM = "unknown";

static const char strP_empty_response[] PROGMEM = "Empty response";
static const char strP_response_oversize[] PROGMEM = "Response oversize";
static const char strP_deserialize_error[] PROGMEM = "Deserialize error";
static const char strP_result_false[] PROGMEM = "Result false";

inline const String getLogLevel(const LogLevel level) {
    PGM_P strP;
    switch (level) {
    case ERROR:
        strP = strP_error;
        break;
    case WARN:
        strP = strP_warn;
        break;
    case INFO:
        strP = strP_info;
        break;
    default:
        strP = strP_unknown;
        break;
    }
    String str = FPSTR(strP);
    str.toUpperCase();
    str = "[" + str + "]";
    return str;
}

#endif