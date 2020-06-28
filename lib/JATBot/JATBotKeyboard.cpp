#include "JATBotMarkup.h"

JATBotMarkup::JATBotMarkup() {};

JATBotMarkup::JATBotMarkup(const char *name)
{
    name_ = name;
    init();
};

void JATBotMarkup::addRow()
{
    if (buttons_.size())
        buttons_ = rows_.createNestedArray();
}

void JATBotMarkup::clear()
{
    doc_.clear();
    init();
}

size_t JATBotMarkup::serialize(char *buf, size_t size)
{
    return serializeJson(doc_, buf, size);
}

JsonObject JATBotMarkup::addButton_(const String &text)
{
    JsonObject btn = buttons_.createNestedObject();
    char *encoded = new char[128];
    urlencode(encoded, text.c_str(), 128);
    btn[FPSTR(str_text)] = encoded;
    delete[] encoded;
    return btn;
}

JsonObject JATBotMarkup::addButton_(const String &text, const String &param,
                                      const bool value)
{
    JsonObject btn = addButton_(text);
    btn[param] = value;
    return btn;
}

JsonObject JATBotMarkup::addButton_(const String &text, const String &param,
                                      const String &value)
{
    JsonObject btn = addButton_(text);
    btn[param] = value;
    return btn;
}

void JATBotMarkup::addParam(const String &param, bool value) { doc_[param] = value; }

void JATBotMarkup::init()
{
    root_ = doc_.to<JsonObject>();
    rows_ = root_.createNestedArray(name_);
    buttons_ = rows_.createNestedArray();
}
