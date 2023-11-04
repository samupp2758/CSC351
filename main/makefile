all: makefile

makefile: shell.cpp fs.cpp ../Read-Write.cpp
	g++ -std=c++20 ../Read-Write.cpp fs.cpp -o fs -funsigned-char
	g++ -std=c++20 shell.cpp -o shell