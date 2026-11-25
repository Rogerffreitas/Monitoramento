#ifndef DATE_H
#define DATE_H

#include <Arduino.h>
#include <time.h>

void inicializarRelogio();
bool obterHoraLocalValida(struct tm &outTm);
bool obterDataAtual(String &dataOut, const char* separador = "-");
String obterDataHoraMinutoAtual();

#endif