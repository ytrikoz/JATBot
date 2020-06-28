#include "JATBotMarkup.h"

bool JATBotInlineKeyboard::addButton(const InlineKeyboardButton type,
                                     const String text, const String param) {
    switch (type) {
    case URL_BUTTON:
        addUrlButton(text, param);
        break;
    case QUERY_BUTTON:
        addQueryButton(text, param);
        break;
    default:
        break;
    }
    return true;
}

void JATBotInlineKeyboard::addUrlButton(const String text, const String url) {
    addButton_(text, "url", url);
}

void JATBotInlineKeyboard::addQueryButton(const String text,
                                          const String callback) {
    addButton_(text, "callback_data", callback);
}