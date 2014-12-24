
all:
	make -C hal_cc2530
	make -C hal_cc2530_TI
	make -C test_uart
	make -C test_connection
	make -C test_connection_hal
	make -C test_someradio

clean:
	make -C hal_cc2530 clean
	make -C hal_cc2530_TI clean
	make -C test_uart clean
	make -C test_connection clean
	make -C test_connection_hal clean
	make -C test_someradio clean
