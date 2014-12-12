SOLUTION = hal_cc2530

CC_TOOL = /opt/cc-tools/bin/cc-tool

SRC_COMMON = $(wildcard common/*.c)
SRC_BASICRF = $(wildcard basicrf/*.c)
SRC_RADIOS = $(wildcard radios/cc2530/*.c)
SRC_UTILS = $(wildcard utils/*.c)

SRC = $(wildcard *.c)
OBJ = $(patsubst %.c, %.rel, $(SRC))

all: $(OBJ)
	sdcc-sdcclib $(SOLUTION).lib *.rel

%.rel: %.c
	sdcc-sdcc -c $(SDCCCFLAGS) $<

clean:
	-rm -rf *.asm
	-rm -rf *.ihx
	-rm -rf *.lk
	-rm -rf *.lst
	-rm -rf *.map
	-rm -rf *.mem
	-rm -rf *.rel
	-rm -rf *.rst
	-rm -rf *.sym  
	-rm -rf *.hex
	-rm -rf *.lib
