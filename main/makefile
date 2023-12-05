all: makefile

makefile: shell.h _tcp_fs.h shell.cpp fs.cpp ../Read-Write.cpp
	g++ -std=c++11 ../Read-Write.cpp fs.cpp -o fs -funsigned-char
	g++ -std=c++11 shell.cpp -framework Cocoa -framework Carbon -framework CoreServices -o shell