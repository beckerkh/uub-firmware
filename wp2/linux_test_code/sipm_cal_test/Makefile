   CCARM=arm-xilinx-linux-gnueabi-gcc

cflags=-I.
HEADERS = sde_trigger.h xil_assert.h xstatus.h \
	sde_trigger_options.h xil_types.h \
	xparameters.h sde_trigger_defs.h \
	xparameters_ps.h uub_sipm_cal.h

all: uub_sipm_cal

uub_sipm_cal: uub_sipm_cal.c map_regs.c muon_buf_utils.c $(HEADERS)
	$(CCARM) -o uub_sipm_cal uub_sipm_cal.c map_regs.c muon_buf_utils.c \
	$(cflags) -lrt

clean:;
	uub_sipm_cal
