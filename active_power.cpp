#include "active_power.h"
#include <ESP8266HTTPClient.h>
#include "date.h"

bool obterCaminhoActivePower(String &caminhoOut) {
  String dataFormatada;
  if (!obterDataAtual(dataFormatada, "-")) {
    return false;
  }

  caminhoOut = "/active-power/" + dataFormatada + ".json";
  return true;
}

bool extrairCampoFloatActivePower(const String& json, const char* chave, float& saida) {
  String padrao = "\"" + String(chave) + "\"";
  int idxChave = json.indexOf(padrao);
  if (idxChave < 0) return false;

  int idxDoisPontos = json.indexOf(':', idxChave + padrao.length());
  if (idxDoisPontos < 0) return false;

  int inicio = idxDoisPontos + 1;
  while (inicio < (int)json.length() &&
         (json[inicio] == ' ' || json[inicio] == '\n' || json[inicio] == '\r' || json[inicio] == '\t')) {
    inicio++;
  }

  int fim = inicio;
  while (fim < (int)json.length() &&
         ((json[fim] >= '0' && json[fim] <= '9') || json[fim] == '.' || json[fim] == '-')) {
    fim++;
  }

  if (fim <= inicio) return false;
  saida = json.substring(inicio, fim).toFloat();
  return true;
}

void atualizarActivePower() {
  String caminhoAtivo;
  if (!obterCaminhoActivePower(caminhoAtivo)) {
    logErro("NTP_TIME", "Data ainda nao sincronizada", -1);
    return;
  }

  String resposta;
  int code = firebaseReq("GET", caminhoAtivo.c_str(), "", resposta);
  if (code != HTTP_CODE_OK) {
    logErro("FIREBASE_GET", resposta, code);
    return;
  }

  resposta.trim();
  float meteringAtual = 0.0f;
  float consumoAtual = 0.0f;

  if (resposta.length() > 0 && resposta != "null") {
    if (!extrairCampoFloatActivePower(resposta, "energy_metering", meteringAtual) ||
        !extrairCampoFloatActivePower(resposta, "energy_consumption", consumoAtual)) {
      logErro("JSON_PARSE", resposta, -1);
      return;
    }
  }

  if (EP > meteringAtual) {
    float delta = EP - meteringAtual;
    float novoConsumo = consumoAtual + delta;

    String payload = "{\"energy_metering\":" + String(EP, 2) +
                     ",\"energy_consumption\":" + String(novoConsumo, 2) + "}";

    String putResposta;
    int putCode = firebaseReq("PUT", caminhoAtivo.c_str(), payload, putResposta);

    if (putCode <= 0 || putCode >= 400) {
      logErro("FIREBASE_PUT", putResposta, putCode);
    }
  }
}