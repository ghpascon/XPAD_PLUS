#include "vars.h"
#include "script_all.h"
#include "script_ant_config.h"
#include "script_one_ant_config.h"
#include "script_reader_config.h"
#include "script_reader.h"
#include "script_reader_modes.h"
#include "script_gpo_test.h"
#include "script_last_packs.h"

class WEB_SERVER
{
public:
    void setup()
    {
        if (!hotspot_on)
            return;
        config_web_server();
        style_web_server();
        routes_web_server();
        script_web_server();
        server.begin();
    }

    void config_web_server()
    {
        const String ssid = get_esp_name();
        const char *password = "smartx12345";
        WiFi.softAP(ssid.c_str(), password);
    }

    void style_web_server()
    {
        server.on("/style.css", HTTP_GET, [](AsyncWebServerRequest *request)
                  { request->send(200, "text/css", style()); });
    }

    void routes_web_server()
    {
        server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
                  { request->send(200, "text/html", index_page()); });

        // server.on("/reader", HTTP_GET, [](AsyncWebServerRequest *request)
        //           { request->send(LittleFS, "/html/reader.html", "text/html"); });

        server.on("/ant_config", HTTP_GET, [](AsyncWebServerRequest *request)
                  { request->send(200, "text/html", one_ant ? one_ant_config_page() : ant_config_page()); });

        // server.on("/reader_config", HTTP_GET, [](AsyncWebServerRequest *request)
        //           { request->send(LittleFS, "/html/reader_config.html", "text/html"); });

        // server.on("/reader_modes", HTTP_GET, [](AsyncWebServerRequest *request)
        //           { request->send(LittleFS, "/html/reader_modes.html", "text/html"); });

        // server.on("/gpo_test", HTTP_GET, [](AsyncWebServerRequest *request)
        //           { request->send(LittleFS, "/html/gpo_test.html", "text/html"); });

        // server.on("/last_packs", HTTP_GET, [](AsyncWebServerRequest *request)
        //           { request->send(LittleFS, "/html/last_packs.html", "text/html"); });
    }
    void script_web_server()
    {
        all_script();
        reader_script();
        config_ant_script();
        config_one_ant_script();
        config_reader_script();
        reader_modes_script();
        gpo_test_script();
        last_packs_script();
    }
};