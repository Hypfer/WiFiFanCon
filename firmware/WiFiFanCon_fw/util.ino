#include <stdint.h>
#include <Esp.h>

uint32_t getChipId() {
  uint32_t id = 0;
  for (int i = 0; i < 17; i = i + 8) {
    id |= ((ESP.getEfuseMac() >> (40 - i)) & 0xff) << i;
  }

  return id;
}
