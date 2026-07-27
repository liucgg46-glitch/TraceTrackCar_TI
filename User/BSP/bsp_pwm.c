#include "bsp_pwm.h"

typedef struct {
    uint8_t enabled;
    uint8_t is_servo;
    DL_TIMER_CC_INDEX cc_index;
} BSP_PWM_Cfg_t;

static const BSP_PWM_Cfg_t s_pwm_cfg[BSP_PWM_COUNT] = {
    [BSP_PWM_CH1] = {BSP_PWM_CH1_ENABLE, 0U, GPIO_PWM_MOTOR_C0_IDX},
    [BSP_PWM_CH2] = {BSP_PWM_CH2_ENABLE, 0U, GPIO_PWM_MOTOR_C1_IDX},
    [BSP_PWM_CH3] = {BSP_PWM_CH3_ENABLE, 0U, GPIO_PWM_MOTOR_C2_IDX},
    [BSP_PWM_CH4] = {BSP_PWM_CH4_ENABLE, 0U, GPIO_PWM_MOTOR_C3_IDX},
    [BSP_PWM_CH5] = {BSP_PWM_CH5_ENABLE, 1U, GPIO_PWM_SERVO_C0_IDX},
    [BSP_PWM_CH6] = {BSP_PWM_CH6_ENABLE, 1U, GPIO_PWM_SERVO_C1_IDX}
};

static uint8_t PWM_IsAvailable(BSP_PWM_Id_t id)
{
    return ((id < BSP_PWM_COUNT) && (s_pwm_cfg[id].enabled != 0U)) ? 1U : 0U;
}

void BSP_PWM_Init(BSP_PWM_Id_t id)
{
    if (PWM_IsAvailable(id) == 0U) {
        return;
    }

    if (s_pwm_cfg[id].is_servo != 0U) {
        (void)BSP_PWM_SetCompare(id, 1500U);
    } else {
        (void)BSP_PWM_SetDutyPermille(id, 0U);
    }
}

void BSP_PWM_InitAll(void)
{
    BSP_PWM_Id_t id;

    for (id = (BSP_PWM_Id_t)0; id < BSP_PWM_COUNT;
         id = (BSP_PWM_Id_t)(id + 1)) {
        BSP_PWM_Init(id);
    }
}

BSP_Status_t BSP_PWM_SetCompare(BSP_PWM_Id_t id, uint16_t compare)
{
    if (PWM_IsAvailable(id) == 0U) {
        return BSP_PARAM;
    }

    if (s_pwm_cfg[id].is_servo != 0U) {
        uint16_t pulse_us = compare;

        if (pulse_us > BSP_PWM_SERVO_PERIOD_US) {
            pulse_us = BSP_PWM_SERVO_PERIOD_US;
        }
        /*
         * 边沿向上 PWM 的比较值与高电平脉宽反向；TIMG0 为 1 MHz，
         * 因此旧工程传入的微秒脉宽无需换算单位。
         */
        DL_TimerG_setCaptureCompareValue(PWM_SERVO_INST,
            (uint32_t)BSP_PWM_SERVO_PERIOD_US - pulse_us,
            s_pwm_cfg[id].cc_index);
    } else {
        if (compare > BSP_PWM_MOTOR_PERIOD_COUNTS) {
            compare = BSP_PWM_MOTOR_PERIOD_COUNTS;
        }
        DL_TimerA_setCaptureCompareValue(
            PWM_MOTOR_INST, compare, s_pwm_cfg[id].cc_index);
    }
    return BSP_OK;
}

BSP_Status_t BSP_PWM_SetDutyPermille(BSP_PWM_Id_t id, uint16_t permille)
{
    uint32_t period;
    uint32_t pulse;

    if (PWM_IsAvailable(id) == 0U) {
        return BSP_PARAM;
    }
    if (permille > 1000U) {
        permille = 1000U;
    }

    period = BSP_PWM_GetPeriod(id);
    if (s_pwm_cfg[id].is_servo != 0U) {
        pulse = (period * permille) / 1000U;
        return BSP_PWM_SetCompare(id, (uint16_t)pulse);
    }
	/*
	 * 当前 TIMA0 PWM 输出的有效占空比与 compare 成正比：
	 * compare=0 对应 0%，compare=LOAD 对应 100%。
	 */
	pulse = (period * permille) / 1000U;
	return BSP_PWM_SetCompare(id, (uint16_t)pulse);
}

uint16_t BSP_PWM_GetPeriod(BSP_PWM_Id_t id)
{
    if (PWM_IsAvailable(id) == 0U) {
        return 0U;
    }
    return (s_pwm_cfg[id].is_servo != 0U) ?
               BSP_PWM_SERVO_PERIOD_US : BSP_PWM_MOTOR_PERIOD_COUNTS;
}

void BSP_PWM_Start(BSP_PWM_Id_t id)
{
    if (PWM_IsAvailable(id) == 0U) {
        return;
    }

    if (s_pwm_cfg[id].is_servo != 0U) {
        DL_TimerG_startCounter(PWM_SERVO_INST);
    } else {
        DL_TimerA_startCounter(PWM_MOTOR_INST);
    }
}

void BSP_PWM_Stop(BSP_PWM_Id_t id)
{
    if (PWM_IsAvailable(id) == 0U) {
        return;
    }

    if (s_pwm_cfg[id].is_servo != 0U) {
        (void)BSP_PWM_SetCompare(id, 0U);
    } else {
        (void)BSP_PWM_SetDutyPermille(id, 0U);
    }
}
