#ifndef JATBOT_ERROR_H
#define JATBOT_ERROR_H
#pragma once

enum ErrorCode
{
  RESPONSE_EMPTY = -1,
  BUFFER_TOO_SMALL = -2,
  DESERIALIZE_ERROR = -3,
  RESULT_ERROR = -4,
};

class JATBotError
{
public:
  static JATBotError OK() { return JATBotError(); }

  JATBotError()
  {
    code_ = 0;
    memset(message_, '\x00', sizeof(message_));
  }

  JATBotError(const int code, const char *message)
  {
    code_ = code;
    strncpy(message_, message, 128);
    Serial.print(code_);
    Serial.print(':');
    Serial.print(' ');
    Serial.println(message);
  }

  int code() const { return code_; }

  const char *message() const
  {
    return message_;
  }

  operator bool() const { return code_; }

private:
  int code_ = 0;
  char message_[128];
};

#endif