#ifndef __BSP_EXTI_H
#define __BSP_EXTI_H

#include "bsp_common.h"
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
 * MSPM0 外部 GPIO 中断分发层。
 *
 * 目标核心板没有原 STM32 工程中的 PE0/PE1/PE2。CH1 改接用户自己的
 * ICM20948 INT（PB21），CH2/CH3 在确定实际接线前保持禁用。
 * 回调在中断上下文执行，只允许置标志或记录时间，不能阻塞。
 */
typedef void (*BSP_EXTI_Callback_t)(void *ctx);

#define BSP_EXTI_CH1_ENABLE 1U
#define BSP_EXTI_CH1_PORT   GPIO_BOARD_IO_PORT
#define BSP_EXTI_CH1_PIN    GPIO_BOARD_IO_ICM20948_INT_PIN

#define BSP_EXTI_CH2_ENABLE 0U
#define BSP_EXTI_CH3_ENABLE 0U

#define BSP_EXTI_ANY_ENABLE \
    (BSP_EXTI_CH1_ENABLE || BSP_EXTI_CH2_ENABLE || BSP_EXTI_CH3_ENABLE)

typedef enum {
#if BSP_EXTI_CH1_ENABLE
    BSP_EXTI_CH1,
#endif
#if BSP_EXTI_CH2_ENABLE
    BSP_EXTI_CH2,
#endif
#if BSP_EXTI_CH3_ENABLE
    BSP_EXTI_CH3,
#endif
    BSP_EXTI_COUNT
} BSP_EXTI_Id_t;

void BSP_EXTI_Init(BSP_EXTI_Id_t id);
void BSP_EXTI_InitAll(void);
BSP_Status_t BSP_EXTI_AttachCallback(
    BSP_EXTI_Id_t id, BSP_EXTI_Callback_t cb, void *ctx);

/*
 * GPIOB 与后轮软件编码器共用 GROUP1_IRQHandler。
 * 中断入口先统一清除 pending，再把位掩码交给此函数分发。
 */
void BSP_EXTI_DispatchIRQ(uint32_t gpio_pins);
uint32_t BSP_EXTI_GetEnabledPins(GPIO_Regs *port);

#ifdef __cplusplus
}
#endif

#endif /* __BSP_EXTI_H */
