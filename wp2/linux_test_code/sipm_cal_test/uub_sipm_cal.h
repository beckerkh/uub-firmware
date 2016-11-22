// Header file for simple standalone test routine to interact with the
// sde_trigger PL.
//
// 25-May-2016 DFN Initial version extracted from trigger_test.c


#include <stdio.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include "xparameters.h"  // Peripheral parameters (GPIO addresses, etc.)
#include "sde_trigger_defs.h"

#define MAX_BUF 2
#define TRIG_THR0 400
#define TRIG_THR1 400
#define TRIG_THR2 400
#define TRIG_SSD 400

#define MUONS_PER_BUF (MUON_MEM_WORDS/MUON_BURST_LEN)
#define ADC_MASK ((1<<ADC_WIDTH)-1)

// Registers
extern volatile u32 *regs;

// Muon memory buffers
extern volatile u32 *muon_buf0;
extern volatile u32 *muon_buf1;
extern u32 muon_mem0[MUON_MEM_WORDS];
extern u32 muon_mem1[MUON_MEM_WORDS];
extern u16 muon_adc[4][MUON_BURST_LEN][MUONS_PER_BUF];
extern u16 muon_seq_num[MUON_BURST_LEN][MUONS_PER_BUF];
extern u16 muon_trig_tags[MUONS_PER_BUF];
extern u32 muon_burst_start[MUONS_PER_BUF];
extern u32 muon_buffer_start;
extern u32 muon_buffer_end;
extern int mu_word_count;
extern int toread_muon_buf_num;

void read_muon_buffers();
void unpack_muon_buffers();
void check_muon_buffers();
void print_muon_buffers();

// Macros
#define write_trig(RegNumber, Data) regs[RegNumber] = Data
#define read_trig(RegNumber) regs[RegNumber]

