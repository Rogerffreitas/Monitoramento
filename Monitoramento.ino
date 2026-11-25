#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClientSecure.h>
#include "ota.h"
#include "ddsu666.h"
#include "active_power.h"
#include "date.h"

const char* FIREBASE_HOST = "ddsu666-default-rtdb.firebaseio.com";
const char* PATH_MONITOR  = "/monitoramento-energia.json";
const char* PATH_LOGS     = "/logs-erros.json";

float U = 0.0f, I = 0.0f, PKW = 0.0f, PF = 0.0f, EP = 0.0f, RE = 0.0f, activePowerFirebase = 0.0f;
String ultimoErroModbus = "";

unsigned long ultimoEnvio = 0;
const unsigned long intervaloEnvio = 100000;

String escapeJson(String s) {
  s.replace("\\", "\\\\");
  s.replace("\"", "\\\"");
  s.replace("\n", " ");
  s.replace("\r", " ");
  return s;
}

int firebaseReq(const char* method, const char* path, const String &payload, String &resposta) {
  resposta = "";
  if (WiFi.status() != WL_CONNECTED) return -1;

  WiFiClientSecure client;
  client.setInsecure();
  client.setTimeout(10000);

  HTTPClient https;
  String url = String("https://") + FIREBASE_HOST + path;
  if (!https.begin(client, url)) return -1;

  if (payload.length() > 0) https.addHeader("Content-Type", "application/json");

  int code = (String(method) == "GET") ? https.GET() : https.sendRequest(method, payload);
  resposta = (code > 0) ? https.getString() : https.errorToString(code);
  https.end();
  yield();
  return code;
}

void logErro(const char* origem, const String &msg, int codigo = 0) {
  String resp;
  String body = "{\"origem\":\"" + String(origem) + "\",\"mensagem\":\"" + escapeJson(msg) +
                "\",\"codigo\":" + String(codigo) + ",\"free_heap\":" + String(ESP.getFreeHeap()) +
                ",\"uptime_ms\":" + String(millis()) + "}";
  firebaseReq("PUT", PATH_LOGS, body, resp);
}

void enviarMonitoramento(uint8_t leiturasOk) {
  String resp;
  String ultimaAtualizacao = obterDataHoraMinutoAtual();
  String body = "{\"tensao_v\":" + String(U, 2) +
              ",\"corrente_a\":" + String(I, 3) +
              ",\"potencia_ativa_kw\":" + String(PKW, 3) +
              ",\"fator_potencia\":" + String(PF, 3) +
              ",\"active_power\":" + String(EP, 2) +
              ",\"reverse_power\":" + String(RE, 2) +
              ",\"leituras_ok\":" + String(leiturasOk) +
              ",\"ultima_atualizacao\":\"" + escapeJson(ultimaAtualizacao) + "\"" +
              ",\"ultimo_erro\":\"" + escapeJson(ultimoErroModbus) + "\"" +
              ",\"free_heap\":" + String(ESP.getFreeHeap()) +
              ",\"uptime_ms\":" + String(millis()) + "}";

  int code = firebaseReq("PATCH", PATH_MONITOR, body, resp);
  if (code <= 0 || code >= 400) {
    logErro("FIREBASE_HTTP", "PATCH falhou: " + resp, code);
  }
}

void setup() {
  inicializarDDSU666();

  setupOtaWifi();
  if (WiFi.status() != WL_CONNECTED) {
    logErro("WIFI", "WiFi nao conectado no setup", WiFi.status());
  }
  inicializarRelogio();
}

void loop() {
  ArduinoOTA.handle();
  yield();

  if (millis() - ultimoEnvio >= intervaloEnvio) {
    ultimoEnvio = millis();
    if (WiFi.status() != WL_CONNECTED) return;

    uint8_t ok = lerDDSU666();
    atualizarActivePower();
    enviarMonitoramento(ok);
  }
}