#ifndef DDSU666_H
#define DDSU666_H

#include <Arduino.h>

#ifndef DDSU666_SLAVE_ID
#define DDSU666_SLAVE_ID 1
#endif

#ifndef DIR_PIN
#define DIR_PIN 4
#endif

extern float U;
extern float I;
extern float PKW;
extern float PF;
extern float EP;
extern float RE;
extern String ultimoErroModbus;

extern void logErro(const char* origem, const String &msg, int codigo);

void inicializarDDSU666();
uint8_t lerDDSU666();

#endif