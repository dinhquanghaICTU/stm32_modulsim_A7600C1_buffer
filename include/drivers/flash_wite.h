#ifndef __FLASH_WITE_H__
#define __FLASH_WITE_H__
#include <stdbool.h>
#include <stdint.h>

bool flash_writer_init(void);
bool flash_writer_write(const uint8_t *data, uint32_t len);
void flash_writer_finish(void);
uint32_t flash_writer_get_address(void);

#endif //__FLASH_WITE_H__