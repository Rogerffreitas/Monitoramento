#ifndef ACTIVE_POWER_H
#define ACTIVE_POWER_H

#include <Arduino.h>

// Dependencias definidas no sketch principal
extern float EP;
extern int firebaseReq(const char* method, const char* path, const String &payload, String &resposta);
extern void logErro(const char* origem, const String &msg, int codigo);

bool extrairCampoFloatActivePower(const String& json, const char* chave, float& saida);
void atualizarActivePower();

#endif