# TraceTrackCar_TI 文档

本目录只保存当前 MSPM0G3519 工程的稳定说明。原 STM32 Markdown 会被移除；迁移历史仍保留在仓库 `Migration` 目录，不作为当前接线依据。

## 文档索引

- [工程说明](工程说明.md)：分层、启动流程、SysConfig 和构建规则。
- [参数与引脚](参数与引脚.md)：MSPM0G3519 外设参数和主要接线。
- [测试任务](测试任务.md)：专项测试开关、任务组合和测试入口。
- [通信说明](通信说明.md)：Type-C 调试串口、E220 和 K210。
- [上板与注意事项](上板与注意事项.md)：安全、联调顺序和常见问题。

## 维护规则

- 引脚以 `User/Config/empty_mspm0g3519.syscfg` 和 BSP 头文件为准。
- 测试入口以 `User/Test/test.h` 为准。
- 正常运行指示使用 `AppDiagnostics_HeartbeatUpdate()`。
- `Test_GPIO_Toggle()`只是普通 GPIO 翻转测试。
- SysConfig 改动后重新生成 `ti_msp_dl_config.c/.h`。
