SOLUTION = hal_cc2530_TI

CC_TOOL = /opt/cc-tools/bin/cc-tool

SDCCCFLAGS += -Dchip=2530
SDCCCFLAGS += -Itargets/interface
SDCCCFLAGS += -Itargets/srf05_soc/
SDCCCFLAGS += -Ibasicrf
SDCCCFLAGS += -Icommon
SDCCCFLAGS += -Icommon/cc8051
SDCCCFLAGS += -Iradios/cc2530
SDCCCFLAGS += -Iutils

SRC_COMMON = $(wildcard common/*.c)
SRC_BASICRF = $(wildcard basicrf/*.c)
SRC_RADIOS = $(wildcard radios/cc2530/*.c)
SRC_UTILS = $(wildcard utils/*.c)

SRC = $(SRC_COMMON) $(SRC_BASICRF) $(SRC_RADIOS) $(SRC_UTILS)
OBJ = $(patsubst %.c, %.o, $(SRC))

all: $(OBJ)
	echo $^

%.o: %.c
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

