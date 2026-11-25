#ifndef OTA_H
#define OTA_H

#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>

void setupOtaWifi() {
  WiFi.mode(WIFI_STA);

  WiFiManager wifiManager;
  // Desativa logs seriais do WiFiManager para não enviar texto no barramento RS485
  wifiManager.setDebugOutput(false);

  // Abre o portal "MEDIDOR-DDSU" caso não encontre uma rede salva
  if (!wifiManager.autoConnect("MEDIDOR-DDSU")) {
    delay(3000);
    ESP.restart();
  }

  ArduinoOTA.setHostname("medidor-ddsu");
  ArduinoOTA.begin();
}

#endif
