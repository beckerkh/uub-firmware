################################################################################

# This XDC is used only for OOC mode of synthesis, implementation
# This constraints file contains default clock frequencies to be used during
# out-of-context flows such as OOC Synthesis and Hierarchical Designs.
# This constraints file is not used in normal top-down synthesis (default flow
# of Vivado)
################################################################################
create_clock -name FPGA_CK_N -period 8.3333333333333339 [get_ports FPGA_CK_N]
create_clock -name FPGA_CK_P -period 8.3333333333333339 [get_ports FPGA_CK_P]
create_clock -name ADC2_CK_P -period 8.3333333333333339 [get_ports ADC2_CK_P]
create_clock -name ADC2_CK_N -period 8.3333333333333339 [get_ports ADC2_CK_N]
create_clock -name ADC3_CK_P -period 8.3333333333333339 [get_ports ADC3_CK_P]
create_clock -name ADC3_CK_N -period 8.3333333333333339 [get_ports ADC3_CK_N]
create_clock -name ADC4_CK_P -period 8.3333333333333339 [get_ports ADC4_CK_P]
create_clock -name ADC4_CK_N -period 8.3333333333333339 [get_ports ADC4_CK_N]
create_clock -name ADC0_CK_P -period 8.3333333333333339 [get_ports ADC0_CK_P]
create_clock -name ADC1_CK_P -period 8.3333333333333339 [get_ports ADC1_CK_P]
create_clock -name ADC1_CK_N -period 8.3333333333333339 [get_ports ADC1_CK_N]
create_clock -name ADC0_CK_N -period 8.3333333333333339 [get_ports ADC0_CK_N]
create_clock -name zync_block_processing_system7_0_FCLK_CLK0 -period 20 [get_pins zync_block/processing_system7_0/FCLK_CLK0]
create_clock -name zync_block_processing_system7_0_FCLK_CLK1 -period 4.8000001228800029 [get_pins zync_block/processing_system7_0/FCLK_CLK1]

################################################################################