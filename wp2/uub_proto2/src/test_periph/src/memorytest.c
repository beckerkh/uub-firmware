//**************************************************************************
//
// Copyright (C) 2009 - 2014 Xilinx, Inc.  All rights reserved.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// Use of the Software is limited solely to applications:
// (a) running on a Xilinx device, or
// (b) that interact with a Xilinx device through a bus or interconnect.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// XILINX CONSORTIUM BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
// WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF
// OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
//
// Except as contained in this notice, the name of the Xilinx shall not be used
// in advertising or otherwise to promote the sale, use or other dealings in
// this Software without prior written authorization from Xilinx.
//
//**************************************************************************
 
 #include <stdio.h>
 #include "xparameters.h"
 #include "xil_types.h"
 #include "xstatus.h"
 #include "xil_testmem.h" 
 #include "memory_config.h"
 
 struct memory_range_s memory_ranges[] = {
 // ps7_ram_0 memory will not be tested since appl. resides in the same memory
    {
        "ps7_ddr_0",
        "ps7_ddr",
        0x00100000,
        535822336,
    },
    {
        "axi_bram_ctrl_0",
        "axi_bram_ctrl",
        0x40000000,
        8192,
    },
    {
        "ps7_ram_1",
        "ps7_ram",
        0xFFFF0000,
        65024,
    },
 };
 
 //*
 //* memorytest.c: Test memory ranges present in the Hardware Design.
 //*
 //* This application runs with D-Caches disabled. 
 //* As a result cacheline requests will not be generated.
 //*
 //* The BSP enables caches by default, so this application disables D-Caches 
 //* before running memory tests.
 //*
  
 void test_memory_range(struct memory_range_s *range) 
 
 //* The default linker script generated for this application does not have
 //* heap memory allocated. This implies that this program cannot use any
 //* routines that allocate memory on heap (printf is one such function).
 //* If you'd like to add such functions, then please generate a linker script
 //* that does allocate sufficient heap memory.
 {
     XStatus status;
 
 print("\n\r   Testing memory region: ");   print(range->name);
 print("\n\r       Memory Controller: ");   print(range->ip);
 print("\n\r            Base Address: 0x"); putnum(range->base);
 print("\n\r                    Size: 0x"); putnum(range->size);
 
 status = Xil_TestMem32((u32*)range->base, 1024, 0xAAAA5555, XIL_TESTMEM_ALLMEMTESTS);
 print("\n\r             32-bit test: "); print(status == XST_SUCCESS? "PASSED!":"FAILED!");
 
 status = Xil_TestMem16((u16*)range->base, 2048, 0xAA55, XIL_TESTMEM_ALLMEMTESTS);
 print("\n\r             16-bit test: "); print(status == XST_SUCCESS? "PASSED!":"FAILED!");
 
 status = Xil_TestMem8((u8*)range->base, 4096, 0xA5, XIL_TESTMEM_ALLMEMTESTS);
 print("\n\r              8-bit test: "); print(status == XST_SUCCESS? "PASSED!":"FAILED!"); 
 
 }
 
 int TstRamBasis()
 {
   int i;
   int n_memory_ranges = 3;
 
   print("\n\r Starting Memory Test Application");
 
   for (i = 0; i < n_memory_ranges; i++) {test_memory_range(&memory_ranges[i]);}
   
   print("\n\r");
   return XST_SUCCESS;
 }
 