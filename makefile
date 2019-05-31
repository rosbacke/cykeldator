all: main.elf unittest interpreter

unittest: host_build/mcu_src/mcu_if_test
interpreter : host_build/src/interpreter/interpreter 

out/main.elf : target_build/src/target_main/main
	cp target_build/src/target_main/main out/main.elf
	arm-none-eabi-objdump -C -S out/main.elf > out/main_dump.txt
	arm-none-eabi-objcopy -O ihex out/main.elf out/main.hex
	arm-none-eabi-objdump -C -D out/main.elf > out/main.txt

upload: out/main.elf
	@echo 'target remote | openocd -f board/st_nucleo_f103rb.cfg -f interface/stlink-v2-1.cfg -c "gdb_port pipe; log_output openocd.log"' > upload.gdb
	@echo 'monitor halt'
	@echo 'monitor reset halt'
	@echo 'load' >> upload.gdb
	@echo 'monitor reset' >> upload.gdb
	@echo 'monitor exit' >> upload.gdb
	$(GDB) --batch -x upload.gdb out/main.elf

debug: out/main.elf
	echo 'target remote | openocd -f board/st_nucleo_f103rb.cfg -f interface/stlink-v2-1.cfg -c "gdb_port pipe; log_output openocd.log"' > debug.gdb
	echo 'load' >> debug.gdb
	echo 'monitor reset halt' >> debug.gdb
	$(GDB) -x debug.gdb out/main.elf

start_openocd:
	sudo openocd  -f board/st_nucleo_f103rb.cfg -f interface/stlink-v2.cfg


# Require Boot0 set to '1' and an manual reset before upload.
uploadserial: out/main.elf
	stm32flash /dev/ttyACM1 -w out/main.hex

clean:
	rm -rf target_build
	rm -rf host_build
	rm -rf build
	rm -f main.elf main.hex main_dump.txt target_build signalchain_test
	rm -f interpreter
	rm -f out/*


cmake_build: cmake_target_build cmake_host_build

target_build/src/target_main/main : cmake_target_build
host_build/mcu_src/mcu_if_test : cmake_host_build
host_build/src/interpreter/interpreter : cmake_host_build

cmake_target_build:
	mkdir -p target_build && cd target_build && cmake .. -DCMAKE_TOOLCHAIN_FILE=../cmake/stm32_toolchain.cmake -DBUILD_TARGET=1
	cd target_build && make

cmake_host_build:
	mkdir -p host_build && cd host_build && cmake .. -DBUILD_HOST=1
	cd host_build && make && make test

cmake_upload: out/main.elf
	@echo 'target remote | openocd -f board/st_nucleo_f103rb.cfg -f interface/stlink-v2.cfg -c "gdb_port pipe; log_output openocd.log"' > upload.gdb
	@echo 'monitor halt'
	@echo 'monitor reset halt'
	@echo 'load' >> upload.gdb
	@echo 'monitor reset' >> upload.gdb
	@echo 'monitor exit' >> upload.gdb
	$(GDB) --batch -x upload.gdb target_build/main
