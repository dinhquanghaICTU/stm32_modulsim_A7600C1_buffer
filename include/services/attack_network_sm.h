#ifndef __ATTACK_NETWORK_SM_H__
#define __ATTACK_NETWORK_SM_H__

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum{
    SMS_ATTACK_NW_IDLE,
    SMS_ATTACK_NW_AT,
    SMS_ATTACK_NW_WAIT_AT,
    SMS_ATTACK_NW_CPIN,
    SMS_ATTACK_NW_WAIT_CPIN,
    SMS_ATTACK_NW_CSQ,
    SMS_ATTACK_NW_WAIT_CSQ,
    SMS_ATTACK_NW_CREG,
    SMS_ATTACK_NW_WAIT_CREG,
    SMS_ATTACK_NW_CPMS,
    SMS_ATTACK_NW_WAIT_CPMS,
    SMS_ATTACK_NW_CMGF,
    SMS_ATTACK_NW_WAIT_CMGF,
    SMS_ATTACK_NW_CSCS,
    SMS_ATTACK_NW_WAIT_CSCS,
    SMS_ATTACK_NW_DONE,
    SMS_ATTACK_NW_ERROR,
} attack_network_t;

void sms_attack_network_init(void);
void sms_attack_network_process(void);
attack_network_t sms_attack_network_get_state(void);

#ifdef __cplusplus
}
#endif

#endif // __ATTACK_NETWORK_SM_H__