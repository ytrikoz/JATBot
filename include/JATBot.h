#ifndef JATBOT_H
#define JATBOT_H

#pragma once

#define ARDUINOJSON_DECODE_UNICODE 1
#define ARDUINOJSON_ENABLE_PROGMEM 1
#define ARDUINOJSON_USE_LONG_LONG 1

#define JATBOT_VERSION "0.4"
#define TELEGRAM_HOST "api.telegram.org"

#include <Arduino.h>
#include <ArduinoJson.h>

#include "DebugUtils.h"
#include "JATBotCommon.h"
#include "JATBotError.h"
#include "JATBotHttpClient.h"
#include "JATBotMarkup.h"
#include "JATBotObject.h"
#include "JATBotUtils.h"

class JATBotRequest
{
public:
  JATBotRequest(JATBotHttpClient *http) { http_ = http; };

  ~JATBotRequest() { http_->end(); };

  int send(const char *host, const char *auth, const char *method,
           const char *path);

  Stream &responseStream() { return http_->getStream(); };

  void analyzeError(const char *method, int status,
                    const char *path_with_auth);

  const JATBotError &error() const { return error_; }

protected:
  JATBotHttpClient *http_;
  JATBotError error_;
};

class JATBot
{
public:
  JATBot(const char *auth)
  {
    strcpy(auth_, auth);
    http_ = JATBotHttpClient::create();
    http_->setReuseConnection(true);
    req_ = new JATBotRequest(http_);
    ready_ = false;
  }

  JATBot(JATBot &&rval)
  {
    strlcpy(auth_, rval.auth_, sizeof(auth_));
    http_ = rval.http_;
    req_ = rval.req_;
    ready_ = rval.ready_;
  }

  bool ready() { return ready_; }

  const JATBotError &error();

  bool getMe(TBUser &user);

  bool getUpdates(TMessage &message);

  bool answerCallbackQuery(const char *query_id, const char *text,
                           const bool alert = false);

  bool sendMessage(const long long int chat_id, const char *text);

  bool testConnection();

  bool success();

  bool failed();

  JATBotReplyKeyboard *showReplyKeyboard();

  JATBotInlineKeyboard *showInlineKeyboard();

  void hideReplyKeyboard();

protected:
  void request(const char *path);

  bool response(DynamicJsonDocument &doc);

private:
  bool ready_;
  char auth_[46];
  long long int update_id_ = 0;
  JATBotError error_;
  JATBotHttpClient *http_;
  JATBotRequest *req_;
  JATBotMarkup *markup = nullptr;
};

#endif