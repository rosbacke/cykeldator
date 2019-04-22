



INC := -Iu8g2/csrc -Iu8g2/cppsrc -I. 
FLAGS:= $(INC) -Os -g -mthumb -mcpu=cortex-m3 -ffunction-sections -nostdlib $(SPEC) -fno-exceptions -fno-rtti -ffunction-sections -fdata-sections

all:
	arm-none-eabi-gcc -Os $(FLAGS) -c u8g2/csrc/*.c 
	arm-none-eabi-g++ -Os $(FLAGS) -c test.cpp u8g2/cppsrc/*.cpp
	arm-none-eabi-ar rc display.a *.o

clean:
	rm *.o
	rm display.a
		