#ifndef __BSP_COMMON_H
#define __BSP_COMMON_H

#include "ti_msp_dl_config.h"
#include "vehicle_config.h"
#include "project_status.h"
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* BSP 与上层共用一套平台无关状态码。 */
typedef Project_Status_t BSP_Status_t;
#define BSP_OK       PROJECT_OK
#define BSP_ERROR    PROJECT_ERROR
#define BSP_BUSY     PROJECT_BUSY
#define BSP_TIMEOUT  PROJECT_TIMEOUT
#define BSP_PARAM    PROJECT_PARAM

#ifndef BSP_WEAK
#if defined(__CC_ARM) || defined(__ARMCC_VERSION)
#define BSP_WEAK __attribute__((weak))
#elif defined(__GNUC__)
#define BSP_WEAK __attribute__((weak))
#elif defined(__ICCARM__)
#define BSP_WEAK __weak
#else
#define BSP_WEAK
#endif
#endif

#ifndef BSP_GET_TICK
uint32_t GetTick(void);
#define BSP_GET_TICK() GetTick()
#endif

/*
 * 返回进入临界区前的 PRIMASK，退出时必须原样传回。
 * 这样不会错误地打开进入前已经关闭的中断。
 */
static inline uint32_t BSP_EnterCritical(void)
{
    uint32_t primask = __get_PRIMASK();
    __disable_irq();
    return primask;
}

static inline void BSP_ExitCritical(uint32_t primask)
{
    __set_PRIMASK(primask);
}

static inline uint32_t BSP_GetCoreClockHz(void)
{
    return CPUCLK_FREQ;
}

#ifdef __cplusplus
}
#endif

#endif /* __BSP_COMMON_H */
