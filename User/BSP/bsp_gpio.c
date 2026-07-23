#include "bsp_gpio.h"

typedef struct {
    GPIO_Regs *port;
    uint32_t pin;
    uint8_t is_output;
    uint8_t initial_level;
} BSP_GPIO_Cfg_t;

#define GPIO_CFG_OUT(port_, pin_, level_) {(port_), (pin_), 1U, (level_)}
#define GPIO_CFG_IN(port_, pin_)          {(port_), (pin_), 0U, 0U}

static const BSP_GPIO_Cfg_t s_gpio_cfg[BSP_GPIO_COUNT] = {
    [BSP_GPIO_LED1] = GPIO_CFG_OUT(GPIO_BOARD_OUTPUTS_PORT,
                                   GPIO_BOARD_OUTPUTS_LED1_PIN, 1U),
    [BSP_GPIO_LED2] = GPIO_CFG_OUT(GPIO_BOARD_OUTPUTS_PORT,
                                   GPIO_BOARD_OUTPUTS_LED2_PIN, 1U),
    [BSP_GPIO_RGB_DATA] = GPIO_CFG_OUT(GPIO_BOARD_OUTPUTS_PORT,
                                      GPIO_BOARD_OUTPUTS_RGB_DATA_PIN, 0U),
    [BSP_GPIO_ICM20948_CS] = GPIO_CFG_OUT(GPIO_BOARD_IO_PORT,
                                         GPIO_BOARD_IO_ICM20948_CS_PIN, 1U),
    [BSP_GPIO_ICM20948_INT] = GPIO_CFG_IN(GPIO_BOARD_IO_PORT,
                                         GPIO_BOARD_IO_ICM20948_INT_PIN),
    [BSP_GPIO_USER_KEY] = GPIO_CFG_IN(GPIO_BOARD_IO_PORT,
                                     GPIO_BOARD_IO_USER_KEY_PIN),
    [BSP_GPIO_MOTOR_FL_IN1] = GPIO_CFG_OUT(GPIO_BOARD_IO_PORT,
                                           GPIO_BOARD_IO_MOTOR_FL_IN1_PIN, 0U),
    [BSP_GPIO_MOTOR_FL_IN2] = GPIO_CFG_OUT(GPIO_BOARD_IO_PORT,
                                           GPIO_BOARD_IO_MOTOR_FL_IN2_PIN, 0U),
    [BSP_GPIO_MOTOR_FR_IN1] = GPIO_CFG_OUT(GPIO_BOARD_IO_PORT,
                                           GPIO_BOARD_IO_MOTOR_FR_IN1_PIN, 0U),
    [BSP_GPIO_MOTOR_FR_IN2] = GPIO_CFG_OUT(GPIO_BOARD_IO_PORT,
                                           GPIO_BOARD_IO_MOTOR_FR_IN2_PIN, 0U),
    [BSP_GPIO_MOTOR_RL_IN1] = GPIO_CFG_OUT(GPIO_BOARD_IO_PORT,
                                           GPIO_BOARD_IO_MOTOR_RL_IN1_PIN, 0U),
    [BSP_GPIO_MOTOR_RL_IN2] = GPIO_CFG_OUT(GPIO_BOARD_IO_PORT,
                                           GPIO_BOARD_IO_MOTOR_RL_IN2_PIN, 0U),
    [BSP_GPIO_MOTOR_RR_IN1] = GPIO_CFG_OUT(GPIO_BOARD_IO_PORT,
                                           GPIO_BOARD_IO_MOTOR_RR_IN1_PIN, 0U),
    [BSP_GPIO_MOTOR_RR_IN2] = GPIO_CFG_OUT(GPIO_BOARD_IO_PORT,
                                           GPIO_BOARD_IO_MOTOR_RR_IN2_PIN, 0U),
    [BSP_GPIO_LCD_CS] = GPIO_CFG_OUT(GPIO_DISPLAY_GRAY_PORT,
                                    GPIO_DISPLAY_GRAY_LCD_CS_PIN, 1U),
    [BSP_GPIO_LCD_DC] = GPIO_CFG_OUT(GPIO_DISPLAY_GRAY_PORT,
                                    GPIO_DISPLAY_GRAY_LCD_DC_PIN, 0U),
    [BSP_GPIO_LCD_BL] = GPIO_CFG_OUT(GPIO_BOARD_OUTPUTS_PORT,
                                    GPIO_BOARD_OUTPUTS_LCD_BL_PIN, 0U),
    [BSP_GPIO_LCD_RESET] = GPIO_CFG_OUT(GPIO_BOARD_IO_PORT,
                                       GPIO_BOARD_IO_LCD_RESET_PIN, 1U),
    [BSP_GPIO_GRAY_S0] = GPIO_CFG_OUT(GPIO_BOARD_OUTPUTS_PORT,
                                     GPIO_BOARD_OUTPUTS_GRAY_S0_PIN, 0U),
    [BSP_GPIO_GRAY_S1] = GPIO_CFG_OUT(GPIO_BOARD_OUTPUTS_PORT,
                                     GPIO_BOARD_OUTPUTS_GRAY_S1_PIN, 0U),
    [BSP_GPIO_GRAY_S2] = GPIO_CFG_OUT(GPIO_DISPLAY_GRAY_PORT,
                                     GPIO_DISPLAY_GRAY_GRAY_S2_PIN, 0U),
    [BSP_GPIO_E220_AUX] = GPIO_CFG_IN(GPIO_BOARD_IO_PORT,
                                     GPIO_BOARD_IO_E220_AUX_PIN),
    [BSP_GPIO_LASER_EN] = GPIO_CFG_OUT(GPIO_DISPLAY_GRAY_PORT,
                                      GPIO_DISPLAY_GRAY_LASER_EN_PIN, 0U),
    [BSP_GPIO_HX711_DOUT] = GPIO_CFG_IN(GPIO_DISPLAY_GRAY_PORT,
                                       GPIO_DISPLAY_GRAY_HX711_DOUT_PIN),
    [BSP_GPIO_HX711_PD_SCK] = GPIO_CFG_OUT(GPIO_BOARD_OUTPUTS_PORT,
                                          GPIO_BOARD_OUTPUTS_HX711_SCK_PIN, 0U),
    [BSP_GPIO_BUZZER] = GPIO_CFG_OUT(GPIO_BOARD_OUTPUTS_PORT,
                                    GPIO_BOARD_OUTPUTS_BUZZER_PIN, 1U)
};

void BSP_GPIO_Init(BSP_GPIO_Id_t id)
{
    if ((id < BSP_GPIO_COUNT) && (s_gpio_cfg[id].is_output != 0U)) {
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
    if ((id < BSP_GPIO_COUNT) && (s_gpio_cfg[id].is_output != 0U)) {
        DL_GPIO_togglePins(s_gpio_cfg[id].port, s_gpio_cfg[id].pin);
    }
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
