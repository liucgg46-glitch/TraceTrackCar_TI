@echo off
setlocal EnableExtensions DisableDelayedExpansion
chcp 65001 >nul
title TraceTrackCar_TI 封装与构建修复 v2

set "SELF=%~f0"
set "ROOT=%~dp0"

if exist "%ROOT%User\Core\main.c" goto :root_ready
if exist "%CD%\User\Core\main.c" (
    set "ROOT=%CD%"
    goto :root_ready
)
if exist "D:\TIProject\TraceTrackCar_TI\User\Core\main.c" (
    set "ROOT=D:\TIProject\TraceTrackCar_TI"
    goto :root_ready
)

echo [错误] 未找到 TraceTrackCar_TI 工程根目录。
echo 请把本 BAT 放到工程根目录后运行，
echo 或先在工程根目录中打开命令行再运行。
pause
exit /b 1

:root_ready
for %%I in ("%ROOT%.") do set "ROOT=%%~fI"
set "TMPPS=%TEMP%\TraceTrackCar_TI_fix_%RANDOM%_%RANDOM%.ps1"

echo 工程目录：%ROOT%
echo 正在提取修复脚本……

powershell -NoProfile -ExecutionPolicy Bypass -Command ^
  "$raw=[IO.File]::ReadAllText($env:SELF,[Text.Encoding]::UTF8);" ^
  "$marker='#'+'<POWERSHELL>';" ^
  "$index=$raw.IndexOf($marker);" ^
  "if($index -lt 0){throw 'BAT 内部 PowerShell 标记缺失。'};" ^
  "$payload=$raw.Substring($index+$marker.Length).TrimStart([char]13,[char]10);" ^
  "[IO.File]::WriteAllText($env:TMPPS,$payload,(New-Object Text.UTF8Encoding($true)))"

if errorlevel 1 (
    echo [错误] 无法提取内部 PowerShell 脚本。
    if exist "%TMPPS%" del /f /q "%TMPPS%" >nul 2>nul
    pause
    exit /b 1
)

powershell -NoProfile -ExecutionPolicy Bypass -File "%TMPPS%" -Root "%ROOT%"
set "RC=%ERRORLEVEL%"

if exist "%TMPPS%" del /f /q "%TMPPS%" >nul 2>nul

if not "%RC%"=="0" (
    echo.
    echo [失败] 修复脚本中途停止，错误码：%RC%
    echo 已修改文件均在脚本输出的 TEMP 备份目录中保留原件。
    pause
    exit /b %RC%
)

echo.
echo [完成] 请在 GitHub Desktop 中检查 Changes，再使用 Keil 全量编译。
pause
exit /b 0

#<POWERSHELL>
param(
    [Parameter(Mandatory = $true)]
    [string]$Root
)

$ErrorActionPreference = 'Stop'
$Root = [System.IO.Path]::GetFullPath($Root)
$Utf8NoBom = New-Object System.Text.UTF8Encoding($false)
$Timestamp = Get-Date -Format 'yyyyMMdd_HHmmss'
$BackupRoot = Join-Path $env:TEMP ("TraceTrackCar_TI_fix_backup_" + $Timestamp)
$BackedUp = @{}

function Get-ProjectPath {
    param([Parameter(Mandatory = $true)][string]$RelativePath)
    return Join-Path $Root ($RelativePath -replace '/', [System.IO.Path]::DirectorySeparatorChar)
}

function Backup-ProjectFile {
    param([Parameter(Mandatory = $true)][string]$RelativePath)

    if ($BackedUp.ContainsKey($RelativePath)) {
        return
    }

    $Source = Get-ProjectPath $RelativePath
    if (Test-Path -LiteralPath $Source) {
        $Destination = Join-Path $BackupRoot ($RelativePath -replace '/', [System.IO.Path]::DirectorySeparatorChar)
        $DestinationDir = Split-Path -Parent $Destination
        [System.IO.Directory]::CreateDirectory($DestinationDir) | Out-Null
        Copy-Item -LiteralPath $Source -Destination $Destination -Force
    }

    $BackedUp[$RelativePath] = $true
}

function Read-ProjectText {
    param([Parameter(Mandatory = $true)][string]$RelativePath)

    $Path = Get-ProjectPath $RelativePath
    if (-not (Test-Path -LiteralPath $Path)) {
        throw "缺少文件：$RelativePath"
    }
    return [System.IO.File]::ReadAllText($Path, [System.Text.Encoding]::UTF8)
}

function Write-ProjectText {
    param(
        [Parameter(Mandatory = $true)][string]$RelativePath,
        [Parameter(Mandatory = $true)][string]$Content
    )

    Backup-ProjectFile $RelativePath
    $Path = Get-ProjectPath $RelativePath
    [System.IO.Directory]::CreateDirectory((Split-Path -Parent $Path)) | Out-Null
    $Normalized = $Content -replace "`r?`n", "`r`n"
    [System.IO.File]::WriteAllText($Path, $Normalized, $Utf8NoBom)
    Write-Host "[写入] $RelativePath"
}

function Remove-ProjectFile {
    param([Parameter(Mandatory = $true)][string]$RelativePath)

    $Path = Get-ProjectPath $RelativePath
    if (Test-Path -LiteralPath $Path) {
        Backup-ProjectFile $RelativePath
        Remove-Item -LiteralPath $Path -Force
        Write-Host "[删除] $RelativePath"
    } else {
        Write-Host "[跳过] $RelativePath 已不存在"
    }
}

function Replace-BlockCommentBefore {
    param(
        [Parameter(Mandatory = $true)][string]$Text,
        [Parameter(Mandatory = $true)][string]$Marker,
        [Parameter(Mandatory = $true)][string]$NewComment
    )

    $MarkerIndex = $Text.IndexOf($Marker)
    if ($MarkerIndex -lt 0) {
        throw "未找到代码标记：$Marker"
    }

    $CommentEnd = $Text.LastIndexOf('*/', $MarkerIndex)
    if ($CommentEnd -lt 0) {
        throw "在 $Marker 前未找到块注释结尾"
    }

    $CommentStart = $Text.LastIndexOf('/*', $CommentEnd)
    if ($CommentStart -lt 0) {
        throw "在 $Marker 前未找到块注释开头"
    }

    return $Text.Substring(0, $CommentStart) +
           $NewComment +
           $Text.Substring($CommentEnd + 2)
}

function Replace-LineCommentBefore {
    param(
        [Parameter(Mandatory = $true)][string]$Text,
        [Parameter(Mandatory = $true)][string]$Marker,
        [Parameter(Mandatory = $true)][string]$NewComment
    )

    $Pattern = '(?m)^//[^\r\n]*\r?\n(?=' + [regex]::Escape($Marker) + ')'
    $Regex = New-Object System.Text.RegularExpressions.Regex($Pattern)
    if (-not $Regex.IsMatch($Text)) {
        throw "在 $Marker 前未找到单行注释"
    }
    return $Regex.Replace($Text, $NewComment + "`r`n", 1)
}

function Replace-LiteralRequired {
    param(
        [Parameter(Mandatory = $true)][string]$Text,
        [Parameter(Mandatory = $true)][string]$Old,
        [Parameter(Mandatory = $true)][string]$New,
        [Parameter(Mandatory = $true)][string]$Description
    )

    if (-not $Text.Contains($Old)) {
        throw "未找到待替换内容：$Description"
    }
    return $Text.Replace($Old, $New)
}

if (-not (Test-Path -LiteralPath (Get-ProjectPath 'User/Core/main.c'))) {
    throw "指定目录不是 TraceTrackCar_TI 工程根目录：$Root"
}

[System.IO.Directory]::CreateDirectory($BackupRoot) | Out-Null
Write-Host "工程目录：$Root"
Write-Host "备份目录：$BackupRoot"
Write-Host ""

# -----------------------------------------------------------------------------
# 1. K210 道路通信测试归位
# -----------------------------------------------------------------------------
$CanonicalK210Test = Read-ProjectText 'User/Test/test_k210_comm.c'
if (-not $CanonicalK210Test.Contains('void Test_K210_RoadCommUpdate(void)')) {
    throw 'User/Test/test_k210_comm.c 中缺少 Test_K210_RoadCommUpdate，停止删除重复文件。'
}

$CanonicalK210Header = Read-ProjectText 'User/Test/test_k210_comm.h'
if (-not $CanonicalK210Header.Contains('void Test_K210_RoadCommUpdate(void);')) {
    throw 'User/Test/test_k210_comm.h 中缺少 Test_K210_RoadCommUpdate 声明，停止删除重复文件。'
}

Remove-ProjectFile 'User/APP/test_k210_road_comm.c'
Remove-ProjectFile 'User/APP/test_k210_road_comm.h'

$ProjectFile = 'keil/TraceTrackCar_TI.uvprojx'
$ProjectXml = Read-ProjectText $ProjectFile
$ProjectXml = [regex]::Replace(
    $ProjectXml,
    '(?ms)\r?\n\s*<File>\s*<FileName>test_k210_road_comm\.c</FileName>.*?<FilePath>\.\./User/APP/test_k210_road_comm\.c</FilePath>.*?</File>',
    ''
)
$ProjectXml = [regex]::Replace(
    $ProjectXml,
    '(?ms)\r?\n\s*<File>\s*<FileName>test_k210_road_comm\.h</FileName>.*?<FilePath>\.\./User/APP/test_k210_road_comm\.h</FilePath>.*?</File>',
    ''
)
Write-ProjectText $ProjectFile $ProjectXml

# 统一 K210 专项测试头文件，使文件名、头文件保护宏和实际职责一致。
Write-ProjectText 'User/Test/test_k210_comm.h' @'
#ifndef __TEST_K210_COMM_H
#define __TEST_K210_COMM_H

#include "k210_comm.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
 * K210 专项通信测试公共接口。
 * 数字、道路和配置档位测试均集中在 test_k210_comm.c。
 */
extern volatile K210_Comm_Info_t g_k210_digit_debug_info;
extern volatile K210_Comm_Info_t g_k210_road_debug_info;
extern volatile uint8_t g_k210_profile_test_selected;
extern volatile uint8_t g_k210_profile_test_remaining;
extern volatile uint32_t g_k210_profile_test_tx_count;
extern volatile uint32_t g_k210_profile_test_busy_count;

void Test_K210_DigitCommUpdate(void);
void Test_K210_RoadCommUpdate(void);
void Test_K210_VisionCommUpdate(void);
void Test_K210_RoadProfileUpdate(void);
void Test_K210_SingleDigitCommUpdate(void);

#ifdef __cplusplus
}
#endif

#endif /* __TEST_K210_COMM_H */
'@

# 清理专项测试公共头文件中的旧平台说明和零散单行注释。
Write-ProjectText 'User/Test/test.h' @'
#ifndef __TEST_H
#define __TEST_H

#include "test_config.h"

#if (PROJECT_TEST_TASKS_ENABLE != 0U)

/* 基础板级资源测试。 */
void Test_GPIO_Toggle(void);
void Test_StatusLight_Update(void);
void Test_Buzzer_Update(void);
void Test_PWM_Ramp(void);
void Test_Encoder_Log(void);
void Test_Key_Update(void);
void Test_EXTI_Init(void);
void Test_EXTI_Log(void);
void Test_UART_Echo(void);
void Test_UART_Stats(void);
void Test_I2C_Scan(void);

/* 灰度和循迹测试。 */
void Test_Gray4051_Update(void);
void Test_Gray4051_Log(void);
void Test_LineCmd_Update(void);
void Test_LineCmd_Log(void);
void Test_RouteCmd_Update(void);
void Test_RouteLog(void);

/* 执行器、底盘和里程测试。 */
void Test_E220_Link_Update(void);
void Test_MotorCmd_Update(void);
void Test_MotorCmd_Log(void);
void Test_ChassisCmd_Update(void);
void Test_ChassisWatchdog_Update(void);
void Test_ChassisCmd_Log(void);
void Test_DrvEncoder_Log(void);
void Test_CountPerRev_Update(void);
void Test_MotionCmd_Update(void);
void Test_MotionCmd_Log(void);
void Test_DriveProfile_Update(void);

/* 传感器和显示测试。 */
void Test_VL53L1X_Update(void);
void Test_HX711_Update(void);
void Test_ICM20948_Update(void);
void Test_ICM20948_Mag_Update(void);
void Test_Attitude_Update(void);
void Test_LCD_Ascii_Update(void);
void Test_OLED_Ascii_Update(void);
void Test_AsyncDisplay_Update(void);

/* 应用和通信测试。 */
void Test_TaskFSM_Log(void);
void Test_K210_CommUpdate(void);

#endif /* PROJECT_TEST_TASKS_ENABLE */

#endif /* __TEST_H */
'@

# 修正 K210 专项测试文件中的旧平台接线说明。
$CanonicalK210Test = Read-ProjectText 'User/Test/test_k210_comm.c'
$CanonicalK210Test = Replace-BlockCommentBefore `
    $CanonicalK210Test `
    'static void Test_K210_SendText(const char *text)' `
@'
/*
 * ============================================================================
 * K210 统一通信测试
 * ============================================================================
 *
 * 文件编码：UTF-8
 *
 * K210 通信：
 *   K210 IO8 TX -> MSPM0G3519 PB5 / UART1 RX
 *   K210 IO6 RX <- MSPM0G3519 PB4 / UART1 TX
 *
 * 调试输出：
 *   MSPM0G3519 PA10 / UART0 TX -> 核心板 CH340 Type-C 串口
 *
 * 本文件统一包含数字识别、道路识别及后续视觉通信测试。
 * 所有日志通过 DEBUG_UART_PORT 输出，不依赖 printf 串口重定向。
 * ============================================================================
 */
'@
Write-ProjectText 'User/Test/test_k210_comm.c' $CanonicalK210Test

# 若其他文件曾错误引用拆分头文件，统一改回正式测试头文件。
$SourceFiles = Get-ChildItem -LiteralPath (Get-ProjectPath 'User') -Recurse -File |
    Where-Object { $_.Extension -in @('.c', '.h') }
foreach ($File in $SourceFiles) {
    $Relative = $File.FullName.Substring($Root.Length).TrimStart('\', '/') -replace '\\', '/'
    if ($Relative -in @('User/APP/test_k210_road_comm.c', 'User/APP/test_k210_road_comm.h')) {
        continue
    }

    $Text = [System.IO.File]::ReadAllText($File.FullName, [System.Text.Encoding]::UTF8)
    if ($Text.Contains('"test_k210_road_comm.h"')) {
        $Text = $Text.Replace('"test_k210_road_comm.h"', '"test_k210_comm.h"')
        Write-ProjectText $Relative $Text
    }
}

# -----------------------------------------------------------------------------
# 2. 将 TI 平台初始化完整收口到 BSP
# -----------------------------------------------------------------------------
Write-ProjectText 'User/Core/main.c' @'
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
'@

Write-ProjectText 'User/BSP/bsp_all.h' @'
#ifndef __BSP_ALL_H
#define __BSP_ALL_H

#include "bsp_common.h"
#include "bsp_systick.h"
#include "bsp_gpio.h"
#include "bsp_exti.h"
#include "bsp_pwm.h"
#include "bsp_encoder.h"
#include "bsp_adc.h"
#include "bsp_key.h"
#include "bsp_uart.h"
#include "bsp_i2c.h"
#include "bsp_spi.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
 * BSP 统一初始化入口。
 * 平台级 SysConfig 初始化和各 BSP 模块初始化均在本函数内部完成，
 * Core 层不直接依赖 TI 生成接口。
 */
BSP_Status_t BSP_InitAll(void);
void BSP_TaskAll(void);

#ifdef __cplusplus
}
#endif

#endif /* __BSP_ALL_H */
'@

Write-ProjectText 'User/BSP/bsp_all.c' @'
#include "bsp_all.h"

BSP_Status_t BSP_InitAll(void)
{
    BSP_Status_t ret;

    /*
     * TI SysConfig 生成的时钟、引脚、外设和 DMA 初始化属于板级职责。
     * 必须在任何 BSP 模块访问外设前执行一次。
     */
    SYSCFG_DL_init();

    ret = BSP_SysTick_Init(BSP_GetCoreClockHz());
    if (ret != BSP_OK) return ret;

    BSP_GPIO_InitAll();
    BSP_EXTI_InitAll();
    BSP_PWM_InitAll();
    BSP_Encoder_InitAll();
    BSP_ADC_Init();
    BSP_Key_InitAll();

    BSP_UART_InitAll();
    BSP_I2C_InitAll();
    BSP_SPI_InitAll();

    return BSP_OK;
}

void BSP_TaskAll(void)
{
    BSP_UART_TaskAll();
    BSP_I2C_TaskAll();
    BSP_SPI_TaskAll();
}
'@

# -----------------------------------------------------------------------------
# 3. 完整化 TI Clang、GCC 和 IAR 构建
# -----------------------------------------------------------------------------
Write-ProjectText 'ticlang/makefile' @'
MSPM0_SDK_INSTALL_DIR ?= D:/TI/CCS/mspm0_sdk_2_11_00_07
BUILD_TESTS ?= 0

ifneq ($(wildcard $(MSPM0_SDK_INSTALL_DIR)/imports.mak),)
include $(MSPM0_SDK_INSTALL_DIR)/imports.mak
else
TICLANG_ARMCOMPILER ?= D:/TI/CCS/ccs/tools/compiler/ti-cgt-armllvm_5.1.1.LTS
SYSCONFIG_TOOL ?= D:/TI/CCS/ccs/utils/sysconfig_1.28.0/sysconfig_cli.bat
RM = cmd /c del /f /q
DEVNULL = NUL
SLASH_FIXUP = $(subst /,\,$1)
endif

CC = "$(TICLANG_ARMCOMPILER)/bin/tiarmclang"
LNK = "$(TICLANG_ARMCOMPILER)/bin/tiarmclang"

SYSCFG_CMD_STUB = $(SYSCONFIG_TOOL) --compiler ticlang --product $(MSPM0_SDK_INSTALL_DIR)/.metadata/product.json
SYSCFG_FILES := $(shell $(SYSCFG_CMD_STUB) --listGeneratedFiles --listReferencedFiles --output . ../User/Config/empty_mspm0g3519.syscfg)

SYSCFG_C_FILES = $(filter %.c,$(SYSCFG_FILES))
SYSCFG_H_FILES = $(filter %.h,$(SYSCFG_FILES))
SYSCFG_OPT_FILES = $(filter %.opt,$(SYSCFG_FILES))

TEST_C_FILES =
ifeq ($(BUILD_TESTS),1)
TEST_C_FILES := $(wildcard ../User/Test/*.c)
endif

USER_C_FILES = \
    ../User/Core/main.c \
    $(wildcard ../User/Common/*.c) \
    $(wildcard ../User/BSP/*.c) \
    $(wildcard ../User/Algorithm/*.c) \
    $(wildcard ../User/Driver/*.c) \
    $(wildcard ../User/APP/*.c) \
    $(wildcard ../User/Route/*.c) \
    $(TEST_C_FILES) \
    $(wildcard ../User/VL53L1_core/*.c) \
    $(wildcard ../User/VL53L1_platform/*.c)

OBJECTS = \
    $(patsubst %.c,%.obj,$(notdir $(USER_C_FILES))) \
    $(patsubst %.c,%.obj,$(notdir $(SYSCFG_C_FILES)))

NAME = TraceTrackCar_TI

CFLAGS = \
    -I. \
    -I../User/Core \
    -I../User/Config \
    -I../User/Common \
    -I../User/BSP \
    -I../User/Driver \
    -I../User/Algorithm \
    -I../User/APP \
    -I../User/Route \
    -I../User/Test \
    -I../User/VL53L1_core \
    -I../User/VL53L1_platform \
    $(addprefix @,$(SYSCFG_OPT_FILES)) \
    -O2 \
    "-I$(MSPM0_SDK_INSTALL_DIR)/source/third_party/CMSIS/Core/Include" \
    "-I$(MSPM0_SDK_INSTALL_DIR)/source" \
    -gdwarf-3 \
    -mcpu=cortex-m0plus \
    -march=thumbv6m \
    -mfloat-abi=soft \
    -mthumb \
    -std=c11 \
    -Wall \
    -Wextra \
    -Werror

LFLAGS = \
    -ldevice.cmd.genlibs \
    "-L$(MSPM0_SDK_INSTALL_DIR)/source" \
    -L. \
    device_linker.cmd \
    "-Wl,-m,$(NAME).map" \
    -Wl,--rom_model \
    -Wl,--warn_sections \
    "-L$(TICLANG_ARMCOMPILER)/lib" \
    -llibc.a

.PHONY: all clean syscfg

all: $(NAME).out

.INTERMEDIATE: syscfg
$(SYSCFG_FILES): syscfg
	@ echo generation complete

syscfg: ../User/Config/empty_mspm0g3519.syscfg
	@ echo Generating configuration files...
	@ $(SYSCFG_CMD_STUB) --output $(@D) $<

define C_RULE
$(basename $(notdir $(1))).obj: $(1) $(SYSCFG_H_FILES)
	@ echo Building $$@
	@ $(CC) $(CFLAGS) -c $$< -o $$@
endef

$(foreach c_file,$(SYSCFG_C_FILES),$(eval $(call C_RULE,$(c_file))))
$(foreach c_file,$(USER_C_FILES),$(eval $(call C_RULE,$(c_file))))

$(NAME).out: $(OBJECTS)
	@ echo Linking $@
	@ $(LNK) -Wl,-u,_c_int00 $(OBJECTS) $(LFLAGS) -o $@

clean:
	@ echo Cleaning...
	-@ $(RM) $(call SLASH_FIXUP,$(OBJECTS)) > $(DEVNULL) 2>&1
	-@ $(RM) $(NAME).out > $(DEVNULL) 2>&1
	-@ $(RM) $(NAME).map > $(DEVNULL) 2>&1
	-@ $(RM) $(call SLASH_FIXUP,$(SYSCFG_FILES)) > $(DEVNULL) 2>&1
'@

Write-ProjectText 'gcc/makefile' @'
MSPM0_SDK_INSTALL_DIR ?= D:/TI/CCS/mspm0_sdk_2_11_00_07
BUILD_TESTS ?= 0

include $(MSPM0_SDK_INSTALL_DIR)/imports.mak

CC = "$(GCC_ARMCOMPILER)/bin/arm-none-eabi-gcc"
LNK = "$(GCC_ARMCOMPILER)/bin/arm-none-eabi-gcc"

SYSCFG_CMD_STUB = $(SYSCONFIG_TOOL) --compiler gcc --product $(MSPM0_SDK_INSTALL_DIR)/.metadata/product.json
SYSCFG_FILES := $(shell $(SYSCFG_CMD_STUB) --listGeneratedFiles --listReferencedFiles --output . ../User/Config/empty_mspm0g3519.syscfg)

SYSCFG_C_FILES = $(filter %.c,$(SYSCFG_FILES))
SYSCFG_H_FILES = $(filter %.h,$(SYSCFG_FILES))

TEST_C_FILES =
ifeq ($(BUILD_TESTS),1)
TEST_C_FILES := $(wildcard ../User/Test/*.c)
endif

USER_C_FILES = \
    ../User/Core/main.c \
    $(wildcard ../User/Common/*.c) \
    $(wildcard ../User/BSP/*.c) \
    $(wildcard ../User/Algorithm/*.c) \
    $(wildcard ../User/Driver/*.c) \
    $(wildcard ../User/APP/*.c) \
    $(wildcard ../User/Route/*.c) \
    $(TEST_C_FILES) \
    $(wildcard ../User/VL53L1_core/*.c) \
    $(wildcard ../User/VL53L1_platform/*.c)

STARTUP_C_FILES = ../gcc/startup_mspm0g3519_gcc.c
ALL_C_FILES = $(USER_C_FILES) $(SYSCFG_C_FILES) $(STARTUP_C_FILES)
OBJECTS = $(patsubst %.c,%.obj,$(notdir $(ALL_C_FILES)))
NAME = TraceTrackCar_TI

CFLAGS = \
    -I. \
    -I../User/Core \
    -I../User/Config \
    -I../User/Common \
    -I../User/BSP \
    -I../User/Driver \
    -I../User/Algorithm \
    -I../User/APP \
    -I../User/Route \
    -I../User/Test \
    -I../User/VL53L1_core \
    -I../User/VL53L1_platform \
    -D__MSPM0G3519__ \
    -O2 \
    "-I$(MSPM0_SDK_INSTALL_DIR)/source/third_party/CMSIS/Core/Include" \
    "-I$(MSPM0_SDK_INSTALL_DIR)/source" \
    -mcpu=cortex-m0plus \
    -march=armv6-m \
    -mthumb \
    -mfloat-abi=soft \
    -std=c11 \
    -ffunction-sections \
    -fdata-sections \
    -g3 \
    -Wall \
    -Wextra

LFLAGS = \
    -nostartfiles \
    -Wl,-T,../gcc/mspm0g3519.lds \
    "-Wl,-Map,$(NAME).map" \
    -Wl,--gc-sections \
    -mcpu=cortex-m0plus \
    -march=armv6-m \
    -mthumb \
    -mfloat-abi=soft \
    "$(MSPM0_SDK_INSTALL_DIR)/source/ti/driverlib/lib/gcc/m0p/mspm0gx51x/driverlib.a" \
    --specs=nano.specs \
    -Wl,--start-group \
    -lc \
    -lm \
    -lgcc \
    -lnosys \
    -Wl,--end-group

.PHONY: all clean syscfg

all: $(NAME).out

.INTERMEDIATE: syscfg
$(SYSCFG_FILES): syscfg
	@ echo generation complete

syscfg: ../User/Config/empty_mspm0g3519.syscfg
	@ echo Generating configuration files...
	@ $(SYSCFG_CMD_STUB) --output $(@D) $<

define C_RULE
$(basename $(notdir $(1))).obj: $(1) $(SYSCFG_H_FILES)
	@ echo Building $$@
	@ $(CC) $(CFLAGS) -c $$< -o $$@
endef

$(foreach c_file,$(ALL_C_FILES),$(eval $(call C_RULE,$(c_file))))

$(NAME).out: $(OBJECTS)
	@ echo Linking $@
	@ $(LNK) $(OBJECTS) $(LFLAGS) -o $@

clean:
	@ echo Cleaning...
	-@ $(RM) $(OBJECTS) > $(DEVNULL) 2>&1
	-@ $(RM) $(NAME).out > $(DEVNULL) 2>&1
	-@ $(RM) $(NAME).map > $(DEVNULL) 2>&1
	-@ $(RM) $(SYSCFG_FILES) > $(DEVNULL) 2>&1
'@

Write-ProjectText 'iar/makefile' @'
MSPM0_SDK_INSTALL_DIR ?= D:/TI/CCS/mspm0_sdk_2_11_00_07
BUILD_TESTS ?= 0

include $(MSPM0_SDK_INSTALL_DIR)/imports.mak

CC = "$(IAR_ARMCOMPILER)/bin/iccarm"
LNK = "$(IAR_ARMCOMPILER)/bin/ilinkarm"

SYSCFG_CMD_STUB = $(SYSCONFIG_TOOL) --compiler iar --product $(MSPM0_SDK_INSTALL_DIR)/.metadata/product.json
SYSCFG_FILES := $(shell $(SYSCFG_CMD_STUB) --listGeneratedFiles --listReferencedFiles --output . ../User/Config/empty_mspm0g3519.syscfg)

SYSCFG_C_FILES = $(filter %.c,$(SYSCFG_FILES))
SYSCFG_H_FILES = $(filter %.h,$(SYSCFG_FILES))

TEST_C_FILES =
ifeq ($(BUILD_TESTS),1)
TEST_C_FILES := $(wildcard ../User/Test/*.c)
endif

USER_C_FILES = \
    ../User/Core/main.c \
    $(wildcard ../User/Common/*.c) \
    $(wildcard ../User/BSP/*.c) \
    $(wildcard ../User/Algorithm/*.c) \
    $(wildcard ../User/Driver/*.c) \
    $(wildcard ../User/APP/*.c) \
    $(wildcard ../User/Route/*.c) \
    $(TEST_C_FILES) \
    $(wildcard ../User/VL53L1_core/*.c) \
    $(wildcard ../User/VL53L1_platform/*.c)

STARTUP_C_FILES = ../iar/startup_mspm0g3519_iar.c
ALL_C_FILES = $(USER_C_FILES) $(SYSCFG_C_FILES) $(STARTUP_C_FILES)
OBJECTS = $(patsubst %.c,%.obj,$(notdir $(ALL_C_FILES)))
NAME = TraceTrackCar_TI

CFLAGS = \
    -I. \
    -I../User/Core \
    -I../User/Config \
    -I../User/Common \
    -I../User/BSP \
    -I../User/Driver \
    -I../User/Algorithm \
    -I../User/APP \
    -I../User/Route \
    -I../User/Test \
    -I../User/VL53L1_core \
    -I../User/VL53L1_platform \
    -D__MSPM0G3519__ \
    -Om \
    "-I$(MSPM0_SDK_INSTALL_DIR)/source/third_party/CMSIS/Core/Include" \
    "-I$(MSPM0_SDK_INSTALL_DIR)/source" \
    --debug \
    --silent \
    -e \
    --aeabi \
    --thumb \
    --diag_suppress=Pa050 \
    --cpu=Cortex-M0+ \
    --vla

LFLAGS = \
    "$(MSPM0_SDK_INSTALL_DIR)/source/ti/driverlib/lib/iar/m0p/mspm0gx51x/driverlib.a" \
    --config ../iar/mspm0g3519.icf \
    --map "$(NAME).map" \
    --silent \
    --semihosting=iar_breakpoint \
    --cpu=Cortex-M0+

.PHONY: all clean syscfg

all: $(NAME).out

.INTERMEDIATE: syscfg
$(SYSCFG_FILES): syscfg
	@ echo generation complete

syscfg: ../User/Config/empty_mspm0g3519.syscfg
	@ echo Generating configuration files...
	@ $(SYSCFG_CMD_STUB) --output $(@D) $<

define C_RULE
$(basename $(notdir $(1))).obj: $(1) $(SYSCFG_H_FILES)
	@ echo Building $$@
	@ $(CC) $(CFLAGS) $$< -o $$@
endef

$(foreach c_file,$(ALL_C_FILES),$(eval $(call C_RULE,$(c_file))))

$(NAME).out: $(OBJECTS)
	@ echo Linking $@
	@ $(LNK) $(OBJECTS) $(LFLAGS) -o $@

clean:
	@ echo Cleaning...
	-@ $(RM) $(OBJECTS) > $(DEVNULL) 2>&1
	-@ $(RM) $(NAME).out > $(DEVNULL) 2>&1
	-@ $(RM) $(NAME).map > $(DEVNULL) 2>&1
	-@ $(RM) $(SYSCFG_FILES) > $(DEVNULL) 2>&1
'@

# -----------------------------------------------------------------------------
# 4. 注释、旧平台说明和乱码清理
# -----------------------------------------------------------------------------
Write-ProjectText 'User/Driver/driver_all.c' @'
#include "driver_all.h"
#include "drv_motor.h"
#include "drv_encoder.h"
#include "drv_gray_sensor.h"
#include "drv_vl53l1x.h"
#include "drv_icm20948.h"
#include "drv_hx711.h"
#include "drv_lcd_tft.h"
#include "drv_oled_i2c.h"
#include "drv_e220.h"
#include "drv_servo.h"
#include "drv_laser.h"
#include "drv_status_light.h"
#include "drv_buzzer.h"

void Driver_Init(void)
{
    /*
     * E220 使用逻辑端口 UART_PORT_E220，对应 MSPM0G3519 硬件 UART4。
     * 普通直连或 E220 模式由 BSP/vehicle_config.h 选择。
     */
    Drv_E220_Init();

    /* 电机 PWM 与方向 GPIO 组合层；底层 PWM/GPIO 已由 BSP_InitAll() 初始化。 */
    Motor_Init();

    /* 四轮编码器映射层；硬件 QEI 和软件 QEI 已由 BSP_InitAll() 初始化。 */
    Drv_Encoder_Init();

    /* 灰度器件层根据 drv_gray_sensor.h 选择 4051 或 MCU-I2C 实现。 */
    Drv_GraySensor_Init();

    /* VL53L1X 仅初始化器件状态机，I2C 配置由周期任务分步推进。 */
    Drv_VL53L1X_Init();

    /* 外接 ICM20948 仅初始化器件状态机，SPI 配置由周期任务分步推进。 */
    Drv_ICM20948_Init();

    /* HX711 初始化缓存和 PD_SCK，采样由 Sensor_Update() 非阻塞推进。 */
    Drv_HX711_Init();

    Drv_LcdTft_Init();
    Drv_OledI2c_Init();

    /* 舵机、激光、状态灯和蜂鸣器统一在 Driver 层初始化。 */
    Drv_Laser_Init();
    Drv_Servo_Init();
    Drv_StatusLight_Init();
    Drv_Buzzer_Init();
}

void Driver_Task(void)
{
    Drv_E220_Task();
    Drv_LcdTft_Task();
    Drv_OledI2c_Task();
    Drv_Servo_Task();
    Drv_Laser_Task();
}
'@

$SensorManager = Read-ProjectText 'User/APP/sensor_manager.c'
$SensorManager = Replace-BlockCommentBefore `
    $SensorManager `
    '(void)Drv_ICM20948_Update();' `
@'
    /*
     * 外接 ICM20948 使用 SPI1；TFT 使用独立的 SPI0。
     * VL53L1X 与 MCU-I2C 灰度模块共享 I2C0，HX711 使用 GPIO 时序。
     * 所有器件均由状态机推进，上层 APP/Route 只读取缓存，
     * 不得再次直接调用各驱动的 Update()。
     */
'@
if ($SensorManager.Contains('ICM-20948 使用 SPI2') -or
    $SensorManager.Contains('共用 I2C1') -or
    $SensorManager.Contains('与 LCD 共用 SPI2')) {
    throw 'sensor_manager.c 的旧 STM32 外设注释未完全清除。'
}
Write-ProjectText 'User/APP/sensor_manager.c' $SensorManager

Write-ProjectText 'User/Algorithm/attitude_estimator.h' @'
#ifndef __ATTITUDE_ESTIMATOR_H
#define __ATTITUDE_ESTIMATOR_H

#include "project_status.h"
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
 * ICM-20948 姿态融合层。
 *
 * 本模块位于 Algorithm 层：
 *   - Driver 只负责输出可靠的加速度、角速度和磁场数据；
 *   - 本模块负责 Mahony 反馈、磁场异常门控和在线零偏；
 *   - APP/Route 只读取最终 Roll、Pitch 和 Yaw。
 */

/* ============================== 融合参数 ================================== */

/* 加速度计对 Roll/Pitch 的 Mahony 比例反馈，数值越大收敛越快。 */
#define ATTITUDE_MAHONY_ACCEL_KP                 2.0f

/*
 * Yaw 数据源策略：
 *   - Roll/Pitch 继续使用陀螺仪与加速度计 Mahony 修正；
 *   - Yaw 使用校准后的 Z 轴角速度积分，并由磁力计缓慢修正；
 *   - 编码器航向角和角速度不参与姿态估计。
 */
#define ATTITUDE_SEPARATE_YAW_ENABLE              1U
#define ATTITUDE_ENCODER_YAW_CORRECTION_ENABLE    0U
#define ATTITUDE_MAG_YAW_CORRECTION_ENABLE        1U
#define ATTITUDE_MAG_DISABLE_WHEN_MOTOR_ACTIVE    1U

/* 磁力计只用于绕重力方向的低增益 Yaw 修正。 */
#define ATTITUDE_MAHONY_MAG_KP                   0.05f

/* 合法积分周期；超出范围时使用标称周期，避免停顿后一次积分过大。 */
#define ATTITUDE_NOMINAL_DT_S                     0.00978f
#define ATTITUDE_MIN_DT_S                         0.002f
#define ATTITUDE_MAX_DT_S                         0.050f

/* 只有加速度模长接近 1 g 时，才允许它修正 Roll/Pitch。 */
#define ATTITUDE_ACCEL_CORRECTION_MIN_G           0.80f
#define ATTITUDE_ACCEL_CORRECTION_MAX_G           1.20f

/* 静止检测与运行中残余零偏更新。 */
#define ATTITUDE_STATIONARY_ACCEL_MIN_G           0.97f
#define ATTITUDE_STATIONARY_ACCEL_MAX_G           1.03f
#define ATTITUDE_STATIONARY_GYRO_MAX_DPS          0.35f
#define ATTITUDE_STATIONARY_SAMPLE_COUNT        100U
#define ATTITUDE_ONLINE_BIAS_ALPHA                0.005f
#define ATTITUDE_ONLINE_BIAS_MAX_DPS              5.0f

/* ============================== 磁场校准 ================================== */

/*
 * 以下默认值来自本车此前 M/N 测试输出。
 * 更换 IMU、安装方向或车体结构后必须重新标定。
 */
#define ATTITUDE_MAG_CAL_DEFAULT_VALID             1U
#define ATTITUDE_MAG_CAL_OFFSET_X_UT             -43.05f
#define ATTITUDE_MAG_CAL_OFFSET_Y_UT              30.75f
#define ATTITUDE_MAG_CAL_OFFSET_Z_UT              -4.35f
#define ATTITUDE_MAG_CAL_SCALE_X                   1.004f
#define ATTITUDE_MAG_CAL_SCALE_Y                   0.973f
#define ATTITUDE_MAG_CAL_SCALE_Z                   1.022f

/* min/max 标定至少覆盖足够样本，并保证每个轴具有足够旋转范围。 */
#define ATTITUDE_MAG_CAL_MIN_SAMPLES             300U
#define ATTITUDE_MAG_CAL_MIN_SPAN_UT              20.0f

/* 磁场异常门控：绝对强度、相对基准变化和方向突变。 */
#define ATTITUDE_MAG_FIELD_MIN_UT                 15.0f
#define ATTITUDE_MAG_FIELD_MAX_UT                100.0f
#define ATTITUDE_MAG_FIELD_REL_TOLERANCE           0.15f
#define ATTITUDE_MAG_REFERENCE_ALPHA               0.002f
#define ATTITUDE_MAG_ACQUIRE_SAMPLES              10U
#define ATTITUDE_MAG_DIRECTION_MIN_DOT             0.85f
#define ATTITUDE_MAG_STALE_TIMEOUT_MS             250U

typedef struct {
    float offset_uT[3];
    float scale[3];
    uint8_t valid;
} Attitude_MagCalibration_t;

typedef struct {
    float x;
    float y;
    float z;
} Attitude_Vector3f_t;

/*
 * 姿态算法使用纯数据输入，不暴露具体 IMU 驱动类型。
 * APP 适配层负责把传感器驱动缓存转换为该结构。
 */
typedef struct {
    Attitude_Vector3f_t accel_filtered_g;
    Attitude_Vector3f_t gyro_filtered_dps;
    Attitude_Vector3f_t mag_uT;
    Attitude_Vector3f_t mag_filtered_uT;
    uint32_t timestamp_ms;
    uint8_t mag_valid;
    uint8_t mag_updated;
} Attitude_Input_t;

typedef struct {
    float q[4];                       /* w、x、y、z */
    float roll_deg;
    float pitch_deg;
    float yaw_deg;

    /* 驱动上电零偏之后，本融合层继续学习到的残余零偏。 */
    float online_gyro_bias_dps[3];

    /*
     * 编码器字段为兼容既有诊断接口而保留。
     * 当前配置下 encoder_used 始终为 0，不参与姿态融合。
     */
    float encoder_yaw_deg;
    float encoder_yaw_rate_dps;
    float mag_norm_uT;
    float mag_reference_uT;

    uint32_t timestamp_ms;
    uint32_t update_count;
    uint32_t mag_accept_count;
    uint32_t mag_reject_count;
    uint32_t mag_calibration_samples;

    uint8_t initialized;
    uint8_t valid;
    uint8_t stationary;
    uint8_t encoder_used;
    uint8_t encoder_heading_valid;
    uint8_t mag_available;
    uint8_t mag_healthy;
    uint8_t mag_used;
    uint8_t mag_calibrating;
    uint8_t mag_calibrated;
} Attitude_Info_t;

void Attitude_Init(void);
void Attitude_Reset(void);

/* 每次出现新输入时间戳时融合一次；重复时间戳返回 PROJECT_BUSY。 */
Project_Status_t Attitude_Update(const Attitude_Input_t *input,
                                 uint8_t motor_active);

/* 输入源离线或数据失效时由适配层调用，防止上层继续使用陈旧姿态。 */
void Attitude_Invalidate(void);
Project_Status_t Attitude_GetInfo(Attitude_Info_t *info);

float Attitude_GetRollDeg(void);
float Attitude_GetPitchDeg(void);
float Attitude_GetYawDeg(void);
uint8_t Attitude_IsValid(void);

/* 只改变对外输出的 Yaw 零点，不重置四元数、Roll/Pitch 或校准状态。 */
void Attitude_ZeroYaw(void);

/* 运行时磁力计标定：在 Start 与 Finish 之间缓慢转遍所有方向。 */
void Attitude_MagCalibrationStart(void);
Project_Status_t Attitude_MagCalibrationFinish(Attitude_MagCalibration_t *result);
Project_Status_t Attitude_SetMagCalibration(const Attitude_MagCalibration_t *calibration);
Project_Status_t Attitude_GetMagCalibration(Attitude_MagCalibration_t *calibration);

#ifdef __cplusplus
}
#endif

#endif /* __ATTITUDE_ESTIMATOR_H */
'@

$TestFile = Read-ProjectText 'User/Test/test.c'

$TestFile = Replace-BlockCommentBefore `
    $TestFile `
    'void Test_K210_CommUpdate(void)' `
@'
/*
 * K210 与 MSPM0G3519 通信测试任务。
 *
 * 功能：
 *   1. 读取 K210 发送的完整多数字快照；
 *   2. 通过 DEBUG_UART_PORT 输出数字、置信度和横坐标；
 *   3. 每 500 ms 输出一次通信状态和错误统计。
 *
 * 建议任务周期：
 *   { Test_K210_CommUpdate, 10U, 0U },
 */
'@

$TestFile = Replace-LineCommentBefore `
    $TestFile `
    'void Test_GPIO_Toggle(void)' `
    '/* LED1 每 500 ms 翻转一次，用于确认调度器持续运行。 */'

$TestFile = [regex]::Replace(
    $TestFile,
    '(?m)^//[^\r\n]*\r?\n(?=/\*\r?\n \* 蜂鸣器按键测试)',
    ''
)

$TestFile = Replace-LineCommentBefore `
    $TestFile `
    'void Test_Encoder_Log(void)' `
    '/* 输出左右编码器增量、速度和累计计数。 */'

$TestFile = Replace-BlockCommentBefore `
    $TestFile `
    '#define GRAY_ADC_READ_RAW()' `
@'
/*
 * 74HC4051 八路模拟灰度模块最小测试。
 *
 * MSPM0G3519 接线：
 *   灰度 OUT/SIG/AO -> PA25 / ADC0 通道 2 / BSP_ADC_CH1
 *   灰度 S0         -> PA24 / BSP_GPIO_GRAY_S0
 *   灰度 S1         -> PA31 / BSP_GPIO_GRAY_S1
 *   灰度 S2         -> PC1  / BSP_GPIO_GRAY_S2
 *
 * 任务表建议：
 *   { AppTask_BSP_Background, 1U,   0U },
 *   { Test_Gray4051_Update,   1U,   0U },
 *   { Test_Gray4051_Log,      200U, 0U },
 */
'@

$TestFile = Replace-BlockCommentBefore `
    $TestFile `
    'static volatile uint32_t g_exti_count' `
@'
/*
 * 外部中断计数测试。
 * 中断回调只更新计数，日志由周期任务输出。
 */
'@

$ExtiCountRegex = [regex]::new('(?m)^\s*g_exti_count\+\+;.*$')
$TestFile = $ExtiCountRegex.Replace(
    $TestFile,
    '    g_exti_count++;   /* 中断回调中只计数，不执行串口输出。 */',
    1
)

$TestFile = Replace-LineCommentBefore `
    $TestFile `
    'void Test_UART_Echo(void)' `
    '/* 调试串口回显测试。 */'

$TestFile = Replace-LineCommentBefore `
    $TestFile `
    'void Test_I2C_Scan(void)' `
    '/* 扫描 I2C0 总线上的 7 位从设备地址。 */'

$TestFile = Replace-BlockCommentBefore `
    $TestFile `
    'void Test_MotorCmd_Update(void)' `
@'
/*
 * 电机开环命令测试。
 * 调试串口命令：
 *   w：四轮前进，输出 300‰；
 *   s：四轮后退，输出 300‰；
 *   a：原地左转；
 *   d：原地右转；
 *   0：立即停车。
 *
 * 首次测试必须架空车轮，确认电机映射和方向后再落地。
 */
'@

$TestFile = Replace-LineCommentBefore `
    $TestFile `
    'static void Test_CountPerRev_Print(void)' `
    '/* 输出四轮累计计数，用于标定每圈脉冲数。 */'

$TestFile = [regex]::Replace(
    $TestFile,
    '(?m)^//[^\r\n]*\r?\n(?=/\*\r?\n \* Part4)',
    ''
)

$TestFile = Replace-BlockCommentBefore `
    $TestFile `
    'static const char *LineTypeName(LineType_t type)' `
@'
/*
 * 灰度循迹串口联调命令：
 *   1：启动循迹；
 *   0 或 x：停止循迹并停车；
 *   w：把当前八路采样记录为白底；
 *   b：把当前八路采样记录为黑线；
 *   t：根据白底和黑线记录生成八路阈值；
 *   d：恢复默认统一阈值 LINE_DETECT_DEFAULT_THRESHOLD；
 *   p：立即打印 raw、threshold、mask、error、type 和输出。
 *
 * 推荐标定流程：
 *   1. 八路传感器全部对准白底，发送 w；
 *   2. 八路传感器全部压在黑线上，发送 b；
 *   3. 发送 t 生成阈值；
 *   4. 发送 p 检查 mask；
 *   5. 发送 1 开始循迹。
 */
'@

$TestFile = Replace-LiteralRequired `
    $TestFile `
    '    /* Read the 8 thresholds currently used by line_detect. */' `
    '    /* 读取 line_detect 当前实际使用的八路阈值。 */' `
    '灰度阈值英文注释'

$TestFile = Replace-BlockCommentBefore `
    $TestFile `
    'static const char *Test_VL53L1X_StateName(Drv_VL53L1X_State_t state)' `
@'
/*
 * VL53L1X 独立日志测试。
 * 测距状态机由 Sensor_Update() 推进，本函数只读取当前驱动快照，
 * 并通过 DEBUG_UART_PORT 输出状态。
 *
 * 建议任务周期：
 *   { Test_VL53L1X_Update, 200U, 0U },
 */
'@

$TestFile = Replace-LiteralRequired `
    $TestFile `
    '    /* Before attitude becomes valid, show why motion is still interlocked. */' `
    '    /* 姿态尚未有效时输出联锁原因，便于确认运动控制为何仍被禁止。 */' `
    '姿态联锁英文注释'

$TestFile = $TestFile.Replace(
    ' * 不使用 printf 浮点功能，兼容 Keil ARMCC 默认配置。',
    ' * 不使用 printf 浮点格式，避免扩大固件并兼容精简 C 库配置。'
)
$TestFile = $TestFile.Replace(
    '    /* USART1 只保留路线复位命令，路线状态改由 LCD 显示。 */',
    '    /* 调试串口只保留路线复位命令，路线状态改由 LCD 显示。 */'
)

$TestFile = $TestFile.Replace('"KEY1 PRESSED (PE4)\r\n"', '"KEY1 PRESSED\r\n"')
$TestFile = $TestFile.Replace('"KEY1 RELEASED (PE4)\r\n"', '"KEY1 RELEASED\r\n"')
$TestFile = $TestFile.Replace('"KEY2 PRESSED (PE3)\r\n"', '"KEY2 PRESSED\r\n"')
$TestFile = $TestFile.Replace('"KEY2 RELEASED (PE3)\r\n"', '"KEY2 RELEASED\r\n"')
$TestFile = $TestFile.Replace('"KEY3 PRESSED (PE2)\r\n"', '"KEY3 PRESSED\r\n"')
$TestFile = $TestFile.Replace('"KEY3 RELEASED (PE2)\r\n"', '"KEY3 RELEASED\r\n"')
$TestFile = $TestFile.Replace('"KEY4 PRESSED (PE1)\r\n"', '"KEY4 PRESSED\r\n"')
$TestFile = $TestFile.Replace('"KEY4 RELEASED (PE1)\r\n"', '"KEY4 RELEASED\r\n"')
$TestFile = $TestFile.Replace('"SPI1 DMA ASYNC"', '"SPI0 DMA ASYNC"')
$TestFile = $TestFile.Replace('然后通过USART1发送到串口助手。', '然后通过 DEBUG_UART_PORT 发送到串口助手。')

Write-ProjectText 'User/Test/test.c' $TestFile

# -----------------------------------------------------------------------------
# 5. 稳定文档与项目规则
# -----------------------------------------------------------------------------
Write-ProjectText 'Doc/README.md' @'
# TraceTrackCar_TI 文档索引

本目录根层文档对应 MSPM0G3519 工程。`Legacy_STM32` 仅保存原 STM32F407
项目资料，其中的引脚、外设实例和初始化方式不能直接用于当前工程。

## 当前工程文档

- [说明文档](说明文档.md)：工程结构、启动流程、外设架构和构建方法。
- [参数和引脚说明](参数和引脚说明.md)：MSPM0G3519 核心板接线、通信参数及 DMA 分配。
- [注意事项](注意事项.md)：供电、外部 ICM20948、异步传输和生成配置注意事项。
- [上板测试步骤](上板测试步骤.md)：按风险从低到高进行硬件联调。

## 迁移记录

迁移过程、软件验证记录和仍需实板确认的项目统一放在 `Migration`，不写入稳定功能文档：

- [迁移总览](../Migration/README.md)
- [最终差异与上板清单](../Migration/FINAL_GAPS_AND_BRINGUP.md)
- [引脚复用记录](../Migration/PINMUX_DRAFT.md)
- [封装与构建清理记录](../Migration/ENCAPSULATION_CLEANUP.md)
'@

$PinDoc = Read-ProjectText 'Doc/参数和引脚说明.md'
if (-not $PinDoc.Contains('| 调试串口 / CH340 | UART0 | PA10 TX，PA11 RX |')) {
    $PinDoc = Replace-LiteralRequired `
        $PinDoc `
        '| K210 | UART1 | PB4 TX，PB5 RX |' `
        "| 调试串口 / CH340 | UART0 | PA10 TX，PA11 RX |`r`n| K210 | UART1 | PB4 TX，PB5 RX |" `
        '参数文档调试串口行'
}
Write-ProjectText 'Doc/参数和引脚说明.md' $PinDoc

Write-ProjectText 'Doc/说明文档.md' @'
# TraceTrackCar_TI 说明文档

## 1. 工程定位

本工程将原 STM32F407 循迹小车项目迁移到 MSPM0G3519。目标硬件为启是科技
MSPM0G3519 V2.0 核心板，不依赖 EVM 扩展板；惯性传感器使用外接 ICM20948，
不使用板载 IMU。

本文只说明稳定的软件结构、接口和构建方法。迁移过程、编译验证记录和待确认项
统一记录在 `Migration` 目录。

## 2. 目录结构

```text
TraceTrackCar_TI
├─ User
│  ├─ Core          主函数
│  ├─ Config        SysConfig 工程及生成的 TI 外设配置
│  ├─ Common        平台无关状态码和可移植抽象接口
│  ├─ BSP           GPIO、PWM、编码器、UART、I2C、SPI、ADC 等板级抽象
│  ├─ Driver        电机、编码器、ICM20948、显示、灰度、舵机等器件驱动
│  ├─ Algorithm     姿态、控制、循迹和里程计算法
│  ├─ APP           业务编排、控制仲裁和上下层适配
│  ├─ Route         赛道状态和控制意图
│  ├─ Test          嵌入式专项测试
│  ├─ VL53L1_core   VL53L1X 官方核心驱动
│  └─ VL53L1_platform
│                   VL53L1X 平台适配层
├─ K210             K210 侧脚本和资源
├─ Doc              稳定功能文档
│  └─ Legacy_STM32  原 STM32 平台参考文档
├─ Migration        迁移记录、检查脚本和差异清单
├─ keil             Keil ARM Compiler 6 工程
├─ ticlang          TI Arm Clang Makefile
├─ gcc              GNU Arm Embedded Makefile
└─ iar              IAR Arm Makefile
```

测试代码只能放在 `User/Test`。正式固件默认不编译测试源；使用 Makefile 构建
专项测试固件时传入 `BUILD_TESTS=1`。

## 3. 启动流程

`User/Core/main.c` 只执行以下分层启动链：

1. `BSP_InitAll()`；
2. `Driver_Init()`；
3. `App_Init()`；
4. `Scheduler_Init()`；
5. 主循环持续执行 `Scheduler_Run()`。

`BSP_InitAll()` 内部首先调用 `SYSCFG_DL_init()`，随后初始化 SysTick、GPIO、
中断、PWM、编码器、ADC、按键、UART、I2C 和 SPI。Core 层不直接包含或调用
TI SysConfig 生成接口。

SysTick 提供 1 ms 软件时基。硬件中断只完成必要的数据搬运和状态记录，较重处理
及用户回调在任务上下文中执行。

## 4. 外设实现

### 4.1 外部 ICM20948

ICM20948 使用硬件 SPI1，片选和中断脚由 GPIO 控制。PB21 为 ICM20948 外部
中断输入，并与后轮编码器 GPIO 中断共用 GPIOB 中断入口；中断服务程序根据挂起位
分别派发。

ICM20948 使用短帧阻塞式 SPI 访问，不占用显示和 I2C 使用的四个 DMA 通道。

### 4.2 显示和 I2C

- 显示接口使用硬件 SPI0，逻辑总线名为 `SPI_BUS1`。
- SPI0 发送使用 DMA 通道 0，接收使用 DMA 通道 1。
- I2C0 发送使用 DMA 通道 2，接收使用 DMA 通道 3。
- 异步完成回调由 BSP 周期任务派发，不在硬件中断中执行。

### 4.3 编码器

前轮编码器使用定时器 QEI。后轮编码器使用 GPIO 双相采样和软件累计，以适配
MSPM0G3519 的定时器资源。四轮正负方向和每圈计数必须按上板测试步骤实测确认。

### 4.4 按键与调试串口

原 STM32 工程中的 KEY5 映射到核心板 USER 按键 PB31。调试串口使用硬件 UART0，
PA10 为 TX、PA11 为 RX，对应核心板 CH340 Type-C 串口。

## 5. 构建方式

### 5.1 Keil ARM Compiler 6

打开：

```text
keil/TraceTrackCar_TI.uvprojx
```

SysConfig 修改后先重新生成 `User/Config/ti_msp_dl_config.c/.h`，再执行全量编译。

### 5.2 TI Arm Clang

在 `ticlang` 目录执行：

```text
gmake
```

专项测试固件执行：

```text
gmake BUILD_TESTS=1
```

### 5.3 GNU Arm Embedded

在 `gcc` 目录执行：

```text
gmake
```

专项测试固件追加 `BUILD_TESTS=1`。工具链路径由 MSPM0 SDK 的 `imports.mak`
或命令行变量提供。

### 5.4 IAR Arm

在 `iar` 目录执行：

```text
gmake
```

专项测试固件追加 `BUILD_TESTS=1`。IAR 安装路径由 MSPM0 SDK 的 `imports.mak`
或命令行变量提供。

## 6. 硬件确认原则

编译无法确认电机极性、编码器方向、传感器安装方向、供电能力和最终控制参数。
这些内容必须标记为“待确认”，并按[上板测试步骤](上板测试步骤.md)逐项验证。
软件验证记录和迁移缺口以 `Migration` 目录为准。
'@

Write-ProjectText 'Migration/ENCAPSULATION_CLEANUP.md' @'
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
'@

Write-ProjectText 'AGENTS.md' @'
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
'@

# -----------------------------------------------------------------------------
# 6. 静态检查
# -----------------------------------------------------------------------------
$MainText = Read-ProjectText 'User/Core/main.c'
if ($MainText.Contains('SYSCFG_DL_init') -or $MainText.Contains('ti_msp_dl_config.h')) {
    throw 'main.c 仍直接依赖 TI 平台初始化。'
}

$BspAllText = Read-ProjectText 'User/BSP/bsp_all.c'
if (-not $BspAllText.Contains('SYSCFG_DL_init();')) {
    throw 'BSP_InitAll() 未包含 SYSCFG_DL_init()。'
}

$ProjectXml = Read-ProjectText $ProjectFile
if ($ProjectXml.Contains('test_k210_road_comm')) {
    throw 'Keil 工程仍引用 test_k210_road_comm。'
}

foreach ($Makefile in @('ticlang/makefile', 'gcc/makefile', 'iar/makefile')) {
    $MakeText = Read-ProjectText $Makefile
    foreach ($Required in @(
        '$(wildcard ../User/Common/*.c)',
        '$(wildcard ../User/BSP/*.c)',
        '$(wildcard ../User/Driver/*.c)',
        '$(wildcard ../User/Algorithm/*.c)',
        '$(wildcard ../User/APP/*.c)',
        '$(wildcard ../User/Route/*.c)',
        '$(wildcard ../User/VL53L1_core/*.c)',
        '$(wildcard ../User/VL53L1_platform/*.c)',
        'BUILD_TESTS'
    )) {
        if (-not $MakeText.Contains($Required)) {
            throw "$Makefile 缺少源文件规则：$Required"
        }
    }
}

$TextCheckFiles = @(
    'User/Test/test.c',
    'User/Test/test.h',
    'User/Test/test_k210_comm.c',
    'User/APP/sensor_manager.c',
    'User/Driver/driver_all.c',
    'User/Algorithm/attitude_estimator.h',
    'Doc/README.md',
    'Doc/说明文档.md',
    'Doc/参数和引脚说明.md',
    'Migration/ENCAPSULATION_CLEANUP.md',
    'AGENTS.md'
)

$MojibakePattern = '娴嬭瘯|锛|澶氳矾|涓插彛|鐢垫満|鐏板害|浠诲姟|纭欢|€|�'
$MojibakeHits = @()
foreach ($Relative in $TextCheckFiles) {
    $Text = Read-ProjectText $Relative
    if ([regex]::IsMatch($Text, $MojibakePattern)) {
        $MojibakeHits += $Relative
    }
}
if ($MojibakeHits.Count -ne 0) {
    throw ('以下文件仍疑似包含乱码：' + ($MojibakeHits -join ', '))
}

$OldPlatformPattern = 'STM32 PA|ADC1_IN10|PD10|PD11|PD12|PE1|PE2|PE3|PE4|ICM-20948 使用 SPI2|共用 I2C1'
$OldPlatformHits = @()
foreach ($Relative in @('User/Test/test.c', 'User/Test/test_k210_comm.c', 'User/APP/sensor_manager.c')) {
    $Text = Read-ProjectText $Relative
    if ([regex]::IsMatch($Text, $OldPlatformPattern)) {
        $OldPlatformHits += $Relative
    }
}
if ($OldPlatformHits.Count -ne 0) {
    throw ('以下文件仍包含旧平台引脚或外设说明：' + ($OldPlatformHits -join ', '))
}

$DuplicateReferences = Get-ChildItem -LiteralPath (Get-ProjectPath 'User') -Recurse -File |
    Where-Object { $_.Extension -in @('.c', '.h') } |
    Select-String -SimpleMatch 'test_k210_road_comm'
if ($DuplicateReferences) {
    throw '源码中仍存在 test_k210_road_comm 引用。'
}

if (Get-Command git -ErrorAction SilentlyContinue) {
    Push-Location $Root
    try {
        & git diff --check
        if ($LASTEXITCODE -ne 0) {
            throw 'git diff --check 发现空白或补丁格式问题。'
        }
    } finally {
        Pop-Location
    }
}

Write-Host ""
Write-Host "修复完成。"
Write-Host "当前测试任务配置未被修改。"
Write-Host "原文件备份位于：$BackupRoot"
Write-Host ""
Write-Host "Makefile 构建规则："
Write-Host "  正式固件：gmake"
Write-Host "  专项测试：gmake BUILD_TESTS=1"
Write-Host ""
Write-Host "请在 GitHub Desktop 中检查改动后再提交。"
