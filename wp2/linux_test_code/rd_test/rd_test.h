// Header file for simple standalone test routine to interact with the
// radio detector interface PL.
//
// 02-Nov-2018 DFN Initial version



#include <stdio.h>
#include "xparameters.h"  // Peripheral parameters (GPIO addresses, etc.)
#include "xil_types.h"

#include <stdlib.h>
#include <stdint.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

void map_registers();

#define RD_MEM_DEPTH 8192
#define RD_MEM_WORDS 2048
#define RD_MEM_NBUF 4

// Macros

#ifndef FAKE_RD_BASE
  #define FAKE_RD_BASE XPAR_FAKE_RD_BLOCK_FAKE_RD_INTERFACE_0_S00_AXI_BASEADDR
#endif

#ifndef FAKE_RD_EVENT0_BASE
  #define FAKE_RD_EVENT0_BASE XPAR_FAKE_RD_BLOCK_AXI_BRAM_CTRL_0_S_AXI_BASEADDR
#endif

#ifndef RD_EVENT0_BASE
  #define RD_EVENT0_BASE XPAR_FAKE_RD_BLOCK_AXI_BRAM_CTRL_1_S_AXI_BASEADDR
#endif

extern volatile u32 *rd_regs;

#define write_rd(RegNumber, Data) rd_regs[RegNumber] = Data
#define read_rd(RegNumber) rd_regs[RegNumber]

extern volatile u32 fake_rd_mem_ptr[1];
extern u32 fake_rd_mem_addr[1];
extern volatile u32 rd_mem_ptr[1];
extern u32 rd_mem_addr[1];


