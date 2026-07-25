# MSPM0G3519 迁移收口与上板清单

## 软件迁移结论

STM32 工程的业务层、算法层、赛道层、器件驱动接口和调度启动链已经迁移到 MSPM0G3519。目标固件不再依赖 STM32 CMSIS、标准外设库或 STM32 中断类型。

当前已完成：

- `BSP_InitAll -> Driver_Init -> App_Init -> Scheduler_Init -> Scheduler_Run` 完整启动链。
- 四路电机方向、四路 PWM、两路硬件 QEI 与两路软件 QEI。
- PB31 USER 按键承接原 KEY5；KEY1~KEY4 不占用引脚。
- 用户自己的外接 ICM20948 使用 SPI1，不使用核心板原理图中的 ICM45686。
- K210、E220、I2C0、SPI0/SPI1、ADC、舵机、激光、HX711、蜂鸣器以及显示接口。
- SPI0 显示异步传输使用 DMA_CH0/CH1。
- I2C0 异步传输使用 DMA_CH2/CH3；组合事务在TX_DONE后等待控制器IDLE，再发重复START并由RX_DONE收尾。
- PB21 ICM20948 INT 与后轮软件编码器共用 GPIOB GROUP1 中断入口。
- TI Clang 与 Keil ARM Compiler 6.24 完整编译链接。

## 外接 ICM20948 接线

| ICM20948 信号 | MSPM0G3519 | 说明 |
| --- | --- | --- |
| VCC | 3V3 | 不允许接 5V；模块是否自带稳压和电平转换需自行确认 |
| GND | GND | 必须共地 |
| SCLK | PB16 | SPI1 时钟 |
| MOSI / SDI | PB15 | SPI1 控制器输出 |
| MISO / SDO | PB14 | SPI1 控制器输入 |
| CS | PA9 | 软件片选，空闲高电平 |
| INT | PB21 | GPIO 双边沿中断；不用中断时可不接 |

当前驱动按 SPI Mode 0、4 MHz 配置。模块的 AD0/SDO 复用、INT 极性和电气电平应以用户手中模块原理图为准。

## 建议上板顺序

1. 不接电机和外设，只接 3V3、GND、SWD，确认可下载并停在 `main`。
2. 验证 PA14/PA17 板载 LED 与 PB31 USER 按键。
3. 接 USB-TTL，分别验证 K210 UART1（PB4/PB5）和 E220 UART4（PB10/PB11）。
4. 单独接 ICM20948，先读取 WHO_AM_I，再检查静止加速度、角速度和温度；AK09916失败时确认六轴降级仍可运行，最后接 PB21 INT。
5. 单独接灰度传感器；I2C型号先确认ping返回0x66且8路数据持续更新，再确认数据顺序与黑白极性。
6. 架空车轮，只接电机驱动逻辑电源，逐轮核对方向脚、PWM 通道和急停。
7. 手动转动每个车轮，核对 FL/FR/RL/RR 编码器正负方向和每圈计数。
8. 低占空比运行速度环，再重新标定轮径、减速比、PID/PI 和最大输出。
9. 最后接显示、VL53L1X、HX711、舵机、激光、蜂鸣器等可选模块。

## 仍需实物确认

以下内容无法仅靠源码、PDF 和编译器确认，不应视为已经上板通过：

- 四个电机的正反方向以及驱动板 IN1/IN2 有效逻辑。
- 四个编码器的相序、方向、每圈计数和最高转速下的软件 QEI 丢边沿情况。
- 轮径、轮距、减速比、最大速度和全部 PID/PI 参数。
- 灰度探头顺序、黑白极性、阈值和安装高度。
- ICM20948 模块的轴向、INT 有效电平、供电与电平转换。
- I2C 总线是否已有合适的 3.3V 上拉，以及多个模块并联后的总线电容。
- 舵机安全脉宽、机械零位和独立供电能力。
- HX711、蜂鸣器、激光模块的实际有效电平。
- K210/E220 的实际波特率、交叉接线和电源峰值电流。
- 没有接到核心板的 OLED/TFT、VL53L1X、灰度 MCU 等设备不能完成 DMA 实物验证。
- 2026-07-25修正的I2C重复START时序和ICM20948六轴降级路径已经通过TI Clang完整编译，仍需按测试手册第19、23、24节烧录复验。

## 有意不迁移的 STM32 平台文件

以下文件不是小车业务资源，不能进入 MSPM0 固件：

- STM32F4 CMSIS 与 `STM32F4xx_StdPeriph_Driver`。
- `startup_stm32f40xx.s`、`stm32f4xx_it.*`、STM32 链接脚本。
- STM32 DMA Stream/Channel、EXTI Line、NVIC 分组和 RCC 配置。
- STM32 工程的 Objects、Listings、AXF、HEX、MAP 等历史生成物。

这些内容已经分别由 MSPM0 SDK/DriverLib、MSPM0 启动文件、SysConfig、MSPM0 DMA/GPIO 中断和目标链接脚本替代。

## DMA 实现边界

- LCD/TFT 实际调用的 SPI_BUS1 异步接口使用真实 DMA。
- I2C OLED/灰度 MCU 异步接口使用真实 DMA。
- ICM20948 的短寄存器事务仍使用同步 SPI；这是器件协议调用方式，不影响功能迁移。
- SPI_BUS2 没有异步调用者；若未来调用 `BSP_SPI_TransferAsync_DMA`，当前采用任务兼容后端，届时应再分配一对 DMA 通道。
- UART 使用 FIFO 中断和软件环形缓冲。K210/E220 都是变长协议流，持续 RX/TX 中断比单帧 DMA 更符合当前解析方式。
