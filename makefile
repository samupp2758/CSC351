all: rw

rw: Read-Write.cpp FS.h
	g++ Read-Write.cpp Read-Write-m.cpp -o rw -funsigned-char


clean:
	rm -f *.exe