/*
 * Copyright (c) 2023, Texas Instruments Incorporated - http://www.ti.com
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/*
 *  ============ ti_msp_dl_config.h =============
 *  Configured MSPM0 DriverLib module declarations
 *
 *  DO NOT EDIT - This file is generated for the MSPM0G351X
 *  by the SysConfig tool.
 */
#ifndef ti_msp_dl_config_h
#define ti_msp_dl_config_h

#define CONFIG_MSPM0G351X
#define CONFIG_MSPM0G3519

#if defined(__ti_version__) || defined(__TI_COMPILER_VERSION__)
#define SYSCONFIG_WEAK __attribute__((weak))
#elif defined(__IAR_SYSTEMS_ICC__)
#define SYSCONFIG_WEAK __weak
#elif defined(__GNUC__)
#define SYSCONFIG_WEAK __attribute__((weak))
#endif

#include <ti/devices/msp/msp.h>
#include <ti/driverlib/driverlib.h>
#include <ti/driverlib/m0p/dl_core.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
 *  ======== SYSCFG_DL_init ========
 *  Perform all required MSP DL initialization
 *
 *  This function should be called once at a point before any use of
 *  MSP DL.
 */


/* clang-format off */

#define POWER_STARTUP_DELAY                                                (16)



#define CPUCLK_FREQ                                                     32000000



/* Defines for PWM_MOTOR */
#define PWM_MOTOR_INST                                                     TIMA0
#define PWM_MOTOR_INST_IRQHandler                               TIMA0_IRQHandler
#define PWM_MOTOR_INST_INT_IRQN                                 (TIMA0_INT_IRQn)
#define PWM_MOTOR_INST_CLK_FREQ                                         32000000
/* GPIO defines for channel 0 */
#define GPIO_PWM_MOTOR_C0_PORT                                             GPIOC
#define GPIO_PWM_MOTOR_C0_PIN                                      DL_GPIO_PIN_2
#define GPIO_PWM_MOTOR_C0_IOMUX                                  (IOMUX_PINCM76)
#define GPIO_PWM_MOTOR_C0_IOMUX_FUNC                 IOMUX_PINCM76_PF_TIMA0_CCP0
#define GPIO_PWM_MOTOR_C0_IDX                                DL_TIMER_CC_0_INDEX
/* GPIO defines for channel 1 */
#define GPIO_PWM_MOTOR_C1_PORT                                             GPIOC
#define GPIO_PWM_MOTOR_C1_PIN                                      DL_GPIO_PIN_4
#define GPIO_PWM_MOTOR_C1_IOMUX                                  (IOMUX_PINCM78)
#define GPIO_PWM_MOTOR_C1_IOMUX_FUNC                 IOMUX_PINCM78_PF_TIMA0_CCP1
#define GPIO_PWM_MOTOR_C1_IDX                                DL_TIMER_CC_1_INDEX
/* GPIO defines for channel 2 */
#define GPIO_PWM_MOTOR_C2_PORT                                             GPIOC
#define GPIO_PWM_MOTOR_C2_PIN                                      DL_GPIO_PIN_0
#define GPIO_PWM_MOTOR_C2_IOMUX                                  (IOMUX_PINCM74)
#define GPIO_PWM_MOTOR_C2_IOMUX_FUNC                 IOMUX_PINCM74_PF_TIMA0_CCP2
#define GPIO_PWM_MOTOR_C2_IDX                                DL_TIMER_CC_2_INDEX
/* GPIO defines for channel 3 */
#define GPIO_PWM_MOTOR_C3_PORT                                             GPIOA
#define GPIO_PWM_MOTOR_C3_PIN                                     DL_GPIO_PIN_28
#define GPIO_PWM_MOTOR_C3_IOMUX                                   (IOMUX_PINCM3)
#define GPIO_PWM_MOTOR_C3_IOMUX_FUNC                  IOMUX_PINCM3_PF_TIMA0_CCP3
#define GPIO_PWM_MOTOR_C3_IDX                                DL_TIMER_CC_3_INDEX

/* Defines for PWM_SERVO */
#define PWM_SERVO_INST                                                     TIMG0
#define PWM_SERVO_INST_IRQHandler                               TIMG0_IRQHandler
#define PWM_SERVO_INST_INT_IRQN                                 (TIMG0_INT_IRQn)
#define PWM_SERVO_INST_CLK_FREQ                                          1000000
/* GPIO defines for channel 0 */
#define GPIO_PWM_SERVO_C0_PORT                                             GPIOA
#define GPIO_PWM_SERVO_C0_PIN                                     DL_GPIO_PIN_12
#define GPIO_PWM_SERVO_C0_IOMUX                                  (IOMUX_PINCM34)
#define GPIO_PWM_SERVO_C0_IOMUX_FUNC                 IOMUX_PINCM34_PF_TIMG0_CCP0
#define GPIO_PWM_SERVO_C0_IDX                                DL_TIMER_CC_0_INDEX
/* GPIO defines for channel 1 */
#define GPIO_PWM_SERVO_C1_PORT                                             GPIOA
#define GPIO_PWM_SERVO_C1_PIN                                     DL_GPIO_PIN_13
#define GPIO_PWM_SERVO_C1_IOMUX                                  (IOMUX_PINCM35)
#define GPIO_PWM_SERVO_C1_IOMUX_FUNC                 IOMUX_PINCM35_PF_TIMG0_CCP1
#define GPIO_PWM_SERVO_C1_IDX                                DL_TIMER_CC_1_INDEX




/* Defines for QEI_FRONT_LEFT */
#define QEI_FRONT_LEFT_INST                                                TIMG8
#define QEI_FRONT_LEFT_INST_IRQHandler                          TIMG8_IRQHandler
#define QEI_FRONT_LEFT_INST_INT_IRQN                            (TIMG8_INT_IRQn)
/* Pin configuration defines for QEI_FRONT_LEFT PHA Pin */
#define GPIO_QEI_FRONT_LEFT_PHA_PORT                                       GPIOA
#define GPIO_QEI_FRONT_LEFT_PHA_PIN                               DL_GPIO_PIN_26
#define GPIO_QEI_FRONT_LEFT_PHA_IOMUX                            (IOMUX_PINCM59)
#define GPIO_QEI_FRONT_LEFT_PHA_IOMUX_FUNC             IOMUX_PINCM59_PF_TIMG8_CCP0
/* Pin configuration defines for QEI_FRONT_LEFT PHB Pin */
#define GPIO_QEI_FRONT_LEFT_PHB_PORT                                       GPIOA
#define GPIO_QEI_FRONT_LEFT_PHB_PIN                               DL_GPIO_PIN_27
#define GPIO_QEI_FRONT_LEFT_PHB_IOMUX                            (IOMUX_PINCM60)
#define GPIO_QEI_FRONT_LEFT_PHB_IOMUX_FUNC             IOMUX_PINCM60_PF_TIMG8_CCP1

/* Defines for QEI_FRONT_RIGHT */
#define QEI_FRONT_RIGHT_INST                                               TIMG9
#define QEI_FRONT_RIGHT_INST_IRQHandler                         TIMG9_IRQHandler
#define QEI_FRONT_RIGHT_INST_INT_IRQN                           (TIMG9_INT_IRQn)
/* Pin configuration defines for QEI_FRONT_RIGHT PHA Pin */
#define GPIO_QEI_FRONT_RIGHT_PHA_PORT                                      GPIOB
#define GPIO_QEI_FRONT_RIGHT_PHA_PIN                              DL_GPIO_PIN_29
#define GPIO_QEI_FRONT_RIGHT_PHA_IOMUX                           (IOMUX_PINCM66)
#define GPIO_QEI_FRONT_RIGHT_PHA_IOMUX_FUNC             IOMUX_PINCM66_PF_TIMG9_CCP0
/* Pin configuration defines for QEI_FRONT_RIGHT PHB Pin */
#define GPIO_QEI_FRONT_RIGHT_PHB_PORT                                      GPIOB
#define GPIO_QEI_FRONT_RIGHT_PHB_PIN                              DL_GPIO_PIN_30
#define GPIO_QEI_FRONT_RIGHT_PHB_IOMUX                           (IOMUX_PINCM67)
#define GPIO_QEI_FRONT_RIGHT_PHB_IOMUX_FUNC             IOMUX_PINCM67_PF_TIMG9_CCP1



/* Defines for I2C_SENSOR */
#define I2C_SENSOR_INST                                                     I2C0
#define I2C_SENSOR_INST_IRQHandler                               I2C0_IRQHandler
#define I2C_SENSOR_INST_INT_IRQN                                   I2C0_INT_IRQn
#define I2C_SENSOR_BUS_SPEED_HZ                                           400000
#define GPIO_I2C_SENSOR_SDA_PORT                                           GPIOA
#define GPIO_I2C_SENSOR_SDA_PIN                                    DL_GPIO_PIN_0
#define GPIO_I2C_SENSOR_IOMUX_SDA                                 (IOMUX_PINCM1)
#define GPIO_I2C_SENSOR_IOMUX_SDA_FUNC                  IOMUX_PINCM1_PF_I2C0_SDA
#define GPIO_I2C_SENSOR_SCL_PORT                                           GPIOA
#define GPIO_I2C_SENSOR_SCL_PIN                                    DL_GPIO_PIN_1
#define GPIO_I2C_SENSOR_IOMUX_SCL                                 (IOMUX_PINCM2)
#define GPIO_I2C_SENSOR_IOMUX_SCL_FUNC                  IOMUX_PINCM2_PF_I2C0_SCL


/* Defines for UART_DEBUG */
#define UART_DEBUG_INST                                                     UART0
#define UART_DEBUG_INST_FREQUENCY                                        32000000
#define UART_DEBUG_INST_IRQHandler                               UART0_IRQHandler
#define UART_DEBUG_INST_INT_IRQN                                   UART0_INT_IRQn
#define GPIO_UART_DEBUG_RX_PORT                                             GPIOA
#define GPIO_UART_DEBUG_TX_PORT                                             GPIOA
#define GPIO_UART_DEBUG_RX_PIN                                     DL_GPIO_PIN_11
#define GPIO_UART_DEBUG_TX_PIN                                     DL_GPIO_PIN_10
#define GPIO_UART_DEBUG_IOMUX_RX                                  (IOMUX_PINCM22)
#define GPIO_UART_DEBUG_IOMUX_TX                                  (IOMUX_PINCM21)
#define GPIO_UART_DEBUG_IOMUX_RX_FUNC                   IOMUX_PINCM22_PF_UART0_RX
#define GPIO_UART_DEBUG_IOMUX_TX_FUNC                   IOMUX_PINCM21_PF_UART0_TX
#define UART_DEBUG_BAUD_RATE                                             (115200)
#define UART_DEBUG_IBRD_32_MHZ_115200_BAUD                                   (17)
#define UART_DEBUG_FBRD_32_MHZ_115200_BAUD                                   (23)
/* Defines for UART_K210 */
#define UART_K210_INST                                                     UART1
#define UART_K210_INST_FREQUENCY                                        32000000
#define UART_K210_INST_IRQHandler                               UART1_IRQHandler
#define UART_K210_INST_INT_IRQN                                   UART1_INT_IRQn
#define GPIO_UART_K210_RX_PORT                                             GPIOB
#define GPIO_UART_K210_TX_PORT                                             GPIOB
#define GPIO_UART_K210_RX_PIN                                      DL_GPIO_PIN_5
#define GPIO_UART_K210_TX_PIN                                      DL_GPIO_PIN_4
#define GPIO_UART_K210_IOMUX_RX                                  (IOMUX_PINCM18)
#define GPIO_UART_K210_IOMUX_TX                                  (IOMUX_PINCM17)
#define GPIO_UART_K210_IOMUX_RX_FUNC                   IOMUX_PINCM18_PF_UART1_RX
#define GPIO_UART_K210_IOMUX_TX_FUNC                   IOMUX_PINCM17_PF_UART1_TX
#define UART_K210_BAUD_RATE                                             (115200)
#define UART_K210_IBRD_32_MHZ_115200_BAUD                                   (17)
#define UART_K210_FBRD_32_MHZ_115200_BAUD                                   (23)
/* Defines for UART_E220 */
#define UART_E220_INST                                                     UART4
#define UART_E220_INST_FREQUENCY                                        32000000
#define UART_E220_INST_IRQHandler                               UART4_IRQHandler
#define UART_E220_INST_INT_IRQN                                   UART4_INT_IRQn
#define GPIO_UART_E220_RX_PORT                                             GPIOB
#define GPIO_UART_E220_TX_PORT                                             GPIOB
#define GPIO_UART_E220_RX_PIN                                     DL_GPIO_PIN_11
#define GPIO_UART_E220_TX_PIN                                     DL_GPIO_PIN_10
#define GPIO_UART_E220_IOMUX_RX                                  (IOMUX_PINCM28)
#define GPIO_UART_E220_IOMUX_TX                                  (IOMUX_PINCM27)
#define GPIO_UART_E220_IOMUX_RX_FUNC                   IOMUX_PINCM28_PF_UART4_RX
#define GPIO_UART_E220_IOMUX_TX_FUNC                   IOMUX_PINCM27_PF_UART4_TX
#define UART_E220_BAUD_RATE                                             (115200)
#define UART_E220_IBRD_32_MHZ_115200_BAUD                                   (17)
#define UART_E220_FBRD_32_MHZ_115200_BAUD                                   (23)




/* Defines for SPI_DISPLAY */
#define SPI_DISPLAY_INST                                                   SPI0
#define SPI_DISPLAY_INST_IRQHandler                             SPI0_IRQHandler
#define SPI_DISPLAY_INST_INT_IRQN                                 SPI0_INT_IRQn
#define GPIO_SPI_DISPLAY_PICO_PORT                                        GPIOB
#define GPIO_SPI_DISPLAY_PICO_PIN                                 DL_GPIO_PIN_2
#define GPIO_SPI_DISPLAY_IOMUX_PICO                             (IOMUX_PINCM15)
#define GPIO_SPI_DISPLAY_IOMUX_PICO_FUNC             IOMUX_PINCM15_PF_SPI0_PICO
/* GPIO configuration for SPI_DISPLAY */
#define GPIO_SPI_DISPLAY_SCLK_PORT                                        GPIOB
#define GPIO_SPI_DISPLAY_SCLK_PIN                                 DL_GPIO_PIN_3
#define GPIO_SPI_DISPLAY_IOMUX_SCLK                             (IOMUX_PINCM16)
#define GPIO_SPI_DISPLAY_IOMUX_SCLK_FUNC             IOMUX_PINCM16_PF_SPI0_SCLK
#define GPIO_SPI_DISPLAY_CS0_PORT                                         GPIOA
#define GPIO_SPI_DISPLAY_CS0_PIN                                 DL_GPIO_PIN_18
#define GPIO_SPI_DISPLAY_IOMUX_CS0                              (IOMUX_PINCM40)
#define GPIO_SPI_DISPLAY_IOMUX_CS0_FUNC               IOMUX_PINCM40_PF_SPI0_CS0
/* Defines for SPI_ICM20948 */
#define SPI_ICM20948_INST                                                  SPI1
#define SPI_ICM20948_INST_IRQHandler                            SPI1_IRQHandler
#define SPI_ICM20948_INST_INT_IRQN                                SPI1_INT_IRQn
#define GPIO_SPI_ICM20948_PICO_PORT                                       GPIOB
#define GPIO_SPI_ICM20948_PICO_PIN                               DL_GPIO_PIN_15
#define GPIO_SPI_ICM20948_IOMUX_PICO                            (IOMUX_PINCM32)
#define GPIO_SPI_ICM20948_IOMUX_PICO_FUNC            IOMUX_PINCM32_PF_SPI1_PICO
#define GPIO_SPI_ICM20948_POCI_PORT                                       GPIOB
#define GPIO_SPI_ICM20948_POCI_PIN                               DL_GPIO_PIN_14
#define GPIO_SPI_ICM20948_IOMUX_POCI                            (IOMUX_PINCM31)
#define GPIO_SPI_ICM20948_IOMUX_POCI_FUNC            IOMUX_PINCM31_PF_SPI1_POCI
/* GPIO configuration for SPI_ICM20948 */
#define GPIO_SPI_ICM20948_SCLK_PORT                                       GPIOB
#define GPIO_SPI_ICM20948_SCLK_PIN                               DL_GPIO_PIN_16
#define GPIO_SPI_ICM20948_IOMUX_SCLK                            (IOMUX_PINCM33)
#define GPIO_SPI_ICM20948_IOMUX_SCLK_FUNC            IOMUX_PINCM33_PF_SPI1_SCLK
#define GPIO_SPI_ICM20948_CS0_PORT                                        GPIOA
#define GPIO_SPI_ICM20948_CS0_PIN                                 DL_GPIO_PIN_2
#define GPIO_SPI_ICM20948_IOMUX_CS0                              (IOMUX_PINCM7)
#define GPIO_SPI_ICM20948_IOMUX_CS0_FUNC               IOMUX_PINCM7_PF_SPI1_CS0



/* Defines for ADC_GRAY */
#define ADC_GRAY_INST                                                       ADC0
#define ADC_GRAY_INST_IRQHandler                                 ADC0_IRQHandler
#define ADC_GRAY_INST_INT_IRQN                                   (ADC0_INT_IRQn)
#define ADC_GRAY_ADCMEM_0                                     DL_ADC12_MEM_IDX_0
#define ADC_GRAY_ADCMEM_0_REF               DL_ADC12_REFERENCE_VOLTAGE_VDDA_VSSA
#define GPIO_ADC_GRAY_C2_PORT                                              GPIOA
#define GPIO_ADC_GRAY_C2_PIN                                      DL_GPIO_PIN_25
#define GPIO_ADC_GRAY_IOMUX_C2                                   (IOMUX_PINCM55)
#define GPIO_ADC_GRAY_IOMUX_C2_FUNC               (IOMUX_PINCM55_PF_UNCONNECTED)



/* Defines for DMA_CH2 */
#define DMA_CH2_CHAN_ID                                                      (2)
#define I2C_SENSOR_INST_DMA_TRIGGER_0                         (DMA_I2C0_TX_TRIG)
/* Defines for DMA_CH3 */
#define DMA_CH3_CHAN_ID                                                      (3)
#define I2C_SENSOR_INST_DMA_TRIGGER_1                         (DMA_I2C0_RX_TRIG)
/* Defines for DMA_CH1 */
#define DMA_CH1_CHAN_ID                                                      (1)
#define SPI_DISPLAY_INST_DMA_TRIGGER_0                        (DMA_SPI0_RX_TRIG)
/* Defines for DMA_CH0 */
#define DMA_CH0_CHAN_ID                                                      (0)
#define SPI_DISPLAY_INST_DMA_TRIGGER_1                        (DMA_SPI0_TX_TRIG)


/* Port definition for Pin Group GPIO_BOARD_OUTPUTS */
#define GPIO_BOARD_OUTPUTS_PORT                                          (GPIOA)

/* Defines for LED1: GPIOA.14 with pinCMx 36 on package pin 43 */
#define GPIO_BOARD_OUTPUTS_LED1_PIN                             (DL_GPIO_PIN_14)
#define GPIO_BOARD_OUTPUTS_LED1_IOMUX                            (IOMUX_PINCM36)
/* Defines for LED2: GPIOA.17 with pinCMx 39 on package pin 54 */
#define GPIO_BOARD_OUTPUTS_LED2_PIN                             (DL_GPIO_PIN_17)
#define GPIO_BOARD_OUTPUTS_LED2_IOMUX                            (IOMUX_PINCM39)
/* Defines for RGB_DATA: GPIOA.29 with pinCMx 4 on package pin 4 */
#define GPIO_BOARD_OUTPUTS_RGB_DATA_PIN                         (DL_GPIO_PIN_29)
#define GPIO_BOARD_OUTPUTS_RGB_DATA_IOMUX                         (IOMUX_PINCM4)
/* Defines for LCD_BL: GPIOA.30 with pinCMx 5 on package pin 5 */
#define GPIO_BOARD_OUTPUTS_LCD_BL_PIN                           (DL_GPIO_PIN_30)
#define GPIO_BOARD_OUTPUTS_LCD_BL_IOMUX                           (IOMUX_PINCM5)
/* Defines for GRAY_S0: GPIOA.24 with pinCMx 54 on package pin 73 */
#define GPIO_BOARD_OUTPUTS_GRAY_S0_PIN                          (DL_GPIO_PIN_24)
#define GPIO_BOARD_OUTPUTS_GRAY_S0_IOMUX                         (IOMUX_PINCM54)
/* Defines for GRAY_S1: GPIOA.31 with pinCMx 6 on package pin 7 */
#define GPIO_BOARD_OUTPUTS_GRAY_S1_PIN                          (DL_GPIO_PIN_31)
#define GPIO_BOARD_OUTPUTS_GRAY_S1_IOMUX                          (IOMUX_PINCM6)
/* Defines for HX711_SCK: GPIOA.7 with pinCMx 14 on package pin 17 */
#define GPIO_BOARD_OUTPUTS_HX711_SCK_PIN                         (DL_GPIO_PIN_7)
#define GPIO_BOARD_OUTPUTS_HX711_SCK_IOMUX                       (IOMUX_PINCM14)
/* Defines for BUZZER: GPIOA.15 with pinCMx 37 on package pin 44 */
#define GPIO_BOARD_OUTPUTS_BUZZER_PIN                           (DL_GPIO_PIN_15)
#define GPIO_BOARD_OUTPUTS_BUZZER_IOMUX                          (IOMUX_PINCM37)
/* Port definition for Pin Group GPIO_BOARD_IO */
#define GPIO_BOARD_IO_PORT                                               (GPIOB)

/* Defines for ICM20948_CS: GPIOB.12 with pinCMx 29 on package pin 36 */
#define GPIO_BOARD_IO_ICM20948_CS_PIN                           (DL_GPIO_PIN_12)
#define GPIO_BOARD_IO_ICM20948_CS_IOMUX                          (IOMUX_PINCM29)
/* Defines for ICM20948_INT: GPIOB.21 with pinCMx 49 on package pin 68 */
// pins affected by this interrupt request:["ICM20948_INT","ENCODER_RL_A","ENCODER_RL_B","ENCODER_RR_A","ENCODER_RR_B"]
#define GPIO_BOARD_IO_INT_IRQN                                  (GPIOB_INT_IRQn)
#define GPIO_BOARD_IO_INT_IIDX                  (DL_INTERRUPT_GROUP1_IIDX_GPIOB)
#define GPIO_BOARD_IO_ICM20948_INT_IIDX                     (DL_GPIO_IIDX_DIO21)
#define GPIO_BOARD_IO_ICM20948_INT_PIN                          (DL_GPIO_PIN_21)
#define GPIO_BOARD_IO_ICM20948_INT_IOMUX                         (IOMUX_PINCM49)
/* Defines for USER_KEY: GPIOB.31 with pinCMx 68 on package pin 27 */
#define GPIO_BOARD_IO_USER_KEY_PIN                              (DL_GPIO_PIN_31)
#define GPIO_BOARD_IO_USER_KEY_IOMUX                             (IOMUX_PINCM68)
/* Defines for MOTOR_FL_IN1: GPIOB.6 with pinCMx 23 on package pin 30 */
#define GPIO_BOARD_IO_MOTOR_FL_IN1_PIN                           (DL_GPIO_PIN_6)
#define GPIO_BOARD_IO_MOTOR_FL_IN1_IOMUX                         (IOMUX_PINCM23)
/* Defines for MOTOR_FL_IN2: GPIOB.7 with pinCMx 24 on package pin 31 */
#define GPIO_BOARD_IO_MOTOR_FL_IN2_PIN                           (DL_GPIO_PIN_7)
#define GPIO_BOARD_IO_MOTOR_FL_IN2_IOMUX                         (IOMUX_PINCM24)
/* Defines for MOTOR_FR_IN1: GPIOB.8 with pinCMx 25 on package pin 32 */
#define GPIO_BOARD_IO_MOTOR_FR_IN1_PIN                           (DL_GPIO_PIN_8)
#define GPIO_BOARD_IO_MOTOR_FR_IN1_IOMUX                         (IOMUX_PINCM25)
/* Defines for MOTOR_FR_IN2: GPIOB.9 with pinCMx 26 on package pin 33 */
#define GPIO_BOARD_IO_MOTOR_FR_IN2_PIN                           (DL_GPIO_PIN_9)
#define GPIO_BOARD_IO_MOTOR_FR_IN2_IOMUX                         (IOMUX_PINCM26)
/* Defines for MOTOR_RL_IN1: GPIOB.20 with pinCMx 48 on package pin 67 */
#define GPIO_BOARD_IO_MOTOR_RL_IN1_PIN                          (DL_GPIO_PIN_20)
#define GPIO_BOARD_IO_MOTOR_RL_IN1_IOMUX                         (IOMUX_PINCM48)
/* Defines for MOTOR_RL_IN2: GPIOB.24 with pinCMx 52 on package pin 71 */
#define GPIO_BOARD_IO_MOTOR_RL_IN2_PIN                          (DL_GPIO_PIN_24)
#define GPIO_BOARD_IO_MOTOR_RL_IN2_IOMUX                         (IOMUX_PINCM52)
/* Defines for MOTOR_RR_IN1: GPIOB.25 with pinCMx 56 on package pin 75 */
#define GPIO_BOARD_IO_MOTOR_RR_IN1_PIN                          (DL_GPIO_PIN_25)
#define GPIO_BOARD_IO_MOTOR_RR_IN1_IOMUX                         (IOMUX_PINCM56)
/* Defines for MOTOR_RR_IN2: GPIOB.27 with pinCMx 58 on package pin 77 */
#define GPIO_BOARD_IO_MOTOR_RR_IN2_PIN                          (DL_GPIO_PIN_27)
#define GPIO_BOARD_IO_MOTOR_RR_IN2_IOMUX                         (IOMUX_PINCM58)
/* Defines for ENCODER_RL_A: GPIOB.17 with pinCMx 43 on package pin 58 */
#define GPIO_BOARD_IO_ENCODER_RL_A_IIDX                     (DL_GPIO_IIDX_DIO17)
#define GPIO_BOARD_IO_ENCODER_RL_A_PIN                          (DL_GPIO_PIN_17)
#define GPIO_BOARD_IO_ENCODER_RL_A_IOMUX                         (IOMUX_PINCM43)
/* Defines for ENCODER_RL_B: GPIOB.18 with pinCMx 44 on package pin 59 */
#define GPIO_BOARD_IO_ENCODER_RL_B_IIDX                     (DL_GPIO_IIDX_DIO18)
#define GPIO_BOARD_IO_ENCODER_RL_B_PIN                          (DL_GPIO_PIN_18)
#define GPIO_BOARD_IO_ENCODER_RL_B_IOMUX                         (IOMUX_PINCM44)
/* Defines for ENCODER_RR_A: GPIOB.19 with pinCMx 45 on package pin 60 */
#define GPIO_BOARD_IO_ENCODER_RR_A_IIDX                     (DL_GPIO_IIDX_DIO19)
#define GPIO_BOARD_IO_ENCODER_RR_A_PIN                          (DL_GPIO_PIN_19)
#define GPIO_BOARD_IO_ENCODER_RR_A_IOMUX                         (IOMUX_PINCM45)
/* Defines for ENCODER_RR_B: GPIOB.22 with pinCMx 50 on package pin 69 */
#define GPIO_BOARD_IO_ENCODER_RR_B_IIDX                     (DL_GPIO_IIDX_DIO22)
#define GPIO_BOARD_IO_ENCODER_RR_B_PIN                          (DL_GPIO_PIN_22)
#define GPIO_BOARD_IO_ENCODER_RR_B_IOMUX                         (IOMUX_PINCM50)
/* Defines for LCD_RESET: GPIOB.23 with pinCMx 51 on package pin 70 */
#define GPIO_BOARD_IO_LCD_RESET_PIN                             (DL_GPIO_PIN_23)
#define GPIO_BOARD_IO_LCD_RESET_IOMUX                            (IOMUX_PINCM51)
/* Defines for E220_AUX: GPIOB.28 with pinCMx 65 on package pin 24 */
#define GPIO_BOARD_IO_E220_AUX_PIN                              (DL_GPIO_PIN_28)
#define GPIO_BOARD_IO_E220_AUX_IOMUX                             (IOMUX_PINCM65)
/* Port definition for Pin Group GPIO_DISPLAY_GRAY */
#define GPIO_DISPLAY_GRAY_PORT                                           (GPIOC)

/* Defines for LCD_DC: GPIOC.8 with pinCMx 86 on package pin 65 */
#define GPIO_DISPLAY_GRAY_LCD_DC_PIN                             (DL_GPIO_PIN_8)
#define GPIO_DISPLAY_GRAY_LCD_DC_IOMUX                           (IOMUX_PINCM86)
/* Defines for LCD_CS: GPIOC.9 with pinCMx 87 on package pin 66 */
#define GPIO_DISPLAY_GRAY_LCD_CS_PIN                             (DL_GPIO_PIN_9)
#define GPIO_DISPLAY_GRAY_LCD_CS_IOMUX                           (IOMUX_PINCM87)
/* Defines for GRAY_S2: GPIOC.1 with pinCMx 75 on package pin 47 */
#define GPIO_DISPLAY_GRAY_GRAY_S2_PIN                            (DL_GPIO_PIN_1)
#define GPIO_DISPLAY_GRAY_GRAY_S2_IOMUX                          (IOMUX_PINCM75)
/* Defines for LASER_EN: GPIOC.6 with pinCMx 84 on package pin 63 */
#define GPIO_DISPLAY_GRAY_LASER_EN_PIN                           (DL_GPIO_PIN_6)
#define GPIO_DISPLAY_GRAY_LASER_EN_IOMUX                         (IOMUX_PINCM84)
/* Defines for HX711_DOUT: GPIOC.7 with pinCMx 85 on package pin 64 */
#define GPIO_DISPLAY_GRAY_HX711_DOUT_PIN                         (DL_GPIO_PIN_7)
#define GPIO_DISPLAY_GRAY_HX711_DOUT_IOMUX                       (IOMUX_PINCM85)




/* clang-format on */

void SYSCFG_DL_init(void);
void SYSCFG_DL_initPower(void);
void SYSCFG_DL_GPIO_init(void);
void SYSCFG_DL_SYSCTL_init(void);
void SYSCFG_DL_PWM_MOTOR_init(void);
void SYSCFG_DL_PWM_SERVO_init(void);
void SYSCFG_DL_QEI_FRONT_LEFT_init(void);
void SYSCFG_DL_QEI_FRONT_RIGHT_init(void);
void SYSCFG_DL_I2C_SENSOR_init(void);
void SYSCFG_DL_UART_DEBUG_init(void);
void SYSCFG_DL_UART_K210_init(void);
void SYSCFG_DL_UART_E220_init(void);
void SYSCFG_DL_SPI_DISPLAY_init(void);
void SYSCFG_DL_SPI_ICM20948_init(void);
void SYSCFG_DL_ADC_GRAY_init(void);
void SYSCFG_DL_DMA_init(void);

void SYSCFG_DL_SYSTICK_init(void);

bool SYSCFG_DL_saveConfiguration(void);
bool SYSCFG_DL_restoreConfiguration(void);

#ifdef __cplusplus
}
#endif

#endif /* ti_msp_dl_config_h */
