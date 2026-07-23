#ifndef __BSP_GPIO_H
#define __BSP_GPIO_H

#include "bsp_common.h"
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
 * 第二阶段只冻结核心板自带资源和外接 ICM20948 的控制脚。
 * 电机、显示、HX711、E220 等外接资源将在完整 PinMux 通过后追加。
 */
typedef enum {
    BSP_GPIO_LED1 = 0,
    BSP_GPIO_LED2,
    BSP_GPIO_RGB_DATA,
    BSP_GPIO_ICM20948_CS,
    BSP_GPIO_ICM20948_INT,
    BSP_GPIO_USER_KEY,
    BSP_GPIO_COUNT
} BSP_GPIO_Id_t;

/* 保持当前已迁移模块使用的旧别名。 */
#define BSP_GPIO_CH1                BSP_GPIO_LED1
#define BSP_GPIO_CH2                BSP_GPIO_ICM20948_CS
#define BSP_GPIO_CH16               BSP_GPIO_ICM20948_CS
#define BSP_GPIO_CH21               BSP_GPIO_LED1
#define BSP_GPIO_CH22               BSP_GPIO_LED2
#define BSP_GPIO_STATUS_RED         BSP_GPIO_LED1
#define BSP_GPIO_STATUS_GREEN       BSP_GPIO_LED2
#define BSP_GPIO_STATUS_RED_ACTIVE_LEVEL    0U
#define BSP_GPIO_STATUS_GREEN_ACTIVE_LEVEL  0U

void BSP_GPIO_Init(BSP_GPIO_Id_t id);
void BSP_GPIO_InitAll(void);
void BSP_GPIO_Write(BSP_GPIO_Id_t id, uint8_t level);
void BSP_GPIO_Toggle(BSP_GPIO_Id_t id);
uint8_t BSP_GPIO_Read(BSP_GPIO_Id_t id);
GPIO_Regs *BSP_GPIO_GetPort(BSP_GPIO_Id_t id);
uint32_t BSP_GPIO_GetPin(BSP_GPIO_Id_t id);

#ifdef __cplusplus
}
#endif

#endif /* __BSP_GPIO_H */
