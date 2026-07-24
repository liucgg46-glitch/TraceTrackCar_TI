#include "bsp_all.h"
#include "driver_all.h"
#include "app_all.h"
#include "scheduler.h"

int main(void)
{
    if (BSP_InitAll() != BSP_OK) {
        /*
         * BSP 初始化失败时保持 SysConfig 配置的安全初值。
         * 不进入 Driver、APP 和调度器，避免执行器被误启动。
         */
        while (1) {
            __WFI();
        }
    }

    /*
     * 启动顺序保持项目分层约定：
     * BSP 管理目标 MCU 与板级资源，Driver 管理器件，
     * APP 管理业务，Scheduler 推进周期任务。
     */
    Driver_Init();
    App_Init();
    Scheduler_Init();

    while (1) {
        Scheduler_Run();
        __WFI();
    }
}