#pragma once

#ifndef JATBOT_JATBOT_KEYBOARD_H
#define JATBOT_JATBOT_KEYBOARD_H

#define ARDUINOJSON_DECODE_UNICODE 1
#define ARDUINOJSON_ENABLE_PROGMEM 1
#define ARDUINOJSON_USE_LONG_LONG 1

#include <ArduinoJson.h>

#include "JATBotObject.h"
#include "JATBotUtils.h"

class JATBotMarkup
{
public:
  JATBotMarkup();

  JATBotMarkup(const char *name);

  void addRow();

  void clear();

  size_t serialize(char *buf, size_t size);

protected:
  JsonObject addButton_(const String &text);

  JsonObject addButton_(const String &text, const String &param,
                        const bool value);
  JsonObject addButton_(const String &text, const String &param,
                        const String &value);

  void addParam(const String &param, bool value);

  void init();

protected:
  StaticJsonDocument<768> doc_;
  JsonObject root_;
  JsonArray rows_;
  JsonArray buttons_;

private:
  String name_;
};

class JATBotInlineKeyboard : public JATBotMarkup
{
public:
  JATBotInlineKeyboard() : JATBotMarkup("inline_keyboard"){};

  bool addButton(const InlineKeyboardButton type, const String text,
                 const String param);

  void addQueryButton(const String text, const String callback);

  void addUrlButton(const String text, const String url);
};

class JATBotReplyKeyboard : public JATBotMarkup
{
public:
  JATBotReplyKeyboard() : JATBotMarkup("keyboard"){};

  void addButton(const String &text);

  void addButton(ReplyKeyboardButton type, String caption);

  void addLocationButton(const String &text);

  void addContactButton(const String &text);

  void enableResize();

  void enableOneTime();

  void enableSelective();

private:
  bool selective_ = false;
  bool one_time_ = false;
  bool resize_ = false;
};

class JATBotHideKeyboard : public JATBotMarkup {
  public:    
    JATBotHideKeyboard();

    size_t serialize(char* buf, size_t size);
};

#endif