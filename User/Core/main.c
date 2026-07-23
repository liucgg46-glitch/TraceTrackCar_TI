#include "ti_msp_dl_config.h"

/*
 * ============================================================================
 * 板载 LED1 配置
 * ============================================================================
 * 商家提供的核心板资料：
 *     LED1 -> PA14
 *
 * MSPM0G3519：
 *     PA14 对应 GPIOA 的第 14 位
 *     PA14 对应 IOMUX PINCM36
 */
#define LED1_PORT                       (GPIOA)
#define LED1_PIN                        (DL_GPIO_PIN_14)
#define LED1_IOMUX                      (IOMUX_PINCM36)

/*
 * GPIO 外设上电后的稳定等待时间。
 * TI 官方生成代码通常使用 16 个 CPU 周期。
 */
#define GPIO_POWER_STARTUP_DELAY        (16U)

/*
 * 假设 CPU 时钟为 32MHz：
 * 16000000 个周期约为 0.5 秒。
 *
 * LED 每隔约 0.5 秒翻转一次，
 * 所以一个完整亮灭周期约为 1 秒。
 */
#define LED_BLINK_DELAY                 (16000000U)


/**
 * @brief 初始化板载 LED1 对应的 PA14 引脚
 */
static void BSP_LED1_Init(void)
{
    /*
     * 复位 GPIOA 外设，使其恢复到确定状态。
     *
     * 当前只是空工程测试，这样处理没有问题。
     * 以后 GPIOA 上配置了其他设备时，不要在单个 LED
     * 初始化函数里重复复位整个 GPIOA。
     */
    DL_GPIO_reset(LED1_PORT);

    /* 给 GPIOA 外设供电 */
    DL_GPIO_enablePower(LED1_PORT);

    /* 等待 GPIO 外设供电稳定 */
    delay_cycles(GPIO_POWER_STARTUP_DELAY);

    /*
     * 将 PA14 的引脚复用功能配置为数字 GPIO 输出。
     * PA14 对应 PINCM36。
     */
    DL_GPIO_initDigitalOutput(LED1_IOMUX);

    /*
     * 在开启输出前先写入低电平，避免开启输出时产生不确定电平。
     */
    DL_GPIO_clearPins(LED1_PORT, LED1_PIN);

    /* 开启 PA14 的输出功能 */
    DL_GPIO_enableOutput(LED1_PORT, LED1_PIN);
}


int main(void)
{
    /*
     * 执行原有工程中的系统初始化。
     * 主要保留模板原本的系统和时钟初始化流程。
     */
    SYSCFG_DL_init();

    /* 初始化板载 LED1 */
    BSP_LED1_Init();

    while (1)
    {
        /* 延时约 0.5 秒 */
        delay_cycles(LED_BLINK_DELAY);

        /*
         * 翻转 PA14 当前输出状态：
         * 高电平变低电平，低电平变高电平。
         */
        DL_GPIO_togglePins(LED1_PORT, LED1_PIN);
    }
}
