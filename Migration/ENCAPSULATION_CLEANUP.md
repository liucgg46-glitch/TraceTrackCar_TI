# 封装与构建清理记录

## K210 道路通信测试文件

原 STM32 工程没有独立的 `APP/test_k210_road_comm.*`。道路通信测试函数
`Test_K210_RoadCommUpdate()` 原本位于 `Test/test_k210_comm.c`，并由
`Test/test_k210_comm.h` 声明。

迁移过程中在 `User/APP` 额外生成了一份同功能文件，造成测试代码进入正式 APP。
现有提交无法确认当时拆分该文件的具体动机；结合当时 TI Clang 构建只包含 APP、
未包含 Test 的情况，较可能是为了临时把道路日志加入固件。现已删除该重复文件，
唯一实现恢复为：

```text
User/Test/test_k210_comm.c
User/Test/test_k210_comm.h
```

## 平台初始化边界

`SYSCFG_DL_init()` 已移入 `BSP_InitAll()`。`User/Core/main.c` 不再直接依赖
`ti_msp_dl_config.h`，只保留 BSP、Driver、APP 和 Scheduler 启动链。

## 构建同步

以下构建入口均已覆盖 Core、Common、BSP、Driver、Algorithm、APP、Route、
VL53L1X 核心和平台层：

```text
keil/TraceTrackCar_TI.uvprojx
ticlang/makefile
gcc/makefile
iar/makefile
```

Makefile 默认不加入 `User/Test/*.c`。专项测试固件使用：

```text
gmake BUILD_TESTS=1
```

## 文本与注释

已清理迁移代码中的乱码注释、STM32 旧引脚说明、错误的 SPI/I2C 实例说明和
Algorithm 头文件中的英文说明。新增或改写文本统一保存为 UTF-8。