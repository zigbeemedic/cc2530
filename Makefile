
all:
	make -C hal_cc2530
	make -C test_uart

clean:
	make -C hal_cc2530 clean
	make -C test_uart clean