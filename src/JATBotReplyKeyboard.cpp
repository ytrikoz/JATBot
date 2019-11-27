#include "JATBotMarkup.h"

void JATBotReplyKeyboard::addButton(ReplyKeyboardButton type, String caption) {
    switch (type) {
    case SIMPLE_BUTTON:
        addButton(caption);
    case CONTACT_BUTTON:
        addContactButton(caption);
        break;
    case LOCATION_BUTTON:
        addLocationButton(caption);
        break;
    default:
        break;
    }
}
void JATBotReplyKeyboard::addButton(const String &text) { addButton_(text); }

void JATBotReplyKeyboard::addLocationButton(const String &text) {
    addButton_(text, "request_location", true);
}

void JATBotReplyKeyboard::addContactButton(const String &text) {
    addButton_(text, "request_contact", true);
}

void JATBotReplyKeyboard::enableResize() { addParam("resize_keyboard", true); }

void JATBotReplyKeyboard::enableOneTime() {
    addParam("one_time_keyboard", true);
}

void JATBotReplyKeyboard::enableSelective() { addParam("selective", true); }