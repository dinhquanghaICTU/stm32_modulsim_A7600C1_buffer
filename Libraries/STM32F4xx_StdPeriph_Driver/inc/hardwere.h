#ifndef __HARDWARE_H__
#define __HARDWARE_H__

#include <stdint.h>
#include <stddef.h>
#include "stm32f4xx.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_rcc.h"
#include "stm32f4xx_usart.h"
#include "ringbuffer.h"


#define MAX_SIZE 300

RINGBUFFER_t BUFFER_AT;
uint8_t AT_Data[256];

RINGBUFFER_t BUFFER_DEBUG;
uint8_t DEBUG_Data[256];

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
#define AT	USART1


volatile uint8_t uart1_dem[MAX_SIZE];
volatile uint16_t uart1_index;
volatile uint8_t uart2_dem[MAX_SIZE];
volatile uint16_t uart2_index;

//state machine

typedef  enum{
	SIM_IDLE = 0,
	SIM_POWWER_ON,
	SIM_WAIT_BOOT,
	SIM_CHECK_AT,
	SIM_SET_FULL_FUNC,
	SIM_SET_CHARSET,
	SIM_SET_SMS_MODE,
	SIM_CHECK_NETWORK,
	SIM_REGISTER_NETWORK,
	SIM_CHECK_SIGNAL,
	SIM_READY,
	SIM_SEND_SMS,
	SIM_WAIT_SMS_RESPONSE,
	SIM_ERROR
}state_machine_t;

typedef struct{
	state_machine_t	state;
	state_machine_t	next_State;
	uint32_t	timestamp;
	uint16_t	timeout;
	uint8_t		retrycount;
	uint8_t		max_retry;
}sim_config_t; // config_sim


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
void UART_testchuoi(RINGBUFFER_t *buffer, char *msg);
void custom_SendByte(USART_TypeDef* USARTx, uint8_t data);
void guiLenhAT(char *cmd);
void gui_tinnhan(char *number, char *message);
//void state_machine(void);
void handler_TX(USART_TypeDef* USARTx, RINGBUFFER_t *buffer);



#endif /* __HARDWARE_H__ */
