#ifndef __BSP_GPIO_H
#define __BSP_GPIO_H

#include "bsp_common.h"
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
 * MSPM0G3519 核心板及小车外设 GPIO。
 * 所有真实引脚均由 SysConfig 生成，本层只提供稳定的逻辑编号。
 */
typedef enum {
    BSP_GPIO_LED1 = 0,
    BSP_GPIO_LED2,
    BSP_GPIO_RGB_DATA,
    BSP_GPIO_ICM20948_CS,
    BSP_GPIO_ICM20948_INT,
    BSP_GPIO_USER_KEY,
    BSP_GPIO_MOTOR_FL_IN1,
    BSP_GPIO_MOTOR_FL_IN2,
    BSP_GPIO_MOTOR_FR_IN1,
    BSP_GPIO_MOTOR_FR_IN2,
    BSP_GPIO_MOTOR_RL_IN1,
    BSP_GPIO_MOTOR_RL_IN2,
    BSP_GPIO_MOTOR_RR_IN1,
    BSP_GPIO_MOTOR_RR_IN2,
    BSP_GPIO_LCD_CS,
    BSP_GPIO_LCD_DC,
    BSP_GPIO_LCD_BL,
    BSP_GPIO_LCD_RESET,
    BSP_GPIO_GRAY_S0,
    BSP_GPIO_GRAY_S1,
    BSP_GPIO_GRAY_S2,
    BSP_GPIO_E220_AUX,
    BSP_GPIO_LASER_EN,
    BSP_GPIO_HX711_DOUT,
    BSP_GPIO_HX711_PD_SCK,
    BSP_GPIO_BUZZER,
    BSP_GPIO_KEY1,
    BSP_GPIO_KEY2,
    BSP_GPIO_KEY3,
    BSP_GPIO_KEY4,
    BSP_GPIO_COUNT
} BSP_GPIO_Id_t;

/* 兼容原 STM32 工程已经使用的逻辑别名。 */
#define BSP_GPIO_CH1                 BSP_GPIO_LED1
#define BSP_GPIO_CH2                 BSP_GPIO_ICM20948_CS
#define BSP_GPIO_CH3                 BSP_GPIO_MOTOR_FL_IN1
#define BSP_GPIO_CH4                 BSP_GPIO_MOTOR_FL_IN2
#define BSP_GPIO_CH5                 BSP_GPIO_MOTOR_FR_IN1
#define BSP_GPIO_CH6                 BSP_GPIO_MOTOR_FR_IN2
#define BSP_GPIO_CH7                 BSP_GPIO_MOTOR_RL_IN1
#define BSP_GPIO_CH8                 BSP_GPIO_MOTOR_RL_IN2
#define BSP_GPIO_CH9                 BSP_GPIO_MOTOR_RR_IN1
#define BSP_GPIO_CH10                BSP_GPIO_MOTOR_RR_IN2
#define BSP_GPIO_CH16                BSP_GPIO_ICM20948_CS
#define BSP_GPIO_CH21                BSP_GPIO_LED1
#define BSP_GPIO_CH22                BSP_GPIO_LED2

#define BSP_GPIO_STATUS_RED          BSP_GPIO_LED1
#define BSP_GPIO_STATUS_GREEN        BSP_GPIO_LED2
#define BSP_GPIO_STATUS_RED_ACTIVE_LEVEL    0U
#define BSP_GPIO_STATUS_GREEN_ACTIVE_LEVEL  0U
#define BSP_GPIO_BUZZER_ACTIVE_LEVEL         0U

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
