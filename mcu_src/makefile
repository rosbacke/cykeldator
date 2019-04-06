CXX:=arm-none-eabi-g++
OBJ_COPY:=arm-none-eabi-objcopy


CXXFLAGS += -mthumb -mcpu=cortex-m3

SPEC:=-specs=nosys.specs

FLAGS=-Os -g -mthumb -mcpu=cortex-m3 -ffunction-sections -nostdlib $(SPEC)


STFW_D=../thirdparty/STM32F10x_StdPeriph_Lib_V3.5.0/Libraries
CMSIS_D=$(STFW_D)/CMSIS/CM3


SRC=system_stm32f10x.c main.cpp startup_stm32f10x_md.s 
# SRC += $(STFW_D)/STM32F10x_StdPeriph_Driver/src/stm32f10x_rcc.c

#usart.c timer.c drivers.c
DEF=-DSTM32F10X_MD=1
INC:=-I$(CMSIS_D)/CoreSupport -I$(CMSIS_D)/DeviceSupport/ST/STM32F10x
# INC+=-I$(STFW_D)/STM32F10x_StdPeriph_Driver/inc


all: main.bin

main.bin : main.elf
	arm-none-eabi-objcopy main.elf main.bin
	arm-none-eabi-objdump -D main.elf > main.dis 
	
main.elf: $(SRC) makefile
	arm-none-eabi-g++ $(DEF) $(INC) $(FLAGS) -Tstm32_flash.ld -o main.elf $(SRC)
	arm-none-eabi-objdump -S main.elf > main_dump.txt


upload: main.elf
	echo 'target remote | openocd -f board/st_nucleo_f103rb.cfg -f interface/stlink-v2-1.cfg -c "gdb_port pipe; log_output openocd.log"' > upload.gdb
	#echo 'file main.elf' >> upload.gdb
	echo 'load' >> upload.gdb
	echo 'monitor reset' >> upload.gdb
	arm-none-eabi-gdb --batch -x upload.gdb main.elf

debug: main.elf
	echo 'target remote | openocd -f board/st_nucleo_f103rb.cfg -f interface/stlink-v2-1.cfg -c "gdb_port pipe; log_output openocd.log"' > debug.gdb
	#echo 'file main.elf' >> upload.gdb
	echo 'load' >> debug.gdb
	echo 'monitor reset halt' >> debug.gdb
	arm-none-eabi-gdb -x debug.gdb main.elf

start_openocd:
	sudo openocd  -f board/st_nucleo_f103rb.cfg -f interface/stlink-v2-1.cfg



build:
	$(CXX) $(CXXFLAGS) -o out.elf main.cpp
	$(OBJ_COPY) -O ihex out.elf out.hex
