# TraceTrackCar_TI 文档索引

本目录根层文档对应 MSPM0G3519 工程。`Legacy_STM32` 仅保存原 STM32F407
项目资料，其中的引脚、外设实例和初始化方式不能直接用于当前工程。

## 当前工程文档

- [说明文档](说明文档.md)：工程结构、启动流程、外设架构和构建方法。
- [参数和引脚说明](参数和引脚说明.md)：MSPM0G3519 核心板接线、通信参数及 DMA 分配。
- [注意事项](注意事项.md)：供电、外部 ICM20948、异步传输和生成配置注意事项。
- [K210通信测试](K210通信测试.md)：K210公共端口语义、测试入口和任务前置条件。
- [上板测试步骤](上板测试步骤.md)：按风险从低到高进行硬件联调。

## 迁移记录

迁移过程、软件验证记录和仍需实板确认的项目统一放在 `Migration`，不写入稳定功能文档：

- [迁移总览](../Migration/README.md)
- [最终差异与上板清单](../Migration/FINAL_GAPS_AND_BRINGUP.md)
- [引脚复用记录](../Migration/PINMUX_DRAFT.md)
- [封装与构建清理记录](../Migration/ENCAPSULATION_CLEANUP.md)