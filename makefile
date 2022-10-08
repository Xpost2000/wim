CFLAGS=-fno-exceptions -fno-rtti -nostdlib -ffreestanding -mconsole -lkernel32

.PHONY: all
all: wim.exe
wim.exe: wim.cpp makefile
	g++ wim.cpp -o $@ -std=c++17 -Os $(CFLAGS)
wimd.exe: wim.cpp makefile
	g++ wim.cpp -o $@ -std=c++17 -ggdb3 $(CFLAGS)
