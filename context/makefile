GDB:=arm-none-eabi-gdb

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

all: main.elf

main.elf : main.cpp context.cpp 
	$(CXX) $(CFLAGS) $(CXXFLAGS) $(LDFLAGS) -o main.elf main.cpp context.cpp ../src/target_main/startup_stm32f10x_md.s ../src/target_main/system_stm32f10x.c ../mcu_src/mcuaccess.cpp
	arm-none-eabi-objdump -D main.elf > main.txt

upload: main.elf
	@echo 'target remote | openocd -f board/st_nucleo_f103rb.cfg -f interface/stlink-v2.cfg -c "gdb_port pipe; log_output openocd.log"' > upload.gdb
	@echo 'monitor halt'
	@echo 'monitor reset halt'
	@echo 'load' >> upload.gdb
	@echo 'monitor reset' >> upload.gdb
	@echo 'monitor exit' >> upload.gdb
	$(GDB) --batch -x upload.gdb main.elf
