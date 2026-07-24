#include "bsp_common.h"
#include "project_critical.h"

uint32_t Project_EnterCritical(void)
{
    return BSP_EnterCritical();
}

void Project_ExitCritical(uint32_t state)
{
    BSP_ExitCritical(state);
}
uint32_t BSP_GetDeviceIdHash(void)
{
    uint32_t trace_id;
    uint32_t part_info;
    uint32_t hash;

    /*
     * MSPM0的芯片标识位于Factory Region。
     * 使用TI DriverLib访问，禁止沿用STM32F407的0x1FFF7A10地址。
     */
    trace_id = DL_FactoryRegion_getTraceID();
    part_info =
        ((uint32_t)DL_FactoryRegion_getUserIDPart() << 16U) |
        (uint32_t)DL_FactoryRegion_getPartNumber();

    hash = trace_id ^ part_info ^ 0x9E3779B9UL;
    hash ^= hash >> 16U;
    hash *= 0x7FEB352DUL;
    hash ^= hash >> 15U;
    hash *= 0x846CA68BUL;
    hash ^= hash >> 16U;

    /*
     * 正常情况下Trace ID用于区分芯片。
     * 若样片返回0，仍返回非零值；两块板ID碰撞时可在test_config.h中手动覆盖。
     */
    if (hash == 0U) {
        hash = part_info ^ 0x35190001UL;
    }

    return hash;
}
