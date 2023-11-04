all: makefile

makefile: shell.cpp fs.cpp ../Read-Write.cpp
	g++ -std=c++17 ../Read-Write.cpp fs.cpp -o fs
	g++ -std=c++17 shell.cpp -o shell