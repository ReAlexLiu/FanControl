#include "config.h"

#include "ds18b20.h"
#include "fun_control.h"
#include "ota.h"
#include "wifi_config.h"
#include "index.h"

#include <ESP8266WebServer.h>

ESP8266WebServer http_server(80);

void             setup()
{
    Serial.begin(9600);

    esp_utility::config::getInstance().load();

#ifdef ENABLE_WIFI
    esp_utility::wifi_config::getInstance().begin(http_server);
#endif

#ifdef ENABLE_DS18B20
    esp_utility::ds18b20::getInstance().begin(http_server);
#endif

#ifdef ENABLE_OTA
    esp_utility::ota::getInstance().begin(http_server);
#endif

#ifdef ENABLE_FAN
    esp_utility::fun_control::getInstance().begin(http_server);
#endif

    // 设置Web服务器
    http_server.on("/", HTTP_GET, [&]() {
        http_server.send(200, "text/html", esp_utility::index_html);
    });

    // 处理所有未定义的路径，重定向到配网页面（captive portal）
    http_server.onNotFound([&]() {
        // 处理未找到的路径（用于captive portal）
        // 重定向所有请求到配网页面
        http_server.sendHeader("Location", "/", true);
        http_server.send(302, "text/plain", "");
    });

    http_server.begin();
}

void loop()
{
#ifdef ENABLE_DS18B20
    esp_utility::ds18b20::getInstance().update();
#endif

#ifdef ENABLE_OTA
    esp_utility::ota::getInstance().update();
#endif

#ifdef ENABLE_FAN
    esp_utility::fun_control::getInstance().update();
#endif

#ifdef ENABLE_WIFI
    esp_utility::wifi_config::getInstance().update();
#endif

    http_server.handleClient();

    delay(100);
}
