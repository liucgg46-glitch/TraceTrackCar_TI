#ifndef __ATTITUDE_ESTIMATOR_H
#define __ATTITUDE_ESTIMATOR_H

#include "project_status.h"
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
 * ICM-20948 姿态融合层。
 *
 * 本模块位于 Algorithm 层：
 *   - Driver 只负责输出可靠的加速度、角速度和磁场数据；
 *   - 本模块负责 Mahony 反馈、磁场异常门控和在线零偏；
 *   - APP/Route 只读取最终 Roll、Pitch 和 Yaw。
 */

/* ============================== 融合参数 ================================== */

/* 加速度计对 Roll/Pitch 的 Mahony 比例反馈，数值越大收敛越快。 */
#define ATTITUDE_MAHONY_ACCEL_KP                 2.0f

/*
 * Yaw 数据源策略：
 *   - Roll/Pitch 继续使用陀螺仪与加速度计 Mahony 修正；
 *   - Yaw 使用校准后的 Z 轴角速度积分，并由磁力计缓慢修正；
 *   - 编码器航向角和角速度不参与姿态估计。
 */
#define ATTITUDE_SEPARATE_YAW_ENABLE              1U
#define ATTITUDE_ENCODER_YAW_CORRECTION_ENABLE    0U
#define ATTITUDE_MAG_YAW_CORRECTION_ENABLE        1U
#define ATTITUDE_MAG_DISABLE_WHEN_MOTOR_ACTIVE    1U

/* 磁力计只用于绕重力方向的低增益 Yaw 修正。 */
#define ATTITUDE_MAHONY_MAG_KP                   0.05f

/* 合法积分周期；超出范围时使用标称周期，避免停顿后一次积分过大。 */
#define ATTITUDE_NOMINAL_DT_S                     0.00978f
#define ATTITUDE_MIN_DT_S                         0.002f
#define ATTITUDE_MAX_DT_S                         0.050f

/* 只有加速度模长接近 1 g 时，才允许它修正 Roll/Pitch。 */
#define ATTITUDE_ACCEL_CORRECTION_MIN_G           0.80f
#define ATTITUDE_ACCEL_CORRECTION_MAX_G           1.20f

/* 静止检测与运行中残余零偏更新。 */
#define ATTITUDE_STATIONARY_ACCEL_MIN_G           0.97f
#define ATTITUDE_STATIONARY_ACCEL_MAX_G           1.03f
#define ATTITUDE_STATIONARY_GYRO_MAX_DPS          0.35f
#define ATTITUDE_STATIONARY_SAMPLE_COUNT        100U
#define ATTITUDE_ONLINE_BIAS_ALPHA                0.005f
#define ATTITUDE_ONLINE_BIAS_MAX_DPS              5.0f

/* ============================== 磁场校准 ================================== */

/*
 * 以下默认值来自本车此前 M/N 测试输出。
 * 更换 IMU、安装方向或车体结构后必须重新标定。
 */
#define ATTITUDE_MAG_CAL_DEFAULT_VALID             1U
#define ATTITUDE_MAG_CAL_OFFSET_X_UT             -43.05f
#define ATTITUDE_MAG_CAL_OFFSET_Y_UT              30.75f
#define ATTITUDE_MAG_CAL_OFFSET_Z_UT              -4.35f
#define ATTITUDE_MAG_CAL_SCALE_X                   1.004f
#define ATTITUDE_MAG_CAL_SCALE_Y                   0.973f
#define ATTITUDE_MAG_CAL_SCALE_Z                   1.022f

/* min/max 标定至少覆盖足够样本，并保证每个轴具有足够旋转范围。 */
#define ATTITUDE_MAG_CAL_MIN_SAMPLES             300U
#define ATTITUDE_MAG_CAL_MIN_SPAN_UT              20.0f

/* 磁场异常门控：绝对强度、相对基准变化和方向突变。 */
#define ATTITUDE_MAG_FIELD_MIN_UT                 15.0f
#define ATTITUDE_MAG_FIELD_MAX_UT                100.0f
#define ATTITUDE_MAG_FIELD_REL_TOLERANCE           0.15f
#define ATTITUDE_MAG_REFERENCE_ALPHA               0.002f
#define ATTITUDE_MAG_ACQUIRE_SAMPLES              10U
#define ATTITUDE_MAG_DIRECTION_MIN_DOT             0.85f
#define ATTITUDE_MAG_STALE_TIMEOUT_MS             250U

typedef struct {
    float offset_uT[3];
    float scale[3];
    uint8_t valid;
} Attitude_MagCalibration_t;

typedef struct {
    float x;
    float y;
    float z;
} Attitude_Vector3f_t;

/*
 * 姿态算法使用纯数据输入，不暴露具体 IMU 驱动类型。
 * APP 适配层负责把传感器驱动缓存转换为该结构。
 */
typedef struct {
    Attitude_Vector3f_t accel_filtered_g;
    Attitude_Vector3f_t gyro_filtered_dps;
    Attitude_Vector3f_t mag_uT;
    Attitude_Vector3f_t mag_filtered_uT;
    uint32_t timestamp_ms;
    uint8_t mag_valid;
    uint8_t mag_updated;
} Attitude_Input_t;

typedef struct {
    float q[4];                       /* w、x、y、z */
    float roll_deg;
    float pitch_deg;
    float yaw_deg;

    /* 驱动上电零偏之后，本融合层继续学习到的残余零偏。 */
    float online_gyro_bias_dps[3];

    /*
     * 编码器字段为兼容既有诊断接口而保留。
     * 当前配置下 encoder_used 始终为 0，不参与姿态融合。
     */
    float encoder_yaw_deg;
    float encoder_yaw_rate_dps;
    float mag_norm_uT;
    float mag_reference_uT;

    uint32_t timestamp_ms;
    uint32_t update_count;
    uint32_t mag_accept_count;
    uint32_t mag_reject_count;
    uint32_t mag_calibration_samples;

    uint8_t initialized;
    uint8_t valid;
    uint8_t stationary;
    uint8_t encoder_used;
    uint8_t encoder_heading_valid;
    uint8_t mag_available;
    uint8_t mag_healthy;
    uint8_t mag_used;
    uint8_t mag_calibrating;
    uint8_t mag_calibrated;
} Attitude_Info_t;

void Attitude_Init(void);
void Attitude_Reset(void);

/* 每次出现新输入时间戳时融合一次；重复时间戳返回 PROJECT_BUSY。 */
Project_Status_t Attitude_Update(const Attitude_Input_t *input,
                                 uint8_t motor_active);

/* 输入源离线或数据失效时由适配层调用，防止上层继续使用陈旧姿态。 */
void Attitude_Invalidate(void);
Project_Status_t Attitude_GetInfo(Attitude_Info_t *info);

float Attitude_GetRollDeg(void);
float Attitude_GetPitchDeg(void);
float Attitude_GetYawDeg(void);
uint8_t Attitude_IsValid(void);

/* 只改变对外输出的 Yaw 零点，不重置四元数、Roll/Pitch 或校准状态。 */
void Attitude_ZeroYaw(void);

/* 运行时磁力计标定：在 Start 与 Finish 之间缓慢转遍所有方向。 */
void Attitude_MagCalibrationStart(void);
Project_Status_t Attitude_MagCalibrationFinish(Attitude_MagCalibration_t *result);
Project_Status_t Attitude_SetMagCalibration(const Attitude_MagCalibration_t *calibration);
Project_Status_t Attitude_GetMagCalibration(Attitude_MagCalibration_t *calibration);

#ifdef __cplusplus
}
#endif

#endif /* __ATTITUDE_ESTIMATOR_H */