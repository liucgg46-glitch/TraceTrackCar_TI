# SOM_MSPM0G3519_V2.0 引脚约束与候选分配

本文件记录已从原理图确认的板级连接和当前 SysConfig 联合分配。引脚复用冲突已经由 SysConfig 校验通过；电气连接、方向和外设实物仍需上板验证。

## 第二阶段最小基线（已写入 SysConfig）

| 功能 | 引脚 | 当前配置 |
| --- | --- | --- |
| SWDIO / SWCLK | PA19 / PA20 | 调试接口，保留 |
| LED1 / LED2 | PA14 / PA17 | 推挽输出，初始高电平，低电平点亮 |
| SK6812 数据 | PA29 | 推挽输出，初始低电平；协议驱动尚未迁移 |
| USER 按键 | PB31 | 输入、内部上拉，低电平按下 |
| 外接 ICM20948 CS | PB12 | 推挽输出，初始高电平 |
| 外接 ICM20948 INT | PB21 | 浮空输入、GPIO 双边沿中断；与后轮编码器共用 GPIOB GROUP1 入口 |
| 原工程 KEY5 / USER 按键 | PB31 | 输入、内部上拉、低电平按下；KEY1~KEY4 禁用 |
| 电机 PWM | PC2 / PC4 / PC0 / PA28 | TIMA0 CCP0~CCP3，20 kHz，已冻结 |
| 电机方向 | PB6/PB7、PB8/PB9、PB20/PB24、PB25/PB27 | 四组 IN1/IN2，初始全部低电平，已冻结 |
| 左前编码器 | PA26 / PA27 | TIMG8 CCP0/CCP1 硬件 QEI，已冻结 |
| 右前编码器 | PB29 / PB30 | TIMG9 CCP0/CCP1 硬件 QEI，已冻结 |
| 左后编码器 | PB17 / PB18 | GPIO 双边沿软件 QEI，上拉和 3 周期滤波，已冻结 |
| 右后编码器 | PB19 / PB22 | GPIO 双边沿软件 QEI，上拉和 3 周期滤波，已冻结 |
| 系统时钟 | 内部 SYSOSC | 32 MHz |
| 系统节拍 | SysTick | 1 ms |

上述资源以及下节“联合冻结资源”已经放入同一份 SysConfig，并通过 LQFP-80 全局复用冲突检查。

## 第四阶段联合冻结资源

| 功能 | 引脚/外设 | 配置 |
| --- | --- | --- |
| K210 UART | PB4 TX / PB5 RX，UART1 | 115200-8-N-1，RX/TX FIFO 中断 |
| E220 UART | PB10 TX / PB11 RX，UART4 | 115200-8-N-1，AUX=PB28 上拉输入 |
| 传感器 I2C | PA0 SDA / PA1 SCL，I2C0 | 400 kHz；OLED、VL53L1X、灰度 MCU 共用 |
| 外接 ICM20948 SPI | PB16 SCLK / PB15 MOSI / PB14 MISO，SPI1 | 4 MHz、Mode 0；PB12 软件 CS、PB21 INT |
| 显示 SPI | PB3 SCLK / PB2 MOSI，SPI0 | 8 MHz、Mode 0；PC9 CS、PC8 DC、PB23 RESET、PA30 BL |
| 4051 灰度 | PA25 / ADC0 通道 2 | S0=PA24、S1=PA31、S2=PC1 |
| 双舵机 | PA12 / PA13，TIMG0 CCP0/CCP1 | 1 MHz 计数、20 ms 周期（50 Hz） |
| E220 AUX | PB28 | 上拉输入 |
| 激光 | PC6 | 推挽输出，初始关闭 |
| HX711 | PC7 DOUT / PA7 PD_SCK | DOUT 上拉输入，时钟初始低 |
| 蜂鸣器 | PA15 | 低电平有效，初始关闭 |

特别说明：本工程不使用核心板原理图中的 ICM45686。PB12/PB14/PB15/PB16/PB21 全部作为用户外接 ICM20948 的接口使用。

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

## 已采用的功能分配

| 功能 | 候选引脚/资源 | 说明 |
| --- | --- | --- |
| I2C 主总线 | PA1=SCL、PA0=SDA | 对应彩页 SCL/SDA；用于灰度模块、SSD1306、VL53L1X |
| K210 UART | PB4=TX、PB5=RX | 对应彩页 TX1/RX1 |
| E220/备用 UART | PB10=TX、PB11=RX | 对应彩页 TX2/RX2；AUX 另选普通输入 |
| 电机 PWM 1-4 | PC2、PC4、PC0、PA28 | 已由 SysConfig 验证并冻结为 TIMA0 CCP0-CCP3 |
| 电机方向 1-8 | PB6、PB7、PB8、PB9、PB20、PB24、PB25、PB27 | 已由 SysConfig 验证并冻结为普通 GPIO |
| 舵机 PWM 1-2 | PA12、PA13 | TIMG0 CCP0/CCP1，20 ms 周期，已通过 SysConfig 校验 |
| TFT/OLED SPI | PB3=CLK、PB2=MOSI、PC8=DC、PC9=CS、PB23=RES、PA30=BL | 使用核心板 U5 接口 |
| 外接 ICM20948 SPI | PB16=SCLK、PB15=MOSI、PB14=MISO、PB12=CS、PB21=INT | 复用源工程 `drv_icm20948.*` 协议层，只重写 MSPM0 底层适配 |
| 4051 灰度 ADC | PA25 / ADC0 通道 2 | S0=PA24、S1=PA31、S2=PC1 |
| 状态灯 | PA14、PA17、PA29 | 两个普通 LED 加一个 RGB |
| 用户按键 | PB31 | 已冻结为原 STM32 工程 KEY5；KEY1~KEY4 暂时禁用 |
| 前轮编码器 | PA26/PA27=TIMG8、PB29/PB30=TIMG9 | 两路硬件 QEI 已由 SysConfig 验证并冻结 |
| 后轮编码器 | PB17/PB18、PB19/PB22 | 已由 SysConfig 验证并冻结为双边沿 GPIO 软件正交解码 |
| VL53L1X XSHUT | PB26 或其他空闲 GPIO | 需确认没有与最终编码器/控制信号冲突 |
| 激光、HX711、蜂鸣器、E220 AUX | PC6；PC7/PA7；PA15；PB28 | 已写入 SysConfig，实物有效电平待上板复核 |

## 已完成检查与仍需上板确认

1. 已在同一份 SysConfig 中放入 4 路电机 PWM、2 路舵机 PWM、2 路硬件 QEI、2 路 UART、1 路 I2C、两路 SPI 和 ADC，生成通过。
2. 电机使用 TIMA0/20 kHz，舵机使用 TIMG0/50 Hz，两者不共享周期。
3. PA5/PA6、PA8/PA9、PA19/PA20 继续保留，不作为普通空闲引脚。
4. UART 使用 FIFO 中断；PB21 外接 ICM20948 INT 已迁移为 MSPM0 GPIO 双边沿中断；SPI0 显示使用 DMA_CH0/CH1，I2C0 使用 DMA_CH2/CH3。
5. 仍需实测：电机/编码器方向、舵机有效脉宽、HX711 与蜂鸣器有效电平、I2C 上拉、两路 SPI 信号完整性。
