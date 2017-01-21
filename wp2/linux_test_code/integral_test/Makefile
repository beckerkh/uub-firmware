   CCARM=arm-xilinx-linux-gnueabi-gcc

cflags=-I. -g -O0
HEADERS = fe_kernel_interface_defs.h sde_trigger.h xil_assert.h xstatus.h \
	fe_lib.h sde_trigger_options.h xil_types.h read_evt.h \
	shwr_evt_defs.h xparameters.h sde_trigger_defs.h time_tagging_defs.h \
	xparameters_ps.h

all: uub_integral

uub_integral: uub_integral.c read_evt.c $(HEADERS)
	$(CCARM) -o uub_integral uub_integral.c read_evt.c $(cflags) -lrt

clean:;
	rm uub_integral
