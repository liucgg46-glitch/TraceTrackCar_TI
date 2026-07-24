#include "bsp_all.h"

BSP_Status_t BSP_InitAll(void)
{
    BSP_Status_t ret;

    /*
     * TI SysConfig 生成的时钟、引脚、外设和 DMA 初始化属于板级职责。
     * 必须在任何 BSP 模块访问外设前执行一次。
     */
    SYSCFG_DL_init();

    ret = BSP_SysTick_Init(BSP_GetCoreClockHz());
    if (ret != BSP_OK) return ret;

    BSP_GPIO_InitAll();
    BSP_EXTI_InitAll();
    BSP_PWM_InitAll();
    BSP_Encoder_InitAll();
    BSP_ADC_Init();
    BSP_Key_InitAll();

    BSP_UART_InitAll();
    BSP_I2C_InitAll();
    BSP_SPI_InitAll();

    return BSP_OK;
}

void BSP_TaskAll(void)
{
    BSP_UART_TaskAll();
    BSP_I2C_TaskAll();
    BSP_SPI_TaskAll();
}