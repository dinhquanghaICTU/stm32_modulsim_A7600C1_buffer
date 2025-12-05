#ifndef __CRC32_H__
#define __CRC32_H__

#include <stdint.h>
#include <stddef.h>

void crc32_init(void);
uint32_t crc32_update(uint32_t crc, const uint8_t *data, size_t len);
void crc32_update_stream(const uint8_t *data, size_t len);  // Dùng global variable
uint32_t crc32_get(void);  // Lấy CRC final từ global variable
uint32_t crc32_final(uint32_t crc);
uint32_t crc32_calculate(const uint8_t *data, size_t len);

#endif //__CRC32_H__

