#ifndef __BSP_ALL_H
#define __BSP_ALL_H

#include "bsp_common.h"
#include "bsp_systick.h"
#include "bsp_gpio.h"
#include "bsp_exti.h"
#include "bsp_pwm.h"
#include "bsp_encoder.h"
#include "bsp_adc.h"
#include "bsp_key.h"
#include "bsp_uart.h"
#include "bsp_i2c.h"
#include "bsp_spi.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
 * BSP 统一初始化入口。
 * 平台级 SysConfig 初始化和各 BSP 模块初始化均在本函数内部完成，
 * Core 层不直接依赖 TI 生成接口。
 */
BSP_Status_t BSP_InitAll(void);
void BSP_TaskAll(void);

#ifdef __cplusplus
}
#endif

#endif /* __BSP_ALL_H */