SOLUTION = test_connection_hal

CC_TOOL = /opt/cc-tools/bin/cc-tool

SDCCCFLAGS += -I../hal_cc2530_TI
SDCCCFLAGS += -I../hal_cc2530_TI/targets/interface
SDCCCFLAGS += -I../hal_cc2530_TI/targets/srf05_soc/
SDCCCFLAGS += -I../hal_cc2530_TI/basicrf
SDCCCFLAGS += -I../hal_cc2530_TI/common
SDCCCFLAGS += -I../hal_cc2530_TI/common/cc8051
SDCCCFLAGS += -I../hal_cc2530_TI/radios/cc2530
SDCCCFLAGS += -I../hal_cc2530_TI/utils
SDCCCFLAGS += -L../hal_cc2530_TI/

SRC = $(wildcard src/*.c)

$(SOLUTION).hex: $(SRC)
	sdcc-sdcc $(SDCCCFLAGS) hal_cc2530_TI.lib $<
	sdcc-packihx $(SOLUTION).ihx > $(SOLUTION).hex

burn: $(SOLUTION).hex
	sudo $(CC_TOOL) -e -w $^ -v
	sudo $(CC_TOOL) --reset

sim: $(SOLUTION).hex
	sdcc-s51 $^

clean:
	-rm -f *.asm
	-rm -f *.ihx
	-rm -f *.lk
	-rm -f *.lst
	-rm -f *.map
	-rm -f *.mem
	-rm -f *.rel
	-rm -f *.rst
	-rm -f *.sym  
	-rm -f *.hex
