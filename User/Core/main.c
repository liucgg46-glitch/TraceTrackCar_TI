#include "ti_msp_dl_config.h"
#include "bsp_all.h"
#include "driver_all.h"
#include "app_all.h"
#include "scheduler.h"

int main(void)
{
    SYSCFG_DL_init();

    if (BSP_InitAll(BSP_GetCoreClockHz()) != BSP_OK) {
        /* 初始化失败时保持所有电机 PWM/方向脚的 SysConfig 安全初值。 */
        while (1) {
            __WFI();
        }
    }

    /*
     * 分层启动顺序保持原工程约定：
     * BSP 只管理 MCU 外设，Driver 管理器件，APP 管理业务，Scheduler 推进任务。
     */
    Driver_Init();
    App_Init();
    Scheduler_Init();

    while (1) {
        Scheduler_Run();
        __WFI();
    }
}
