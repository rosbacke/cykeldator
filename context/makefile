

TP_D:=../thirdparty/STM32F10x_StdPeriph_Lib_V3.5.0/Libraries/CMSIS/CM3

INC:=-I../src/target_main/ -I$(TP_D)/DeviceSupport/ST/STM32F10x -I$(TP_D)/CoreSupport -I$(TP_D)
DEF:=-DSTM32F10X_MD=1

CFLAGS:=$(INC) $(DEF) -Os -g -mthumb -mcpu=cortex-m3 -nostdlib
CFLAGS+=-ffunction-sections -fdata-sections
CXXFLAGS:= -fno-exceptions -fno-rtti

LDFLAGS:= -Wl,--gc-sections -T../src/target_main/stm32_flash.ld

CC:=arm-none-eabi-gcc
CXX:=arm-none-eabi-g++
LD:=arm-none-eabi-g++

all:
	$(CXX) $(CFLAGS) $(CXXFLAGS) $(LDFLAGS) main.cpp context.cpp ../src/target_main/startup_stm32f10x_md.s ../mcu_src/mcuaccess.cpp ../src/target_main/system_stm32f10x.c
	arm-none-eabi-objdump -D a.out > main.txt

