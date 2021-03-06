# Change this to wherever you put BreezySTM32
BREEZY_DIR = $(HOME)/Desktop/BreezySTM32

# Change this to where you put your Arduino libraries
ARDUINO_DIR = $(HOME)/Development/arduino-1.8.1/hardware/teensy/avr/libraries

# Change this to whatever project name you want
PROJECT = Hello

###############################################################################

F3_DIR = $(BREEZY_DIR)/f3
ALIENFLIGHTF3_DIR = $(F3_DIR)/alienflightf3

# Debugger optons, must be empty or GDB
DEBUG ?=

# Serial port/Device for flashing
SERIAL_DEVICE	?= /dev/ttyACM0

# Flash size (KB).  Some low-end chips actually have more flash than advertised, use this to override.
FLASH_SIZE ?= 256

# Working directories
ROOT		 = $(dir $(lastword $(MAKEFILE_LIST)))
SRC_DIR		 = $(ALIENFLIGHTF3_DIR)
OBJECT_DIR	 = $(ROOT)/obj
BIN_DIR		 = $(ROOT)/obj
#CMSIS_DIR	 = $(ROOT)/lib/CMSIS
CMSIS_DIR	 = $(BREEZY_DIR)/lib/CMSIS
INCLUDE_DIRS = $(BREEZY_DIR) $(F3_DIR) $(ALIENFLIGHTF3_DIR) $(SRC_DIR) $(SRC_DIR)/target
LINKER_DIR	 = $(F3_DIR)

# Search path for sources
VPATH		:= $(SRC_DIR):$(SRC_DIR)/startup
USBFS_DIR	= $(BREEZY_DIR)/lib/STM32_USB-FS-Device_Driver
USBPERIPH_SRC = $(notdir $(wildcard $(USBFS_DIR)/src/*.c))

CSOURCES        := $(shell find $(SRC_DIR) -name '*.c')

# F3 TARGETS

STDPERIPH_DIR	= $(BREEZY_DIR)/lib/STM32F30x_StdPeriph_Driver

STDPERIPH_SRC = $(notdir $(wildcard $(STDPERIPH_DIR)/src/*.c))

EXCLUDES	= stm32f30x_crc.c stm32f30x_can.c

STDPERIPH_SRC := $(filter-out ${EXCLUDES}, $(STDPERIPH_SRC))

DEVICE_STDPERIPH_SRC = $(STDPERIPH_SRC)


VPATH		:= $(VPATH):$(CMSIS_DIR)/CM1/CoreSupport:$(CMSIS_DIR)/CM1/DeviceSupport/ST/STM32F30x
CMSIS_SRC	 = $(notdir $(wildcard $(CMSIS_DIR)/CM1/CoreSupport/*.c \
			   $(CMSIS_DIR)/CM1/DeviceSupport/ST/STM32F30x/*.c))

INCLUDE_DIRS := $(INCLUDE_DIRS) \
		   $(STDPERIPH_DIR)/inc \
		   $(CMSIS_DIR)/CM1/CoreSupport \
		   $(CMSIS_DIR)/CM1/DeviceSupport/ST/STM32F30x

# VCP support

VCP_DIR = $(F3_DIR)/vcp

INCLUDE_DIRS := $(INCLUDE_DIRS) \
		$(USBFS_DIR)/inc \
		$(VCP_DIR) 

VPATH := $(VPATH):$(USBFS_DIR)/src

DEVICE_STDPERIPH_SRC := $(DEVICE_STDPERIPH_SRC)\
		   $(USBPERIPH_SRC)

LD_SCRIPT	 = $(LINKER_DIR)/stm32_flash_f303_$(FLASH_SIZE)k.ld

ARCH_FLAGS	 = -mthumb -mcpu=cortex-m4 -mfloat-abi=hard -mfpu=fpv4-sp-d16 -fsingle-precision-constant -Wdouble-promotion
DEVICE_FLAGS = -DSTM32F303xC -DSTM32F303

ifneq ($(FLASH_SIZE),)
DEVICE_FLAGS := $(DEVICE_FLAGS) -DFLASH_SIZE=$(FLASH_SIZE)
endif

SYSTEM_SRC = \
		   $(F3_DIR)/dma.c \
		   $(F3_DIR)/serial.c \
		   $(F3_DIR)/system.c \
		   $(TARGET_SRC) \
		   $(CMSIS_SRC) \
		   $(DEVICE_STDPERIPH_SRC)

FC_COMMON_SRC = \
   	       $(F3_DIR)/exti.c \
		   $(F3_DIR)/io.c \

VCP_SRC = \
		   $(VCP_DIR)/hw_config.c \
		   $(VCP_DIR)/stm32_it.c \
		   $(VCP_DIR)/usb_desc.c \
		   $(VCP_DIR)/usb_endp.c \
		   $(VCP_DIR)/usb_istr.c \
		   $(VCP_DIR)/usb_prop.c \
		   $(VCP_DIR)/usb_pwr.c \
		   $(F3_DIR)/serial_usb_vcp.c \
		   $(F3_DIR)/usb_io.c

STM32F30x_COMMON_SRC = \
		   $(F3_DIR)/startup_stm32f30x_md_gcc.S \
		   $(F3_DIR)/system_stm32f30x_extras.c \
		   $(F3_DIR)/gpio_stm32f30x.c \
		   $(F3_DIR)/serial_uart.c \
		   $(F3_DIR)/serial_uart_stm32f30x.c \
		   $(F3_DIR)/system_stm32f30x.c

$(PROJECT)_CSRC = \
		   timer.c \
		   $(F3_DIR)/timer_stm32f30x.c \
		   $(STM32F30x_COMMON_SRC) \
		   $(FC_COMMON_SRC) \
		   $(SYSTEM_SRC) \
		   $(VCP_SRC) \
		   target.c

VPATH		:= $(VPATH):$(STDPERIPH_DIR)/src

###############################################################################
# Things that might need changing to use different tools
#

# Find out if ccache is installed on the system
CCACHE := ccache
RESULT = $(shell (which $(CCACHE) > /dev/null 2>&1; echo $$?) )
ifneq ($(RESULT),0)
CCACHE :=
endif

# Tool names
CC          := $(CCACHE) arm-none-eabi-gcc
OBJCOPY     := arm-none-eabi-objcopy
SIZE        := arm-none-eabi-size

#
# Tool options.
#

ifeq ($(DEBUG),GDB)
OPTIMIZE	 = -O0
LTO_FLAGS	 = $(OPTIMIZE)
else
OPTIMIZE	 = -Os
LTO_FLAGS	 =  -flto -fuse-linker-plugin $(OPTIMIZE)
endif

DEBUG_FLAGS	 = -ggdb3 -DDEBUG

CFLAGS		 = $(ARCH_FLAGS) \
		   $(LTO_FLAGS) \
		   $(WARN_FLAGS) \
		   $(addprefix -I,$(INCLUDE_DIRS)) \
		   $(DEBUG_FLAGS) \
		   -Wall -Wpedantic -Wextra -Wunsafe-loop-optimizations -Wdouble-promotion -Wundef \
		   -ffunction-sections \
		   -fdata-sections \
		   $(DEVICE_FLAGS) \
		   -DUSE_STDPERIPH_DRIVER \
		   -fverbose-asm -ffat-lto-objects \
		   -save-temps=obj \
		   -MMD -MP

ASFLAGS		 = $(ARCH_FLAGS) \
		   $(WARN_FLAGS) \
		   -x assembler-with-cpp \
		   $(addprefix -I,$(INCLUDE_DIRS)) \
		  -MMD -MP

LDFLAGS		 = -lm \
		   -nostartfiles \
		   --specs=nano.specs \
		   -lc \
		   -lnosys \
		   $(ARCH_FLAGS) \
		   $(LTO_FLAGS) \
		   $(WARN_FLAGS) \
		   $(DEBUG_FLAGS) \
		   -static \
		   -Wl,-gc-sections,-Map,$(TARGET_MAP) \
		   -Wl,-L$(LINKER_DIR) \
		   -Wl,--cref \
		   -T$(LD_SCRIPT)

# Things we will build

TARGET_BIN	 = obj/$(PROJECT).bin
TARGET_HEX	 = $(BIN_DIR)/$(PROJECT).hex
TARGET_ELF	 = $(OBJECT_DIR)/$(PROJECT).elf
TARGET_OBJS	 = $(addsuffix .o,$(addprefix $(OBJECT_DIR)/$(PROJECT)/,$(basename $($(PROJECT)_CSRC)))) \
			   $(OBJECT_DIR)/$(PROJECT)/Arduino.o \
			   $(OBJECT_DIR)/$(PROJECT).o
TARGET_MAP	 = $(OBJECT_DIR)/$(PROJECT).map


## Default make goal:
## hex         : Make filetype hex only
.DEFAULT_GOAL := bin

## Optional make goals:
## all         : Make all filetypes, binary and hex
all: hex bin

## binary      : Make binary filetype
## bin         : Alias of 'binary'
## hex         : Make hex filetype
bin:    $(TARGET_BIN)
binary: $(TARGET_BIN)
hex:    $(TARGET_HEX)

# List of buildable ELF files and their object dependencies.

$(TARGET_HEX): $(TARGET_ELF)
	$(OBJCOPY) -O ihex --set-start 0x8000000 $< $@

$(TARGET_BIN): $(TARGET_ELF)
	$(OBJCOPY) -O binary $< $@

$(TARGET_ELF):  $(TARGET_OBJS)
	$(CC) -o $@ $^ $(LDFLAGS)

# Compile
$(OBJECT_DIR)/$(PROJECT)/%.o: %.c
	@mkdir -p $(dir $@)
	@echo %% $(notdir $<)
	@$(CC) -c -o $@ $(CFLAGS) $<

# Assemble
$(OBJECT_DIR)/$(PROJECT)/%.o: %.s
	@mkdir -p $(dir $@)
	@echo %% $(notdir $<)
	@$(CC) -c -o $@ $(ASFLAGS) $<

$(OBJECT_DIR)/$(PROJECT)/%.o: %.S
	@mkdir -p $(dir $@)
	@echo %% $(notdir $<)
	@$(CC) -c -o $@ $(ASFLAGS) $<

$(OBJECT_DIR)/$(PROJECT)/Arduino.o: $(F3_DIR)/Arduino.cpp $(BREEZY_DIR)/Arduino.h
	@$(CC) -c -o $(OBJECT_DIR)/$(PROJECT)/Arduino.o $(CFLAGS) $(F3_DIR)/Arduino.cpp

$(OBJECT_DIR)/$(PROJECT).o: ./$(PROJECT).cpp
	@$(CC) -c -o $(OBJECT_DIR)/$(PROJECT).o $(CFLAGS) ./$(PROJECT).cpp

clean:
	rm -rf obj

flash_$(PROJECT): $(TARGET_BIN)
	echo -n 'R' >$(SERIAL_DEVICE)
	sleep 2
	dfu-util -a 0 -s 0x08000000:leave -D $(TARGET_BIN)

## flash       : flash firmware (.hex) onto flight controller
flash: flash_$(PROJECT)

unbrick_$(PROJECT): $(TARGET_BIN)
	dfu-util -a 0 -s 0x08000000:leave -D $(TARGET_BIN)

## unbrick     : unbrick flight controller
unbrick: unbrick_$(PROJECT)

# rebuild everything when makefile changes
$(TARGET_OBJS) : Makefile

commit:
	git commit -a --allow-empty-message -m ''
	git push

listen:
	miniterm.py $(SERIAL_DEVICE)
