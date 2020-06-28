#ifndef JATBOT_UTILS_H
#define JATBOT_UTILS_H

#pragma once

#include <Arduino.h>

int i64toa(char *buf, const int64_t value);

void urlencode(char *buf, const char *in, const size_t size);

#endif