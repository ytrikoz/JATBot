#include "main.h"

#include <Arduino.h>
#include <ESP8266WiFi.h>

#include "JATBot.h"

#define POWER_LED_PIN D1
#define WIFI_LED_PIN D4

#define LIGHT_ON_CALLBACK "lightON"
#define LIGHT_OFF_CALLBACK "lightOFF"

bool wifiConnect(uint8_t mode, const char *ssid, const char *password);

char *getLocationStr(char *buf, const TLocation &item);
char *getContactStr(char *buf, const TContact &item);

char* getSketchSize(char *buf);

void handleText(const TMessage &msg);
void handleOther(const TMessage &msg);
void handleQuery(const TMessage &msg);

void fillReplyKeyboard(JATBotReplyKeyboard *kb);
void fillInlineKeyboard(JATBotInlineKeyboard *kb);

class HeapInfo {
  public:
    HeapInfo() { set(); }

    void set() { ESP.getHeapStats(&start_free_, nullptr, &start_frag_); }

    void now() { ESP.getHeapStats(&free_, nullptr, &frag_); }

    char *getDiff(char *buf) {
        now();
        sint16_t diff_free = free_ - start_free_;
        sint8_t diff_frag = frag_ - start_frag_;
        sprintf(buf, "Diff: %d (%d%%)", diff_free, diff_frag);
        return buf;
    }

    char *getInfo(char *buf) {
        now();
        sprintf(buf, "Free: %d (%d%%)", free_, frag_);
        return buf;
    }

    void printDiff(Print &p) {
        char buf[32];
        p.println(getDiff(buf));
    }

    void printInfo(Print &p) {
        char buf[32];
        p.println(getInfo(buf));
    }

  private:
    uint32_t start_free_, free_;
    uint8_t start_frag_, frag_;
};

Print *out;
JATBot *bot;
unsigned long lastUpdate = 0;
HeapInfo heap;

void setup() {
    Serial.begin(115200);
    out = &Serial;
    out->println();

    pinMode(WIFI_LED_PIN, OUTPUT);
    pinMode(POWER_LED_PIN, OUTPUT);

    digitalWrite(WIFI_LED_PIN, HIGH);
    digitalWrite(POWER_LED_PIN, HIGH);

    if (!wifiConnect(WIFI_MODE, WIFI_SSID, WIFI_PASS)) {
        out->println(F("Unable connect to network! Exit."));
        return;
    }

    out->println();
    out->print(F("Got IP: "));
    out->println(WiFi.localIP().toString());
    delay(1000);
    bot = new JATBot(API_TOKEN);
    if (!bot->testConnection()) {
        out->println(F("Unable connect to API! Exit."));
        return;
    }
    out->println(F("Сonnected to API!"));
    heap.set();
}

void loop() {
    unsigned long now = millis();
    if (!bot->ready() || (now - lastUpdate < API_PULL_INTERVAL))
        return;
    lastUpdate = now;
    TMessage msg;
    if (bot->getUpdates(msg)) {
        heap.printDiff(*out);
        switch (msg.type) {
        case MESSAGE_TEXT:
            if (msg.hasContact) {
                char buf[128];
                getContactStr(buf, msg.contact);
                bot->sendMessage(msg.from.id, buf);
            } else if (msg.hasLocation) {
                char buf[64];
                getLocationStr(buf, msg.location);
                bot->sendMessage(msg.from.id, buf);
            } else {
                handleText(msg);
            }
            break;
        case MESSAGE_QUERY:
            handleQuery(msg);
            break;
        default:
            break;
        }
    }
}

void fillInlineKeyboard(JATBotInlineKeyboard *kb) {
    kb->addQueryButton("Light ON", LIGHT_ON_CALLBACK);
    kb->addQueryButton("Light OFF", LIGHT_OFF_CALLBACK);
    kb->addUrlButton("google.com", "https://google.com");
}

void fillReplyKeyboard(JATBotReplyKeyboard *kb) {
    kb->enableResize();
    kb->addButton("Light ON");
    kb->addButton("Uptime");
    kb->addButton("Light OFF");
    kb->addRow();
    kb->addContactButton("Contact");
    kb->addButton("Inline");
    kb->addLocationButton("Location");
    kb->addRow();
    kb->addButton("Free");
    kb->addButton("Size");
    kb->addButton("Version");
    kb->addRow();
    kb->addButton("Hide");
}

void handleQuery(const TMessage &msg) {
    if (msg.queryData.equalsIgnoreCase(LIGHT_ON_CALLBACK)) {
        digitalWrite(WIFI_LED_PIN, LOW);
        bot->answerCallbackQuery(msg.queryId.c_str(), LIGHT_ON_CALLBACK, true);
    } else if (msg.queryData.equalsIgnoreCase(LIGHT_OFF_CALLBACK)) {
        digitalWrite(WIFI_LED_PIN, LOW);
        bot->answerCallbackQuery(msg.queryId.c_str(), LIGHT_OFF_CALLBACK);
    }
}

char *getLocationStr(char *buf, const TLocation &item) {
    sprintf_P(buf, PSTR("Longitude: %f\nLatitude: %f"), item.longitude,
              item.latitude);
    return buf;
}

char *getContactStr(char *buf, const TContact &item) {
    sprintf_P(buf, PSTR("Id: %s First: %s Last: %s Phone: %s vCard: %s"),
              item.user_id.c_str(), item.first_name.c_str(),
              item.last_name.c_str(), item.phone_number.c_str(),
              item.vcard.c_str());
    return buf;
}

char *getVersionStr(char *buf) {
    sprintf_P(buf, PSTR("SDK: %s\nCore: %s\nArduinoJson: %s\nJATBot: %s"),
              ESP.getSdkVersion(), ESP.getCoreVersion().c_str(),
              ARDUINOJSON_VERSION, JATBOT_VERSION);
    return buf;
}

char* getSketchSize(char *buf) {
    return strcpy_P(buf, PSTR("RAM: 36.1% (29572)\nROM: 39.3% (410608)"));
}


void handleText(const TMessage &msg) {
    char buf[128];  
    if (msg == "Version") {
        /*
         * VERSION
         */
        getVersionStr(buf);
    } else if (msg == "Size") {
        /*
         * SIZE
         */
        getSketchSize(buf);
    } else if (msg == "Free") {
        /*
         * FREE
         */
        heap.getInfo(buf);
    } else if (msg == "Inline") {
        /*
         * INLINE
         */
        fillInlineKeyboard(bot->showInlineKeyboard());
        strcpy_P(buf, PSTR("Whatever you wish!"));
    } else if (msg == "Show") {
        /*
         * SHOW 
         */
        fillReplyKeyboard(bot->showReplyKeyboard());
        strcpy_P(buf, PSTR("Your orders have weight!"));
    } else if (msg == "Hide") {
        /*
         * HIDE
         */
        bot->hideReplyKeyboard();
        strcpy_P(buf, PSTR("Hide away"));
    } else if (msg == "light on") {
        /*
         * LIGHT ON
         */
        strcpy_P(buf,  PSTR("Light is On"));
        digitalWrite(WIFI_LED_PIN, LOW);
    } else if (msg == "light off") {
        /*
         * LIGHT OFF
         */
        strcpy_P(buf,  PSTR("Light is Off"));
        digitalWrite(WIFI_LED_PIN, HIGH);
    } else if (msg == "Uptime") {
        /*
         * UPTIME
         */
         sprintf_P(buf,  "Uptime %lu sec",  millis() / 1000);
    } else {
        /*
         * Welcome
         */
        sprintf_P(buf, PSTR("Привет, %s!\nПопробуй еще этих французких булок или напиши 'show' или 'inline'."),
                  msg.from.username.c_str());
        return;
    }

    bot->sendMessage(msg.from.id, buf);
}

void handleOther(const TMessage &msg) {
    bot->sendMessage(msg.from.id, "Unsuported message");
}

bool wifiConnect(uint8_t mode, const char *ssid, const char *password) {
    switch (mode) {
    case NETWORK_STA:
        WiFi.mode(WIFI_STA);
        break;
    case NETWORK_AP_STA:
        WiFi.mode(WIFI_AP_STA);
        break;
    default:
        return false;
        break;
    }
    delay(100);

    WiFi.begin(ssid, password);
    delay(100);

    size_t tries = 0;
    while ((WiFi.status() != WL_CONNECTED) &&
           ((tries++ < WIFI_MAX_TRIES * 10) || WIFI_MAX_TRIES == 0)) {
        if (!(tries % 10))
            out->print('.');
        delay(100);
    }
    return WiFi.isConnected();
}
