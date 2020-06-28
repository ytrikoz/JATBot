#ifndef JATBOT_HTTP_CLIENT_H
#define JATBOT_HTTP_CLIENT_H
#pragma once

#include <Arduino.h>
#include <WiFiClientSecure.h>

class JATBotHttpClient {
  public:
    static JATBotHttpClient *create();

    virtual void setReuseConnection(bool reuse) = 0;

    virtual void begin(const char *host, const char *path) = 0;

    virtual int sendRequest(const char *method, const char *data) = 0;

    virtual Stream &getStream() = 0;

    virtual String errorToString(int error) = 0;

    virtual bool connected() = 0;

    virtual void end() = 0;

    virtual void addHeader(const String &name, const String &value) = 0;

    virtual void collectHeaders(const char *keys[], const int keys_count) = 0;

    virtual String header(const String &name) = 0;
};

#endif