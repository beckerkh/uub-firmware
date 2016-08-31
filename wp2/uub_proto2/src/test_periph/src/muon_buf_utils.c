// Utility routines for reading, unpacking, checking muon buffers.
//
// 25-May-2016 DFN Initial version extracted from trigger_test.c

#include "trigger_test.h"

// Muon memory buffers
extern u32 muon_mem0[MUON_MEM_WORDS];
extern u32 muon_mem1[MUON_MEM_WORDS];
extern u16 muon_adc[4][MUON_BURST_LEN][MUONS_PER_BUF];
extern u16 muon_seq_num[MUON_BURST_LEN][MUONS_PER_BUF];
extern u16 muon_trig_tags[MUONS_PER_BUF];
extern u32 muon_burst_start[MUONS_PER_BUF];
extern u32 muon_buffer_start;
extern u32 muon_buffer_end;
extern int mu_word_count;

extern u32 *mem_addr, *mem_ptr;
extern u32 start_offset;
extern int toread_shwr_buf_num;
extern int toread_muon_buf_num;
extern int status;

extern XAxiCdma AxiCdmaInstance;	// Instance of the XAxiCdma 
extern XScuGic IntController;	// Instance of the Interrupt Controller
extern XScuGic_Config *IntCfgPtr;    // The configuration parameters of the controller
extern XAxiCdma_Config *DmaCfgPtr;

extern int DMA_Error;	/* Dma Error occurs */
extern int Shwr_Data_Read;
extern int Muon_Data_Read;
extern  int DMA_Done;

#ifdef SCATTER_GATHER
extern XAxiCdma_Bd BdTemplate;
extern XAxiCdma_Bd *BdPtr;
extern XAxiCdma_Bd *BdCurPtr;
extern int BdCount;
extern u8 *SrcBufferPtr;
extern int Index;
extern u32 bd_space[512] __attribute__((aligned(64)));;
#endif


  // Read muon memory buffers from PL memory into PS memory via DMA or PDT
  void read_muon_buffers()
  {
    int i;

    disable_trigger_intr();
    mu_word_count = read_trig(MUON_BUF_WORD_COUNT_ADDR);
    muon_buffer_start = read_trig(MUON_BUF_TIME_TAG_A_ADDR);
    muon_buffer_end = read_trig(MUON_BUF_TIME_TAG_B_ADDR);
    enable_trigger_intr();

#ifdef PDT

    // Muon buffer 0
    mem_addr=
      (u32*) TRIGGER_MEMORY_MUON0_BASE;
    mem_addr = mem_addr + toread_muon_buf_num * MUON_MEM_WORDS;
    mem_ptr = mem_addr;
    for (i=0; i<mu_word_count; i++)
      {
        muon_mem0[i] = *mem_ptr;
        mem_ptr++;
        if (mem_ptr >= mem_addr+MUON_MEM_WORDS)
          mem_ptr = mem_ptr-MUON_MEM_WORDS;
      }

    // Muon buffer 1
    mem_addr=
      (u32*) TRIGGER_MEMORY_MUON1_BASE;
    mem_addr = mem_addr + toread_muon_buf_num * MUON_MEM_WORDS;
    mem_ptr = mem_addr;
    for (i=0; i<mu_word_count; i++)
      {
        muon_mem1[i] = *mem_ptr;
        mem_ptr++;
        if (mem_ptr >= mem_addr+MUON_MEM_WORDS) 
          mem_ptr = mem_ptr-MUON_MEM_WORDS;
      }
#endif

    #if defined(SIMPLE) && defined(DMA)

    // Muon buffer 0
    mem_addr=
      (u32*) TRIGGER_MEMORY_MUON0_BASE;
    mem_addr = mem_addr + toread_muon_buf_num * MUON_MEM_WORDS;
    mem_ptr = mem_addr;
    status = do_simple_polled_dma(mem_ptr, muon_mem0, 4*mu_word_count);
    if (status != XST_SUCCESS) printf("Error doing simple polled DMA");
 
    // Muon buffer 1
    mem_addr=
      (u32*) TRIGGER_MEMORY_MUON1_BASE;
    mem_addr = mem_addr + toread_muon_buf_num * MUON_MEM_WORDS;
    mem_ptr = mem_addr;
    status = do_simple_polled_dma(mem_ptr, muon_mem1, 4*mu_word_count);
    if (status != XST_SUCCESS) printf("Error doing simple polled DMA");
#endif

#ifdef SCATTER_GATHER
    status = do_scatter_gather_polled_muon_dma();
#endif
  }

  // Unpack muon memory buffers 
  void unpack_muon_buffers()
  {
    int i, j, k;

    j = 0;
    k = 0;

    for (i=0; i<mu_word_count; i++)
      {
        // Burst time tag?
        if ((muon_mem0[i] & 0x80000000) != 0)
      {
        muon_burst_start[j] = muon_mem0[i] & 0x7fffffff;
        muon_trig_tags[j] = muon_mem1[i] & 0x7fffffff;
        j++;
        k = 0;
      } else {
        muon_adc[0][k][j-1] = muon_mem0[i] & ADC_MASK;
        muon_adc[1][k][j-1] = (muon_mem0[i]>>16) & ADC_MASK;
        muon_adc[2][k][j-1] = muon_mem1[i] & ADC_MASK;
        muon_adc[3][k][j-1] = (muon_mem1[i]>>16) & ADC_MASK;
        muon_seq_num[k][j-1] = ((muon_mem1[i]>>12) & 0xf) 
          | (((muon_mem1[i]>>28) & 0x7) << 4);
        k++;
        }
      }
  }

  void check_muon_buffers()
  {
    int i, j, nmuons, corrupt;

    nmuons = mu_word_count/(MUON_BURST_LEN);

   for (i=0; i<nmuons; i++)
      {
        corrupt = 0;
        for (j=0; j<MUON_BURST_LEN-1; j++)
          {
            if (muon_seq_num[j][i]-1 != j)
              corrupt = 1;
          }
        if (corrupt != 0)
          {   
          printf("trigger_test: Corrupt muon buffer for muon %d\n",i);
          j = 0;
            printf("Muon number %3d  expected sequence num= %2d  Actual seq. num = %d\n",
                   i, j+1, muon_seq_num[j][i]);
            j = MUON_BURST_LEN-3;
              printf("Muon number %3d  expected sequence num= %2d  Actual seq. num = %d\n",
                     i, j+1, muon_seq_num[j][i]);
              j = MUON_BURST_LEN-2;
                printf("Muon number %3d  expected sequence num= %2d  Actual seq. num = %d\n",
                       i, j+1, muon_seq_num[j][i]);
         }
      }
  }

void print_muon_buffers()
{
  int i, j, nmuons, dt;
  double fdt;

  nmuons = mu_word_count/(MUON_BURST_LEN);
  dt = muon_buffer_end - muon_buffer_start;
  if (dt < 0) dt = dt+0x80000000;
  fdt = dt * 8.3333 / 1.e9 / nmuons;
  printf("Trigger_test: mu_word_count = %d  nmuons = %d  ave. dt = %lf\n",
		  mu_word_count, nmuons, fdt);
  printf("\n>>>>>>>>>> BEGINNING OF MUON BUFFER >>>>>>>>>>\n");
  for (i=0; i<nmuons; i++)
    {
      for (j=0; j<MUON_BURST_LEN-1; j++) {
        if (i == 0)
          dt = muon_burst_start[i] - muon_buffer_start;
        else
          dt = muon_burst_start[i] - muon_burst_start[i-1];
        if (dt < 0) dt = dt+0x80000000;
        	fdt = dt * 8.3333 / 1.e9;

        printf("%3d %2d  %3x %3x %3x %3x %2x %lf\n",
               i, j+1, muon_adc[0][j][i], muon_adc[1][j][i],
               muon_adc[2][j][i],  muon_adc[3][j][i], 
               muon_trig_tags[i], fdt);
      }
    }
  printf("<<<<<<<<<< END OF MUON BUFFER <<<<<<<<<<\n\n");
}




