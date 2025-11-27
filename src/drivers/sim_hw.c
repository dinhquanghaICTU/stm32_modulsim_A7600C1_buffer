#include "drivers/hardware.h"
#include "drivers/sim_hw.h"


void onModulSim()
{
    GSM_PWKEY_LOW();
    HW_DelayMs(500);

    GSM_PWKEY_HIGH();
    HW_DelayMs(1500);
}

void offModulSim()
{
    GSM_PWKEY_LOW();
    HW_DelayMs(500);
}

