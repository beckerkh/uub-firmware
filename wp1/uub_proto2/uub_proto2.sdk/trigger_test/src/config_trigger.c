
// Function to configure the triggers according to information in the
// header files.
//
// 07-Feb-2017 DFN Original version

#include "trigger_test_options.h"
#include "trigger_test.h"

void config_trigger()
{

  int status;
#ifdef SHWR_TRIGGERS
  int sb_trig_enab, trigger_mask;
  int thr0, thr1, thr2;
  int shwr_status, toread_shwr_buf_num, cur_shwr_buf_num;
  int full_shwr_bufs, num_full;

#ifdef COMPAT_SB_TRIGGER
  int compat_sb_trig_enab;
#endif
#ifdef SB_TRIGGER
  int sb_trig_enab;
#endif
#ifdef LED_TRIGGER
  int led_trig_enab;
#endif
#endif

#ifdef MUON_TRIGGERS
  int muon_trig_enab;
  int muon_trigger_mask;
#endif
#ifdef DO_LED_PULSE
  int led_control, led_timer, led_pulsewid, led_delay;
  int seconds, prev_seconds, nanosec;
  double time, dt, prev_time;
#endif


#ifdef SHWR_TRIGGERS
  // Enable the shower triggers for this test
  // Set trigger thresholds
#ifdef COMPAT_SB_TRIGGER
  write_trig(COMPATIBILITY_SB_TRIG_THR0_ADDR,(int) TRIG_THR0);
  write_trig(COMPATIBILITY_SB_TRIG_THR1_ADDR,(int) TRIG_THR1);
  write_trig(COMPATIBILITY_SB_TRIG_THR2_ADDR,(int) TRIG_THR2);
  thr0 = read_trig(COMPATIBILITY_SB_TRIG_THR0_ADDR);
  thr1 = read_trig(COMPATIBILITY_SB_TRIG_THR1_ADDR);
  thr2 = read_trig(COMPATIBILITY_SB_TRIG_THR2_ADDR);
  if (thr0 != TRIG_THR0) 
    printf("trigger_test: Trigger threshold 0 error - wrote %d read %d\n",
	   TRIG_THR0,thr0);
  if (thr1 != TRIG_THR1) 
    printf("trigger_test: Trigger threshold 1 error - wrote %d read %d\n",
	   TRIG_THR1,thr1);
  if (thr2 != TRIG_THR2) 
    printf("trigger_test: Trigger threshold 2 error - wrote %d read %d\n",
	   TRIG_THR2,thr2);

  // Define which PMTs to include & coincidence level required
  compat_sb_trig_enab = COMPATIBILITY_SB_TRIG_INCL_PMT0 |
    COMPATIBILITY_SB_TRIG_INCL_PMT1 |
    COMPATIBILITY_SB_TRIG_INCL_PMT2 |
    (1 << COMPATIBILITY_SB_TRIG_COINC_LVL_SHIFT);
  write_trig(COMPATIBILITY_SB_TRIG_ENAB_ADDR, compat_sb_trig_enab);
  status = read_trig(COMPATIBILITY_SB_TRIG_ENAB_ADDR);
  if (status != sb_trig_enab)
    printf("trigger_test: Trigger enable error - wrote %x read %x\n", 
	   compat_sb_trig_enab, status);
#endif

#ifdef SB_TRIGGER
  write_trig(SB_TRIG_THR0_ADDR,(int) (TRIG_THR0));
  write_trig(SB_TRIG_THR1_ADDR,(int) (TRIG_THR1));
  write_trig(SB_TRIG_THR2_ADDR,(int) (TRIG_THR2));
  write_trig(SB_TRIG_SSD_ADDR,(int) (TRIG_SSD));
  thr0 = read_trig(SB_TRIG_THR0_ADDR);
  thr1 = read_trig(SB_TRIG_THR1_ADDR);
  thr2 = read_trig(SB_TRIG_THR2_ADDR);
  if (thr0 != TRIG_THR0) 
    printf("trigger_test: Trigger threshold 0 error - wrote %d read %d\n",
	   TRIG_THR0,thr0);
  if (thr1 != TRIG_THR1) 
    printf("trigger_test: Trigger threshold 1 error - wrote %d read %d\n",
	   TRIG_THR1,thr1);
  if (thr2 != TRIG_THR2) 
    printf("trigger_test: Trigger threshold 2 error - wrote %d read %d\n",
	   TRIG_THR2,thr2);

  // Define which PMTs to include & coincidence level required
  sb_trig_enab =  SB_TRIG_INCL_PMT0 |
    SB_TRIG_INCL_PMT1 |
    SB_TRIG_INCL_PMT2 |
    SB_TRIG_INCL_SSD |
    (1 << SB_TRIG_COINC_LVL_SHIFT) |
    (0 << SB_TRIG_WCD_DELAY_SHIFT) |
    (1 << SB_TRIG_SSD_DELAY_SHIFT) |
    (3 << SB_TRIG_COINC_OVLP_SHIFT) |
    (1 << SB_TRIG_CONSEC_BINS_SHIFT);
  write_trig(SB_TRIG_ENAB_ADDR, sb_trig_enab);
  status = read_trig(SB_TRIG_ENAB_ADDR);
  if (status != sb_trig_enab)
    printf("trigger_test: Error writing SB_TRIG_ENAB, wrote %x read %x\n",
	   sb_trig_enab, status);
#endif

  trigger_mask = 0;
#ifdef SB_TRIGGER
  trigger_mask = trigger_mask | SHWR_BUF_TRIG_SB;
#endif
#ifdef COMPAT_SB_TRIGGER
  trigger_mask = trigger_mask | COMPATIBILITY_SHWR_BUF_TRIG_SB;
#endif
#ifdef PRESCALE_COMPAT_SB_TRIG
  trigger_mask = trigger_mask | COMPAT_PRESCALE_SHWR_BUF_TRIG_SB;
#endif
#ifdef EXT_TRIGGER
  trigger_mask = trigger_mask |  COMPATIBILITY_SHWR_BUF_TRIG_EXT;
#endif
#ifdef PRESCALE_EXT_TRIGGER
  trigger_mask = trigger_mask |  COMPAT_PRESCALE_SHWR_BUF_TRIG_EXT;
#endif
#ifdef LED_TRIGGER
  trigger_mask = trigger_mask |  SHWR_BUF_TRIG_LED;
#endif

  printf("Trigger_test: Enabled triggers = ");
  if ((trigger_mask & SHWR_BUF_TRIG_SB) != 0) 
    printf(" SB");
  if ((trigger_mask & COMPATIBILITY_SHWR_BUF_TRIG_SB) != 0) 
    printf(" COMPAT_SB");
  if ((trigger_mask & COMPATIBILITY_SHWR_BUF_TRIG_EXT) != 0) 
    printf(" EXT");
  if ((trigger_mask & COMPAT_PRESCALE_SHWR_BUF_TRIG_SB) != 0) 
    printf(" PRESCALE_SB");
  if ((trigger_mask & COMPAT_PRESCALE_SHWR_BUF_TRIG_EXT) != 0) 
    printf(" PRESCALE_EXT");
  if ((trigger_mask & SHWR_BUF_TRIG_LED) != 0)
    printf(" LED");
  printf("\n");
  printf("Trigger_test: Shower trigger thresholds = %d = 0x%x\n",
	 (int) (TRIG_THR0), (int) (TRIG_THR0));

  // Flush any stale shower buffers
  shwr_status = read_trig(SHWR_BUF_STATUS_ADDR);
  while ((SHWR_INTR_PEND_MASK & (shwr_status >> SHWR_INTR_PEND_SHIFT)) != 0)
    {
      toread_shwr_buf_num = SHWR_BUF_RNUM_MASK & 
	(shwr_status >> SHWR_BUF_RNUM_SHIFT);
      cur_shwr_buf_num = SHWR_BUF_WNUM_MASK & 
	(shwr_status >> SHWR_BUF_WNUM_SHIFT);
      full_shwr_bufs = SHWR_BUF_FULL_MASK & 
	(shwr_status >> SHWR_BUF_FULL_SHIFT);
      num_full = 0x7 & (shwr_status >> SHWR_BUF_NFULL_SHIFT);
      printf("Shower buf writing %d  to read %d  full %x  num full=%d\n",
	     cur_shwr_buf_num,toread_shwr_buf_num,
	     full_shwr_bufs,num_full);
      write_trig(SHWR_BUF_CONTROL_ADDR, toread_shwr_buf_num);
      shwr_status = read_trig(SHWR_BUF_STATUS_ADDR);
    }
  write_trig(SHWR_BUF_TRIG_MASK_ADDR, trigger_mask);
  status = read_trig(SHWR_BUF_TRIG_MASK_ADDR);
  if (status != trigger_mask) 
    printf("trigger_test: Error setting trigger mask, wrote %x read %x\n",
	   trigger_mask, status);

#else
  write_trig(SHWR_BUF_TRIG_MASK_ADDR, 0);
  printf("Trigger_test: Shower trigger not enabled\n");
#endif

#ifdef MUON_TRIGGERS
  // Enable the muon triggers for this test
  // Set trigger thresholds
  write_trig(MUON_TRIG1_THR0_ADDR,(int) (TRIG_THR0));
  write_trig(MUON_TRIG1_THR1_ADDR,(int) (TRIG_THR1));
  write_trig(MUON_TRIG1_THR2_ADDR,(int) (TRIG_THR2));
  write_trig(MUON_TRIG1_SSD_ADDR,(int) (TRIG_SSD));
 
  // Define which PMTs to include & coincidence level required
  muon_trig_enab =  MUON_TRIG_INCL_PMT0 |
    //    MUON_TRIG_INCL_PMT1 |
    //    MUON_TRIG_INCL_PMT2 |
    //    MUON_TRIG_INCL_SSD |
    (1 << MUON_TRIG_COINC_LVL_SHIFT) |
    (1 << MUON_TRIG_SSD_DELAY_SHIFT) |
    (0 << MUON_TRIG_COINC_OVLP_SHIFT) |
    (0 << MUON_TRIG_CONSEC_BINS_SHIFT);
  write_trig(MUON_TRIG1_ENAB_ADDR, muon_trig_enab);

  //muon_trigger_mask =  MUON_BUF_TRIG_SB1 | MUON_BUF_TRIG_EXT;
  muon_trigger_mask =  MUON_BUF_TRIG_SB1;
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
#else
  write_trig(MUON_TRIG1_ENAB_ADDR, 0);
  printf("Trigger_test: Muon trigger not enabled\n");
#endif
}
