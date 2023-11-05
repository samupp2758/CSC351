all: makefile

makefile: shell.cpp fs.cpp ../Read-Write.cpp
	g++ -std=c++11 ../Read-Write.cpp fs.cpp -o fs -funsigned-char
	g++ -std=c++11 shell.cpp -o shell