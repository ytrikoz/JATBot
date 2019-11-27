#ifndef JATBOT_OBJECT_H
#define JATBOT_OBJECT_H

#pragma once

#include "DebugUtils.h"
#include "JATBotCommon.h"
#include "JATBotUtils.h"

enum InlineKeyboardButton
{
  QUERY_BUTTON,
  URL_BUTTON,
};

enum ReplyKeyboardButton
{
  SIMPLE_BUTTON,
  CONTACT_BUTTON,
  LOCATION_BUTTON
};

static const char str_id[] PROGMEM = "id";
static const char str_is_bot[] PROGMEM = "is_bot";
static const char str_date[] PROGMEM = "date";
static const char str_data[] PROGMEM = "data";
static const char str_from[] PROGMEM = "from";
static const char str_first_name[] PROGMEM = "first_name";
static const char str_last_name[] PROGMEM = "last_name";
static const char str_username[] PROGMEM = "username";
static const char str_message[] PROGMEM = "message";
static const char str_message_id[] PROGMEM = "message_id";
static const char str_language_code[] PROGMEM = "language_code";
static const char str_result[] PROGMEM = "result";
static const char str_description[] PROGMEM = "description";
static const char str_title[] PROGMEM = "title";
static const char str_update_id[] PROGMEM = "update_id";
static const char str_user_id[] PROGMEM = "user_id";
static const char str_longitude[] PROGMEM = "longitude";
static const char str_location[] PROGMEM = "location";
static const char str_latitude[] PROGMEM = "latitude";
static const char str_text[] PROGMEM = "text";
static const char str_phone_number[] PROGMEM = "phone_number";
static const char str_vcard[] PROGMEM = "vcard";
static const char str_chat_instance[] PROGMEM = "chat_instance";
static const char str_callback_query[] PROGMEM = "callback_query";
static const char str_contact[] PROGMEM = "contact";
static const char str_chat[] PROGMEM = "chat";

static const char str_getMe[] PROGMEM = "getMe";
static const char str_answerCallBackQuery[] PROGMEM = "answerCallbackQuery";
static const char str_getUpdates[] PROGMEM = "getUpdates";
static const char str_sendMessage[] PROGMEM = "sendMessage";

static const char str_param_callback_query_id[] PROGMEM = "?callback_query_id=";
static const char str_param_show_alert[] PROGMEM = "&show_alert=true";
static const char str_param_chat_id[] PROGMEM = "?chat_id=";
static const char str_param_text[] PROGMEM = "&text=";
static const char str_param_markup[] PROGMEM = "&reply_markup=";
static const char str_param_limit[] PROGMEM = "?limit=1";
static const char str_param_allowed_updates[] PROGMEM =
    "&allowed_updates=message,callback_query";
static const char str_param_offset[] PROGMEM = "&offset=";

enum MessageType {
    MESSAGE_EMPTY,
    MESSAGE_TEXT,
    MESSAGE_QUERY,
};

struct TBUser {
    bool isBot;
    int32_t id;
    String firstName;
    String lastName;
    String username;
    String langCode;
};

struct TBChat {
    String id;
    String title;
};

struct TLocation {
    float longitude;
    float latitude;
};

struct TContact {
    String phone_number;
    String first_name;
    String last_name;
    String user_id;
    String vcard;
};

struct TMessage {
    MessageType type = MESSAGE_EMPTY;

    long long int message_id = 0;
    TBUser from;
    TBChat chat;
    unsigned long date = 0;
    String queryId;
    String queryData;
    String chat_instance;
    TLocation location;
    bool hasLocation = false;
    TContact contact;
    bool hasContact = false;
    String text;
    bool hasText = false;

    bool operator==(const String &str) const {
        return text.equalsIgnoreCase(str);
    }
};

class JATBotObject {
  public:
    static bool asGetMe(JsonDocument &doc, TBUser &dst) {
        JsonVariant obj = doc[FPSTR(str_result)];
        dst.id = obj[FPSTR(str_id)];
        dst.isBot = obj[FPSTR(str_is_bot)];
        dst.firstName = obj[FPSTR(str_first_name)].as<String>();
        dst.lastName = obj[FPSTR(str_last_name)].as<String>();
        dst.username = obj[FPSTR(str_username)].as<String>();
        dst.langCode = obj[FPSTR(str_language_code)].as<String>();
        return dst.id != 0;
    }

    static bool asUpdateId(JsonDocument &doc, long long int &dst) {
        dst =
            doc[FPSTR(str_result)][0][FPSTR(str_update_id)].as<long long int>();
        return dst != 0;
    }

    static bool asMessage(JsonDocument &doc, TMessage &dst) {
        JsonVariant root = doc[FPSTR(str_result)][0];
        if (!root.getMember(FPSTR(str_message)).isNull()) {
            dst.type = MESSAGE_TEXT;
            parseMessage(root, dst);
        } else if (!root.getMember(FPSTR(str_callback_query)).isNull()) {
            dst.type = MESSAGE_QUERY;
            JsonVariant query = root[FPSTR(str_callback_query)];
            dst.queryId = query[FPSTR(str_id)].as<String>();
            dst.queryData = query[FPSTR(str_data)].as<String>();
            dst.chat_instance = query[FPSTR(str_chat_instance)].as<String>();
            parseMessage(query, dst);
        }
        return dst.type != MESSAGE_EMPTY;
    }

    static void parseMessage(JsonVariant &src, TMessage &dst) {
        JsonVariant obj = src[FPSTR(str_message)];
        dst.message_id = obj[FPSTR(str_message_id)].as<long long int>();
        parseUser(obj, dst.from);
        parseChat(obj, dst.chat);
        dst.date = obj[FPSTR(str_date)];

        if (obj.getMember(FPSTR(str_contact))) {
            parseContact(obj, dst.contact);
            dst.hasContact = true;
        };

        if (obj.getMember(FPSTR(str_location))) {
            parseLocation(obj, dst.location);
            dst.hasLocation = true;
        };

        if (obj.getMember(FPSTR(str_text))) {
            dst.text = obj[FPSTR(str_text)].as<String>();
            dst.hasText = true;
        }
    }

    static void parseLocation(JsonVariant &src, TLocation &dst) {
        JsonVariant obj = src[FPSTR(str_location)];
        dst.longitude = obj[FPSTR(str_longitude)];
        dst.latitude = obj[FPSTR(str_latitude)];
    }

    static void parseUser(JsonVariant &src, TBUser &dst) {
        JsonObjectConst obj = src[FPSTR(str_from)];
        dst.id = obj[FPSTR(str_id)];
        dst.username = obj[FPSTR(str_username)].as<String>();
        dst.firstName = obj[FPSTR(str_first_name)].as<String>();
        dst.lastName = obj[FPSTR(str_last_name)].as<String>();
    }

    static void parseChat(JsonVariant &src, TBChat &dst) {
        JsonObjectConst obj = src[FPSTR(str_chat)];
        dst.id = obj[FPSTR(str_id)].as<String>();
        dst.title = obj[FPSTR(str_title)].as<String>();
    }

    static void parseContact(JsonVariant &src, TContact &dst) {
        JsonObjectConst obj = src[FPSTR(str_contact)];
        dst.user_id = obj[FPSTR(str_user_id)].as<String>();
        dst.first_name = obj[FPSTR(str_first_name)].as<String>();
        dst.last_name = obj[FPSTR(str_last_name)].as<String>();
        dst.phone_number = obj[FPSTR(str_phone_number)].as<String>();
        dst.vcard = obj[FPSTR(str_vcard)].as<String>();
    }
};

#endif