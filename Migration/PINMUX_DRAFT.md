# SOM_MSPM0G3519_V2.0 引脚约束与候选分配

本文件记录已从原理图确认的板级连接、第二阶段已经冻结的最小基线，以及其余候选用途。凡标记“候选”的项目，必须经过 SysConfig 复用冲突检查和上板测试后才能成为最终接线。

## 第二阶段最小基线（已写入 SysConfig）

| 功能 | 引脚 | 当前配置 |
| --- | --- | --- |
| SWDIO / SWCLK | PA19 / PA20 | 调试接口，保留 |
| LED1 / LED2 | PA14 / PA17 | 推挽输出，初始高电平，低电平点亮 |
| SK6812 数据 | PA29 | 推挽输出，初始低电平；协议驱动尚未迁移 |
| USER 按键 | PB31 | 输入、内部上拉，低电平按下 |
| 外接 ICM20948 CS | PB12 | 推挽输出，初始高电平 |
| 外接 ICM20948 INT | PB21 | 浮空输入；中断触发方式待驱动迁移时确定 |
| 系统时钟 | 内部 SYSOSC | 32 MHz |
| 系统节拍 | SysTick | 1 ms |

上述仅冻结板级自检和外接 ICM20948 的 GPIO 控制线。PB14/PB15/PB16 的 SPI 复用仍须与其余外设联合检查后再冻结。

## 板载固定或受约束引脚

| 引脚 | 板载连接 | 处理规则 |
| --- | --- | --- |
| PA5、PA6 | 32.768 kHz 晶振 | 默认保留，不作为普通 GPIO |
| PA8、PA9 | 40 MHz 晶振 | 默认保留；即使排针引出，也不能直接按空闲 GPIO 使用 |
| PA10、PA11 | UART0 BSL / CH340 | 保留给下载/串口功能 |
| PA14 | LED1 | 可作为板载状态灯，原理图为 LED 接 3V3，通常低电平点亮 |
| PA17 | LED2 | 可作为第二状态灯，通常低电平点亮 |
| PA18 | BOOT 键 | 保留 |
| PA19、PA20 | SWDIO、SWCLK | 保留调试接口 |
| PA29 | SK6812 RGB 数据 | 保留给板载 RGB |
| PB12 | 原理图 ICM45686 CS，用户实物不使用板载 IMU | 候选作为外接 ICM20948 CS |
| PB14 | 原理图 ICM45686 MISO，用户实物不使用板载 IMU | 候选作为外接 ICM20948 MISO |
| PB15 | 原理图 ICM45686 MOSI，用户实物不使用板载 IMU | 候选作为外接 ICM20948 MOSI |
| PB16 | 原理图 ICM45686 CLK，用户实物不使用板载 IMU | 候选作为外接 ICM20948 SCLK |
| PB21 | 原理图 ICM45686 INT1，用户实物不使用板载 IMU | 候选作为外接 ICM20948 INT；不用 IMU 中断时可重新分配 |
| PB28 | 原理图 ICM45686 INT2，用户实物不使用板载 IMU | 外接 ICM20948 通常不需要第二中断，可作为候选 GPIO |
| PB31 | USER 键 | 可作为初期板载按键 |
| PB2、PB3、PC8、PC9、PB23、PA30 | U5 OLED/TFT 接口 | 使用显示屏时保留；未接显示屏时仍需在最终 PinMux 中统一处理 |

PA12 和 PA13 在核心板两组排针位置重复引出，两个位置不是两个独立 MCU 引脚。

## 核心板排针信号

### A/B 侧

| 排针 | MCU/电源 | 排针 | MCU/调试 |
| --- | --- | --- | --- |
| A01 | GND | B01 | SWDIO |
| A02 | 5V | B02 | SWCLK |
| A03 | 3V3 | B03 | PA12 |
| A04 | NRST | B04 | PA13 |
| A05 | BOOT | B05 | PB10 |
| A06 | PB4 | B06 | PB11 |
| A07 | PB5 | B07 | PB16 |
| A08 | PB0 | B08 | PB15 |
| A09 | PB1 | B09 | PB14 |
| A10 | PA1 | B10 | PB13 |
| A11 | PA0 | B11 | PA7 |
| A12 | PA15 | B12 | PA8 |
| A13 | PB26 | B13 | PA9 |
| A14 | PB28 | B14 | PA12 |
| A15 | PB29 | B15 | PA13 |
| A16 | PB30 | B16 | PA16 |
| A17 | PC6 | B17 | PA22 |
| A18 | PC7 | B18 | PB22 |

### C/D 侧

| 排针 | MCU/电源 | 排针 | MCU/模拟电源 |
| --- | --- | --- | --- |
| C01 | PA16 | D01 | PB3 |
| C02 | PA22 | D02 | PB2 |
| C03 | PC2 | D03 | PC8 |
| C04 | PC3 | D04 | PC9 |
| C05 | PC4 | D05 | PB23 |
| C06 | PC5 | D06 | PB6 |
| C07 | PC0 | D07 | PB7 |
| C08 | PC1 | D08 | PB8 |
| C09 | PA28 | D09 | PB9 |
| C10 | PA31 | D10 | PB20 |
| C11 | PA27 | D11 | PB24 |
| C12 | PA26 | D12 | PB25 |
| C13 | PA25 | D13 | PB27 |
| C14 | PA24 | D14 | PA14 |
| C15 | PB17 | D15 | PA17 |
| C16 | PB18 | D16 | A3V3 |
| C17 | PB19 | D17 | A5V |
| C18 | PB21 | D18 | AGND |

## 候选功能分配

| 功能 | 候选引脚/资源 | 说明 |
| --- | --- | --- |
| I2C 主总线 | PA1=SCL、PA0=SDA | 对应彩页 SCL/SDA；用于灰度模块、SSD1306、VL53L1X |
| K210 UART | PB4=TX、PB5=RX | 对应彩页 TX1/RX1 |
| E220/备用 UART | PB10=TX、PB11=RX | 对应彩页 TX2/RX2；AUX 另选普通输入 |
| 电机 PWM 1-4 | PC2、PC4、PC0、PA28 | 彩页对应 TIMA0 CH0-CH3；最终需由 SysConfig 验证 |
| 电机方向 1-8 | PB6、PB7、PB8、PB9、PB20、PB24、PB25、PB27 | 全部作为普通 GPIO 的候选方案 |
| 舵机 PWM 1-2 | PA12、PA13 | 对应彩页 TIM1/TIM2；最终需确认定时器实例和 20 ms 周期 |
| TFT/OLED SPI | PB3=CLK、PB2=MOSI、PC8=DC、PC9=CS、PB23=RES、PA30=BL | 使用核心板 U5 接口 |
| 外接 ICM20948 SPI | PB16=SCLK、PB15=MOSI、PB14=MISO、PB12=CS、PB21=INT | 复用源工程 `drv_icm20948.*` 协议层，只重写 MSPM0 底层适配 |
| 4051 灰度 ADC | PA27 或其他空闲 ADC 引脚 | 只需要 1 路 ADC；S0/S1/S2 另选 3 路 GPIO |
| 状态灯 | PA14、PA17、PA29 | 两个普通 LED 加一个 RGB |
| 用户按键 | PB31 | 用于第一阶段和板级自检 |
| 编码器 | TIMG8/TIMG9 QEI/Hall + 其余候选 GPIO 边沿输入 | 引脚尚未冻结；先完成 2WD 两路硬件 QEI，再评估 4WD |
| VL53L1X XSHUT | PB26 或其他空闲 GPIO | 需确认没有与最终编码器/控制信号冲突 |
| 激光、HX711、蜂鸣器、E220 AUX | PB30、PC6、PC7、PB22 等剩余 GPIO | 仅为候选池，按实际外设启用情况分配 |

## 下一阶段必须完成的 PinMux 检查

1. 用 SysConfig 同时放入 4 路电机 PWM、2 路舵机 PWM、2 路硬件 QEI、2 路 UART、1 路 I2C、外接 ICM20948 SPI、显示 SPI 和所需 ADC。
2. 检查定时器通道是否共享同一计数周期，避免电机 PWM 与 20 ms 舵机 PWM 被迫共用周期。
3. 检查 DMA 触发源和通道数量，优先保证 UART 接收、显示发送和 ADC 采样。
4. 不将 PA5/PA6、PA8/PA9、PA19/PA20 当作普通空闲引脚；PB21/PB28 是否占用由外接 ICM20948 最终接线决定。
5. 冻结 PinMux 后再修改 `ti_msp_dl_config.*` 和 BSP 宏，禁止同时维护两套相互矛盾的引脚表。
