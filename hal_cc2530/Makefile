SOLUTION = hal_cc2530

CC_TOOL = /opt/cc-tools/bin/cc-tool

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
