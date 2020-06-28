#include "JATBotHttpClient.h"

#include <ESP8266HTTPClient.h>
#include <ESP8266WiFi.h>

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
        _http.setReuse(reuse);
        _http.forceReuse(reuse);
    }

    void begin(const char *host, const char *path) override {
        _client.setInsecure();
        _http.begin(_client, host, 443, path, true);
    }

    void end() override { _http.end(); }

    void addHeader(const String &name, const String &value) override {
        _http.addHeader(name.c_str(), value.c_str());
    }

    void collectHeaders(const char *header_keys[], const int count) override {
        _http.collectHeaders(header_keys, count);
    }

    String header(const String &name) override {
        return _http.header(name.c_str()).c_str();
    }

    int sendRequest(const char *method, const char *data) override {
        return _http.sendRequest(method, (uint8_t *)data, strlen(data));
    }

    Stream &getStream() override { return _http.getStream(); }

    String errorToString(int error) override {
        return HTTPClient::errorToString(error);
    }

    bool connected() override { return _http.connected(); }

   private:
    WiFiClientSecure _client;
    ForceReuseHTTPClient _http;
};

JATBotHttpClient *JATBotHttpClient::create() {
    return new JATBotHttpClientEsp8266();
}
