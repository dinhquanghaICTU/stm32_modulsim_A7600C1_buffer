#ifndef __HARDWARE_H__
#define __HARDWARE_H__

#include <stdint.h>
#include <stdbool.h>

#include "stm32f4xx.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_rcc.h"
#include "stm32f4xx_usart.h"
#include "misc.h"
#include "drivers/uart_hw.h"


extern volatile uint32_t system_tick_ms;


// -------------------- SIM POWER KEY --------------------


#define GSM_PWKEY_Pin GPIO_Pin_8
#define GSM_PWKEY_GPIO GPIOA

#define GSM_PWKEY_LOW() GPIO_SetBits(GSM_PWKEY_GPIO, GSM_PWKEY_Pin)
#define GSM_PWKEY_HIGH() GPIO_ResetBits(GSM_PWKEY_GPIO, GSM_PWKEY_Pin)

// -------------------- LED --------------------

#define LED_GREEN_NETWORK_PIN GPIO_Pin_9

// -------------------- UART --------------------
#define UART_DEBUG   USART2     
#define UART_SIM     USART1     

// -------------------- API --------------------
uint32_t HW_GetSystemClock(void);

void HW_SystemClockConfig(void);
void HW_DelayInit(void);

uint32_t HW_GetTickMs(void);
void HW_DelayMs(uint32_t ms);
void HW_Delay(uint32_t ms);
bool HW_IsTimeout(uint32_t *timestamp, uint32_t period_ms);


void HW_NvicConfig(void);
void HW_GpioInit(void);

void DebugUart_Init(void);
void SimA7600C1_UartInit(void);

#endif
