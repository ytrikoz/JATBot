#include "JATBot.h"

namespace
{

static const char fingerprint[] =
    "BB:DC:45:2A:07:E3:4A:71:33:40:32:DA:BE:81:F7:72:6F:4A:2B:6B";

#define MARKUP_BUFFER_SIZE 512
#define MESSAGE_BUFFER_SIZE 1024

char *makeTelegramQuery(char *buf, const char *auth, const char *path)
{
    strcpy(buf, "/bot");
    strcat(buf, auth);
    size_t len = strlen(buf);
    if (path[0] != '/')
        buf[len] = '/';
    strcat(buf, path);
    return buf;
}
} // namespace

int JATBotRequest::send(const char *host, const char *auth, const char *method,
                        const char *path)
{
    char *buf = new char[2048];
    memset(buf, '\x00', 2048);

    makeTelegramQuery(buf, auth, path);

    DEBUG_PRINT("sendRequest: ");
    DEBUG_PRINTLN(buf);

    http_->begin(host, buf, fingerprint);
    int status = http_->sendRequest(method, buf);
    analyzeError(method, status, buf);

    delete[] buf;

    return status;
}

void JATBotRequest::analyzeError(const char *method, int status,
                                 const char *path_with_auth)
{
    if (status == 200)
    {
        error_ = JATBotError();
        return;
    }
    char buf[256];

    strcpy(buf, method);
    strcat(buf, ": ");
    strlcat(buf, path_with_auth, 256);

    error_ = JATBotError(status, buf);
}

void JATBot::request(const char *path)
{
    req_->send(TELEGRAM_HOST, auth_, "GET", path);
    error_ = req_->error();
}

bool JATBot::response(DynamicJsonDocument &doc)
{
    // while (req_->responseStream().available()) {
    //        Serial.print((char)req_->responseStream().read());
    DeserializationError jsonError =
        deserializeJson(doc, req_->responseStream());
    if (jsonError)
    {
        error_ = JATBotError(DESERIALIZE_ERROR, jsonError.c_str());
        return false;
    }

    if (!doc["ok"].as<bool>())
    {
        String errorStr = doc[FPSTR(str_description)].as<String>();
        error_ = JATBotError(RESULT_ERROR, errorStr.c_str());
        return false;
    }
    return true;
}

bool JATBot::success() { return error_.code() == 0; }

bool JATBot::failed() { return error_.code() != 0; }

const JATBotError &JATBot::error() { return error_; }

bool JATBot::testConnection()
{
    TBUser user;
    return ready_ = getMe(user) && user.isBot;
}

JATBotReplyKeyboard *JATBot::showReplyKeyboard()
{
    if (markup)
        delete markup;
    markup = new JATBotReplyKeyboard();
    return (JATBotReplyKeyboard *) markup;
}

JATBotInlineKeyboard *JATBot::showInlineKeyboard()
{
     if (markup)
        delete markup;
    markup = new JATBotInlineKeyboard();
    return (JATBotInlineKeyboard* ) markup;
}

bool JATBot::getMe(TBUser &user)
{
    char buf[8];
    strcpy_P(buf, str_getMe);
    request(buf);

    if (success())
    {
        DynamicJsonDocument doc(2048);
        if (response(doc))
        {
            DEBUG_PRINT("getMe memoryUsage:");
            DEBUG_PRINTLN(doc.memoryUsage());
            DEBUG_PRINTJSON(doc);
            return JATBotObject::asGetMe(doc, user);
        }
    }
    return false;
}

void JATBot::hideReplyKeyboard()
{
    if (markup)
        delete markup;
    markup = new JATBotHideKeyboard();
}

bool JATBot::sendMessage(const long long int chat_id, const char *text)
{
    char *buf = new char[MESSAGE_BUFFER_SIZE];
    memset(buf, '\x00', MESSAGE_BUFFER_SIZE);
    strcpy_P(buf, str_sendMessage);

    char id_buf[64];
    i64toa(id_buf, chat_id);
    strcat_P(buf, str_param_chat_id);
    strcat(buf, id_buf);
   
    char *encoded = new char[MESSAGE_BUFFER_SIZE];
    urlencode(encoded, text, MESSAGE_BUFFER_SIZE);

    strcat_P(buf, str_param_text);
    strcat(buf, encoded);
    delete[] encoded;

    if (markup)
    {
        strcat_P(buf, str_param_markup);
        size_t json_size = markup->serialize(&buf[strlen(buf)], MARKUP_BUFFER_SIZE);
        delete markup;    
        markup = nullptr;    
        if (json_size > MESSAGE_BUFFER_SIZE) {
            error_ = JATBotError(BUFFER_TOO_SMALL, "Keyboard");
        };          
    }
    request(buf);
    delete[] buf;
    return success();
}

bool JATBot::getUpdates(TMessage &msg)
{
    char buf[256];
    strcpy_P(buf, str_getUpdates);
    strcat_P(buf, str_param_limit);
    if (update_id_ != 0)
    {
        strcat_P(buf, str_param_offset);
        size_t len = strlen(buf);
        sprintf(buf + len, "%0lld", update_id_);
    }
    strcat_P(buf, str_param_allowed_updates);

    request(buf);

    if (success())
    {
        DynamicJsonDocument doc(2048);
        if (response(doc))
        {
            DEBUG_PRINT("getUpdates memoryUsage:");
            DEBUG_PRINTLN(doc.memoryUsage());
            DEBUG_PRINTJSON(doc);
            long long int id;
            if (JATBotObject::asUpdateId(doc, id))
            {
                update_id_ = ++id;
                return JATBotObject::asMessage(doc, msg);
            }
        }
    }
    return false;
}

bool JATBot::answerCallbackQuery(const char *query_id, const char *text,
                                 const bool alert)
{
    size_t text_len = strlen(text);
    size_t buf_len = 64 + text_len * 4;
    char *buf = new char[buf_len];

    strcpy_P(buf, str_answerCallBackQuery);
    strcat_P(buf, str_param_callback_query_id);
    strcat(buf, query_id);

    if (text_len)
    {
        strcat_P(buf, str_param_text);
        char *encoded = new char[text_len * 2];
        urlencode(encoded, text, text_len * 2);
        strcat(buf, encoded);
        delete[] encoded;
    }

    if (alert)
        strcat_P(buf, str_param_show_alert);

    request(buf);

    delete[] buf;
    return success();
}