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