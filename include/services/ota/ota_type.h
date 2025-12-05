   // ota_types.h
   #ifndef __OTA_TYPES_H__
   #define __OTA_TYPES_H__

   typedef enum
   {
       OTA_ERROR_NONE = 0,
       OTA_ERROR_BUSY,
       OTA_ERROR_DOWNLOAD_FAIL,
       OTA_ERROR_FLASH_WRITE,
       OTA_ERROR_VERIFY_FAIL,
       OTA_ERROR_INVALID_SIZE,
   } ota_error_t;

   #endif