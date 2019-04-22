


INC := -Iu8g2/csrc -Iu8g2/cppsrc -I.

all:
	gcc $(INC) -c u8g2/csrc/*.c 
	g++ $(INC) -c test.cpp u8g2/cppsrc/*.cpp
	g++ -o test *.o
