# This makefile 
#   1) Creates sde_trigger_options.vh with the compile date loaded into
#      a macro.
#   2) Translates sde_trigger_options.vh and sde_trigger_defs.vh to 
#      C header files sde_trigger_options.h and sde_trigger_defs.h
#
# Unfortunately, at this time this makefile has to be run manually before
# repackaging modified sde_trigger IP.
#
# 21-Aug-2014 DFN Initial version.
# 23-Dec-2015 DFN Remove "v1_2" from names.
# 26-May-2016 DFN Remove leading 0's from register address
# 03-Aug-2016 DFN Add include of "time_tagging.h"
# 14-Aug-2016 DFN Add saving of compile date in file sde_trigger_id

all:	sde_trigger_id sde_trigger_options.vh  \
	../drivers/sde_trigger/src/sde_trigger_defs.h \
	../drivers/sde_trigger/src/sde_trigger_options.h \
	../../time_tagging_1.0/drivers/time_tagging_v1_0/src/time_tagging.h

sde_trigger_id: FORCE
	echo "`date +%H%d%m%y`" >sde_trigger_id

sde_trigger_options.vh: FORCE
	echo "\`ifndef _TRIGGER_OPTIONS" >sde_trigger_options.vh
	echo "  \`define _TRIGGER_OPTIONS" >>sde_trigger_options.vh
	echo "  \`define COMPILE_DATE 'h`cat sde_trigger_id`" \
	>>sde_trigger_options.vh
	echo "\`endif" >>sde_trigger_options.vh

../drivers/sde_trigger/src/sde_trigger_defs.h: 	FORCE
	echo "#include \"sde_trigger.h\"" \
	>../drivers/sde_trigger/src/sde_trigger_defs.h
	echo "#include \"sde_trigger_options.h\"" \
	>>../drivers/sde_trigger/src/sde_trigger_defs.h
	echo "#include \"time_tagging.h\"" \
	>>../drivers/sde_trigger/src/sde_trigger_defs.h
	echo "" \
	>>../drivers/sde_trigger/src/sde_trigger_defs.h
	sed -e "s/\`define/\#define/" <sde_trigger_defs.vh | \
	sed -e "s/\`ifndef/\#ifndef/" | \
	sed -e "s/\`endif/\#endif/" | \
	sed -e "s/\`//" | \
	sed -e "s/\`//" | \
	sed -e "s/\`//" | \
	sed -e "s/'h/0x/" | \
	sed -e "s/ADDR 0/ADDR /" | \
	sed -e "s/ADDR 0/ADDR /" | \
	sed -e "s/slv_reg\([0-9]*\)/SDE_TRIGGER_S00_AXI_SLV_REG\1_OFFSET/" \
	>> ../drivers/sde_trigger/src/sde_trigger_defs.h

../drivers/sde_trigger/src/sde_trigger_options.h:	sde_trigger_options.vh
	sed -e "s/\`define/\#define/" <sde_trigger_options.vh | \
	sed -e "s/\`ifndef/\#ifndef/" | \
	sed -e "s/\`endif/\#endif/" | \
	sed -e "s/'h/0x/" \
	> ../drivers/sde_trigger/src/sde_trigger_options.h

clean:
	rm -f ../drivers/sde_trigger/src/sde_trigger_defs.h
	rm -f ../drivers/sde_trigger/src/sde_trigger_options.h
	rm -f sde_trigger_options.vh

FORCE:


