#ifndef __SIM_POWER_SM_H__
#define __SIM_POWER_SM_H__

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif


typedef enum {
    MODULE_IDLE,        
    MODULE_ON,          
    MODULE_WAIT_ON,     
    MODULE_OFF,         
    MODULE_WAIT_OFF, 
    MODULE_ERROR,
} sim_power_sm_t;


typedef void (*sim_power_on_callback_t)(void);


void SimPowerSM_Init(void);
void SimPowerSM_RequestOn(void);
void SimPowerSM_RequestOff(void);
sim_power_sm_t SimPowerSM_GetState(void);
void SimPowerSM_Process(void);

void SimPowerSM_SetOnCallback(sim_power_on_callback_t cb);

#ifdef __cplusplus
}
#endif

#endif /* __SIM_POWER_SM_H__ */
