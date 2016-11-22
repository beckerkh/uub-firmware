// uub_sipm_cal

// Test program to get muon buffers directly by reading the buffers.
// from the programmable logic and produce an output file that can
// be used to check the calibration histograms.

// 20-Nov-2016 DFN Original version

#include "uub_sipm_cal.h"

volatile u32 *muon_buf0;
volatile u32 *muon_buf1;
volatile u32 *regs;

u32 muon_mem0[MUON_MEM_WORDS];
u32 muon_mem1[MUON_MEM_WORDS];
u16 muon_adc[4][MUON_BURST_LEN][MUONS_PER_BUF];
u16 muon_seq_num[MUON_BURST_LEN][MUONS_PER_BUF];
u16 muon_trig_tags[MUONS_PER_BUF];
u32 muon_burst_start[MUONS_PER_BUF];
u32 muon_buffer_start;
u32 muon_buffer_end;
int mu_word_count;
int toread_muon_buf_num;

int cur_muon_buf_num = 0;
int full_muon_bufs = 0;
int cntrl_word = 0;
int num_full;
int muon_status;
int muon_trig_enab;
int muon_trigger_mask;

main()
{
  uint32_t v1, v2;
  int flag, n, i, j;

  // Map registers & buffers
  map_regs();

  // Reset trigger
  write_trig(COMPATIBILITY_GLOBAL_CONTROL_ADDR,1);
  // Disable all shower triggers
  write_trig(SHWR_BUF_TRIG_MASK_ADDR,0);

  // Enable the muon triggers for this test
  // Set trigger thresholds
  write_trig(MUON_TRIG1_THR0_ADDR,(int) (TRIG_THR0));
  write_trig(MUON_TRIG1_THR1_ADDR,(int) (TRIG_THR1));
  write_trig(MUON_TRIG1_THR2_ADDR,(int) (TRIG_THR2));
  write_trig(MUON_TRIG1_SSD_ADDR,(int) (TRIG_SSD));
 
  // Define which PMTs to include & coincidence level required
  muon_trig_enab =  MUON_TRIG_INCL_PMT1 |
    (1 << MUON_TRIG_COINC_LVL_SHIFT) |
    (1 << MUON_TRIG_SSD_DELAY_SHIFT) |
    (0 << MUON_TRIG_COINC_OVLP_SHIFT) |
    (0 << MUON_TRIG_CONSEC_BINS_SHIFT);
  write_trig(MUON_TRIG1_ENAB_ADDR, muon_trig_enab);

  muon_trigger_mask =  MUON_BUF_TRIG_SB1 | MUON_BUF_SIPM_CAL;
  printf("Trigger_test: Enabled muon triggers = ");
  if ((muon_trigger_mask & MUON_BUF_TRIG_SB1) != 0) 
    printf(" SB1");
  if ((muon_trigger_mask & MUON_BUF_TRIG_SB2) != 0) 
    printf(" SB2");
  if ((muon_trigger_mask & MUON_BUF_TRIG_SB3) != 0) 
    printf(" SB3");
  if ((muon_trigger_mask & MUON_BUF_TRIG_SB4) != 0) 
    printf(" SB4");
  if ((muon_trigger_mask & MUON_BUF_TRIG_EXT) != 0) 
    printf(" EXT");
  if ((muon_trigger_mask & MUON_BUF_SIPM_CAL) != 0) 
    printf(" SIPM_CAL");
  printf("\n");
  printf("Trigger_test: Muon trigger thresholds = %d\n",(int) (TRIG_THR0));

  // Flush any stale muon buffers
  muon_status = read_trig(MUON_BUF_STATUS_ADDR);
  while ((MUON_INTR_PEND_MASK & (muon_status >> MUON_INTR_PEND_SHIFT)) != 0)
    {
      toread_muon_buf_num = MUON_BUF_RNUM_MASK & 
        (muon_status >> MUON_BUF_RNUM_SHIFT);
      write_trig(MUON_BUF_CONTROL_ADDR, toread_muon_buf_num);
      muon_status = read_trig(MUON_BUF_STATUS_ADDR);
    }

  write_trig(MUON_BUF_TRIG_MASK_ADDR, muon_trigger_mask);

  // Loop getting fixed number of buffers
  n=0;
  while (n < MAX_BUF)
    {
    muon_status = read_trig(MUON_BUF_STATUS_ADDR);
    if ((MUON_INTR_PEND_MASK & (muon_status >> MUON_INTR_PEND_SHIFT)) != 0)
      {
        toread_muon_buf_num = MUON_BUF_RNUM_MASK & 
          (muon_status >> MUON_BUF_RNUM_SHIFT);
        cur_muon_buf_num = MUON_BUF_WNUM_MASK & 
          (muon_status >> MUON_BUF_WNUM_SHIFT);
        full_muon_bufs = MUON_BUF_FULL_MASK & 
          (muon_status >> MUON_BUF_FULL_SHIFT);
        num_full = 0x7 & (muon_status >> MUON_BUF_NFULL_SHIFT);
        printf("Muon buf writing %d  to read %d  full %x  num full=%d\n",
               cur_muon_buf_num,toread_muon_buf_num,
               full_muon_bufs,num_full);
 
        // Do readout of buffer here ....
        read_muon_buffers();  // Read buffers to local memory

        // Reset full flag
        write_trig(MUON_BUF_CONTROL_ADDR,toread_muon_buf_num);

        unpack_muon_buffers(); // Unpack the buffers
        check_muon_buffers();  // Do sanity check of shower buffers
        print_muon_buffers();  // Print out the buffer

	muon_status = read_trig(MUON_BUF_STATUS_ADDR);
	n++;
      }
  }
} 
