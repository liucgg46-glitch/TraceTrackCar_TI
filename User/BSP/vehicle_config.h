#ifndef __VEHICLE_CONFIG_H
#define __VEHICLE_CONFIG_H

/*
 * MSPM0G3519 小车驱动配置。
 *
 * 2WD：启用左右前轮电机和 TIMG8/TIMG9 硬件 QEI。
 * 4WD：额外启用左右后轮电机；后轮编码器将使用 GPIO 双边沿软件解码。
 *
 * 切换底盘结构时只修改 VEHICLE_DRIVE_MODE。
 */
#define VEHICLE_DRIVE_MODE_2WD             2U
#define VEHICLE_DRIVE_MODE_4WD             4U

#ifndef VEHICLE_DRIVE_MODE
#define VEHICLE_DRIVE_MODE                 VEHICLE_DRIVE_MODE_2WD
#endif

#if (VEHICLE_DRIVE_MODE == VEHICLE_DRIVE_MODE_2WD)
#define VEHICLE_REAR_DRIVE_ENABLE          0U
#define VEHICLE_SPI1_PINS_AVAILABLE        1U
#define VEHICLE_DRIVE_MODE_NAME            "2WD"
#elif (VEHICLE_DRIVE_MODE == VEHICLE_DRIVE_MODE_4WD)
#define VEHICLE_REAR_DRIVE_ENABLE          1U
#define VEHICLE_SPI1_PINS_AVAILABLE        1U
#define VEHICLE_DRIVE_MODE_NAME            "4WD"
#else
#error "VEHICLE_DRIVE_MODE must be VEHICLE_DRIVE_MODE_2WD or VEHICLE_DRIVE_MODE_4WD"
#endif

#endif /* __VEHICLE_CONFIG_H */
