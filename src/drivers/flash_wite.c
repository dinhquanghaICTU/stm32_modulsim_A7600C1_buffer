#include "stm32f4xx_flash.h"
#include "stm32f4xx.h"
#include "drivers/flash_wite.h"

#define OTA_FIRMWARE_BASE_ADDR  0x08020000U
#define OTA_FIRMWARE_SECTOR     FLASH_Sector_5

static uint32_t flash_addr = OTA_FIRMWARE_BASE_ADDR; 
 
bool flash_writer_init(void){
    FLASH_Status status;
    
    //unclock flash
    FLASH_Unlock();
    //delete flag before wite
    FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_OPERR |FLASH_FLAG_WRPERR | FLASH_FLAG_PGAERR | FLASH_FLAG_PGPERR | FLASH_FLAG_PGSERR);
    // delete sector before wite
    status = FLASH_EraseSector(OTA_FIRMWARE_SECTOR,VoltageRange_3);
    // lock when error
    if (status != FLASH_COMPLETE)
    {
        FLASH_Lock();
        return false;
    }

    //move addr flash to firt
    flash_addr = OTA_FIRMWARE_BASE_ADDR;
    return true;

}

bool flash_writer_write(const uint8_t *data, uint32_t len)
{
    FLASH_Status status;
    
    //write byte  main is word for speed 
    uint32_t words = len / 4;
    uint32_t *p_words = (uint32_t*)data;
    
    for (uint32_t i = 0; i < words; i++)
    {
        status = FLASH_ProgramWord(flash_addr, p_words[i]);
        if (status != FLASH_COMPLETE)
            return false;
        flash_addr += 4;
    }
    
    // write redundant for byte 
    uint32_t remainder_start = words * 4;
    for (uint32_t i = remainder_start; i < len; i++)
    {
        status = FLASH_ProgramByte(flash_addr, data[i]);
        if (status != FLASH_COMPLETE)
            return false;
        flash_addr += 1;
    }
    
    return true;


    // use hybrid will write faster than byte and word defauld
}
void flash_writer_finish(void){
    FLASH_Lock();
}

uint32_t flash_writer_get_address(void){
    return flash_addr;
}