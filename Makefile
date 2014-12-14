
all:
	make -C hal_cc2530
	make -C test_uart
	make -C test_someradio

clean:
	make -C hal_cc2530 clean
	make -C test_uart clean
	make -C test_someradio clean
