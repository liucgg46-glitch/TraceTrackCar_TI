#ifndef __BSP_ENCODER_H
#define __BSP_ENCODER_H

#include "bsp_common.h"
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define BSP_ENCODER_UPDATE_PERIOD_MS 10U

/*
 * TIMG8/TIMG9 是 MSPM0G3519 可用的两组硬件 QEI，固定给左右前轮。
 * 后轮 CH3/CH4 使用 GPIO 双边沿中断软件正交解码。
 */
#define BSP_ENCODER_CH1_ENABLE 1U
#define BSP_ENCODER_CH2_ENABLE 1U
#define BSP_ENCODER_CH3_ENABLE VEHICLE_REAR_DRIVE_ENABLE
#define BSP_ENCODER_CH4_ENABLE VEHICLE_REAR_DRIVE_ENABLE

#define BSP_ENCODER_CH1_REVERSE 0U
#define BSP_ENCODER_CH2_REVERSE 0U
#define BSP_ENCODER_CH3_REVERSE 0U
#define BSP_ENCODER_CH4_REVERSE 0U

typedef enum {
    BSP_ENCODER_CH1 = 0,
    BSP_ENCODER_CH2,
    BSP_ENCODER_CH3,
    BSP_ENCODER_CH4,
    BSP_ENCODER_COUNT
} BSP_Encoder_Id_t;

typedef struct {
    int16_t delta_count;
    int32_t total_count;
    int32_t speed_cps;
    uint32_t update_time_ms;
} BSP_Encoder_Info_t;

void BSP_Encoder_Init(BSP_Encoder_Id_t id);
void BSP_Encoder_InitAll(void);
void BSP_Encoder_Update(BSP_Encoder_Id_t id);
void BSP_Encoder_UpdateAll(void);
int16_t BSP_Encoder_GetDelta(BSP_Encoder_Id_t id);
int32_t BSP_Encoder_GetSpeedCps(BSP_Encoder_Id_t id);
int32_t BSP_Encoder_GetTotal(BSP_Encoder_Id_t id);
void BSP_Encoder_ClearTotal(BSP_Encoder_Id_t id);
void BSP_Encoder_ClearAllTotal(void);
BSP_Status_t BSP_Encoder_GetInfo(BSP_Encoder_Id_t id, BSP_Encoder_Info_t *info);

#ifdef __cplusplus
}
#endif

#endif /* __BSP_ENCODER_H */
