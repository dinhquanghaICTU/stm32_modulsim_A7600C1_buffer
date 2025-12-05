#include "stm32f4xx_flash.h"
#include "stm32f4xx.h"
#include "drivers/flash_wite.h"
#include "services/uart_channel.h"
#include <string.h>

#define OTA_FIRMWARE_BASE_ADDR  0x08020000U
#define OTA_FIRMWARE_SECTOR     FLASH_Sector_5

static uint32_t flash_addr = OTA_FIRMWARE_BASE_ADDR; 
 
bool flash_writer_init(void){
    FLASH_Status status;
    
    FLASH_Unlock();
    
    FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_OPERR |FLASH_FLAG_WRPERR | FLASH_FLAG_PGAERR | FLASH_FLAG_PGPERR | FLASH_FLAG_PGSERR);
    
    status = FLASH_EraseSector(OTA_FIRMWARE_SECTOR,VoltageRange_3);
    
    if (status != FLASH_COMPLETE)
    {
        FLASH_Lock();
        return false;
    }

    flash_addr = OTA_FIRMWARE_BASE_ADDR;
    return true;

}


/* this func is wite flash  by byte */

bool flash_writer_write(const uint8_t *data, uint32_t len)
{
    FLASH_Status status;

    if (FLASH_GetStatus() == FLASH_BUSY)
    {
        uart_channel_send_str(UART_CH_DEBUG, "Flash dang ban\r\n");
        while (FLASH_GetStatus() == FLASH_BUSY);
    }

    FLASH_Unlock();

    
    for (uint32_t i = 0; i < len; i++)
    {
        status = FLASH_ProgramByte(flash_addr, data[i]);
        if (status != FLASH_COMPLETE)
        {
            uart_channel_send_format(UART_CH_DEBUG, "[FLASH] ghi loi: status=%d",status);
            return false;
        }
        flash_addr++;
    }

    return true;
}

void flash_writer_finish(void){
    FLASH_Lock();
}

uint32_t flash_writer_get_address(void){
    return flash_addr;
}