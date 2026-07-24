# 项目文档同步规则

- 新增模块或功能处于开发、联调或测试阶段时，不要在每次代码迭代后立即修改稳定文档，也不要把尚未验证的设计写入稳定文档。
- 同一模块或功能测试通过、结果稳定后，再一次性同步更新 `Doc/说明文档.md`；涉及参数、引脚、测试任务、显示使用方法或注意事项时，还应同步更新 `Doc` 中对应文档。
- 功能尚未测试通过时，只在 `Migration` 或交接记录中说明待验证项，不为了形式同步写入阶段性结论；用户明确要求提前记录的内容除外。
- `Doc` 只记录稳定、必要的功能说明、参数、使用方法和注意事项，不记录临时调试进度或当前阶段。
- 在项目全部功能完成前，不在稳定文档中反复维护当前任务表；只更新受本次代码修改直接影响的测试任务示例。
- 文档中的代码文件名、函数名、宏名和变量名必须与源码完全一致，不得擅自改名。
- `User/Test/test.h` 是嵌入式专项测试公共入口的唯一依据；新增、删除或改名 `Test_*` 入口后，必须同步测试任务文档和对应检查脚本。
- Markdown 中的任务列表示例必须使用宏续行格式；任务项注释使用 `/* 中文说明 */`，续行符 `\` 必须是该行最后一个非空白字符。
- 无法由源码、原理图或现有资料确认的硬件信息必须标记为“待确认”，不得猜测。
- 新功能测试通过并准备结束任务前，应复核文档与最终通过测试的源码一致。

# 代码注释规则

- 新增或修改的代码注释统一使用中文；函数名、变量名、宏名、协议字段和必须保持原样的日志文本除外。
- 注释说明用途、约束或原因，不记录临时操作过程和无长期价值的调试进度。
- 源码、脚本和 Markdown 统一使用 UTF-8；发现乱码必须先恢复原意再提交。

# 分层与封装规则

## 目录职责与允许依赖

- `User/Common` 只放与芯片和业务无关的公共契约，例如 `Project_Status_t` 和临界区抽象声明；禁止包含 MSPM0 DriverLib、BSP、Driver、APP、Route 或 Test 头文件。
- `User/BSP` 只负责 MSPM0G3519 外设、板级资源和 Common 抽象在目标板上的实现；不得依赖 Driver、Algorithm、Route、APP 或 Test。
- `User/Driver` 负责具体器件协议和器件状态机，可以依赖 BSP；不得依赖 Algorithm、Route、APP 或 Test。
- `User/Algorithm` 只负责由纯数据驱动的计算，可以依赖 Common、本层头文件和标准库；不得直接包含或调用 BSP、Driver、Route、APP 或 Test。
- `User/Route` 负责赛道状态和控制意图，可以依赖 Common 与 Algorithm；不得直接读取 BSP 节拍、具体传感器、Driver、APP、Motion 或底盘。
- `User/APP` 负责业务编排、控制权仲裁和上下层适配；Driver 数据转换为 Algorithm 输入、硬件状态转换为 Route 输入的代码放在 APP。
- `User/Test` 可以依赖被测各层，但测试声明、测试任务、桩实现和主机测试文件只能放在 Test 目录。
- `User/Core/main.c` 只负责按 `BSP_InitAll → Driver_Init → App_Init → Scheduler_Init` 顺序启动并运行调度器，不直接调用 `SYSCFG_DL_init()`，不承载器件协议、算法或比赛业务。

## 已解决问题形成的强制边界

- `User/Algorithm/attitude_estimator.*` 只接收 `Attitude_Input_t` 和 `motor_active`；采样转换与电机活动判断归 `User/APP/sensor_manager.*`。
- `User/Algorithm/odometer.*` 只接收左右累计毫米值并维护软件清零基准；硬件读取归 `User/APP/odometer_adapter.*`。
- Algorithm 和 Route 所需时间由 APP 以 `now_ms` 传入；禁止在这两层调用 `BSP_GET_TICK()`、`BSP_GetTickMs()`或包含 `bsp_systick.h`。
- Algorithm 和 Route 公共接口统一使用 `User/Common/project_status.h` 中的 `Project_Status_t` 与 `PROJECT_*`。
- Algorithm 需要临界区时只调用 `User/Common/project_critical.h`；目标板实现放在 BSP，主机桩放在 Test。
- I2C/SPI 共享总线只由 `BSP_InitAll()` 初始化一次；Driver 只能初始化器件状态并发起传输。
- Route 只输出 `Route_ControlMode_t`、`Route_ActionRequest_t` 等控制意图；底盘控制权、Motion 启动和 Driver 访问由 APP 处理。

## 新文件归属与构建同步

- 纯数据类型、通用状态码和可移植抽象接口放 Common；MSPM0G3519 实现放 BSP，主机替代实现放 Test。
- 单纯把 Driver 数据送入 Algorithm 的薄适配器放 APP，不把硬件读取塞进算法。
- 赛道规则放 Route，整场任务状态机放 APP，器件读写放 Driver，寄存器和引脚外设操作放 BSP。
- 新增或移动参与固件的 `.c` 文件时，必须同步检查 `keil/TraceTrackCar_TI.uvprojx`、`ticlang/makefile`、`gcc/makefile` 和 `iar/makefile`。
- Makefile 正式构建默认不加入 `User/Test/*.c`；专项测试构建显式使用 `BUILD_TESTS=1`。
- 正式固件保持 `PROJECT_TEST_TASKS_ENABLE=0U`；专项测试完成后恢复正式任务表再做最终构建。用户明确处于测试阶段时，不擅自替用户关闭测试配置。