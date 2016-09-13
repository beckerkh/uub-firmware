# uub-firmware

This repository contains the Vivado and SDK projects for the Zynq
firmware for the Auger Prime UUB.  The goal is to keep only those
files in the repository that are required to recreate the .bit and
.elf files.  After cloning the repository you may need to (eg. should)
perform the following steps:

1) Open the project in Vivado
2) Check & modify as necessary the IP search path
3) Open the block design
4) Reset the output products
5) Generate output products
6) Run synthesis
7) Run implementation
8) Generate bitmap
9) Export hardware and bitmap

10) Exit Vivado and start xSDK (Exit may not be necessary on your machine)
11) Import existing software projects
12) Regenerate the bd files

If you fork the repository with the intent of feeding back your
developments into the repository, please do not add all the Vivado &
SDK temp files to the repository, as that makes including your
developments into the repository master more difficult.  Just add the
subdirectories in ip_repo that you may have added, and the .c and .h
files you created in the SDK.  If you are making a new copy of a
project to modify, please start with a clean copy and commit just those
files before creating lots of temporary files with Vivado or xSDK.


