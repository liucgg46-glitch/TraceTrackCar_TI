#ifndef __BSP_SYSTICK_H
#define __BSP_SYSTICK_H

#include "bsp_common.h"
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define BSP_SYSTICK_HZ 1000UL

BSP_Status_t BSP_SysTick_Init(uint32_t system_core_clock_hz);
void BSP_SysTick_Inc(void);
uint32_t BSP_GetTickMs(void);
uint32_t GetTick(void);
uint8_t BSP_TimeElapsed(uint32_t *last_time_ms, uint32_t period_ms);
uint8_t BSP_IsTimeout(uint32_t start_time_ms, uint32_t timeout_ms);

#ifdef __cplusplus
}
#endif

#endif /* __BSP_SYSTICK_H */
