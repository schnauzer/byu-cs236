CFLAGS=-g -std=c++11 -Wall

all: main

pkg: main
	zip project1.zip *.h *.cpp

main: *.cpp *.h
	g++ *.cpp -o main $(CFLAGS)

test: inputs/* *.cpp *.h
	./test.sh

mem:
	valgrind --leak-check=full ./main proj_2_tests/in21.txt -

comp:
	pmccabe *.cpp *.h
