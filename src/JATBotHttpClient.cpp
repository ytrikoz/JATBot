#include "JATBotHttpClient.h"

#include <ESP8266HTTPClient.h>
#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>

class ForceReuseHTTPClient : public HTTPClient {
  public:
    void end() {
        if (_forceReuse)
            _canReuse = true;
        HTTPClient::end();
    }

    void forceReuse(bool forceReuse) { _forceReuse = forceReuse; }

  protected:
    bool _forceReuse = false;
};

class JATBotHttpClientEsp8266 : public JATBotHttpClient {
  public:
    JATBotHttpClientEsp8266() {}

    void setReuseConnection(bool reuse) override {
        http_.setReuse(reuse);
        http_.forceReuse(reuse);
    }

    void begin(const char *host, const char *path,
               const char fingerprint[20]) override {
        client_.setFingerprint(fingerprint);
        client_.setCiphersLessSecure();
        http_.begin(client_, host, 443, path, true);
    }

    void end() override { http_.end(); }

    void addHeader(const String &name, const String &value) override {
        http_.addHeader(name.c_str(), value.c_str());
    }

    void collectHeaders(const char *header_keys[], const int count) override {
        http_.collectHeaders(header_keys, count);
    }

    String header(const String &name) override {
        return http_.header(name.c_str()).c_str();
    }

    int sendRequest(const char *method, const char *data) override {
        return http_.sendRequest(method, (uint8_t *)data, strlen(data));
    }

    Stream &getStream() override { return http_.getStream(); }

    String errorToString(int error) override {
        return HTTPClient::errorToString(error);
    }

    bool connected() override { return http_.connected(); }

  private:
    WiFiClientSecure client_;
    ForceReuseHTTPClient http_;
};

JATBotHttpClient *JATBotHttpClient::create() {
    return new JATBotHttpClientEsp8266();
}
