# make_it.tcl
#
# Run through all the steps to make uub_proto2 project
#-----------------------------------------------------------

open_bd_design {/home/augerdn/dfnitz/scratch/git_clones/uub-firmware/wp2/uub_proto2/bd/uub_proto2/uub_proto2.bd}
reset_target all [get_files  /home/augerdn/dfnitz/scratch/git_clones/uub-firmware/wp2/uub_proto2/bd/uub_proto2/uub_proto2.bd]
reset_run synth_1
launch_runs synth_1 -jobs 2
wait_on_run synth_1
launch_runs impl_1 -jobs 2
wait_on_run impl_1
launch_runs impl_1 -to_step write_bitstream -jobs 2
wait_on_run impl_1
open_run impl_1
file copy -force /home/augerdn/dfnitz/scratch/git_clones/uub-firmware/wp2/uub_proto2/uub_proto2.runs/impl_1/uub_proto2_wrapper.sysdef /home/augerdn/dfnitz/scratch/git_clones/uub-firmware/wp2/uub_proto2/uub_proto2.sdk/uub_proto2_wrapper.hdf

