SOLUTION = test_connection
SELF_ADDR = 1

CC_TOOL = /opt/cc-tools/bin/cc-tool

SRC = $(wildcard *.c)

SDCCCFLAGS += -I../hal_cc2530/
SDCCCFLAGS += -L../hal_cc2530/
SDCCCFLAGS += hal_cc2530.lib
SDCCCFLAGS += -DSELF_ADDR=$(SELF_ADDR)

$(SOLUTION).hex: $(SRC)
	sdcc-sdcc $(SDCCCFLAGS) $<
	sdcc-packihx $(SOLUTION).ihx > $(SOLUTION).hex

.PHONY:burn
burn: $(SOLUTION).hex
	sudo $(CC_TOOL) -e -w $^ -v
	sudo $(CC_TOOL) --reset

.PHONY:reset
reset:
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

