// Utility routines for reading, unpacking, checking shower buffers.
//
// 25-May-2016 DFN Initial version extracted from trigger_test.c

#include "trigger_test.h"

// Shower memory buffers
extern u32 shw_mem0[SHWR_MEM_WORDS];
extern u32 shw_mem1[SHWR_MEM_WORDS];
extern u32 shw_mem2[SHWR_MEM_WORDS];
extern u32 shw_mem3[SHWR_MEM_WORDS];
extern u32 shw_mem4[SHWR_MEM_WORDS];

// ADC traces & extra bits
extern u16 adc[10][SHWR_MEM_WORDS];
extern u16 filt_adc[3][SHWR_MEM_WORDS];
extern u8 flags[SHWR_MEM_WORDS];

extern u32 *mem_addr, *mem_ptr;
extern u32 start_offset;
extern int toread_shwr_buf_num;
extern int status;

extern XAxiCdma AxiCdmaInstance;	// Instance of the XAxiCdma 
extern XScuGic IntController;	// Instance of the Interrupt Controller
extern XScuGic_Config *IntCfgPtr;    // The configuration parameters of the controller
extern XAxiCdma_Config *DmaCfgPtr;

extern int DMA_Error;	/* Dma Error occurs */
extern int Shwr_Data_Read;
extern int DMA_Done;

#ifdef SCATTER_GATHER
extern XAxiCdma_Bd BdTemplate;
extern XAxiCdma_Bd *BdPtr;
extern XAxiCdma_Bd *BdCurPtr;
extern int BdCount;
extern u8 *SrcBufferPtr;
extern int Index;
extern u32 bd_space[512] __attribute__((aligned(64)));;
#endif

static double prev_time = 0;

  // Read shower memory buffers from PL memory into PS memory
  void read_shw_buffers()
  {
    int trig_id, pps_tics;
    int seconds, tics, delta_tics;
    double time, dt;
#ifdef PDT
    int i;
#endif

    pps_tics = read_ttag(TTAG_SHWR_PPS_TICS_ADDR);
    seconds = read_ttag(TTAG_SHWR_SECONDS_ADDR);
    tics = read_ttag(TTAG_SHWR_TICS_ADDR);

    pps_tics = pps_tics & TTAG_TICS_MASK;
    seconds = seconds & TTAG_SECONDS_MASK;
    tics = tics & TTAG_TICS_MASK;

    delta_tics = tics-pps_tics;
    if (delta_tics < 0) delta_tics = delta_tics + TTAG_TICS_MASK +1;
    printf("pps_tics=%d seconds=%d tics=%d delta_tics=%d\n",
	   pps_tics, seconds, tics, delta_tics);

    // Does not yet account for rollover of seconds
    time = (double) seconds + 8.3333 * (double) delta_tics / 1.e9;
    dt = time - prev_time;
    prev_time = time;

    trig_id = read_trig(SHWR_BUF_TRIG_ID_ADDR);
    printf("trigger_test: Trigger ID = %x ==", trig_id);
    if ((trig_id & SHWR_BUF_TRIG_SB) != 0)
      printf(" SB");
    if ((trig_id & COMPATIBILITY_SHWR_BUF_TRIG_SB) != 0)
      printf(" COMPAT_SB");
    if ((trig_id & COMPATIBILITY_SHWR_BUF_TRIG_EXT) != 0)
      printf(" EXT");
    if ((trig_id & (SHWR_BUF_TRIG_SB<<8)) != 0)
      printf(" SB_DLYD");
    if ((trig_id & (COMPATIBILITY_SHWR_BUF_TRIG_SB<<8)) != 0)
      printf(" COMPAT_SB_DLYD");
    if ((trig_id & (COMPATIBILITY_SHWR_BUF_TRIG_EXT<<8)) != 0)
      printf(" EXT");
    printf(" T = %lf  DT = %lf", time, dt);
    printf("\n");

    start_offset = read_trig(SHWR_BUF_START_ADDR);

#ifdef PDT

    // Shower buffer 0
    mem_addr = (u32*) TRIGGER_MEMORY_SHWR0_BASE;
    mem_addr = mem_addr + toread_shwr_buf_num * SHWR_MEM_WORDS;
    mem_ptr = mem_addr;
    for (i=0; i<SHWR_MEM_WORDS; i++)
      {
        shw_mem0[i] = *mem_ptr;
        mem_ptr++;
        if (mem_ptr >= mem_addr+SHWR_MEM_WORDS)
          mem_ptr = mem_ptr-SHWR_MEM_WORDS;
      }

    // Shower buffer 1
    mem_addr = (u32*) TRIGGER_MEMORY_SHWR1_BASE;
    mem_addr = mem_addr + toread_shwr_buf_num * SHWR_MEM_WORDS;
    mem_ptr = mem_addr;
    for (i=0; i<SHWR_MEM_WORDS; i++)
      {
        shw_mem1[i] = *mem_ptr;
        mem_ptr++;
        if (mem_ptr >= mem_addr+SHWR_MEM_WORDS) 
          mem_ptr = mem_ptr-SHWR_MEM_WORDS;
      }

    // Shower buffer 2
    mem_addr = (u32*) TRIGGER_MEMORY_SHWR2_BASE;
    mem_addr = mem_addr + toread_shwr_buf_num * SHWR_MEM_WORDS;
    mem_ptr = mem_addr;
    for (i=0; i<SHWR_MEM_WORDS; i++)
      {
        shw_mem2[i] = *mem_ptr;
        mem_ptr++;
        if (mem_ptr >= mem_addr+SHWR_MEM_WORDS) 
          mem_ptr = mem_ptr-SHWR_MEM_WORDS;
      }

    // Shower buffer 3
    mem_addr = (u32*) TRIGGER_MEMORY_SHWR3_BASE;
    mem_addr = mem_addr + toread_shwr_buf_num * SHWR_MEM_WORDS;
    mem_ptr = mem_addr;
    for (i=0; i<SHWR_MEM_WORDS; i++)
      {
        shw_mem3[i] = *mem_ptr;
        mem_ptr++;
        if (mem_ptr >= mem_addr+SHWR_MEM_WORDS) 
          mem_ptr = mem_ptr-SHWR_MEM_WORDS;
      }

    // Shower buffer 4
    mem_addr = (u32*) TRIGGER_MEMORY_SHWR4_BASE;
    mem_addr = mem_addr + toread_shwr_buf_num * SHWR_MEM_WORDS;
    mem_ptr = mem_addr;
    for (i=0; i<SHWR_MEM_WORDS; i++)
      {
        shw_mem4[i] = *mem_ptr;
        mem_ptr++;
        if (mem_ptr >= mem_addr+SHWR_MEM_WORDS) 
          mem_ptr = mem_ptr-SHWR_MEM_WORDS;
      }
#endif

#if defined(SIMPLE) && defined(DMA)

    // Shower buffer 0
    mem_addr = (u32*) TRIGGER_MEMORY_SHWR0_BASE;
    mem_addr = mem_addr + toread_shwr_buf_num * SHWR_MEM_WORDS;
    mem_ptr = mem_addr;
    status = do_simple_polled_dma(mem_ptr, shw_mem0, 4*SHWR_MEM_WORDS);
    if (status != XST_SUCCESS) printf("Error doing simple polled DMA");
 
    // Shower buffer 1
    mem_addr = (u32*) TRIGGER_MEMORY_SHWR1_BASE;
    mem_addr = mem_addr + toread_shwr_buf_num * SHWR_MEM_WORDS;
    mem_ptr = mem_addr;
    status = do_simple_polled_dma(mem_ptr, shw_mem1, 4*SHWR_MEM_WORDS);
    if (status != XST_SUCCESS) printf("Error doing simple polled DMA");

    // Shower buffer 2
    mem_addr = (u32*) TRIGGER_MEMORY_SHWR2_BASE;
    mem_addr = mem_addr + toread_shwr_buf_num * SHWR_MEM_WORDS;
    mem_ptr = mem_addr;
    status = do_simple_polled_dma(mem_ptr, shw_mem2, 4*SHWR_MEM_WORDS);
    if (status != XST_SUCCESS) printf("Error doing simple polled DMA");

    // Shower buffer 3
    mem_addr = (u32*) TRIGGER_MEMORY_SHWR3_BASE;
    mem_addr = mem_addr + toread_shwr_buf_num * SHWR_MEM_WORDS;
    mem_ptr = mem_addr;
    status = do_simple_polled_dma(mem_ptr, shw_mem3, 4*SHWR_MEM_WORDS);
    if (status != XST_SUCCESS) printf("Error doing simple polled DMA");

    // Shower buffer 4
    mem_addr = (u32*) TRIGGER_MEMORY_SHWR4_BASE;
    mem_addr = mem_addr + toread_shwr_buf_num * SHWR_MEM_WORDS;
    mem_ptr = mem_addr;
    status = do_simple_polled_dma(mem_ptr, shw_mem4, 4*SHWR_MEM_WORDS);
    if (status != XST_SUCCESS) printf("Error doing simple polled DMA");
#endif  

#ifdef SCATTER_GATHER
    status = do_scatter_gather_polled_shwr_dma();
#endif

  }

  // Unpack shower memory buffers 
  void unpack_shw_buffers()
  {
    int i, j;
    int filt0, filt1, filt2;

    for (i=0; i<SHWR_MEM_WORDS; i++)
      {
        j = i + start_offset;
        if (j >= SHWR_MEM_WORDS) j = j - SHWR_MEM_WORDS;

        adc[0][i] = shw_mem0[j] & ADC_MASK;
        adc[1][i] = (shw_mem0[j] >> 16) & ADC_MASK;

        adc[2][i] = shw_mem1[j] & ADC_MASK;
        adc[3][i] = (shw_mem1[j] >> 16) & ADC_MASK;

        adc[4][i] = shw_mem2[j] & ADC_MASK;
        adc[5][i] = (shw_mem2[j] >> 16) & ADC_MASK;

        adc[6][i] = shw_mem3[j] & ADC_MASK;
        adc[7][i] = (shw_mem3[j] >> 16) & ADC_MASK;

        adc[8][i] = shw_mem4[j] & ADC_MASK;
        adc[9][i] = (shw_mem4[j] >> 16) & ADC_MASK;

        flags[i] = (shw_mem4[j] >> (ADC_WIDTH+16)) & 0xf;

        filt0 = (shw_mem0[j] >> (ADC_WIDTH   )) & 0xf;
        filt1 = (shw_mem0[j] >> (ADC_WIDTH+16)) & 0xf;
        filt2 = (shw_mem1[j] >> (ADC_WIDTH   )) & 0xf;
        filt_adc[0][i] = filt0 + 16*(filt1 + 16*filt2);
        
        filt0 = (shw_mem1[j] >> (ADC_WIDTH+16)) & 0xf;
        filt1 = (shw_mem2[j] >> (ADC_WIDTH   )) & 0xf;
        filt2 = (shw_mem2[j] >> (ADC_WIDTH+16)) & 0xf;
        filt_adc[1][i] = filt0 + 16*(filt1 + 16*filt2);

        filt0 = (shw_mem3[j] >> (ADC_WIDTH   )) & 0xf;
        filt1 = (shw_mem3[j] >> (ADC_WIDTH+16)) & 0xf;
        filt2 = (shw_mem4[j] >> (ADC_WIDTH   )) & 0xf;
        filt_adc[2][i] = filt0 + 16*(filt1 + 16*filt2);
        
      }
  }

void check_shw_buffers()
{

/*   int i, corrupt, trig; */
/* #ifdef RAMP */
/*   int j; */
/* #endif */

/*   corrupt = 0; */
/*   for (i=1; i<SHWR_MEM_WORDS; i++) */
/*     { */
/*       if (flags[i] >= 15) corrupt = 1; */
/*       if (flags[i] == 0) */
/* 	{ */
/* 	  if (flags[i-1] != 14)  */
/* 	    corrupt = 1; */
/* 	} else { */
/* 	if (flags[i] != flags[i-1]+1)  */
/* 	  corrupt = 1; */
/*       } */
/*       if (corrupt != 0) */
/* 	{    */
/* 	  printf("trigger_test: Corrupt buffer: time bin %x  flags=%x %x\n", */
/* 		 i,flags[i],flags[i-1]); */
/* 	  return; */
/*         } */
/*     } */

/* #ifdef RAMP */
/*   for (i=1; i<SHWR_MEM_WORDS; i++) */
/*     { */
/*       for (j=0; j<10; j++) */
/* 	{ */
/* 	  if ((adc[j][i]+1 != adc[j][i-1]) && (adc[j][i] != 0xfff)) */
/* 	    { */
/* 	      printf("trigger_test: Corrupted value ADC %d  @time bin %d = %x", */
/* 		     j,i-1,adc[j][i-1]); */
/* 	      printf("  @time bin %d = %x\n",i,adc[j][i]); */
/* 	    } */
/* 	} */
/*     } */
/* #endif */
}

  void print_shw_buffers()
  {
    int i, trig, first, last, trig2;


  trig = 0;
  for (i=0; i<SHWR_MEM_WORDS; i++) {
    if (trig == 0) {
      if ((filt_adc[0][i] > TRIG_THR0) || (filt_adc[1][i] > TRIG_THR1) ||
	  (filt_adc[1][i] > TRIG_THR2)) {
	trig = i;
	printf("trigger_test: Event should trigger at bin %d = 0x%x\n",trig,trig);
      }
    }
  }
  if (trig == 0) 
    printf("trigger_test: Event should not have triggered\n");

  trig2 = 0;
  for (i=0; i<SHWR_MEM_WORDS; i++) {
    if (trig2 == 0) {
      if (flags[i] != 0) {
	trig2 = i-7;
	printf("trigger_test: Event triggered at bin %d = 0x%x\n",trig2,trig2);
      }
    }
  }
  if (trig2 == 0) 
    printf("trigger_test: Can't find trigger point\n");


    printf("\n>>>>>>>>>> BEGINNING OF EVENT >>>>>>>>>>\n");

    // Inconsistent trigger
    if (abs(trig - trig2) > 20) {
      first = trig2-5;
      if (first < 0) first = 0;
      last = trig2+20;
      if (last > SHWR_MEM_WORDS) last = SHWR_MEM_WORDS;
     
    //      for (i=0; i<SHWR_MEM_WORDS; i++)
      for (i=first; i<last; i++)
      {
        printf("%3x %x %3x %3x %3x %3x %3x %3x %3x %3x %3x %3x %3x %3x %3x\n",
               i, flags[i], adc[0][i], adc[1][i], adc[2][i], 
               adc[3][i], adc[4][i], 
               adc[5][i], adc[6][i], adc[7][i], adc[8][i], adc[9][i],
               filt_adc[0][i], filt_adc[1][i], filt_adc[2][i]);
      }
	printf("...\n");
    }
 
    // Normal trigger
   if (trig != 0) {
// Select a portion of event to print
//      first = trig2-5;
      first = trig2-100;
      if (first < 0) first = 0;
//      last = trig2+20;
      last = trig2+200;
      if (last > SHWR_MEM_WORDS) last = SHWR_MEM_WORDS;
    } 
    //      for (i=0; i<SHWR_MEM_WORDS; i++)  // Whole event
          for (i=first; i<last; i++)  // Portion of an event
    //    for (i=trig2; i<trig2; i++)
      {
        printf("%3x %x %3x %3x %3x %3x %3x %3x %3x %3x %3x %3x %3x %3x %3x\n",
               i, flags[i], adc[0][i], adc[1][i], adc[2][i], 
               adc[3][i], adc[4][i], 
               adc[5][i], adc[6][i], adc[7][i], adc[8][i], adc[9][i],
               filt_adc[0][i], filt_adc[1][i], filt_adc[2][i]);
      }
    printf("<<<<<<<<<< END OF EVENT <<<<<<<<<<\n\n");
  }





