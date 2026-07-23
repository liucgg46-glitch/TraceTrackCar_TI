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
/* Port definition for Pin Group GPIO_BOARD_IO */
#define GPIO_BOARD_IO_PORT                                               (GPIOB)

/* Defines for ICM20948_CS: GPIOB.12 with pinCMx 29 on package pin 36 */
#define GPIO_BOARD_IO_ICM20948_CS_PIN                           (DL_GPIO_PIN_12)
#define GPIO_BOARD_IO_ICM20948_CS_IOMUX                          (IOMUX_PINCM29)
/* Defines for ICM20948_INT: GPIOB.21 with pinCMx 49 on package pin 68 */
#define GPIO_BOARD_IO_ICM20948_INT_PIN                          (DL_GPIO_PIN_21)
#define GPIO_BOARD_IO_ICM20948_INT_IOMUX                         (IOMUX_PINCM49)
/* Defines for USER_KEY: GPIOB.31 with pinCMx 68 on package pin 27 */
#define GPIO_BOARD_IO_USER_KEY_PIN                              (DL_GPIO_PIN_31)
#define GPIO_BOARD_IO_USER_KEY_IOMUX                             (IOMUX_PINCM68)




/* clang-format on */

void SYSCFG_DL_init(void);
void SYSCFG_DL_initPower(void);
void SYSCFG_DL_GPIO_init(void);
void SYSCFG_DL_SYSCTL_init(void);

void SYSCFG_DL_SYSTICK_init(void);


#ifdef __cplusplus
}
#endif

#endif /* ti_msp_dl_config_h */
