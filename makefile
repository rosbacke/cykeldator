CXX:=arm-none-eabi-g++
OBJ_COPY:=arm-none-eabi-objcopy
GDB:=gdb-multiarch

CXXFLAGS += -mthumb -mcpu=cortex-m3

SPEC:=-specs=nosys.specs

FLAGS=-Os -g -mthumb -mcpu=cortex-m3 -ffunction-sections -nostdlib $(SPEC) -fno-exceptions -fno-rtti

HEADERS:=mcu_src/mcuaccess.h mcu_src/isr.h mcu_src/isr_project.h src/Strings.h src/SignalChain.h 

STFW_D=thirdparty/STM32F10x_StdPeriph_Lib_V3.5.0/Libraries
CMSIS_D=$(STFW_D)/CMSIS/CM3

MCU_SRC:=system_stm32f10x.c main.cpp startup_stm32f10x_md.s timer.cpp usart.cpp mcuaccess.cpp isr.cpp
SRC:=$(MCU_SRC:%=mcu_src/%)

#usart.c timer.c drivers.c
DEF=-DSTM32F10X_MD=1

INC:=-I$(CMSIS_D)/CoreSupport -I$(CMSIS_D)
INC+=-Isrc
INC+=-Imcu_src
INC+=-I.
INC+=-I../delegate/include

FLAGS_TEST=-Os -g $(DEF) $(INC) -DUNIT_TEST=1 -I/usr/src/gtest/include -L /usr/src/gtest -L /usr/src/gtest/build -pthread

all: main.hex unittest interpreter

unittest: signalchain_test
	./signalchain_test

TEST_SRC := src/SignalChain.cpp src/SignalChain_test.cpp src/timer_test.cpp mcu_src/timer.cpp mcu_src/mcuaccess.cpp mcu_src/isr.cpp


interpreter : src/interpreter.cpp src/SignalChain.cpp 
	g++ $(FLAGS_TEST) -o interpreter src/interpreter.cpp src/SignalChain.cpp -lfmt

signalchain_test: $(TEST_SRC) $(HEADERS)
	g++ $(FLAGS_TEST) -o signalchain_test $(TEST_SRC) -lgtest -lgtest_main
	arm-none-eabi-objdump -D main.elf > main.dis 

main.elf: $(SRC) makefile  $(HEADERS)
	arm-none-eabi-g++ $(DEF) $(INC) $(FLAGS) -Tmcu_src/stm32_flash.ld -o main.elf $(SRC)
	arm-none-eabi-objdump -C -S main.elf > main_dump.txt

main.hex : main.elf
	arm-none-eabi-objcopy -O ihex main.elf main.hex
	arm-none-eabi-objdump -C -D main.elf > main.txt

upload: main.elf
	@echo 'target remote | openocd -f board/st_nucleo_f103rb.cfg -f interface/stlink-v2-1.cfg -c "gdb_port pipe; log_output openocd.log"' > upload.gdb
	#echo 'file main.elf' >> upload.gdb
	@echo 'monitor halt'
	@echo 'monitor reset halt'
	@echo 'load' >> upload.gdb
	@echo 'monitor reset' >> upload.gdb
	@echo 'monitor exit' >> upload.gdb
	$(GDB) --batch -x upload.gdb main.elf

debug: main.elf
	echo 'target remote | openocd -f board/st_nucleo_f103rb.cfg -f interface/stlink-v2-1.cfg -c "gdb_port pipe; log_output openocd.log"' > debug.gdb
	#echo 'file main.elf' >> upload.gdb
	echo 'load' >> debug.gdb
	echo 'monitor reset halt' >> debug.gdb
	$(GDB) -x debug.gdb main.elf

start_openocd:
	sudo openocd  -f board/st_nucleo_f103rb.cfg -f interface/stlink-v2-1.cfg

# Require Boot0 set to '1' and an manual reset before upload.
uploadserial: main.hex
	stm32flash /dev/ttyACM1 -w main.hex
