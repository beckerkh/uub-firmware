   CCARM=arm-xilinx-linux-gnueabi-gcc

cflags=-I. -g -O0
HEADERS = sde_trigger.h xil_assert.h xstatus.h \
	fe_lib.h sde_trigger_options.h xil_types.h read_evt.h \
	shwr_evt_defs.h xparameters.h sde_trigger_defs.h time_tagging_defs.h \
	xparameters_ps.h trigger_test.h trigger_test_options.h

all: linux_trigger_test

linux_trigger_test: linux_trigger_test.c trigger_test.c shwr_buf_utils.c \
	muon_buf_utils.c map_registers.c config_trigger.c $(HEADERS)
	$(CCARM) -o linux_trigger_test linux_trigger_test.c trigger_test.c \
	shwr_buf_utils.c muon_buf_utils.c map_registers.c config_trigger.c \
	$(cflags) -lrt

clean:;
	rm linux_trigger_test
