#include "bsp_exti.h"

typedef struct {
    GPIO_Regs *port;
    uint32_t pin;
} BSP_EXTI_Cfg_t;

typedef struct {
    BSP_EXTI_Callback_t cb;
    void *ctx;
} BSP_EXTI_Runtime_t;

#if BSP_EXTI_ANY_ENABLE
static const BSP_EXTI_Cfg_t s_exti_cfg[BSP_EXTI_COUNT] = {
#if BSP_EXTI_CH1_ENABLE
    [BSP_EXTI_CH1] = {
        BSP_EXTI_CH1_PORT,
        BSP_EXTI_CH1_PIN
    },
#endif
};

static volatile BSP_EXTI_Runtime_t s_exti_rt[BSP_EXTI_COUNT];
#endif

void BSP_EXTI_Init(BSP_EXTI_Id_t id)
{
#if BSP_EXTI_ANY_ENABLE
    const BSP_EXTI_Cfg_t *cfg;

    if (id >= BSP_EXTI_COUNT) {
        return;
    }

    cfg = &s_exti_cfg[id];
    DL_GPIO_clearInterruptStatus(cfg->port, cfg->pin);
    DL_GPIO_enableInterrupt(cfg->port, cfg->pin);
#else
    (void)id;
#endif
}

void BSP_EXTI_InitAll(void)
{
#if BSP_EXTI_ANY_ENABLE
    BSP_EXTI_Id_t id;

    for (id = (BSP_EXTI_Id_t)0; id < BSP_EXTI_COUNT;
         id = (BSP_EXTI_Id_t)(id + 1)) {
        BSP_EXTI_Init(id);
    }

    NVIC_ClearPendingIRQ(GPIO_BOARD_IO_INT_IRQN);
    NVIC_EnableIRQ(GPIO_BOARD_IO_INT_IRQN);
#endif
}

BSP_Status_t BSP_EXTI_AttachCallback(
    BSP_EXTI_Id_t id, BSP_EXTI_Callback_t cb, void *ctx)
{
#if BSP_EXTI_ANY_ENABLE
    uint32_t primask;

    if (id >= BSP_EXTI_COUNT) {
        return BSP_PARAM;
    }

    primask = BSP_EnterCritical();
    s_exti_rt[id].cb = cb;
    s_exti_rt[id].ctx = ctx;
    BSP_ExitCritical(primask);
    return BSP_OK;
#else
    (void)id;
    (void)cb;
    (void)ctx;
    return BSP_PARAM;
#endif
}

uint32_t BSP_EXTI_GetEnabledPins(GPIO_Regs *port)
{
    uint32_t pins = 0U;

#if BSP_EXTI_ANY_ENABLE
    BSP_EXTI_Id_t id;

    for (id = (BSP_EXTI_Id_t)0; id < BSP_EXTI_COUNT;
         id = (BSP_EXTI_Id_t)(id + 1)) {
        if (s_exti_cfg[id].port == port) {
            pins |= s_exti_cfg[id].pin;
        }
    }
#else
    (void)port;
#endif

    return pins;
}

void BSP_EXTI_DispatchIRQ(uint32_t gpio_pins)
{
#if BSP_EXTI_ANY_ENABLE
    BSP_EXTI_Id_t id;

    for (id = (BSP_EXTI_Id_t)0; id < BSP_EXTI_COUNT;
         id = (BSP_EXTI_Id_t)(id + 1)) {
        BSP_EXTI_Callback_t callback;
        void *context;

        if ((gpio_pins & s_exti_cfg[id].pin) == 0U) {
            continue;
        }

        callback = s_exti_rt[id].cb;
        context = s_exti_rt[id].ctx;
        if (callback != (BSP_EXTI_Callback_t)0) {
            callback(context);
        }
    }
#else
    (void)gpio_pins;
#endif
}
