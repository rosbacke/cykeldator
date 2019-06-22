
INC=-I.. -I/usr/src/gtest/include
FLAGS:= -g -pthread -Wall

ASM:=make_x86_64_sysv_elf_gas.S jump_x86_64_sysv_elf_gas.S ontop_x86_64_sysv_elf_gas.S

all:
	g++ $(INC) $(FLAGS) -o main.elf main.cpp $(ASM)  -lfmt -lgtest -lgtest_main

