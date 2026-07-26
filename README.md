# MSPM0G3519 Project Template

已验证环境：Keil MDK、Arm Compiler 6、DAPLink、MSPM0G3519；默认程序为 PA14 板载 LED 闪烁测试。

```text
MSPM0G3519_Project_Template_v2/
├─ User/
│  ├─ Core/main.c
│  ├─ Config/
│  ├─ APP/
│  ├─ BSP/
│  ├─ Driver/
│  └─ Algorithm/
├─ keil/
├─ gcc/
├─ iar/
├─ ticlang/
├─ .gitignore
├─ README.md
└─ TEMPLATE_GUIDE.md
```

Keil 工程入口：`keil/MSPM0G3519_Template.uvprojx`。

<!-- DEBUG_UART_MAPPING_BEGIN -->
## 当前调试串口与相关引脚

- 测试日志、测试命令输入和串口回显仍统一使用 `DEBUG_UART_PORT`。
- `DEBUG_UART_PORT` 和 `UART_PORT_DEBUG` 名称保持不变，底层 `UART_DEBUG` 已由 UART0 改为 UART3。
- UART3：PC6 为 TX，PC7 为 RX，115200-8-N-1，通过外接 USB-TTL 连接电脑。
- UART0 不再由本工程作为调试串口使用。
- `LASER_EN` 保留原名称并迁移到 PC3。
- `HX711_DOUT` 保留原名称并迁移到 PC5。
- K210 仍使用 UART1（PB4/PB5），E220 仍使用 UART4（PB10/PB11）。
<!-- DEBUG_UART_MAPPING_END -->
