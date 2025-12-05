#ifndef __OTA_PORT_H__
#define __OTA_PORT_H__

#include <stdint.h>

#define OTA_SLOT_BASE_ADDR      (0x08040000UL)         
#define OTA_SLOT_SIZE_BYTES     (256UL * 1024UL)       
#define OTA_SLOT_END_ADDR       (OTA_SLOT_BASE_ADDR + OTA_SLOT_SIZE_BYTES)

#define OTA_AUTO_JUMP_AFTER_SUCCESS   0    

#endif /* __OTA_PORT_H__ */