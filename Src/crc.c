#include "crc.h"

uint32_t CRC32_Digest(const uint8_t* data, const size_t size) {
   uint8_t byte;
   uint32_t crc = 0xffffffff;
   uint32_t mask;

   for (size_t i = 0; i < size; i++) {
      byte = data[i];
      crc = crc ^ byte;

      for (uint8_t j = 0; j < 8; j++) {
         mask = -(crc & 1);
         crc = (crc >> 1) ^ (0xedb88320 & mask);
      }
   }

   return ~crc;
}
