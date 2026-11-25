#include "ddsu666.h"
#include <ModbusMaster.h>
#include <string.h>

struct RegConfig {
  uint16_t addr;
  const char* nome;
  float* destino;
};

static ModbusMaster node;

static RegConfig regsDDSU[] = {
  {0x2000, "U", &U},
  {0x2002, "I", &I},
  {0x2004, "PKW", &PKW},
  {0x200A, "PF", &PF},
  {0x2006, "EP", &EP},
  {0x400A, "RE", &RE}
};

static void preTransmission() {
#ifdef DIR_PIN
  digitalWrite(DIR_PIN, HIGH);
  delayMicroseconds(50);
#endif
}

static void postTransmission() {
#ifdef DIR_PIN
  delayMicroseconds(50);
  digitalWrite(DIR_PIN, LOW);
#endif
}

static float regsToFloat(uint16_t hi, uint16_t lo) {
  uint32_t raw = ((uint32_t)hi << 16) | lo;
  float v = 0.0f;
  memcpy(&v, &raw, sizeof(v));
  return v;
}

void inicializarDDSU666() {
#ifdef DIR_PIN
  pinMode(DIR_PIN, OUTPUT);
  digitalWrite(DIR_PIN, LOW);
#endif

  Serial.begin(9600);
  node.begin(DDSU666_SLAVE_ID, Serial);
  node.preTransmission(preTransmission);
  node.postTransmission(postTransmission);
}

uint8_t lerDDSU666() {
  uint8_t okCount = 0;
  ultimoErroModbus = "";

  for (auto &r : regsDDSU) {
    *r.destino = 0.0f;
    uint8_t res = node.readHoldingRegisters(r.addr, 2);
    if (res == node.ku8MBSuccess) {
      *r.destino = regsToFloat(node.getResponseBuffer(0), node.getResponseBuffer(1));
      okCount++;
    } else { 
      ultimoErroModbus = String("Falha ") + r.nome + " (0x" + String(r.addr, HEX) + ") err=0x" + String(res, HEX);
      logErro("MODBUS", ultimoErroModbus, res);
    }
    yield();
  }
  return okCount;
}