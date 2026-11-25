#include "date.h"

static constexpr long FUSO_HORARIO_SEGUNDOS = -3L * 3600L;
static constexpr time_t TIMESTAMP_MINIMO_VALIDO = 1700000000L;

void inicializarRelogio() {
  configTime(FUSO_HORARIO_SEGUNDOS, 0, "pool.ntp.org", "time.nist.gov", "br.pool.ntp.org");
}

bool obterHoraLocalValida(struct tm &outTm) {
  time_t agora = time(nullptr);
  if (agora < TIMESTAMP_MINIMO_VALIDO) {
    return false;
  }
  localtime_r(&agora, &outTm);
  return true;
}

bool obterDataAtual(String &dataOut, const char* separador) {
  struct tm infoTempo;
  if (!obterHoraLocalValida(infoTempo)) {
    return false;
  }

  char buf[16];
  snprintf(buf, sizeof(buf), "%02d%s%02d%s%04d",
           infoTempo.tm_mday,
           separador,
           infoTempo.tm_mon + 1,
           separador,
           infoTempo.tm_year + 1900);
  dataOut = String(buf);
  return true;
}

String obterDataHoraMinutoAtual() {
  struct tm infoTempo;
  if (!obterHoraLocalValida(infoTempo)) {
    return "aguardando_ntp";
  }

  char buf[20];
  snprintf(buf, sizeof(buf), "%02d/%02d/%04d %02d:%02d",
           infoTempo.tm_mday,
           infoTempo.tm_mon + 1,
           infoTempo.tm_year + 1900,
           infoTempo.tm_hour,
           infoTempo.tm_min);
  return String(buf);
}