/*
 * Copyright (c) 2023, Texas Instruments Incorporated
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
 *  ============ ti_msp_dl_config.c =============
 *  Configured MSPM0 DriverLib module definitions
 *
 *  DO NOT EDIT - This file is generated for the MSPM0G351X
 *  by the SysConfig tool.
 */

#include "ti_msp_dl_config.h"

DL_TimerA_backupConfig gPWM_MOTORBackup;
DL_TimerG_backupConfig gQEI_FRONT_LEFTBackup;
DL_SPI_backupConfig gSPI_DISPLAYBackup;
DL_SPI_backupConfig gSPI_ICM20948Backup;

/*
 *  ======== SYSCFG_DL_init ========
 *  Perform any initialization needed before using any board APIs
 */
SYSCONFIG_WEAK void SYSCFG_DL_init(void)
{
    SYSCFG_DL_initPower();
    SYSCFG_DL_GPIO_init();
    /* Module-Specific Initializations*/
    SYSCFG_DL_SYSCTL_init();
    SYSCFG_DL_PWM_MOTOR_init();
    SYSCFG_DL_PWM_SERVO_init();
    SYSCFG_DL_QEI_FRONT_LEFT_init();
    SYSCFG_DL_QEI_FRONT_RIGHT_init();
        SYSCFG_DL_I2C_SENSOR_init();
    SYSCFG_DL_UART_DEBUG_init();
    SYSCFG_DL_UART_K210_init();
    SYSCFG_DL_UART_E220_init();
    SYSCFG_DL_SPI_DISPLAY_init();
    SYSCFG_DL_SPI_ICM20948_init();
    SYSCFG_DL_ADC_GRAY_init();
    SYSCFG_DL_DMA_init();
    SYSCFG_DL_SYSTICK_init();
    /* Ensure backup structures have no valid state */
	gPWM_MOTORBackup.backupRdy 	= false;
	gQEI_FRONT_LEFTBackup.backupRdy 	= false;

	gSPI_DISPLAYBackup.backupRdy 	= false;
	gSPI_ICM20948Backup.backupRdy 	= false;

}
/*
 * User should take care to save and restore register configuration in application.
 * See Retention Configuration section for more details.
 */
SYSCONFIG_WEAK bool SYSCFG_DL_saveConfiguration(void)
{
    bool retStatus = true;

	retStatus &= DL_TimerA_saveConfiguration(PWM_MOTOR_INST, &gPWM_MOTORBackup);
	retStatus &= DL_TimerG_saveConfiguration(QEI_FRONT_LEFT_INST, &gQEI_FRONT_LEFTBackup);
	retStatus &= DL_SPI_saveConfiguration(SPI_DISPLAY_INST, &gSPI_DISPLAYBackup);
	retStatus &= DL_SPI_saveConfiguration(SPI_ICM20948_INST, &gSPI_ICM20948Backup);

    return retStatus;
}


SYSCONFIG_WEAK bool SYSCFG_DL_restoreConfiguration(void)
{
    bool retStatus = true;

	retStatus &= DL_TimerA_restoreConfiguration(PWM_MOTOR_INST, &gPWM_MOTORBackup, false);
	retStatus &= DL_TimerG_restoreConfiguration(QEI_FRONT_LEFT_INST, &gQEI_FRONT_LEFTBackup, false);
	retStatus &= DL_SPI_restoreConfiguration(SPI_DISPLAY_INST, &gSPI_DISPLAYBackup);
	retStatus &= DL_SPI_restoreConfiguration(SPI_ICM20948_INST, &gSPI_ICM20948Backup);

    return retStatus;
}

SYSCONFIG_WEAK void SYSCFG_DL_initPower(void)
{
    DL_GPIO_reset(GPIOA);
    DL_GPIO_reset(GPIOB);
    DL_GPIO_reset(GPIOC);
    DL_TimerA_reset(PWM_MOTOR_INST);
    DL_TimerG_reset(PWM_SERVO_INST);
    DL_TimerG_reset(QEI_FRONT_LEFT_INST);
    DL_TimerG_reset(QEI_FRONT_RIGHT_INST);
        DL_I2C_reset(I2C_SENSOR_INST);
    DL_UART_Main_reset(UART_DEBUG_INST);
    DL_UART_Main_reset(UART_K210_INST);
    DL_UART_Main_reset(UART_E220_INST);
    DL_SPI_reset(SPI_DISPLAY_INST);
    DL_SPI_reset(SPI_ICM20948_INST);
    DL_ADC12_reset(ADC_GRAY_INST);



    DL_GPIO_enablePower(GPIOA);
    DL_GPIO_enablePower(GPIOB);
    DL_GPIO_enablePower(GPIOC);
    DL_TimerA_enablePower(PWM_MOTOR_INST);
    DL_TimerG_enablePower(PWM_SERVO_INST);
    DL_TimerG_enablePower(QEI_FRONT_LEFT_INST);
    DL_TimerG_enablePower(QEI_FRONT_RIGHT_INST);
        DL_I2C_enablePower(I2C_SENSOR_INST);
    DL_UART_Main_enablePower(UART_DEBUG_INST);
    DL_UART_Main_enablePower(UART_K210_INST);
    DL_UART_Main_enablePower(UART_E220_INST);
    DL_SPI_enablePower(SPI_DISPLAY_INST);
    DL_SPI_enablePower(SPI_ICM20948_INST);
    DL_ADC12_enablePower(ADC_GRAY_INST);


    delay_cycles(POWER_STARTUP_DELAY);
}

SYSCONFIG_WEAK void SYSCFG_DL_GPIO_init(void)
{

    DL_GPIO_initPeripheralOutputFunction(GPIO_PWM_MOTOR_C0_IOMUX,GPIO_PWM_MOTOR_C0_IOMUX_FUNC);
    DL_GPIO_enableOutput(GPIO_PWM_MOTOR_C0_PORT, GPIO_PWM_MOTOR_C0_PIN);
    DL_GPIO_initPeripheralOutputFunction(GPIO_PWM_MOTOR_C1_IOMUX,GPIO_PWM_MOTOR_C1_IOMUX_FUNC);
    DL_GPIO_enableOutput(GPIO_PWM_MOTOR_C1_PORT, GPIO_PWM_MOTOR_C1_PIN);
    DL_GPIO_initPeripheralOutputFunction(GPIO_PWM_MOTOR_C2_IOMUX,GPIO_PWM_MOTOR_C2_IOMUX_FUNC);
    DL_GPIO_enableOutput(GPIO_PWM_MOTOR_C2_PORT, GPIO_PWM_MOTOR_C2_PIN);
    DL_GPIO_initPeripheralOutputFunction(GPIO_PWM_MOTOR_C3_IOMUX,GPIO_PWM_MOTOR_C3_IOMUX_FUNC);
    DL_GPIO_enableOutput(GPIO_PWM_MOTOR_C3_PORT, GPIO_PWM_MOTOR_C3_PIN);
    DL_GPIO_initPeripheralOutputFunction(GPIO_PWM_SERVO_C0_IOMUX,GPIO_PWM_SERVO_C0_IOMUX_FUNC);
    DL_GPIO_enableOutput(GPIO_PWM_SERVO_C0_PORT, GPIO_PWM_SERVO_C0_PIN);
    DL_GPIO_initPeripheralOutputFunction(GPIO_PWM_SERVO_C1_IOMUX,GPIO_PWM_SERVO_C1_IOMUX_FUNC);
    DL_GPIO_enableOutput(GPIO_PWM_SERVO_C1_PORT, GPIO_PWM_SERVO_C1_PIN);

    DL_GPIO_initPeripheralInputFunction(GPIO_QEI_FRONT_LEFT_PHA_IOMUX,GPIO_QEI_FRONT_LEFT_PHA_IOMUX_FUNC);
    DL_GPIO_initPeripheralInputFunction(GPIO_QEI_FRONT_LEFT_PHB_IOMUX,GPIO_QEI_FRONT_LEFT_PHB_IOMUX_FUNC);
    DL_GPIO_initPeripheralInputFunction(GPIO_QEI_FRONT_RIGHT_PHA_IOMUX,GPIO_QEI_FRONT_RIGHT_PHA_IOMUX_FUNC);
    DL_GPIO_initPeripheralInputFunction(GPIO_QEI_FRONT_RIGHT_PHB_IOMUX,GPIO_QEI_FRONT_RIGHT_PHB_IOMUX_FUNC);


	DL_GPIO_initPeripheralInputFunctionFeatures(
		 GPIO_I2C_SENSOR_IOMUX_SDA, GPIO_I2C_SENSOR_IOMUX_SDA_FUNC,
		 DL_GPIO_INVERSION_DISABLE, DL_GPIO_RESISTOR_NONE,
		 DL_GPIO_HYSTERESIS_DISABLE, DL_GPIO_WAKEUP_DISABLE);
	DL_GPIO_initPeripheralInputFunctionFeatures(
		 GPIO_I2C_SENSOR_IOMUX_SCL, GPIO_I2C_SENSOR_IOMUX_SCL_FUNC,
		 DL_GPIO_INVERSION_DISABLE, DL_GPIO_RESISTOR_NONE,
		 DL_GPIO_HYSTERESIS_DISABLE, DL_GPIO_WAKEUP_DISABLE);
    DL_GPIO_enableHiZ(GPIO_I2C_SENSOR_IOMUX_SDA);
    DL_GPIO_enableHiZ(GPIO_I2C_SENSOR_IOMUX_SCL);

        DL_GPIO_initPeripheralOutputFunction(
        GPIO_UART_DEBUG_IOMUX_TX, GPIO_UART_DEBUG_IOMUX_TX_FUNC);
    DL_GPIO_initPeripheralInputFunction(
        GPIO_UART_DEBUG_IOMUX_RX, GPIO_UART_DEBUG_IOMUX_RX_FUNC);
    DL_GPIO_initPeripheralOutputFunction(
        GPIO_UART_K210_IOMUX_TX, GPIO_UART_K210_IOMUX_TX_FUNC);
    DL_GPIO_initPeripheralInputFunction(
        GPIO_UART_K210_IOMUX_RX, GPIO_UART_K210_IOMUX_RX_FUNC);
    DL_GPIO_initPeripheralOutputFunction(
        GPIO_UART_E220_IOMUX_TX, GPIO_UART_E220_IOMUX_TX_FUNC);
    DL_GPIO_initPeripheralInputFunction(
        GPIO_UART_E220_IOMUX_RX, GPIO_UART_E220_IOMUX_RX_FUNC);

    DL_GPIO_initPeripheralOutputFunction(
        GPIO_SPI_DISPLAY_IOMUX_SCLK, GPIO_SPI_DISPLAY_IOMUX_SCLK_FUNC);
    DL_GPIO_initPeripheralOutputFunction(
        GPIO_SPI_DISPLAY_IOMUX_PICO, GPIO_SPI_DISPLAY_IOMUX_PICO_FUNC);
    DL_GPIO_initPeripheralOutputFunction(
        GPIO_SPI_DISPLAY_IOMUX_CS0, GPIO_SPI_DISPLAY_IOMUX_CS0_FUNC);
    DL_GPIO_initPeripheralOutputFunction(
        GPIO_SPI_ICM20948_IOMUX_SCLK, GPIO_SPI_ICM20948_IOMUX_SCLK_FUNC);
    DL_GPIO_initPeripheralOutputFunction(
        GPIO_SPI_ICM20948_IOMUX_PICO, GPIO_SPI_ICM20948_IOMUX_PICO_FUNC);
    DL_GPIO_initPeripheralInputFunction(
        GPIO_SPI_ICM20948_IOMUX_POCI, GPIO_SPI_ICM20948_IOMUX_POCI_FUNC);
    DL_GPIO_initPeripheralOutputFunction(
        GPIO_SPI_ICM20948_IOMUX_CS0, GPIO_SPI_ICM20948_IOMUX_CS0_FUNC);

    DL_GPIO_initDigitalOutput(GPIO_BOARD_OUTPUTS_LED1_IOMUX);

    DL_GPIO_initDigitalOutput(GPIO_BOARD_OUTPUTS_LED2_IOMUX);

    DL_GPIO_initDigitalOutput(GPIO_BOARD_OUTPUTS_RGB_DATA_IOMUX);

    DL_GPIO_initDigitalOutput(GPIO_BOARD_OUTPUTS_LCD_BL_IOMUX);

    DL_GPIO_initDigitalOutput(GPIO_BOARD_OUTPUTS_GRAY_S0_IOMUX);

    DL_GPIO_initDigitalOutput(GPIO_BOARD_OUTPUTS_GRAY_S1_IOMUX);

    DL_GPIO_initDigitalOutput(GPIO_BOARD_OUTPUTS_HX711_SCK_IOMUX);

    DL_GPIO_initDigitalOutput(GPIO_BOARD_OUTPUTS_BUZZER_IOMUX);

    DL_GPIO_initDigitalOutput(GPIO_BOARD_IO_ICM20948_CS_IOMUX);

    DL_GPIO_initDigitalInputFeatures(GPIO_BOARD_IO_ICM20948_INT_IOMUX,
		 DL_GPIO_INVERSION_DISABLE, DL_GPIO_RESISTOR_NONE,
		 DL_GPIO_HYSTERESIS_DISABLE, DL_GPIO_WAKEUP_DISABLE);

    DL_GPIO_initDigitalInputFeatures(GPIO_BOARD_IO_USER_KEY_IOMUX,
		 DL_GPIO_INVERSION_DISABLE, DL_GPIO_RESISTOR_PULL_UP,
		 DL_GPIO_HYSTERESIS_DISABLE, DL_GPIO_WAKEUP_DISABLE);

    DL_GPIO_initDigitalOutput(GPIO_BOARD_IO_MOTOR_FL_IN1_IOMUX);

    DL_GPIO_initDigitalOutput(GPIO_BOARD_IO_MOTOR_FL_IN2_IOMUX);

    DL_GPIO_initDigitalOutput(GPIO_BOARD_IO_MOTOR_FR_IN1_IOMUX);

    DL_GPIO_initDigitalOutput(GPIO_BOARD_IO_MOTOR_FR_IN2_IOMUX);

    DL_GPIO_initDigitalOutput(GPIO_BOARD_IO_MOTOR_RL_IN1_IOMUX);

    DL_GPIO_initDigitalOutput(GPIO_BOARD_IO_MOTOR_RL_IN2_IOMUX);

    DL_GPIO_initDigitalOutput(GPIO_BOARD_IO_MOTOR_RR_IN1_IOMUX);

    DL_GPIO_initDigitalOutput(GPIO_BOARD_IO_MOTOR_RR_IN2_IOMUX);

    DL_GPIO_initDigitalInputFeatures(GPIO_BOARD_IO_ENCODER_RL_A_IOMUX,
		 DL_GPIO_INVERSION_DISABLE, DL_GPIO_RESISTOR_PULL_UP,
		 DL_GPIO_HYSTERESIS_DISABLE, DL_GPIO_WAKEUP_DISABLE);

    DL_GPIO_initDigitalInputFeatures(GPIO_BOARD_IO_ENCODER_RL_B_IOMUX,
		 DL_GPIO_INVERSION_DISABLE, DL_GPIO_RESISTOR_PULL_UP,
		 DL_GPIO_HYSTERESIS_DISABLE, DL_GPIO_WAKEUP_DISABLE);

    DL_GPIO_initDigitalInputFeatures(GPIO_BOARD_IO_ENCODER_RR_A_IOMUX,
		 DL_GPIO_INVERSION_DISABLE, DL_GPIO_RESISTOR_PULL_UP,
		 DL_GPIO_HYSTERESIS_DISABLE, DL_GPIO_WAKEUP_DISABLE);

    DL_GPIO_initDigitalInputFeatures(GPIO_BOARD_IO_ENCODER_RR_B_IOMUX,
		 DL_GPIO_INVERSION_DISABLE, DL_GPIO_RESISTOR_PULL_UP,
		 DL_GPIO_HYSTERESIS_DISABLE, DL_GPIO_WAKEUP_DISABLE);

    DL_GPIO_initDigitalOutput(GPIO_BOARD_IO_LCD_RESET_IOMUX);

    DL_GPIO_initDigitalInputFeatures(GPIO_BOARD_IO_E220_AUX_IOMUX,
		 DL_GPIO_INVERSION_DISABLE, DL_GPIO_RESISTOR_PULL_UP,
		 DL_GPIO_HYSTERESIS_DISABLE, DL_GPIO_WAKEUP_DISABLE);

    DL_GPIO_initDigitalOutput(GPIO_DISPLAY_GRAY_LCD_DC_IOMUX);

    DL_GPIO_initDigitalOutput(GPIO_DISPLAY_GRAY_LCD_CS_IOMUX);

    DL_GPIO_initDigitalOutput(GPIO_DISPLAY_GRAY_GRAY_S2_IOMUX);

    DL_GPIO_initDigitalOutput(GPIO_DISPLAY_GRAY_LASER_EN_IOMUX);

    DL_GPIO_initDigitalInputFeatures(GPIO_DISPLAY_GRAY_HX711_DOUT_IOMUX,
		 DL_GPIO_INVERSION_DISABLE, DL_GPIO_RESISTOR_PULL_UP,
		 DL_GPIO_HYSTERESIS_DISABLE, DL_GPIO_WAKEUP_DISABLE);

    DL_GPIO_initDigitalInputFeatures(GPIO_BOARD_OUTPUTS_KEY4_IOMUX,
         DL_GPIO_INVERSION_DISABLE, DL_GPIO_RESISTOR_PULL_UP,
         DL_GPIO_HYSTERESIS_DISABLE, DL_GPIO_WAKEUP_DISABLE);
    DL_GPIO_initDigitalInputFeatures(GPIO_BOARD_IO_KEY1_IOMUX,
         DL_GPIO_INVERSION_DISABLE, DL_GPIO_RESISTOR_PULL_UP,
         DL_GPIO_HYSTERESIS_DISABLE, DL_GPIO_WAKEUP_DISABLE);

    DL_GPIO_initDigitalInputFeatures(GPIO_BOARD_IO_KEY2_IOMUX,
         DL_GPIO_INVERSION_DISABLE, DL_GPIO_RESISTOR_PULL_UP,
         DL_GPIO_HYSTERESIS_DISABLE, DL_GPIO_WAKEUP_DISABLE);

    DL_GPIO_initDigitalInputFeatures(GPIO_BOARD_IO_KEY3_IOMUX,
         DL_GPIO_INVERSION_DISABLE, DL_GPIO_RESISTOR_PULL_UP,
         DL_GPIO_HYSTERESIS_DISABLE, DL_GPIO_WAKEUP_DISABLE);
    DL_GPIO_clearPins(GPIO_BOARD_OUTPUTS_PORT, GPIO_BOARD_OUTPUTS_RGB_DATA_PIN |
		GPIO_BOARD_OUTPUTS_LCD_BL_PIN |
		GPIO_BOARD_OUTPUTS_GRAY_S0_PIN |
		GPIO_BOARD_OUTPUTS_GRAY_S1_PIN |
		GPIO_BOARD_OUTPUTS_HX711_SCK_PIN);
    DL_GPIO_setPins(GPIO_BOARD_OUTPUTS_PORT, GPIO_BOARD_OUTPUTS_LED1_PIN |
		GPIO_BOARD_OUTPUTS_LED2_PIN |
		GPIO_BOARD_OUTPUTS_BUZZER_PIN);
    DL_GPIO_enableOutput(GPIO_BOARD_OUTPUTS_PORT, GPIO_BOARD_OUTPUTS_LED1_PIN |
		GPIO_BOARD_OUTPUTS_LED2_PIN |
		GPIO_BOARD_OUTPUTS_RGB_DATA_PIN |
		GPIO_BOARD_OUTPUTS_LCD_BL_PIN |
		GPIO_BOARD_OUTPUTS_GRAY_S0_PIN |
		GPIO_BOARD_OUTPUTS_GRAY_S1_PIN |
		GPIO_BOARD_OUTPUTS_HX711_SCK_PIN |
		GPIO_BOARD_OUTPUTS_BUZZER_PIN);
    DL_GPIO_clearPins(GPIO_BOARD_IO_PORT, GPIO_BOARD_IO_MOTOR_FL_IN1_PIN |
		GPIO_BOARD_IO_MOTOR_FL_IN2_PIN |
		GPIO_BOARD_IO_MOTOR_FR_IN1_PIN |
		GPIO_BOARD_IO_MOTOR_FR_IN2_PIN |
		GPIO_BOARD_IO_MOTOR_RL_IN1_PIN |
		GPIO_BOARD_IO_MOTOR_RL_IN2_PIN |
		GPIO_BOARD_IO_MOTOR_RR_IN1_PIN |
		GPIO_BOARD_IO_MOTOR_RR_IN2_PIN);
    DL_GPIO_setPins(GPIO_BOARD_IO_PORT, GPIO_BOARD_IO_ICM20948_CS_PIN |
		GPIO_BOARD_IO_LCD_RESET_PIN);
    DL_GPIO_enableOutput(GPIO_BOARD_IO_PORT, GPIO_BOARD_IO_ICM20948_CS_PIN |
		GPIO_BOARD_IO_MOTOR_FL_IN1_PIN |
		GPIO_BOARD_IO_MOTOR_FL_IN2_PIN |
		GPIO_BOARD_IO_MOTOR_FR_IN1_PIN |
		GPIO_BOARD_IO_MOTOR_FR_IN2_PIN |
		GPIO_BOARD_IO_MOTOR_RL_IN1_PIN |
		GPIO_BOARD_IO_MOTOR_RL_IN2_PIN |
		GPIO_BOARD_IO_MOTOR_RR_IN1_PIN |
		GPIO_BOARD_IO_MOTOR_RR_IN2_PIN |
		GPIO_BOARD_IO_LCD_RESET_PIN);
    DL_GPIO_setUpperPinsPolarity(GPIO_BOARD_IO_PORT, DL_GPIO_PIN_21_EDGE_RISE_FALL |
		DL_GPIO_PIN_17_EDGE_RISE_FALL |
		DL_GPIO_PIN_18_EDGE_RISE_FALL |
		DL_GPIO_PIN_19_EDGE_RISE_FALL |
		DL_GPIO_PIN_22_EDGE_RISE_FALL);
    DL_GPIO_setUpperPinsInputFilter(GPIO_BOARD_IO_PORT, DL_GPIO_PIN_17_INPUT_FILTER_3_CYCLES |
		DL_GPIO_PIN_18_INPUT_FILTER_3_CYCLES |
		DL_GPIO_PIN_19_INPUT_FILTER_3_CYCLES |
		DL_GPIO_PIN_22_INPUT_FILTER_3_CYCLES);
    DL_GPIO_clearInterruptStatus(GPIO_BOARD_IO_PORT, GPIO_BOARD_IO_ICM20948_INT_PIN |
		GPIO_BOARD_IO_ENCODER_RL_A_PIN |
		GPIO_BOARD_IO_ENCODER_RL_B_PIN |
		GPIO_BOARD_IO_ENCODER_RR_A_PIN |
		GPIO_BOARD_IO_ENCODER_RR_B_PIN);
    DL_GPIO_enableInterrupt(GPIO_BOARD_IO_PORT, GPIO_BOARD_IO_ICM20948_INT_PIN |
		GPIO_BOARD_IO_ENCODER_RL_A_PIN |
		GPIO_BOARD_IO_ENCODER_RL_B_PIN |
		GPIO_BOARD_IO_ENCODER_RR_A_PIN |
		GPIO_BOARD_IO_ENCODER_RR_B_PIN);
    DL_GPIO_clearPins(GPIO_DISPLAY_GRAY_PORT, GPIO_DISPLAY_GRAY_LCD_DC_PIN |
		GPIO_DISPLAY_GRAY_GRAY_S2_PIN |
		GPIO_DISPLAY_GRAY_LASER_EN_PIN);
    DL_GPIO_setPins(GPIO_DISPLAY_GRAY_PORT, GPIO_DISPLAY_GRAY_LCD_CS_PIN);
    DL_GPIO_enableOutput(GPIO_DISPLAY_GRAY_PORT, GPIO_DISPLAY_GRAY_LCD_DC_PIN |
		GPIO_DISPLAY_GRAY_LCD_CS_PIN |
		GPIO_DISPLAY_GRAY_GRAY_S2_PIN |
		GPIO_DISPLAY_GRAY_LASER_EN_PIN);

}



SYSCONFIG_WEAK void SYSCFG_DL_SYSCTL_init(void)
{

	//Low Power Mode is configured to be SLEEP0
    DL_SYSCTL_setBORThreshold(DL_SYSCTL_BOR_THRESHOLD_LEVEL_0);


	DL_SYSCTL_setSYSOSCFreq(DL_SYSCTL_SYSOSC_FREQ_BASE);
	/* Set default configuration */
	DL_SYSCTL_disableHFXT();
	DL_SYSCTL_disableSYSPLL();

}


/*
 * Timer clock configuration to be sourced by  / 1 (32000000 Hz)
 * timerClkFreq = (timerClkSrc / (timerClkDivRatio * (timerClkPrescale + 1)))
 *   32000000 Hz = 32000000 Hz / (1 * (0 + 1))
 */
static const DL_TimerA_ClockConfig gPWM_MOTORClockConfig = {
    .clockSel = DL_TIMER_CLOCK_BUSCLK,
    .divideRatio = DL_TIMER_CLOCK_DIVIDE_1,
    .prescale = 0U
};

static const DL_TimerA_PWMConfig gPWM_MOTORConfig = {
    .pwmMode = DL_TIMER_PWM_MODE_EDGE_ALIGN_UP,
    .period = 1600,
    .isTimerWithFourCC = true,
    .startTimer = DL_TIMER_START,
};

SYSCONFIG_WEAK void SYSCFG_DL_PWM_MOTOR_init(void) {

    DL_TimerA_setClockConfig(
        PWM_MOTOR_INST, (DL_TimerA_ClockConfig *) &gPWM_MOTORClockConfig);

    DL_TimerA_initPWMMode(
        PWM_MOTOR_INST, (DL_TimerA_PWMConfig *) &gPWM_MOTORConfig);

    // Set Counter control to the smallest CC index being used
    DL_TimerA_setCounterControl(PWM_MOTOR_INST,DL_TIMER_CZC_CCCTL0_ZCOND,DL_TIMER_CAC_CCCTL0_ACOND,DL_TIMER_CLC_CCCTL0_LCOND);

    DL_TimerA_setCaptureCompareOutCtl(PWM_MOTOR_INST, DL_TIMER_CC_OCTL_INIT_VAL_LOW,
		DL_TIMER_CC_OCTL_INV_OUT_DISABLED, DL_TIMER_CC_OCTL_SRC_FUNCVAL,
		DL_TIMERA_CAPTURE_COMPARE_0_INDEX);

    DL_TimerA_setCaptCompUpdateMethod(PWM_MOTOR_INST, DL_TIMER_CC_UPDATE_METHOD_IMMEDIATE, DL_TIMERA_CAPTURE_COMPARE_0_INDEX);
    DL_TimerA_setCaptureCompareValue(PWM_MOTOR_INST, 1600, DL_TIMER_CC_0_INDEX);

    DL_TimerA_setCaptureCompareOutCtl(PWM_MOTOR_INST, DL_TIMER_CC_OCTL_INIT_VAL_LOW,
		DL_TIMER_CC_OCTL_INV_OUT_DISABLED, DL_TIMER_CC_OCTL_SRC_FUNCVAL,
		DL_TIMERA_CAPTURE_COMPARE_1_INDEX);

    DL_TimerA_setCaptCompUpdateMethod(PWM_MOTOR_INST, DL_TIMER_CC_UPDATE_METHOD_IMMEDIATE, DL_TIMERA_CAPTURE_COMPARE_1_INDEX);
    DL_TimerA_setCaptureCompareValue(PWM_MOTOR_INST, 1600, DL_TIMER_CC_1_INDEX);

    DL_TimerA_setCaptureCompareOutCtl(PWM_MOTOR_INST, DL_TIMER_CC_OCTL_INIT_VAL_LOW,
		DL_TIMER_CC_OCTL_INV_OUT_DISABLED, DL_TIMER_CC_OCTL_SRC_FUNCVAL,
		DL_TIMERA_CAPTURE_COMPARE_2_INDEX);

    DL_TimerA_setCaptCompUpdateMethod(PWM_MOTOR_INST, DL_TIMER_CC_UPDATE_METHOD_IMMEDIATE, DL_TIMERA_CAPTURE_COMPARE_2_INDEX);
    DL_TimerA_setCaptureCompareValue(PWM_MOTOR_INST, 1600, DL_TIMER_CC_2_INDEX);

    DL_TimerA_setCaptureCompareOutCtl(PWM_MOTOR_INST, DL_TIMER_CC_OCTL_INIT_VAL_LOW,
		DL_TIMER_CC_OCTL_INV_OUT_DISABLED, DL_TIMER_CC_OCTL_SRC_FUNCVAL,
		DL_TIMERA_CAPTURE_COMPARE_3_INDEX);

    DL_TimerA_setCaptCompUpdateMethod(PWM_MOTOR_INST, DL_TIMER_CC_UPDATE_METHOD_IMMEDIATE, DL_TIMERA_CAPTURE_COMPARE_3_INDEX);
    DL_TimerA_setCaptureCompareValue(PWM_MOTOR_INST, 1600, DL_TIMER_CC_3_INDEX);

    DL_TimerA_enableClock(PWM_MOTOR_INST);



    DL_TimerA_setCCPDirection(PWM_MOTOR_INST , DL_TIMER_CC0_OUTPUT | DL_TIMER_CC1_OUTPUT | DL_TIMER_CC2_OUTPUT | DL_TIMER_CC3_OUTPUT );


}
/*
 * Timer clock configuration to be sourced by  / 1 (32000000 Hz)
 * timerClkFreq = (timerClkSrc / (timerClkDivRatio * (timerClkPrescale + 1)))
 *   1000000 Hz = 32000000 Hz / (1 * (31 + 1))
 */
static const DL_TimerG_ClockConfig gPWM_SERVOClockConfig = {
    .clockSel = DL_TIMER_CLOCK_BUSCLK,
    .divideRatio = DL_TIMER_CLOCK_DIVIDE_1,
    .prescale = 31U
};

static const DL_TimerG_PWMConfig gPWM_SERVOConfig = {
    .pwmMode = DL_TIMER_PWM_MODE_EDGE_ALIGN_UP,
    .period = 20000,
    .isTimerWithFourCC = false,
    .startTimer = DL_TIMER_START,
};

SYSCONFIG_WEAK void SYSCFG_DL_PWM_SERVO_init(void) {

    DL_TimerG_setClockConfig(
        PWM_SERVO_INST, (DL_TimerG_ClockConfig *) &gPWM_SERVOClockConfig);

    DL_TimerG_initPWMMode(
        PWM_SERVO_INST, (DL_TimerG_PWMConfig *) &gPWM_SERVOConfig);

    // Set Counter control to the smallest CC index being used
    DL_TimerG_setCounterControl(PWM_SERVO_INST,DL_TIMER_CZC_CCCTL0_ZCOND,DL_TIMER_CAC_CCCTL0_ACOND,DL_TIMER_CLC_CCCTL0_LCOND);

    DL_TimerG_setCaptureCompareOutCtl(PWM_SERVO_INST, DL_TIMER_CC_OCTL_INIT_VAL_LOW,
		DL_TIMER_CC_OCTL_INV_OUT_DISABLED, DL_TIMER_CC_OCTL_SRC_FUNCVAL,
		DL_TIMERG_CAPTURE_COMPARE_0_INDEX);

    DL_TimerG_setCaptCompUpdateMethod(PWM_SERVO_INST, DL_TIMER_CC_UPDATE_METHOD_IMMEDIATE, DL_TIMERG_CAPTURE_COMPARE_0_INDEX);
    DL_TimerG_setCaptureCompareValue(PWM_SERVO_INST, 20000, DL_TIMER_CC_0_INDEX);

    DL_TimerG_setCaptureCompareOutCtl(PWM_SERVO_INST, DL_TIMER_CC_OCTL_INIT_VAL_LOW,
		DL_TIMER_CC_OCTL_INV_OUT_DISABLED, DL_TIMER_CC_OCTL_SRC_FUNCVAL,
		DL_TIMERG_CAPTURE_COMPARE_1_INDEX);

    DL_TimerG_setCaptCompUpdateMethod(PWM_SERVO_INST, DL_TIMER_CC_UPDATE_METHOD_IMMEDIATE, DL_TIMERG_CAPTURE_COMPARE_1_INDEX);
    DL_TimerG_setCaptureCompareValue(PWM_SERVO_INST, 20000, DL_TIMER_CC_1_INDEX);

    DL_TimerG_enableClock(PWM_SERVO_INST);



    DL_TimerG_setCCPDirection(PWM_SERVO_INST , DL_TIMER_CC0_OUTPUT | DL_TIMER_CC1_OUTPUT );


}
static const DL_TimerG_ClockConfig gQEI_FRONT_LEFTClockConfig = {
    .clockSel = DL_TIMER_CLOCK_BUSCLK,
    .divideRatio = DL_TIMER_CLOCK_DIVIDE_1,
    .prescale = 0U
};


SYSCONFIG_WEAK void SYSCFG_DL_QEI_FRONT_LEFT_init(void) {

    DL_TimerG_setClockConfig(
        QEI_FRONT_LEFT_INST, (DL_TimerG_ClockConfig *) &gQEI_FRONT_LEFTClockConfig);

    DL_TimerG_configQEI(QEI_FRONT_LEFT_INST, DL_TIMER_QEI_MODE_2_INPUT,
        DL_TIMER_CC_INPUT_INV_NOINVERT, DL_TIMER_CC_0_INDEX);
    DL_TimerG_configQEI(QEI_FRONT_LEFT_INST, DL_TIMER_QEI_MODE_2_INPUT,
        DL_TIMER_CC_INPUT_INV_NOINVERT, DL_TIMER_CC_1_INDEX);
    DL_TimerG_setLoadValue(QEI_FRONT_LEFT_INST, 65535);
    DL_TimerG_enableClock(QEI_FRONT_LEFT_INST);
    DL_TimerG_startCounter(QEI_FRONT_LEFT_INST);
}
static const DL_TimerG_ClockConfig gQEI_FRONT_RIGHTClockConfig = {
    .clockSel = DL_TIMER_CLOCK_BUSCLK,
    .divideRatio = DL_TIMER_CLOCK_DIVIDE_1,
    .prescale = 0U
};


SYSCONFIG_WEAK void SYSCFG_DL_QEI_FRONT_RIGHT_init(void) {

    DL_TimerG_setClockConfig(
        QEI_FRONT_RIGHT_INST, (DL_TimerG_ClockConfig *) &gQEI_FRONT_RIGHTClockConfig);

    DL_TimerG_configQEI(QEI_FRONT_RIGHT_INST, DL_TIMER_QEI_MODE_2_INPUT,
        DL_TIMER_CC_INPUT_INV_NOINVERT, DL_TIMER_CC_0_INDEX);
    DL_TimerG_configQEI(QEI_FRONT_RIGHT_INST, DL_TIMER_QEI_MODE_2_INPUT,
        DL_TIMER_CC_INPUT_INV_NOINVERT, DL_TIMER_CC_1_INDEX);
    DL_TimerG_setLoadValue(QEI_FRONT_RIGHT_INST, 65535);
    DL_TimerG_enableClock(QEI_FRONT_RIGHT_INST);
    DL_TimerG_startCounter(QEI_FRONT_RIGHT_INST);
}


static const DL_I2C_ClockConfig gI2C_SENSORClockConfig = {
    .clockSel = DL_I2C_CLOCK_BUSCLK,
    .divideRatio = DL_I2C_CLOCK_DIVIDE_1,
};

SYSCONFIG_WEAK void SYSCFG_DL_I2C_SENSOR_init(void) {

    DL_I2C_setClockConfig(I2C_SENSOR_INST,
        (DL_I2C_ClockConfig *) &gI2C_SENSORClockConfig);
    DL_I2C_disableAnalogGlitchFilter(I2C_SENSOR_INST);

    /* Configure Controller Mode */
    DL_I2C_resetControllerTransfer(I2C_SENSOR_INST);
    /* Set frequency to 400000 Hz*/
    DL_I2C_setTimerPeriod(I2C_SENSOR_INST, 7);
    DL_I2C_setControllerTXFIFOThreshold(I2C_SENSOR_INST, DL_I2C_TX_FIFO_LEVEL_EMPTY);
    DL_I2C_setControllerRXFIFOThreshold(I2C_SENSOR_INST, DL_I2C_RX_FIFO_LEVEL_BYTES_1);
    DL_I2C_enableControllerClockStretching(I2C_SENSOR_INST);

    /* Configure Interrupts */
    DL_I2C_enableInterrupt(I2C_SENSOR_INST,
                           DL_I2C_INTERRUPT_CONTROLLER_ARBITRATION_LOST |
                           DL_I2C_INTERRUPT_CONTROLLER_NACK |
                           DL_I2C_INTERRUPT_CONTROLLER_RX_DONE |
                           DL_I2C_INTERRUPT_CONTROLLER_TX_DONE);

    /* Configure DMA Event 1 */
    DL_I2C_enableDMAEvent(I2C_SENSOR_INST, DL_I2C_EVENT_ROUTE_1,
                          DL_I2C_DMA_INTERRUPT_CONTROLLER_TXFIFO_TRIGGER);
    /* Configure DMA Event 2 */
    DL_I2C_enableDMAEvent(I2C_SENSOR_INST, DL_I2C_EVENT_ROUTE_2,
                          DL_I2C_DMA_INTERRUPT_CONTROLLER_RXFIFO_TRIGGER);

    /* Enable module */
    DL_I2C_enableController(I2C_SENSOR_INST);


}

static const DL_UART_Main_ClockConfig gUART_DEBUGClockConfig = {
    .clockSel    = DL_UART_MAIN_CLOCK_BUSCLK,
    .divideRatio = DL_UART_MAIN_CLOCK_DIVIDE_RATIO_1
};

static const DL_UART_Main_Config gUART_DEBUGConfig = {
    .mode        = DL_UART_MAIN_MODE_NORMAL,
    .direction   = DL_UART_MAIN_DIRECTION_TX_RX,
    .flowControl = DL_UART_MAIN_FLOW_CONTROL_NONE,
    .parity      = DL_UART_MAIN_PARITY_NONE,
    .wordLength  = DL_UART_MAIN_WORD_LENGTH_8_BITS,
    .stopBits    = DL_UART_MAIN_STOP_BITS_ONE
};

SYSCONFIG_WEAK void SYSCFG_DL_UART_DEBUG_init(void)
{
    DL_UART_Main_setClockConfig(
        UART_DEBUG_INST, (DL_UART_Main_ClockConfig *) &gUART_DEBUGClockConfig);

    DL_UART_Main_init(UART_DEBUG_INST, (DL_UART_Main_Config *) &gUART_DEBUGConfig);

    /* 32 MHz BUSCLK, 115200 bit/s, actual baud rate 115211.52. */
    DL_UART_Main_setOversampling(UART_DEBUG_INST, DL_UART_OVERSAMPLING_RATE_16X);
    DL_UART_Main_setBaudRateDivisor(
        UART_DEBUG_INST,
        UART_DEBUG_IBRD_32_MHZ_115200_BAUD,
        UART_DEBUG_FBRD_32_MHZ_115200_BAUD);

    DL_UART_Main_enableInterrupt(
        UART_DEBUG_INST,
        DL_UART_MAIN_INTERRUPT_RX | DL_UART_MAIN_INTERRUPT_TX);

    DL_UART_Main_enableFIFOs(UART_DEBUG_INST);
    DL_UART_Main_setRXFIFOThreshold(
        UART_DEBUG_INST, DL_UART_RX_FIFO_LEVEL_ONE_ENTRY);
    DL_UART_Main_setTXFIFOThreshold(
        UART_DEBUG_INST, DL_UART_TX_FIFO_LEVEL_3_4_EMPTY);

    DL_UART_Main_enable(UART_DEBUG_INST);
}
static const DL_UART_Main_ClockConfig gUART_K210ClockConfig = {
    .clockSel    = DL_UART_MAIN_CLOCK_BUSCLK,
    .divideRatio = DL_UART_MAIN_CLOCK_DIVIDE_RATIO_1
};

static const DL_UART_Main_Config gUART_K210Config = {
    .mode        = DL_UART_MAIN_MODE_NORMAL,
    .direction   = DL_UART_MAIN_DIRECTION_TX_RX,
    .flowControl = DL_UART_MAIN_FLOW_CONTROL_NONE,
    .parity      = DL_UART_MAIN_PARITY_NONE,
    .wordLength  = DL_UART_MAIN_WORD_LENGTH_8_BITS,
    .stopBits    = DL_UART_MAIN_STOP_BITS_ONE
};

SYSCONFIG_WEAK void SYSCFG_DL_UART_K210_init(void)
{
    DL_UART_Main_setClockConfig(UART_K210_INST, (DL_UART_Main_ClockConfig *) &gUART_K210ClockConfig);

    DL_UART_Main_init(UART_K210_INST, (DL_UART_Main_Config *) &gUART_K210Config);
    /*
     * Configure baud rate by setting oversampling and baud rate divisors.
     *  Target baud rate: 115200
     *  Actual baud rate: 115211.52
     */
    DL_UART_Main_setOversampling(UART_K210_INST, DL_UART_OVERSAMPLING_RATE_16X);
    DL_UART_Main_setBaudRateDivisor(UART_K210_INST, UART_K210_IBRD_32_MHZ_115200_BAUD, UART_K210_FBRD_32_MHZ_115200_BAUD);


    /* Configure Interrupts */
    DL_UART_Main_enableInterrupt(UART_K210_INST,
                                 DL_UART_MAIN_INTERRUPT_RX |
                                 DL_UART_MAIN_INTERRUPT_TX);

    /* Configure FIFOs */
    DL_UART_Main_enableFIFOs(UART_K210_INST);
    DL_UART_Main_setRXFIFOThreshold(UART_K210_INST, DL_UART_RX_FIFO_LEVEL_ONE_ENTRY);
    DL_UART_Main_setTXFIFOThreshold(UART_K210_INST, DL_UART_TX_FIFO_LEVEL_3_4_EMPTY);

    DL_UART_Main_enable(UART_K210_INST);
}
static const DL_UART_Main_ClockConfig gUART_E220ClockConfig = {
    .clockSel    = DL_UART_MAIN_CLOCK_BUSCLK,
    .divideRatio = DL_UART_MAIN_CLOCK_DIVIDE_RATIO_1
};

static const DL_UART_Main_Config gUART_E220Config = {
    .mode        = DL_UART_MAIN_MODE_NORMAL,
    .direction   = DL_UART_MAIN_DIRECTION_TX_RX,
    .flowControl = DL_UART_MAIN_FLOW_CONTROL_NONE,
    .parity      = DL_UART_MAIN_PARITY_NONE,
    .wordLength  = DL_UART_MAIN_WORD_LENGTH_8_BITS,
    .stopBits    = DL_UART_MAIN_STOP_BITS_ONE
};

SYSCONFIG_WEAK void SYSCFG_DL_UART_E220_init(void)
{
    DL_UART_Main_setClockConfig(UART_E220_INST, (DL_UART_Main_ClockConfig *) &gUART_E220ClockConfig);

    DL_UART_Main_init(UART_E220_INST, (DL_UART_Main_Config *) &gUART_E220Config);
    /*
     * Configure baud rate by setting oversampling and baud rate divisors.
     *  Target baud rate: 115200
     *  Actual baud rate: 115211.52
     */
    DL_UART_Main_setOversampling(UART_E220_INST, DL_UART_OVERSAMPLING_RATE_16X);
    DL_UART_Main_setBaudRateDivisor(UART_E220_INST, UART_E220_IBRD_32_MHZ_115200_BAUD, UART_E220_FBRD_32_MHZ_115200_BAUD);


    /* Configure Interrupts */
    DL_UART_Main_enableInterrupt(UART_E220_INST,
                                 DL_UART_MAIN_INTERRUPT_RX |
                                 DL_UART_MAIN_INTERRUPT_TX);

    /* Configure FIFOs */
    DL_UART_Main_enableFIFOs(UART_E220_INST);
    DL_UART_Main_setRXFIFOThreshold(UART_E220_INST, DL_UART_RX_FIFO_LEVEL_ONE_ENTRY);
    DL_UART_Main_setTXFIFOThreshold(UART_E220_INST, DL_UART_TX_FIFO_LEVEL_3_4_EMPTY);

    DL_UART_Main_enable(UART_E220_INST);
}

static const DL_SPI_Config gSPI_DISPLAY_config = {
    .mode        = DL_SPI_MODE_CONTROLLER,
    .frameFormat = DL_SPI_FRAME_FORMAT_MOTO4_POL0_PHA0,
    .parity      = DL_SPI_PARITY_NONE,
    .dataSize    = DL_SPI_DATA_SIZE_8,
    .bitOrder    = DL_SPI_BIT_ORDER_MSB_FIRST,
    .chipSelectPin = DL_SPI_CHIP_SELECT_0,
};

static const DL_SPI_ClockConfig gSPI_DISPLAY_clockConfig = {
    .clockSel    = DL_SPI_CLOCK_BUSCLK,
    .divideRatio = DL_SPI_CLOCK_DIVIDE_RATIO_1
};

SYSCONFIG_WEAK void SYSCFG_DL_SPI_DISPLAY_init(void) {
    DL_SPI_setClockConfig(SPI_DISPLAY_INST, (DL_SPI_ClockConfig *) &gSPI_DISPLAY_clockConfig);

    DL_SPI_init(SPI_DISPLAY_INST, (DL_SPI_Config *) &gSPI_DISPLAY_config);

    /* Configure Controller mode */
    /*
     * Set the bit rate clock divider to generate the serial output clock
     *     outputBitRate = (spiInputClock) / ((1 + SCR) * 2)
     *     8000000 = (32000000)/((1 + 1) * 2)
     */
    DL_SPI_setBitRateSerialClockDivider(SPI_DISPLAY_INST, 1);

    /* Enable SPI TX interrupt as a trigger for DMA */
    DL_SPI_enableDMATransmitEvent(SPI_DISPLAY_INST);

    /* Enable SPI RX interrupt as a trigger for DMA */
    DL_SPI_enableDMAReceiveEvent(SPI_DISPLAY_INST, DL_SPI_DMA_INTERRUPT_RX);
    /* Set RX and TX FIFO threshold levels */
    DL_SPI_setFIFOThreshold(SPI_DISPLAY_INST, DL_SPI_RX_FIFO_LEVEL_ONE_FRAME, DL_SPI_TX_FIFO_LEVEL_ONE_FRAME);
    DL_SPI_enableInterrupt(SPI_DISPLAY_INST, (DL_SPI_INTERRUPT_DMA_DONE_RX |
		DL_SPI_INTERRUPT_DMA_DONE_TX |
		DL_SPI_INTERRUPT_TX_EMPTY));

    /* Enable module */
    DL_SPI_enable(SPI_DISPLAY_INST);
}
static const DL_SPI_Config gSPI_ICM20948_config = {
    .mode        = DL_SPI_MODE_CONTROLLER,
    .frameFormat = DL_SPI_FRAME_FORMAT_MOTO4_POL0_PHA0,
    .parity      = DL_SPI_PARITY_NONE,
    .dataSize    = DL_SPI_DATA_SIZE_8,
    .bitOrder    = DL_SPI_BIT_ORDER_MSB_FIRST,
    .chipSelectPin = DL_SPI_CHIP_SELECT_0,
};

static const DL_SPI_ClockConfig gSPI_ICM20948_clockConfig = {
    .clockSel    = DL_SPI_CLOCK_BUSCLK,
    .divideRatio = DL_SPI_CLOCK_DIVIDE_RATIO_1
};

SYSCONFIG_WEAK void SYSCFG_DL_SPI_ICM20948_init(void) {
    DL_SPI_setClockConfig(SPI_ICM20948_INST, (DL_SPI_ClockConfig *) &gSPI_ICM20948_clockConfig);

    DL_SPI_init(SPI_ICM20948_INST, (DL_SPI_Config *) &gSPI_ICM20948_config);

    /* Configure Controller mode */
    /*
     * Set the bit rate clock divider to generate the serial output clock
     *     outputBitRate = (spiInputClock) / ((1 + SCR) * 2)
     *     4000000 = (32000000)/((1 + 3) * 2)
     */
    DL_SPI_setBitRateSerialClockDivider(SPI_ICM20948_INST, 3);
    /* Set RX and TX FIFO threshold levels */
    DL_SPI_setFIFOThreshold(SPI_ICM20948_INST, DL_SPI_RX_FIFO_LEVEL_ONE_FRAME, DL_SPI_TX_FIFO_LEVEL_ONE_FRAME);

    /* Enable module */
    DL_SPI_enable(SPI_ICM20948_INST);
}

/* ADC_GRAY Initialization */
static const DL_ADC12_ClockConfig gADC_GRAYClockConfig = {
    .clockSel       = DL_ADC12_CLOCK_SYSOSC,
    .divideRatio    = DL_ADC12_CLOCK_DIVIDE_8,
    .freqRange      = DL_ADC12_CLOCK_FREQ_RANGE_24_TO_32,
};
SYSCONFIG_WEAK void SYSCFG_DL_ADC_GRAY_init(void)
{
    DL_ADC12_setClockConfig(ADC_GRAY_INST, (DL_ADC12_ClockConfig *) &gADC_GRAYClockConfig);
    DL_ADC12_configConversionMem(ADC_GRAY_INST, ADC_GRAY_ADCMEM_0,
        DL_ADC12_INPUT_CHAN_2, DL_ADC12_REFERENCE_VOLTAGE_VDDA_VSSA, DL_ADC12_SAMPLE_TIMER_SOURCE_SCOMP0, DL_ADC12_AVERAGING_MODE_DISABLED,
        DL_ADC12_BURN_OUT_SOURCE_DISABLED, DL_ADC12_TRIGGER_MODE_AUTO_NEXT, DL_ADC12_WINDOWS_COMP_MODE_DISABLED);
    DL_ADC12_setPowerDownMode(ADC_GRAY_INST,DL_ADC12_POWER_DOWN_MODE_MANUAL);
    DL_ADC12_setSampleTime0(ADC_GRAY_INST,500);
    DL_ADC12_enableConversions(ADC_GRAY_INST);
}

static const DL_DMA_Config gDMA_CH2Config = {
    .transferMode   = DL_DMA_SINGLE_TRANSFER_MODE,
    .extendedMode   = DL_DMA_NORMAL_MODE,
    .destIncrement  = DL_DMA_ADDR_UNCHANGED,
    .srcIncrement   = DL_DMA_ADDR_INCREMENT,
    .destWidth      = DL_DMA_WIDTH_BYTE,
    .srcWidth       = DL_DMA_WIDTH_BYTE,
    .trigger        = I2C_SENSOR_INST_DMA_TRIGGER_0,
    .triggerType    = DL_DMA_TRIGGER_TYPE_EXTERNAL,
};

SYSCONFIG_WEAK void SYSCFG_DL_DMA_CH2_init(void)
{
    DL_DMA_initChannel(DMA, DMA_CH2_CHAN_ID , (DL_DMA_Config *) &gDMA_CH2Config);
}
static const DL_DMA_Config gDMA_CH3Config = {
    .transferMode   = DL_DMA_SINGLE_TRANSFER_MODE,
    .extendedMode   = DL_DMA_NORMAL_MODE,
    .destIncrement  = DL_DMA_ADDR_INCREMENT,
    .srcIncrement   = DL_DMA_ADDR_UNCHANGED,
    .destWidth      = DL_DMA_WIDTH_BYTE,
    .srcWidth       = DL_DMA_WIDTH_BYTE,
    .trigger        = I2C_SENSOR_INST_DMA_TRIGGER_1,
    .triggerType    = DL_DMA_TRIGGER_TYPE_EXTERNAL,
};

SYSCONFIG_WEAK void SYSCFG_DL_DMA_CH3_init(void)
{
    DL_DMA_initChannel(DMA, DMA_CH3_CHAN_ID , (DL_DMA_Config *) &gDMA_CH3Config);
}
static const DL_DMA_Config gDMA_CH1Config = {
    .transferMode   = DL_DMA_SINGLE_TRANSFER_MODE,
    .extendedMode   = DL_DMA_NORMAL_MODE,
    .destIncrement  = DL_DMA_ADDR_INCREMENT,
    .srcIncrement   = DL_DMA_ADDR_UNCHANGED,
    .destWidth      = DL_DMA_WIDTH_BYTE,
    .srcWidth       = DL_DMA_WIDTH_BYTE,
    .trigger        = SPI_DISPLAY_INST_DMA_TRIGGER_0,
    .triggerType    = DL_DMA_TRIGGER_TYPE_EXTERNAL,
};

SYSCONFIG_WEAK void SYSCFG_DL_DMA_CH1_init(void)
{
    DL_DMA_initChannel(DMA, DMA_CH1_CHAN_ID , (DL_DMA_Config *) &gDMA_CH1Config);
}
static const DL_DMA_Config gDMA_CH0Config = {
    .transferMode   = DL_DMA_SINGLE_TRANSFER_MODE,
    .extendedMode   = DL_DMA_NORMAL_MODE,
    .destIncrement  = DL_DMA_ADDR_UNCHANGED,
    .srcIncrement   = DL_DMA_ADDR_INCREMENT,
    .destWidth      = DL_DMA_WIDTH_BYTE,
    .srcWidth       = DL_DMA_WIDTH_BYTE,
    .trigger        = SPI_DISPLAY_INST_DMA_TRIGGER_1,
    .triggerType    = DL_DMA_TRIGGER_TYPE_EXTERNAL,
};

SYSCONFIG_WEAK void SYSCFG_DL_DMA_CH0_init(void)
{
    DL_DMA_initChannel(DMA, DMA_CH0_CHAN_ID , (DL_DMA_Config *) &gDMA_CH0Config);
}
SYSCONFIG_WEAK void SYSCFG_DL_DMA_init(void){
    SYSCFG_DL_DMA_CH2_init();
    SYSCFG_DL_DMA_CH3_init();
    SYSCFG_DL_DMA_CH1_init();
    SYSCFG_DL_DMA_CH0_init();
}


SYSCONFIG_WEAK void SYSCFG_DL_SYSTICK_init(void)
{
    /*
     * Initializes the SysTick period to 1.00 ms,
     * enables the interrupt, and starts the SysTick Timer
     */
    DL_SYSTICK_config(32000);
}
