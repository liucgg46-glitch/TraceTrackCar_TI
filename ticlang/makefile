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