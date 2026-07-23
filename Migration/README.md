# TraceTrackCar STM32F407 -> MSPM0G3519 迁移计划

## 项目边界

- 源工程：`D:\stm32project\TraceTrackCar`
- 目标工程：`D:\TIProject\TraceTrackCar_TI`
- 目标硬件：启是科技 `SOM_MSPM0G3519_V2.0` 核心板，不假定用户拥有 EVM 多功能底板。
- 目标芯片：原理图标注 `MSPM0G3519SPNR`，80 引脚 LQFP。
- IMU 实物：不使用原理图所示 ICM45686，使用用户自己的外接 ICM20948。
- 迁移原则：保留上层 API、算法、协议和业务行为；用 MSPM0 DriverLib/SysConfig 重写芯片相关实现；不把 STM32 启动代码和标准外设库混入目标固件。

## 分阶段计划

| 阶段 | 工作内容 | 当前状态 |
| --- | --- | --- |
| 1 | 盘点源/目标工程、复制可维护资源、建立可移植性基线、记录硬件约束 | 已完成 |
| 2 | 在 SysConfig 中确定时钟和完整 PinMux，重写公共状态、临界区、SysTick、GPIO，保持 LED 自检可运行 | 已完成：联合 PinMux 已通过生成校验 |
| 3 | 重写电机方向、4 路 PWM、编码器采集；先完成 2WD 闭环，再评估 4WD 后轮编码器实现 | 已完成代码迁移，待上板核对方向 |
| 4 | 重写 UART/I2C/SPI/ADC/DMA/中断 BSP，并保持原有非阻塞接口和超时语义 | 已完成；显示 SPI 与 I2C 异步接口使用真实 DMA |
| 5 | 逐个恢复电机、灰度、OLED/TFT、VL53L1X、K210、E220、HX711、舵机、激光、蜂鸣器等驱动 | Driver 全目录已编译；核心启动链已接入主要外设 |
| 6 | 迁移调度器、应用状态机、赛道逻辑和诊断功能，消除全部 STM32 头文件依赖 | 已完成编译迁移；待上板验证 |
| 7 | 多工具链编译检查、主机单测、上板分模块联调、控制参数复标和最终缺口报告 | 软件检查及缺口报告已完成；待用户上板联调与参数复标 |

## 第一阶段已完成内容

1. 将下列目录原样复制到 `User` 下，作为后续迁移基线：
   - `Common`
   - `Algorithm`
   - `Route`
   - `APP`
   - `Driver`
   - `BSP`
   - `Test`
   - `VL53L1_core`
   - `VL53L1_platform`
2. 将独立的 K210 配套工程完整复制到根目录 `K210`。
3. 将源工程历史文档复制到 `Doc/Legacy_STM32`。这些文档保留原 STM32 引脚语义，不代表 MSPM0 最终接线。
4. 对所有复制文件执行 SHA-256 校验，源/目标不一致文件数为 0。
5. 运行原有主机算法测试：
   - 姿态估计：5/5 通过。
   - 里程计：3/3 通过。
6. 资料核对覆盖：
   - MSPM0G3519 中文数据手册。
   - 核心板 V2.0 原理图。
   - 开箱手册。
   - 核心板引脚分配图和板载资源图。

`Migration/MIGRATED_FILE_MANIFEST.sha256` 记录的是第一阶段“刚复制完成”时的源文件基线。进入后续移植后，目标文件会被有意改写，因此目标文件的当前哈希不再要求与该基线一致。

## 第二阶段已完成的最小硬件基线

1. 修正目标器件封装：
   - 原模板误选为 100 引脚 `LQFP-100(PZ)`。
   - 已按核心板原理图上的 `MSPM0G3519SPNR` 改为 80 引脚 `LQFP-80(PN)`。
2. SysConfig 已生成并验证以下最小资源：
   - PA19/PA20：SWDIO/SWCLK。
   - PA14/PA17：板载 LED1/LED2，均按低电平点亮处理。
   - PA29：板载 SK6812 数据输出。
   - PB31：USER 按键输入，上拉，低电平按下。
   - PB12：外接 ICM20948 片选，启动后保持高电平。
   - PB21：外接 ICM20948 中断输入，GPIO 双边沿中断。
   - SysTick：1 ms 中断节拍。
3. 当前基线使用内部 SYSOSC 32 MHz，不依赖核心板 40 MHz 外部晶振；在通信速率和控制周期最终冻结前再决定是否切换 HFXT。
4. 已用 MSPM0 DriverLib 重写公共临界区、SysTick 和最小 GPIO 层，`main` 仅执行初始化和板级自检：
   - LED1 每 500 ms 翻转。
   - LED2 跟随 USER 按键。
   - 外接 ICM20948 的 CS 保持未选中状态。
5. TI Clang 严格编译和完整链接均通过；当前固件尺寸为：
   - `.text`：1144 字节。
   - `.data`：0 字节。
   - `.bss`：260 字节。
6. 原有主机算法测试继续通过：
   - 姿态估计：5/5。
   - 里程计：3/3。

第二阶段尚未完成的部分是舵机、UART、I2C、两组 SPI、ADC 和 DMA 的联合复用检查。只有这些资源在同一份 SysConfig 中无冲突后，完整 PinMux 才能冻结。

上述联合复用检查现已在第四阶段完成；本段保留为阶段历史记录。

## 第三阶段当前进度

1. 原 STM32 工程的 KEY5 已迁移为核心板 USER 按键：
   - 引脚：PB31。
   - 输入方式：内部上拉。
   - 有效电平：低电平。
   - 保留原有非阻塞消抖、按下事件和释放事件 API。
   - KEY1~KEY4 暂时禁用，不占用 MSPM0 引脚。
2. 四路直流电机输出已经迁移：
   - FL PWM：PC2 / TIMA0 CCP0。
   - FR PWM：PC4 / TIMA0 CCP1。
   - RL PWM：PC0 / TIMA0 CCP2。
   - RR PWM：PA28 / TIMA0 CCP3。
   - PWM 频率：20 kHz。
   - 上电比较值为 1600，对应 0% 占空比。
   - 四组方向脚分别为 PB6/PB7、PB8/PB9、PB20/PB24、PB25/PB27，启动时全部为低电平。
3. 前轮编码器已经迁移到两组硬件 QEI：
   - 左前：PA26/PA27 / TIMG8。
   - 右前：PB29/PB30 / TIMG9。
   - BSP 保留原有增量、累计计数和 counts/s 接口。
4. 4WD 后轮编码器已迁移为软件正交解码：
   - 左后：PB17/PB18。
   - 右后：PB19/PB22。
   - 四个输入均使用内部上拉、3 周期数字滤波和上升/下降双边沿中断。
   - 合法格雷码状态变化每个边沿累计一次；两位同时变化按毛刺或丢边沿处理，不累计。
   - GROUP1 中断只读取和清除这四个编码器引脚的中断标志。
5. `drv_encoder` 已接入启动链，四轮继续使用统一的 FL/FR/RL/RR 增量、累计计数、counts/s 和 mm/s 接口。
6. TI Clang 已使用 `-Wall -Wextra -Werror` 成功编译并链接 `bsp_key`、`bsp_pwm`、`bsp_encoder`、`drv_motor` 和 `drv_encoder`。当前固件 `.text` 为 3504 字节、`.bss` 为 788 字节。电机初始化只执行安全停车，不会在启动自检中驱动车轮。

## 第四、五阶段本轮进度

1. 完整联合 PinMux 已通过 SysConfig 校验：
   - K210：PB4/PB5，UART1，115200。
   - E220：PB10/PB11，UART4，115200；AUX=PB28。
   - I2C0：PA0/PA1，400 kHz。
   - 外接 ICM20948：SPI1，PB16/PB15/PB14，CS=PB12，INT=PB21。
   - 显示：SPI0，PB3/PB2，CS=PC9，DC=PC8，RESET=PB23，BL=PA30。
   - 灰度 4051：PA25/ADC0 通道 2，S0/S1/S2=PA24/PA31/PC1。
   - 舵机：PA12/PA13，TIMG0，50 Hz。
2. 已用 MSPM0 DriverLib 重写 `bsp_uart`、`bsp_i2c`、`bsp_spi`、`bsp_adc`，并扩充 GPIO/PWM：
   - UART 为 RX/TX FIFO 中断加软件环形缓冲。
   - I2C/SPI 的同步接口、超时和忙状态已保留。
   - 显示 SPI0 使用 DMA_CH0/CH1；I2C0 使用 DMA_CH2/CH3。
   - 异步调用立即启动 DMA，任务函数只处理完成回调和超时，不再轮询搬运整帧。
3. TI Clang 已编译并链接 `User/Driver` 全目录、VL53L1X 核心/平台层和 `k210_comm.c`。
4. 主启动链当前实际启用：四电机、四编码器、PB31 用户键、外接 ICM20948、4051 灰度、双舵机、E220 和 K210。只有核心板时不会强制启动 OLED/TFT、VL53L1X、HX711、激光和蜂鸣器的设备访问。
5. 当前固件尺寸：`.text` 18712 字节、`.data` 0 字节、`.bss` 3486 字节。
6. 严格构建通过，姿态估计 5/5、里程计 3/3 主机测试继续通过。

## 第六阶段编译迁移结果

1. `User/Common`、`User/Algorithm`、`User/APP` 和 `User/Route` 中的全部 C 源文件均已加入 TI Clang 与 Keil 工程。
2. 启动流程已切换为完整分层启动链：
   - `BSP_InitAll`
   - `Driver_Init`
   - `App_Init`
   - `Scheduler_Init`
   - 主循环持续执行 `Scheduler_Run`
3. 调度任务已经包含 `Key_Update`，原 KEY5 功能由核心板 PB31 USER 按键提供；KEY1~KEY4 保持禁用。
4. Keil 工程按原工程层次登记 `Common/BSP/Algorithm/Driver/Test/APP/Route/VL53L1_core/VL53L1_platform`：
   - 工程文件总数 147。
   - `.h` 文件 77 个。
   - 路径重复项 0。
   - 源工程上述九个目录到目标磁盘的缺失数为 0，目标磁盘到 Keil 工程的缺失数也为 0。
   - 两个 `Test` 专项测试 `.c` 在工程树中可见，但默认排除正式固件编译，避免与正式 APP 调试入口重复定义。
   - `Migration/sync_keil_project.ps1` 可在以后增删模块后重新同步分组。
5. TI Clang 使用 `-Wall -Wextra -Werror` 清理后全量构建通过，当前固件尺寸为：
   - `.text`：64880 字节。
   - `.data`：0 字节。
   - `.bss`：25461 字节。
6. 姿态估计 5/5、里程计 3/3 主机测试继续通过。
7. `bsp_exti.c/.h` 已改写为 MSPM0 GPIO 中断：
   - CH1 对应外接 ICM20948 的 PB21 INT。
   - PB21 与 PB17/PB18/PB19/PB22 后轮软件编码器共用 GPIOB GROUP1 中断入口，由统一入口读取、清除并分发标志。
   - CH2/CH3 因核心板没有原 STM32 PE1/PE2 的等价固定连接，保持禁用，后续如增加实物接线再分配。
8. Keil ARM Compiler 6.24 已实际完成全量构建与链接：0 Error、0 Warning；生成 AXF 和 HEX。

## 最终软件验证结果

1. SysConfig 在 LQFP-80 MSPM0G3519 上完成联合资源校验，四个 DMA Full Channel 无冲突。
2. TI Clang 使用 `-Wall -Wextra -Werror` 完整构建和链接通过：
   - `.text`：66624 字节。
   - `.data`：0 字节。
   - `.bss`：25477 字节。
3. Keil ARM Compiler 6.24 完整构建：0 Error、0 Warning。
4. 姿态估计主机测试 5/5、里程计主机测试 3/3。
5. Keil 工程保持 147 个文件、77 个头文件、0 个重复路径；业务层源文件缺失数为 0。
6. 最终上板顺序、外接 ICM20948 接线、DMA 边界和无法离线验证的项目见 `Migration/FINAL_GAPS_AND_BRINGUP.md`。

## 不直接复制进目标固件的内容

以下内容没有丢失业务功能，但不能作为 MSPM0 固件源码直接使用：

- STM32F4 CMSIS、STM32 标准外设库和 `libraries`。
- STM32 启动文件、`stm32f4xx_it.*`、链接脚本和旧 Keil/调试工程。
- `build`、`Objects`、`Listings`、临时文件和历史二进制产物。
- 旧工程中与 STM32 DMA Stream/Channel、NVIC 分组和定时器实例绑定的配置。

这些内容属于旧芯片工具链或生成物。后续将用目标工程已有的 MSPM0 SDK、启动文件、链接文件和 SysConfig 生成文件替代。

## 完成判定

只有满足以下条件才算“全部可迁移功能已完成”：

- 目标工程不再包含 `stm32f4xx*.h`、STM32 标准外设库类型或 STM32 中断处理依赖。
- 目标固件能够从 `BSP_InitAll -> Driver_Init -> App_Init -> Scheduler_Init` 启动并持续运行。
- 原有主机算法测试继续通过。
- 每个已接入外设完成独立上板测试。
- 2WD/4WD、电机方向、编码器方向、灰度阈值、PID/PI 参数按 MSPM0 的实际采样周期重新验证。
- 无法验证或无法等价实现的项目在最终缺口清单中明确标为“待确认”或“需要额外硬件”。
