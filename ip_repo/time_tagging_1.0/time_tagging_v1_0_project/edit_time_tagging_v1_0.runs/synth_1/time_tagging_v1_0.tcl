# 
# Synthesis run script generated by Vivado
# 

  set_param gui.test TreeTableDev
set_msg_config -id {HDL 9-1061} -limit 100000
set_msg_config -id {HDL 9-1654} -limit 100000
create_project -in_memory -part xc7z020clg484-1
set_property target_language Verilog [current_project]
set_param project.compositeFile.enableAutoGeneration 0
set_property default_lib xil_defaultlib [current_project]
set_property ip_repo_paths {
  C:/zynq2/time_tagging/2014_2/ip_repo/time_tagging_1.0
  C:/zynq2/time_tagging/2014_2/ip_repo/time_tagging_1.0
} [current_fileset]
read_verilog -library xil_defaultlib {
  C:/zynq2/time_tagging/2014_2/ip_repo/time_tagging_1.0/hdl/reg2.v
  C:/zynq2/time_tagging/2014_2/ip_repo/time_tagging_1.0/hdl/reg1.v
  C:/zynq2/time_tagging/2014_2/ip_repo/time_tagging_1.0/hdl/org.v
  C:/zynq2/time_tagging/2014_2/ip_repo/time_tagging_1.0/hdl/norg.v
  C:/zynq2/time_tagging/2014_2/ip_repo/time_tagging_1.0/hdl/mux_8.v
  C:/zynq2/time_tagging/2014_2/ip_repo/time_tagging_1.0/hdl/inv.v
  C:/zynq2/time_tagging/2014_2/ip_repo/time_tagging_1.0/hdl/dff.v
  C:/zynq2/time_tagging/2014_2/ip_repo/time_tagging_1.0/hdl/decode_8.v
  C:/zynq2/time_tagging/2014_2/ip_repo/time_tagging_1.0/hdl/counter_28.v
  C:/zynq2/time_tagging/2014_2/ip_repo/time_tagging_1.0/hdl/counter_27.v
  C:/zynq2/time_tagging/2014_2/ip_repo/time_tagging_1.0/hdl/andg.v
  C:/zynq2/time_tagging/2014_2/ip_repo/time_tagging_1.0/hdl/time_tagging_v1_0_S00_AXI.v
  C:/zynq2/time_tagging/2014_2/ip_repo/time_tagging_1.0/hdl/time_tagging_v1_0.v
  C:/zynq2/time_tagging/2014_2/ip_repo/time_tagging_1.0/hdl/counter_27wld.v
  C:/zynq2/time_tagging/2014_2/ip_repo/time_tagging_1.0/hdl/time_tagging_hw_v1_0.v
}
set_param synth.vivado.isSynthRun true
set_property webtalk.parent_dir C:/zynq2/time_tagging/2014_2/ip_repo/IP_Projects/edit_time_tagging_v1_0.cache/wt [current_project]
set_property parent.project_dir C:/zynq2/time_tagging/2014_2/ip_repo/IP_Projects [current_project]
catch { write_hwdef -file time_tagging_v1_0.hwdef }
synth_design -top time_tagging_v1_0 -part xc7z020clg484-1
write_checkpoint time_tagging_v1_0.dcp
report_utilization -file time_tagging_v1_0_utilization_synth.rpt -pb time_tagging_v1_0_utilization_synth.pb
