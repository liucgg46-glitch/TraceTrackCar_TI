#include "bsp_systick.h"

static volatile uint32_t s_bsp_tick_ms;

BSP_Status_t BSP_SysTick_Init(uint32_t system_core_clock_hz)
{
    uint32_t period_cycles;
    uint32_t primask;

    if (system_core_clock_hz < BSP_SYSTICK_HZ) {
        return BSP_PARAM;
    }

    period_cycles = system_core_clock_hz / BSP_SYSTICK_HZ;
    if ((period_cycles == 0U) || (period_cycles > 0x01000000UL)) {
        return BSP_PARAM;
    }

    primask = BSP_EnterCritical();
    s_bsp_tick_ms = 0U;
    DL_SYSTICK_config(period_cycles);
    BSP_ExitCritical(primask);

    return BSP_OK;
}

void BSP_SysTick_Inc(void)
{
    s_bsp_tick_ms++;
}

uint32_t BSP_GetTickMs(void)
{
    return s_bsp_tick_ms;
}

uint32_t GetTick(void)
{
    return BSP_GetTickMs();
}

uint8_t BSP_TimeElapsed(uint32_t *last_time_ms, uint32_t period_ms)
{
    uint32_t now;

    if (last_time_ms == 0) {
        return 0U;
    }

    now = BSP_GetTickMs();
    if ((uint32_t)(now - *last_time_ms) >= period_ms) {
        *last_time_ms = now;
        return 1U;
    }

    return 0U;
}

uint8_t BSP_IsTimeout(uint32_t start_time_ms, uint32_t timeout_ms)
{
    return ((uint32_t)(BSP_GetTickMs() - start_time_ms) >= timeout_ms) ? 1U : 0U;
}

void SysTick_Handler(void)
{
    BSP_SysTick_Inc();
}
