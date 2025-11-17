
#include "hardwere.h"
#include <string.h>

void guiLenhAT(char *cmd)
{
    // Reset buffer
    uart1_index = 0;
    memset((void*)uart1_dem, 0, MAX_SIZE);

    // Gửi lệnh AT
    UART_testchuoi(USART1, cmd);
    UART_testchuoi(USART1, "\r\n");

    // Echo ra DEBUG
    UART_testchuoi(DEBUG, "Gui: ");
    UART_testchuoi(DEBUG, cmd);
    UART_testchuoi(DEBUG, "\r\n");

    // Đợi phản hồi
    Delay_ms(2000);

    // In phản hồi
    if (uart1_index > 0)
    {
        UART_testchuoi(DEBUG, "Nhan: ");
        for (uint16_t i = 0; i < uart1_index; i++)
            custom_SendByte(DEBUG, uart1_dem[i]);
        UART_testchuoi(DEBUG, "\r\n");
    }
    else
    {
        UART_testchuoi(DEBUG, "Khong nhan duoc phan hoi!\r\n");
    }
}

void test_tinnhan(char *number, char *message)
{
    char cmd[100];

    UART_testchuoi(DEBUG, "--- BAT DAU GUI SMS ---\r\n");

    // Kiểm tra mạng TRƯỚC KHI gửi SMS
    guiLenhAT("AT+CREG?");
    Delay_ms(1000);

    // Chế độ text
    guiLenhAT("AT+CMGF=1");
    Delay_ms(1000); // Tăng từ 500ms

    // Reset buffer
    uart1_index = 0;
    memset((void*)uart1_dem, 0, MAX_SIZE);

    // Gửi lệnh AT+CMGS
    snprintf(cmd, sizeof(cmd), "AT+CMGS=\"%s\"", number);
    UART_testchuoi(USART1, cmd);
    UART_testchuoi(USART1, "\r\n");

    UART_testchuoi(DEBUG, "Gui lenh: ");
    UART_testchuoi(DEBUG, cmd);
    UART_testchuoi(DEBUG, "\r\n");

    // Đợi prompt '>' (tăng timeout)
    Delay_ms(3000); // Tăng từ 2000ms

    // Kiểm tra có nhận được '>' không
    uint8_t found_prompt = 0;
    for(uint16_t i = 0; i < uart1_index; i++) {
        if(uart1_dem[i] == '>') {
            found_prompt = 1;
            break;
        }
    }

    if(!found_prompt) {
        UART_testchuoi(DEBUG, "Loi: Khong nhan duoc prompt '>'\r\n");
        return;
    }

    UART_testchuoi(DEBUG, "Da nhan prompt '>'\r\n");

    // Reset buffer
    uart1_index = 0;

    // Gửi nội dung tin nhắn
    UART_testchuoi(USART1, message);
    UART_testchuoi(USART1, "\x1A"); // Ctrl+Z

    UART_testchuoi(DEBUG, "Dang gui tin nhan...\r\n");

    // Đợi kết quả (tăng lên 30s vì Viettel có thể chậm)
    Delay_ms(30000); // Tăng từ 15000ms

    // In kết quả
    if (uart1_index > 0)
    {
        UART_testchuoi(DEBUG, "Ket qua: ");
        for (uint16_t i = 0; i < uart1_index; i++)
            custom_SendByte(DEBUG, uart1_dem[i]);
        UART_testchuoi(DEBUG, "\r\n");
    }
    else
    {
        UART_testchuoi(DEBUG, "Khong nhan duoc ket qua!\r\n");
    }

    UART_testchuoi(DEBUG, "--- KET THUC GUI SMS ---\r\n");
}

