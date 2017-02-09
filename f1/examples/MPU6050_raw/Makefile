###############################################################################
# "THE BEER-WARE LICENSE" (Revision 42):
# <msmith@FreeBSD.ORG> wrote this file. As long as you retain this notice you
# can do whatever you want with this stuff. If we meet some day, and you think
# this stuff is worth it, you can buy me a beer in return

###############################################################################

# Change this to wherever you put BreezySTM32
BREEZY_DIR = $(HOME)/Desktop/BreezySTM32

# Change this to whatever example you want
TARGET		?= MPU6050_raw

###############################################################################

F1_DIR = $(BREEZY_DIR)/f1

# Compile-time options
OPTIONS		?=

# Debugger optons, must be empty or GDB
DEBUG ?=

# Serial port/Device for flashing
SERIAL_DEVICE	?= /dev/ttyUSB0

# Working directories
ROOT		 = $(dir $(lastword $(MAKEFILE_LIST)))
SRC_DIR		 = $(ROOT)
CMSIS_DIR	 = $(BREEZY_DIR)/lib/CMSIS
STDPERIPH_DIR	 = $(BREEZY_DIR)/lib/STM32F10x_StdPeriph_Driver
OBJECT_DIR	 = $(ROOT)/obj
BIN_DIR		 = $(ROOT)/obj

$(TARGET)_CSRC = \
		   $(F1_DIR)/startup_stm32f10x_md_gcc.S \
		   $(F1_DIR)/drv_gpio.c \
		   $(F1_DIR)/drv_i2c.c \
		   $(F1_DIR)/drv_pwm.c \
		   $(F1_DIR)/drv_system.c \
		   $(F1_DIR)/drv_serial.c \
		   $(F1_DIR)/drv_uart.c \
		   $(F1_DIR)/drv_timer.c \
		   $(CMSIS_SRC) \
		   $(STDPERIPH_SRC)

VPATH		:= $(SRC_DIR):$(SRC_DIR)/startups

# Search path and source files for the CMSIS sources
VPATH		:= $(VPATH):$(CMSIS_DIR)/CM3/CoreSupport:$(CMSIS_DIR)/CM3/DeviceSupport/ST/STM32F10x
CMSIS_SRC	 = $(notdir $(wildcard $(CMSIS_DIR)/CM3/CoreSupport/*.c \
			               $(CMSIS_DIR)/CM3/DeviceSupport/ST/STM32F10x/*.c))

# Search path and source files for the ST stdperiph library
VPATH		:= $(VPATH):$(STDPERIPH_DIR)/src
STDPERIPH_SRC	 = $(notdir $(wildcard $(STDPERIPH_DIR)/src/*.c))

###############################################################################
# Things that might need changing to use different tools
#

# Tool names
C++		 = arm-none-eabi-g++
CC		 = arm-none-eabi-gcc 
OBJCOPY	 = arm-none-eabi-objcopy

#
# Tool options.
#
INCLUDE_DIRS	 = $(SRC_DIR) \
		   $(F1_DIR) \
		   $(F1_DIR)/arduino \
		   $(STDPERIPH_DIR)/inc \
		   $(CMSIS_DIR)/CM3/CoreSupport \
		   $(CMSIS_DIR)/CM3/DeviceSupport/ST/STM32F10x \

ARCH_FLAGS	 = -mthumb -mcpu=cortex-m3

ifeq ($(DEBUG),GDB)
OPTIMIZE	 = -Og

else
OPTIMIZE	 = -Os
LTO_FLAGS	 = -flto -fuse-linker-plugin $(OPTIMIZE)
endif

DEBUG_FLAGS	 = -ggdb3

CFLAGS		 = $(ARCH_FLAGS) \
		   $(LTO_FLAGS) \
		   $(addprefix -D,$(OPTIONS)) \
		   $(addprefix -I,$(INCLUDE_DIRS)) \
		   $(DEBUG_FLAGS) \
		   -Wall -pedantic -Wextra -Wshadow -Wunsafe-loop-optimizations \
		   -ffunction-sections \
		   -fdata-sections \
		   -DSTM32F10X_MD \
		   -DUSE_STDPERIPH_DRIVER \
		   -D$(TARGET)

ASFLAGS		 = $(ARCH_FLAGS) \
		   -x assembler-with-cpp \
		   $(addprefix -I,$(INCLUDE_DIRS))

LD_SCRIPT	 = $(F1_DIR)/stm32_flash.ld
LDFLAGS		 = -lm \
		   -nostartfiles \
		   -lc \
		   --specs=rdimon.specs \
		   $(ARCH_FLAGS) \
		   $(LTO_FLAGS) \
		   $(DEBUG_FLAGS) \
		   -static \
		   -Wl,-gc-sections,-Map,$(TARGET_MAP) \
		   -T$(LD_SCRIPT)

#
# Things we will build
#

TARGET_HEX	 = $(BIN_DIR)/$(TARGET).hex
TARGET_ELF	 = $(BIN_DIR)/$(TARGET).elf
TARGET_OBJS	 = $(addsuffix .o,$(addprefix $(OBJECT_DIR)/$(TARGET)/,$(basename $($(TARGET)_CSRC)))) \
			   $(OBJECT_DIR)/$(TARGET)/Arduino.o \
			   $(OBJECT_DIR)/$(TARGET)/HardwareSerial.o \
			   $(OBJECT_DIR)/$(TARGET)/HardwareWire.o \
			   $(OBJECT_DIR)/$(TARGET)/MPU6050.o \
			   $(OBJECT_DIR)/$(TARGET).o
TARGET_MAP   = $(OBJECT_DIR)/$(TARGET).map

# List of buildable ELF files and their object dependencies.
# It would be nice to compute these lists, but that seems to be just beyond make.

$(TARGET_HEX): $(TARGET_ELF)
	$(OBJCOPY) -O ihex --set-start 0x8000000 $< $@

$(TARGET_ELF):  $(TARGET_OBJS)
	$(CC) -o $@ $^ $(LDFLAGS)

MKDIR_OBJDIR = @mkdir -p $(dir $@)

# Compile
$(OBJECT_DIR)/$(TARGET)/%.o: %.c
	$(MKDIR_OBJDIR)
	@echo %% $(notdir $<)
	@$(CC) -c -o $@ $(CFLAGS) $<

# Assemble
$(OBJECT_DIR)/$(TARGET)/%.o: %.S
	$(MKDIR_OBJDIR)
	@echo %% $(notdir $<)
	@$(CC) -c -o $@ $(ASFLAGS) $< 

# C++ support
%.o: %.cpp
	$(CC) -c $(CFLAGS) -o $@ $^

$(OBJECT_DIR)/$(TARGET)/Arduino.o: $(F1_DIR)/Arduino.cpp $(F1_DIR)/Arduino.h
	@$(CC) -c -o $(OBJECT_DIR)/$(TARGET)/Arduino.o $(CFLAGS) $(F1_DIR)/Arduino.cpp

$(OBJECT_DIR)/$(TARGET)/HardwareSerial.o: $(F1_DIR)/HardwareSerial.cpp $(F1_DIR)/Arduino.h
	@$(CC) -c -o $(OBJECT_DIR)/$(TARGET)/HardwareSerial.o $(CFLAGS) $(F1_DIR)/HardwareSerial.cpp

$(OBJECT_DIR)/$(TARGET)/HardwareWire.o: $(F1_DIR)/HardwareWire.cpp $(F1_DIR)/Arduino.h
	@$(CC) -c -o $(OBJECT_DIR)/$(TARGET)/HardwareWire.o $(CFLAGS) $(F1_DIR)/HardwareWire.cpp

$(OBJECT_DIR)/$(TARGET)/MPU6050.o: $(F1_DIR)/MPU6050.cpp $(F1_DIR)/MPU6050.h
	@$(CC) -c -o $(OBJECT_DIR)/$(TARGET)/MPU6050.o $(CFLAGS) $(F1_DIR)/MPU6050.cpp

$(OBJECT_DIR)/$(TARGET).o: ./$(TARGET).cpp
	@$(CC) -c -o $(OBJECT_DIR)/$(TARGET).o $(CFLAGS) ./$(TARGET).cpp

clean:
	rm -rf $(TARGET_HEX) $(TARGET_ELF) $(TARGET_OBJS) $(TARGET_MAP) *.o obj 

flash_$(TARGET): $(TARGET_HEX)
	stty -F $(SERIAL_DEVICE) raw speed 115200 -crtscts cs8 -parenb -cstopb -ixon
	stm32flash -w $(TARGET_HEX) -v -g 0x0 -b 115200 $(SERIAL_DEVICE)

flash: flash_$(TARGET)

listen:
	miniterm.py $(SERIAL_DEVICE) 115200