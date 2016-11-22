ifdef ARM
   CC=arm-xilinx-linux-gnueabi-gcc
endif


cflags=-I.

all: reg regf test0 addr

test0: test0.c read_evt.c 
	$(CC) -o test0 test0.c read_evt.c $(cflags) -lrt

reg: reg.c
	$(CC) -o reg reg.c $(cflags) 

regf: reg_file.c
	$(CC) -o regf reg_file.c $(cflags) 

addr: addr.c
	$(CC) -o addr addr.c $(cflags) 


clean:;
	rm reg regf test0 addr
