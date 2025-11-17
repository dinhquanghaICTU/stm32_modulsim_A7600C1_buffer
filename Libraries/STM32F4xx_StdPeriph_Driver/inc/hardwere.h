#ifndef __HARDWARE_H__
#define __HARDWARE_H__

#include <stdint.h>
#include <stddef.h>
#include "stm32f4xx.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_rcc.h"
#include "stm32f4xx_usart.h"

// Buffer size - PHẢI ĐỊNH NGHĨA TRƯỚC
#define MAX_SIZE 300

// SIM
#define SIM_STATUS_Pin GPIO_Pin_12
#define SIM_STATUS_GPIO GPIOA

// Nguon
#define SIM_PWKEY_Pin GPIO_Pin_8
#define SIM_PWKEY_GPIO GPIOA
#define SIM_PWKEY_LOW()  GPIO_ResetBits(SIM_PWKEY_GPIO, SIM_PWKEY_Pin)
#define SIM_PWKEY_HIGH() GPIO_SetBits(SIM_PWKEY_GPIO, SIM_PWKEY_Pin)

// LED debug
#define LED_GREEN_NETWORK_PIN GPIO_Pin_9
#define LED_GREEN_NETWORK_PORT GPIOB
#define LED_BLUE_STATEMACHINE_PIN GPIO_Pin_4
#define LED_BLUE_STATEMACHINE_PORT GPIOB
#define LED_4G_PIN LED_GREEN_NETWORK_PIN
#define LED_4G_PORT LED_GREEN_NETWORK_PORT
#define LED_STT_PIN LED_BLUE_STATEMACHINE_PIN
#define LED_STT_PORT LED_BLUE_STATEMACHINE_PORT

// UART Debug
#define DEBUG USART2

// Global buffers - khai báo extern SAU KHI đã define MAX_SIZE
extern volatile uint8_t uart1_dem[MAX_SIZE];
extern volatile uint16_t uart1_index;
extern volatile uint8_t uart2_dem[MAX_SIZE];
extern volatile uint16_t uart2_index;

// Function prototypes
void SystemClock_Config_rieng(void);
void Delay_Init(void);
void Delay_us(uint32_t us);
void Delay_ms(uint32_t ms);
void delay(uint32_t ms);
uint32_t Get_SystemClock(void);
void onModulSim(void);
void offModulSim(void);
void NVIC_config(void);
void gpio_init(void);
void UART2_config_os(void);
void USART1_Sim_A7600C1_Config(void);
void led_debug_on(void);
void led_debug_off(void);
void led_SendUartDone(void);
void UART_testchuoi(USART_TypeDef* USARTx, char *msg);
void custom_SendByte(USART_TypeDef* USARTx, uint8_t data);
void guiLenhAT(char *cmd);
void test_tinnhan(char *number, char *message);

#endif /* __HARDWARE_H__ */
