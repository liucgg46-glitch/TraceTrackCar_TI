#include "ti_msp_dl_config.h"
#include "bsp_common.h"
#include "bsp_gpio.h"
#include "bsp_systick.h"

#define LED1_TOGGLE_PERIOD_MS 500U

int main(void)
{
    uint32_t led1_last_toggle_ms = 0U;

    SYSCFG_DL_init();

    if (BSP_SysTick_Init(BSP_GetCoreClockHz()) != BSP_OK) {
        while (1) {
            __WFI();
        }
    }

    BSP_GPIO_InitAll();

    while (1) {
        if (BSP_TimeElapsed(&led1_last_toggle_ms, LED1_TOGGLE_PERIOD_MS) != 0U) {
            BSP_GPIO_Toggle(BSP_GPIO_LED1);
        }

        /*
         * USER 键按下时 PB31 为低电平，LED2 也是低电平点亮，
         * 因此可直接把按键电平写到 LED2。
         */
        BSP_GPIO_Write(BSP_GPIO_LED2, BSP_GPIO_Read(BSP_GPIO_USER_KEY));

        __WFI();
    }
}
