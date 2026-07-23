#include "bsp_gpio.h"

typedef struct {
    GPIO_Regs *port;
    uint32_t pin;
    uint8_t is_output;
    uint8_t initial_level;
} BSP_GPIO_Cfg_t;

static const BSP_GPIO_Cfg_t s_gpio_cfg[BSP_GPIO_COUNT] = {
    [BSP_GPIO_LED1] = {
        GPIO_BOARD_OUTPUTS_PORT,
        GPIO_BOARD_OUTPUTS_LED1_PIN,
        1U,
        1U
    },
    [BSP_GPIO_LED2] = {
        GPIO_BOARD_OUTPUTS_PORT,
        GPIO_BOARD_OUTPUTS_LED2_PIN,
        1U,
        1U
    },
    [BSP_GPIO_RGB_DATA] = {
        GPIO_BOARD_OUTPUTS_PORT,
        GPIO_BOARD_OUTPUTS_RGB_DATA_PIN,
        1U,
        0U
    },
    [BSP_GPIO_ICM20948_CS] = {
        GPIO_BOARD_IO_PORT,
        GPIO_BOARD_IO_ICM20948_CS_PIN,
        1U,
        1U
    },
    [BSP_GPIO_ICM20948_INT] = {
        GPIO_BOARD_IO_PORT,
        GPIO_BOARD_IO_ICM20948_INT_PIN,
        0U,
        0U
    },
    [BSP_GPIO_USER_KEY] = {
        GPIO_BOARD_IO_PORT,
        GPIO_BOARD_IO_USER_KEY_PIN,
        0U,
        0U
    }
};

void BSP_GPIO_Init(BSP_GPIO_Id_t id)
{
    if (id >= BSP_GPIO_COUNT) {
        return;
    }

    if (s_gpio_cfg[id].is_output != 0U) {
        BSP_GPIO_Write(id, s_gpio_cfg[id].initial_level);
    }
}

void BSP_GPIO_InitAll(void)
{
    BSP_GPIO_Id_t id;

    for (id = (BSP_GPIO_Id_t)0; id < BSP_GPIO_COUNT;
         id = (BSP_GPIO_Id_t)(id + 1)) {
        BSP_GPIO_Init(id);
    }
}

void BSP_GPIO_Write(BSP_GPIO_Id_t id, uint8_t level)
{
    const BSP_GPIO_Cfg_t *cfg;

    if (id >= BSP_GPIO_COUNT) {
        return;
    }

    cfg = &s_gpio_cfg[id];
    if (cfg->is_output == 0U) {
        return;
    }

    if (level != 0U) {
        DL_GPIO_setPins(cfg->port, cfg->pin);
    } else {
        DL_GPIO_clearPins(cfg->port, cfg->pin);
    }
}

void BSP_GPIO_Toggle(BSP_GPIO_Id_t id)
{
    if ((id >= BSP_GPIO_COUNT) || (s_gpio_cfg[id].is_output == 0U)) {
        return;
    }

    DL_GPIO_togglePins(s_gpio_cfg[id].port, s_gpio_cfg[id].pin);
}

uint8_t BSP_GPIO_Read(BSP_GPIO_Id_t id)
{
    if (id >= BSP_GPIO_COUNT) {
        return 0U;
    }

    return ((DL_GPIO_readPins(s_gpio_cfg[id].port, s_gpio_cfg[id].pin) &
             s_gpio_cfg[id].pin) != 0U) ? 1U : 0U;
}

GPIO_Regs *BSP_GPIO_GetPort(BSP_GPIO_Id_t id)
{
    return (id < BSP_GPIO_COUNT) ? s_gpio_cfg[id].port : (GPIO_Regs *)0;
}

uint32_t BSP_GPIO_GetPin(BSP_GPIO_Id_t id)
{
    return (id < BSP_GPIO_COUNT) ? s_gpio_cfg[id].pin : 0U;
}
