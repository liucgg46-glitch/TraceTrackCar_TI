#ifndef __BSP_PWM_H
#define __BSP_PWM_H

#include "bsp_common.h"
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* CH1~CH4：TIMA0 20 kHz 电机 PWM；CH5~CH6：TIMG0 50 Hz 舵机 PWM。 */
#define BSP_PWM_CH1_ENABLE 1U
#define BSP_PWM_CH2_ENABLE 1U
#define BSP_PWM_CH3_ENABLE VEHICLE_REAR_DRIVE_ENABLE
#define BSP_PWM_CH4_ENABLE VEHICLE_REAR_DRIVE_ENABLE
#define BSP_PWM_CH5_ENABLE 1U
#define BSP_PWM_CH6_ENABLE 1U

#define BSP_PWM_MOTOR_PERIOD_COUNTS 1600U
#define BSP_PWM_MOTOR_FREQUENCY_HZ  20000U
#define BSP_PWM_SERVO_PERIOD_US      20000U
#define BSP_PWM_SERVO_FREQUENCY_HZ   50U

typedef enum {
    BSP_PWM_CH1 = 0,
    BSP_PWM_CH2,
    BSP_PWM_CH3,
    BSP_PWM_CH4,
    BSP_PWM_CH5,
    BSP_PWM_CH6,
    BSP_PWM_COUNT
} BSP_PWM_Id_t;

#define BSP_PWM_SERVO_HORIZONTAL BSP_PWM_CH5
#define BSP_PWM_SERVO_PITCH      BSP_PWM_CH6

void BSP_PWM_Init(BSP_PWM_Id_t id);
void BSP_PWM_InitAll(void);
BSP_Status_t BSP_PWM_SetCompare(BSP_PWM_Id_t id, uint16_t compare);
BSP_Status_t BSP_PWM_SetDutyPermille(BSP_PWM_Id_t id, uint16_t permille);
uint16_t BSP_PWM_GetPeriod(BSP_PWM_Id_t id);
void BSP_PWM_Start(BSP_PWM_Id_t id);
void BSP_PWM_Stop(BSP_PWM_Id_t id);

#ifdef __cplusplus
}
#endif

#endif /* __BSP_PWM_H */
