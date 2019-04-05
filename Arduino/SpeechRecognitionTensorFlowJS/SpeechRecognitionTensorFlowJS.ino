#define SKETCH_VERSION "1.0.0"

#include <ESPAsyncWiFiManager.h> //https://github.com/alanswx/ESPAsyncWiFiManager
#include <ESPAsyncWebServer.h>   // https://github.com/me-no-dev/ESPAsyncWebServer
#include <SPIFFSEditor.h>

#include <DNSServer.h>
#include "httpFiles.h"

#define HOSTNAME "ESPTFJSSpeach"
#define ledPin LED_BUILTIN          //GPIO16
char AP_PASS[32] = "esptfjsspeech"; // AP Password
const String http_username = "admin";
const String http_password = "esptfjsspeech";

char NameChipId[64] = {0}, chipId[9] = {0};
IPAddress apIP(192, 168, 4, 1);
const byte DNS_PORT = 53;
bool shouldReboot = false;
String ledState;

AsyncWebServer server(80);
AsyncWebSocket ws("/ws");
DNSServer dns;

String processor(const String &var)
{
    Serial.print(var);
    if (var == "STATE")
    {
        if (!digitalRead(ledPin))
            ledState = "ON";
        else
            ledState = "OFF";
        Serial.print(": ");
        Serial.println(ledState);
        return ledState;
    }
    return String();
}

bool processRequest(String state)
{
    if (state == "ON")
    {
        digitalWrite(ledPin, LOW);
        return true;
    }
    else if (state == "OFF")
    {
        digitalWrite(ledPin, HIGH);
        return true;
    }
    return false;
}

#include "WSServerHelper.h"

void setup()
{
    Serial.begin(115200);
    delay(10);
    pinMode(ledPin, OUTPUT);
    digitalWrite(ledPin, LOW);

    Serial.println(F("---------------------------"));
    Serial.println(F("Starting SPIFFs"));

    //SPIFFS.format();
    if (SPIFFS.begin())
    {
        Dir dir = SPIFFS.openDir("/");
        while (dir.next())
        {
            String fileName = dir.fileName();
            size_t fileSize = dir.fileSize();
            if (Serial)
                Serial.printf("FS File: %s, size: %dB\n", fileName.c_str(), fileSize);
        }

        FSInfo fs_info;
        SPIFFS.info(fs_info);
        if (Serial)
        {
            Serial.printf("FS Usage: %d/%d bytes\n", fs_info.usedBytes, fs_info.totalBytes);
            Serial.println(F("SPIFFs started"));
            Serial.println(F("---------------------------"));
        }
    }

    snprintf(chipId, sizeof(chipId), "%08x", (uint32_t)ESP.getChipId());
    snprintf(NameChipId, sizeof(NameChipId), "%s_%08x", HOSTNAME, (uint32_t)ESP.getChipId());
    WiFi.hostname(const_cast<char *>(NameChipId));

    WiFi.mode(WIFI_AP_STA);
    AsyncWiFiManager wm(&server, &dns);
    bool res = wm.autoConnect(NameChipId);
    if (!res)
    {
        Serial.println(F("Failed to connect and hit timeout"));
    }
    else
    {
        Serial.println(F("---------------------------------------"));
        Serial.print(F("Router IP: "));
        Serial.println(WiFi.localIP());
    }

    Serial.println(F("---------------------------------------"));

    WiFi.softAPConfig(apIP, apIP, IPAddress(255, 255, 255, 0));
    WiFi.softAP(NameChipId, AP_PASS);
    Serial.print(F("HotSpt IP: "));
    Serial.println(WiFi.softAPIP());
    dns.start(DNS_PORT, "*", WiFi.softAPIP());

    Serial.print(F("Async HTTP server starting "));
    ws.onEvent(onWsEvent);
    server.addHandler(&ws);
    server.addHandler(new SPIFFSEditor(http_username, http_password));
    server.on("/heap", HTTP_GET, [](AsyncWebServerRequest *request) {
        request->send(200, "text/plain", String(ESP.getFreeHeap()));
    });
    Serial.print(F("."));
    server.on("/on", HTTP_GET, [](AsyncWebServerRequest *request) {
        processRequest("ON");
        request->send(SPIFFS, "/index.htm", String(), false, processor);
    });
    Serial.print(F("."));
    server.on("/off", HTTP_GET, [](AsyncWebServerRequest *request) {
        processRequest("OFF");
        request->send(SPIFFS, "/index.htm", String(), false, processor);
    });
    Serial.print(F("."));
    server.on("/upload", HTTP_GET, handleUpload);
    Serial.print(F("."));
    server.on("/upload", HTTP_POST, processUploadReply, processUpload);
    Serial.print(F("."));
    server.on("/update", HTTP_GET, handleUpdate);
    Serial.print(F("."));
    server.on("/update", HTTP_POST, processUpdateReply, processUpdate);
    Serial.print(F("."));
    server.on("/", HTTP_GET, handleRoot);
    Serial.print(F("."));
    server.on("/style.css", HTTP_GET, [](AsyncWebServerRequest *request) {
        request->send(SPIFFS, "/style.css","text/css");
    });
    Serial.print(F("."));
    server.on("/speech-commands.min.js", HTTP_GET, [](AsyncWebServerRequest *request) {
        request->send(SPIFFS, "/speech-commands.min.js", "application/javascript");
    });
    Serial.print(F("."));
    server.on("/tf.min.js", HTTP_GET, [](AsyncWebServerRequest *request) {
        request->send(SPIFFS, "/tf.min.js", "application/javascript");
    });
    Serial.print(F("."));
    server.on("/scipt.js", HTTP_GET, [](AsyncWebServerRequest *request) {
        request->send(SPIFFS, "/script.js", "application/javascript");
    });
    Serial.print(F("."));
    server.on("/version", HTTP_GET, [](AsyncWebServerRequest *request) {
        request->send(200, "text/plain", SKETCH_VERSION);
    });
    Serial.print(F("."));
    server.on("/reset_wifi", HTTP_GET, [&](AsyncWebServerRequest *request) {
        Serial.println(F("/reset_wlan"));
        request->send(200, "text/html", "<META http-equiv='refresh' content='15;URL=/'><body align=center><H4>Resetting WLAN and restarting...</H4></body>");
        AsyncWiFiManager wm(&server, &dns);
        wm.resetSettings();
        shouldReboot = true;
    });
    Serial.print(F("."));
    server.on("/restart", HTTP_GET, [&](AsyncWebServerRequest *request) {
        Serial.println(F("/restart"));
        request->send(200, "text/html", "<META http-equiv='refresh' content='15;URL=/'><body align=center><H4>Restarting...</H4></body>");
        shouldReboot = true;
    });
    Serial.print(F("."));
    server.onNotFound(handleNotFound);
    Serial.print(F("."));
    DefaultHeaders::Instance().addHeader("Access-Control-Allow-Origin", "*");
    server.begin();
    Serial.println(F(" done!"));
}

void loop()
{
    if (shouldReboot)
    {
        Serial.println(F("Rebooting..."));
        delay(100);
        ESP.restart();
    }
}
