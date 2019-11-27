#include "JATBotMarkup.h"

JATBotHideKeyboard::JATBotHideKeyboard() : JATBotMarkup()
{
  doc_["remove_keyboard"] = true;
};
