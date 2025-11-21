
#include "hardwere.h"
#include <string.h>

// void custom


void guiLenhAT(char *cmd)
{
    uart1_index = 0;
    memset((void*)uart1_dem, 0, MAX_SIZE); 


    UART_testchuoi(&BUFFER_AT, cmd);
    UART_testchuoi(&BUFFER_AT, "\r\n");
    handler_TX(AT, &BUFFER_AT);


    UART_testchuoi(&BUFFER_DEBUG, "Gui: ");
    UART_testchuoi(&BUFFER_DEBUG, cmd);
    UART_testchuoi(&BUFFER_DEBUG, "\r\n");
    handler_TX(DEBUG, &BUFFER_DEBUG);  


    Delay_ms(3000);


    if (uart1_index > 0) {
        UART_testchuoi(&BUFFER_DEBUG, "Nhan: ");
        handler_TX(DEBUG, &BUFFER_DEBUG);

        for (uint16_t i = 0; i < uart1_index; i++) {
            custom_SendByte(DEBUG, uart1_dem[i]);
        }
        
        UART_testchuoi(&BUFFER_DEBUG, "\r\n");
        handler_TX(DEBUG, &BUFFER_DEBUG);
    }
}


void gui_tinnhan(char *number, char *message)
{
    char cmd[100];

    UART_testchuoi(&BUFFER_DEBUG, "[SMS] Kiem tra trang thai mang...\r\n");
    handler_TX(DEBUG, &BUFFER_DEBUG);
    guiLenhAT("AT+CREG?");
    Delay_ms(1000);
    
    UART_testchuoi(&BUFFER_DEBUG, "[SMS] Chuyen che do text...\r\n");
    handler_TX(DEBUG, &BUFFER_DEBUG);
    guiLenhAT("AT+CMGF=1");
    Delay_ms(1000); 


    UART_testchuoi(&BUFFER_DEBUG, "[SMS] Bat dau gui tin nhan...\r\n");
    handler_TX(DEBUG, &BUFFER_DEBUG);

 
    uart1_index = 0;
    memset((void*)uart1_dem, 0, MAX_SIZE);

    snprintf(cmd, sizeof(cmd), "AT+CMGS=\"%s\"", number);
    guiLenhAT(cmd);  


    Delay_ms(3000); 

    uint8_t found_prompt = 0;
    for(uint16_t i = 0; i < uart1_index; i++) {
        if(uart1_dem[i] == '>') {
            found_prompt = 1;
            break;
        }
    }

    // if(found_prompt=0) {
    //     UART_testchuoi(&BUFFER_DEBUG, "[ERROR] Khong nhan duoc prompt '>'\r\n");
    //     handler_TX(DEBUG, &BUFFER_DEBUG);
        
    //     UART_testchuoi(&BUFFER_DEBUG, "Received: ");
    //     handler_TX(DEBUG, &BUFFER_DEBUG);
    //     for(uint16_t i = 0; i < uart1_index; i++) {
    //         custom_SendByte(DEBUG, uart1_dem[i]);
    //     }
    //     UART_testchuoi(&BUFFER_DEBUG, "\r\n");
    //     handler_TX(DEBUG, &BUFFER_DEBUG);
    //     return;
    // }

    UART_testchuoi(&BUFFER_DEBUG, "[SMS] Da nhan prompt '>'\r\n");
    handler_TX(DEBUG, &BUFFER_DEBUG);

    uart1_index = 0;
    memset((void*)uart1_dem, 0, MAX_SIZE);

    UART_testchuoi(&BUFFER_AT, message);
    handler_TX(AT, &BUFFER_AT);
    

    custom_SendByte(AT, 0x1A);

    UART_testchuoi(&BUFFER_DEBUG, "[SMS] Dang gui...\r\n");
    handler_TX(DEBUG, &BUFFER_DEBUG);

    Delay_ms(30000); 

    if (uart1_index > 0) {
        UART_testchuoi(&BUFFER_DEBUG, "[SMS] Ket qua: ");
        handler_TX(DEBUG, &BUFFER_DEBUG);
        
        for (uint16_t i = 0; i < uart1_index; i++) {
            custom_SendByte(DEBUG, uart1_dem[i]);
        }
        
        UART_testchuoi(&BUFFER_DEBUG, "\r\n");
        handler_TX(DEBUG, &BUFFER_DEBUG);

        if (strstr((char*)uart1_dem, "+CMGS:") != NULL || 
            strstr((char*)uart1_dem, "OK") != NULL) {
            UART_testchuoi(&BUFFER_DEBUG, "[SMS] GUI THANH CONG!\r\n");
            handler_TX(DEBUG, &BUFFER_DEBUG);
        } else if (strstr((char*)uart1_dem, "ERROR") != NULL) {
            UART_testchuoi(&BUFFER_DEBUG, "[SMS] GUI THAT BAI!\r\n");
            handler_TX(DEBUG, &BUFFER_DEBUG);
        }
    } else {
        UART_testchuoi(&BUFFER_DEBUG, "[ERROR] Khong nhan duoc ket qua!\r\n");
        handler_TX(DEBUG, &BUFFER_DEBUG);
    }
}
