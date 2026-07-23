#include "bsp_encoder.h"
#include "bsp_exti.h"

typedef enum {
    BSP_ENCODER_SOURCE_HARDWARE_QEI = 0,
    BSP_ENCODER_SOURCE_SOFTWARE_QEI
} BSP_Encoder_Source_t;

typedef struct {
    uint8_t enabled;
    BSP_Encoder_Source_t source;
    GPTIMER_Regs *timer;
    GPIO_Regs *gpio_port;
    uint32_t pin_a;
    uint32_t pin_b;
    uint8_t reverse;
} BSP_Encoder_Cfg_t;

typedef struct {
    uint16_t last_hardware_counter;
    int32_t last_software_counter;
    volatile int32_t software_counter;
    volatile uint8_t previous_state;
    int16_t delta_count;
    int32_t total_count;
    int32_t speed_cps;
    uint32_t last_update_ms;
    uint32_t update_time_ms;
    uint8_t initialized;
} BSP_Encoder_Runtime_t;

static const BSP_Encoder_Cfg_t s_enc_cfg[BSP_ENCODER_COUNT] = {
    [BSP_ENCODER_CH1] = {
        BSP_ENCODER_CH1_ENABLE,
        BSP_ENCODER_SOURCE_HARDWARE_QEI,
        QEI_FRONT_LEFT_INST,
        (GPIO_Regs *)0,
        0U,
        0U,
        BSP_ENCODER_CH1_REVERSE
    },
    [BSP_ENCODER_CH2] = {
        BSP_ENCODER_CH2_ENABLE,
        BSP_ENCODER_SOURCE_HARDWARE_QEI,
        QEI_FRONT_RIGHT_INST,
        (GPIO_Regs *)0,
        0U,
        0U,
        BSP_ENCODER_CH2_REVERSE
    },
    [BSP_ENCODER_CH3] = {
        BSP_ENCODER_CH3_ENABLE,
        BSP_ENCODER_SOURCE_SOFTWARE_QEI,
        (GPTIMER_Regs *)0,
        GPIO_BOARD_IO_PORT,
        GPIO_BOARD_IO_ENCODER_RL_A_PIN,
        GPIO_BOARD_IO_ENCODER_RL_B_PIN,
        BSP_ENCODER_CH3_REVERSE
    },
    [BSP_ENCODER_CH4] = {
        BSP_ENCODER_CH4_ENABLE,
        BSP_ENCODER_SOURCE_SOFTWARE_QEI,
        (GPTIMER_Regs *)0,
        GPIO_BOARD_IO_PORT,
        GPIO_BOARD_IO_ENCODER_RR_A_PIN,
        GPIO_BOARD_IO_ENCODER_RR_B_PIN,
        BSP_ENCODER_CH4_REVERSE
    }
};

static volatile BSP_Encoder_Runtime_t s_enc_rt[BSP_ENCODER_COUNT];

/*
 * 状态编码为 A:B，A 是高位。每个合法格雷码边沿计数一次；
 * 两位同时变化视为毛刺或丢边沿，不累计。
 */
static const int8_t s_quadrature_step[16] = {
     0,  1, -1,  0,
    -1,  0,  0,  1,
     1,  0,  0, -1,
     0, -1,  1,  0
};

static uint8_t Encoder_IsAvailable(BSP_Encoder_Id_t id)
{
    if ((id >= BSP_ENCODER_COUNT) || (s_enc_cfg[id].enabled == 0U)) {
        return 0U;
    }

    if (s_enc_cfg[id].source == BSP_ENCODER_SOURCE_HARDWARE_QEI) {
        return (s_enc_cfg[id].timer != (GPTIMER_Regs *)0) ? 1U : 0U;
    }

    return (s_enc_cfg[id].gpio_port != (GPIO_Regs *)0) ? 1U : 0U;
}

static uint8_t Encoder_ReadSoftwareState(BSP_Encoder_Id_t id)
{
    uint32_t pins;
    uint8_t state = 0U;

    pins = DL_GPIO_readPins(
        s_enc_cfg[id].gpio_port,
        s_enc_cfg[id].pin_a | s_enc_cfg[id].pin_b);
    if ((pins & s_enc_cfg[id].pin_a) != 0U) {
        state |= 2U;
    }
    if ((pins & s_enc_cfg[id].pin_b) != 0U) {
        state |= 1U;
    }
    return state;
}

static void Encoder_DecodeSoftwareEdge(BSP_Encoder_Id_t id)
{
    uint8_t current_state;
    uint8_t transition;

    if ((Encoder_IsAvailable(id) == 0U) ||
        (s_enc_cfg[id].source != BSP_ENCODER_SOURCE_SOFTWARE_QEI)) {
        return;
    }

    current_state = Encoder_ReadSoftwareState(id);
    transition =
        (uint8_t)((s_enc_rt[id].previous_state << 2U) | current_state);
    s_enc_rt[id].previous_state = current_state;
    s_enc_rt[id].software_counter += s_quadrature_step[transition];
}

void BSP_Encoder_Init(BSP_Encoder_Id_t id)
{
    uint32_t now;

    if (Encoder_IsAvailable(id) == 0U) {
        return;
    }

    now = BSP_GET_TICK();
    if (s_enc_cfg[id].source == BSP_ENCODER_SOURCE_HARDWARE_QEI) {
        s_enc_rt[id].last_hardware_counter =
            (uint16_t)DL_TimerG_getTimerCount(s_enc_cfg[id].timer);
    } else {
        s_enc_rt[id].software_counter      = 0;
        s_enc_rt[id].last_software_counter = 0;
        s_enc_rt[id].previous_state = Encoder_ReadSoftwareState(id);
    }

    s_enc_rt[id].delta_count    = 0;
    s_enc_rt[id].total_count    = 0;
    s_enc_rt[id].speed_cps      = 0;
    s_enc_rt[id].last_update_ms = now;
    s_enc_rt[id].update_time_ms = now;
    s_enc_rt[id].initialized    = 1U;
}

void BSP_Encoder_InitAll(void)
{
    BSP_Encoder_Id_t id;
    uint32_t software_pins =
        GPIO_BOARD_IO_ENCODER_RL_A_PIN |
        GPIO_BOARD_IO_ENCODER_RL_B_PIN |
        GPIO_BOARD_IO_ENCODER_RR_A_PIN |
        GPIO_BOARD_IO_ENCODER_RR_B_PIN;

    for (id = (BSP_Encoder_Id_t)0; id < BSP_ENCODER_COUNT;
         id = (BSP_Encoder_Id_t)(id + 1)) {
        BSP_Encoder_Init(id);
    }

    if ((BSP_ENCODER_CH3_ENABLE != 0U) ||
        (BSP_ENCODER_CH4_ENABLE != 0U)) {
        DL_GPIO_clearInterruptStatus(GPIO_BOARD_IO_PORT, software_pins);
        NVIC_ClearPendingIRQ(GPIO_BOARD_IO_INT_IRQN);
        NVIC_EnableIRQ(GPIO_BOARD_IO_INT_IRQN);
    }
}

void BSP_Encoder_Update(BSP_Encoder_Id_t id)
{
    int32_t current_count;
    int32_t delta_32;
    int16_t delta;
    uint32_t now;
    uint32_t elapsed_ms;

    if ((Encoder_IsAvailable(id) == 0U) ||
        (s_enc_rt[id].initialized == 0U)) {
        return;
    }

    if (s_enc_cfg[id].source == BSP_ENCODER_SOURCE_HARDWARE_QEI) {
        uint16_t hardware_counter =
            (uint16_t)DL_TimerG_getTimerCount(s_enc_cfg[id].timer);
        delta = (int16_t)(
            hardware_counter - s_enc_rt[id].last_hardware_counter);
        s_enc_rt[id].last_hardware_counter = hardware_counter;
    } else {
        current_count = s_enc_rt[id].software_counter;
        delta_32 = current_count - s_enc_rt[id].last_software_counter;
        s_enc_rt[id].last_software_counter = current_count;

        if (delta_32 > INT16_MAX) {
            delta = INT16_MAX;
        } else if (delta_32 < INT16_MIN) {
            delta = INT16_MIN;
        } else {
            delta = (int16_t)delta_32;
        }
    }

    if (s_enc_cfg[id].reverse != 0U) {
        delta = (delta == INT16_MIN) ? INT16_MAX : (int16_t)(-delta);
    }

    now = BSP_GET_TICK();
    elapsed_ms = now - s_enc_rt[id].last_update_ms;
    if (elapsed_ms == 0U) {
        elapsed_ms = BSP_ENCODER_UPDATE_PERIOD_MS;
    }

    s_enc_rt[id].delta_count = delta;
    s_enc_rt[id].total_count += delta;
    s_enc_rt[id].speed_cps =
        ((int32_t)delta * 1000L) / (int32_t)elapsed_ms;
    s_enc_rt[id].last_update_ms = now;
    s_enc_rt[id].update_time_ms = now;
}

void BSP_Encoder_UpdateAll(void)
{
    BSP_Encoder_Id_t id;

    for (id = (BSP_Encoder_Id_t)0; id < BSP_ENCODER_COUNT;
         id = (BSP_Encoder_Id_t)(id + 1)) {
        BSP_Encoder_Update(id);
    }
}

int16_t BSP_Encoder_GetDelta(BSP_Encoder_Id_t id)
{
    return (id < BSP_ENCODER_COUNT) ? s_enc_rt[id].delta_count : 0;
}

int32_t BSP_Encoder_GetSpeedCps(BSP_Encoder_Id_t id)
{
    return (id < BSP_ENCODER_COUNT) ? s_enc_rt[id].speed_cps : 0;
}

int32_t BSP_Encoder_GetTotal(BSP_Encoder_Id_t id)
{
    return (id < BSP_ENCODER_COUNT) ? s_enc_rt[id].total_count : 0;
}

void BSP_Encoder_ClearTotal(BSP_Encoder_Id_t id)
{
    uint32_t primask;

    if (id >= BSP_ENCODER_COUNT) {
        return;
    }

    primask = BSP_EnterCritical();
    s_enc_rt[id].total_count = 0;
    s_enc_rt[id].delta_count = 0;
    s_enc_rt[id].speed_cps   = 0;

    if (Encoder_IsAvailable(id) != 0U) {
        if (s_enc_cfg[id].source == BSP_ENCODER_SOURCE_HARDWARE_QEI) {
            s_enc_rt[id].last_hardware_counter =
                (uint16_t)DL_TimerG_getTimerCount(s_enc_cfg[id].timer);
        } else {
            s_enc_rt[id].last_software_counter =
                s_enc_rt[id].software_counter;
        }
    }
    BSP_ExitCritical(primask);
}

void BSP_Encoder_ClearAllTotal(void)
{
    BSP_Encoder_Id_t id;

    for (id = (BSP_Encoder_Id_t)0; id < BSP_ENCODER_COUNT;
         id = (BSP_Encoder_Id_t)(id + 1)) {
        BSP_Encoder_ClearTotal(id);
    }
}

BSP_Status_t BSP_Encoder_GetInfo(
    BSP_Encoder_Id_t id, BSP_Encoder_Info_t *info)
{
    if ((id >= BSP_ENCODER_COUNT) || (info == (BSP_Encoder_Info_t *)0)) {
        return BSP_PARAM;
    }

    info->delta_count    = s_enc_rt[id].delta_count;
    info->total_count    = s_enc_rt[id].total_count;
    info->speed_cps      = s_enc_rt[id].speed_cps;
    info->update_time_ms = s_enc_rt[id].update_time_ms;
    return BSP_OK;
}

void GROUP1_IRQHandler(void)
{
    uint32_t pending;
    uint32_t rear_left_pins =
        GPIO_BOARD_IO_ENCODER_RL_A_PIN |
        GPIO_BOARD_IO_ENCODER_RL_B_PIN;
    uint32_t rear_right_pins =
        GPIO_BOARD_IO_ENCODER_RR_A_PIN |
        GPIO_BOARD_IO_ENCODER_RR_B_PIN;
    uint32_t encoder_pins = rear_left_pins | rear_right_pins;
    uint32_t exti_pins = BSP_EXTI_GetEnabledPins(GPIO_BOARD_IO_PORT);
    uint32_t handled_pins = encoder_pins | exti_pins;

    if (DL_Interrupt_getPendingGroup(DL_INTERRUPT_GROUP_1) !=
        GPIO_BOARD_IO_INT_IIDX) {
        return;
    }

    pending = DL_GPIO_getEnabledInterruptStatus(
        GPIO_BOARD_IO_PORT, handled_pins);
    DL_GPIO_clearInterruptStatus(GPIO_BOARD_IO_PORT, pending);

    if ((pending & rear_left_pins) != 0U) {
        Encoder_DecodeSoftwareEdge(BSP_ENCODER_CH3);
    }
    if ((pending & rear_right_pins) != 0U) {
        Encoder_DecodeSoftwareEdge(BSP_ENCODER_CH4);
    }

    BSP_EXTI_DispatchIRQ(pending & exti_pins);
}
