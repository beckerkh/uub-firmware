
// Simple standalone test routine to interact with the sde_trigger PL.
//
// 26-Feb-2016 DFN Initial (reconstructed from lost code) version
//                 The initial version does not test interrupts - 
//                 to be restored later
// 11-Apr-2016 DFN Add simple polled DMA
// 29-Apr-2016 DFN Implement TRIGGER_INTERRUPT

#include "trigger_test.h"

// Shower memory buffers
u32 shw_mem0[SHWR_MEM_WORDS] __attribute__((aligned(64)));
u32 shw_mem1[SHWR_MEM_WORDS] __attribute__((aligned(64)));
u32 shw_mem2[SHWR_MEM_WORDS] __attribute__((aligned(64)));
u32 shw_mem3[SHWR_MEM_WORDS] __attribute__((aligned(64)));
u32 shw_mem4[SHWR_MEM_WORDS] __attribute__((aligned(64)));

// ADC traces & extra bits
u16 adc[10][SHWR_MEM_WORDS];
u16 filt_adc[3][SHWR_MEM_WORDS];
u8 flags[SHWR_MEM_WORDS];

// Muon memory buffers
u32 muon_mem0[MUON_MEM_WORDS] __attribute__((aligned(64)));
u32 muon_mem1[MUON_MEM_WORDS] __attribute__((aligned(64)));
u16 muon_adc[4][MUON_BURST_LEN][MUONS_PER_BUF];
u16 muon_seq_num[MUON_BURST_LEN][MUONS_PER_BUF];
u16 muon_trig_tags[MUONS_PER_BUF];
u32 muon_burst_start[MUONS_PER_BUF];
u32 muon_buffer_start;
u32 muon_buffer_end;
int mu_word_count;

u32 *mem_addr, *mem_ptr;
u32 start_offset;
int toread_shwr_buf_num;
int toread_muon_buf_num;
int status;

static XAxiCdma AxiCdmaInstance;	// Instance of the XAxiCdma 
static  XAxiCdma_Config *DmaCfgPtr;

#ifdef TRIGGER_INTERRUPT
static XScuGic IntController;	// Instance of the Interrupt Controller
static XScuGic_Config *IntCfgPtr;  // The config. parameters of the controller
#endif

volatile static int DMA_Error = 0;	/* Dma Error occurs */
volatile static int Shwr_Data_Read = 0;
volatile static int Muon_Data_Read = 0;
volatile static int DMA_Done = 0;

#ifdef SCATTER_GATHER
XAxiCdma_Bd BdTemplate;
XAxiCdma_Bd *BdPtr;
XAxiCdma_Bd *BdCurPtr;
int BdCount;
u8 *SrcBufferPtr;
int Index;

// bd_space should be an exact multiple of BD size for both shower & muon
// buffer transfers, and should be aligned on 64 byte boundary.  
#define ONE_BD_LEN 16
#define BD_SPACE_LEN ONE_BD_LEN*NUM_SHWR_BDS_TO_TRANSFER*NUM_MUON_BDS_TO_TRANSFER
u32 bd_space[BD_SPACE_LEN] __attribute__((aligned(64)));
#endif

void trigger_test()
{
#ifdef TRIGGER_POLLED
  int cur_shwr_buf_num = 0;
  int full_shwr_bufs = 0;
  int cur_muon_buf_num = 0;
  int full_muon_bufs = 0;
  int cntrl_word = 0;
  int num_full;
  int shwr_status, muon_status;
#endif
  int id;

#ifdef SHWR_TRIGGERS
  int trigger_mask;
  #ifdef COMPAT_SB_TRIGGER
    int compat_sb_trig_enab;
  #endif
  #ifdef SB_TRIGGER
    int sb_trig_enab;
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

  // Select any special test options.
  int test_options = 0;

  // Select fake or true GPS for 1pps
#ifdef USE_FAKE_GPS
  test_options = 1;
#endif

  // Select fake or true signals
#ifdef USE_FAKE_SIGNAL
  test_options = test_options | 2;
#endif
  INTERFACE_UUB_DFN3_mWriteReg(XPAR_INTERFACE_UUB_DFN3_0_S00_AXI_BASEADDR,
                               INTERFACE_UUB_DFN3_S00_AXI_SLV_REG2_OFFSET,
			       test_options);

  // Set up trigger

  // Check for sane ID
  id = read_trig(ID_REG_ADDR);
  printf("trigger_test: id=%x\n",id);

  // Reset trigger
  write_trig(COMPATIBILITY_GLOBAL_CONTROL_ADDR,1);

  // Disable all shower triggers
  write_trig(SHWR_BUF_TRIG_MASK_ADDR,0);
  status = read_trig(SHWR_BUF_TRIG_MASK_ADDR);
  if (status != 0) printf("trigger_test: Error reading TRIG_MASK %x\n",status);

#ifdef DMA
  // Initialize the XAxiCdma device.
  DmaCfgPtr = XAxiCdma_LookupConfig(XPAR_ZYNC_BLOCK_AXI_CDMA_0_DEVICE_ID);
  if (status != XST_SUCCESS)
    {
      printf("trigger_test: Failed to initialize XAxiCdma device.\n");
      return;
    }
  
  status = XAxiCdma_CfgInitialize(&AxiCdmaInstance, DmaCfgPtr,
                                  DmaCfgPtr->BaseAddress);
  if (status != XST_SUCCESS) 
    {
      printf("trigger_test: Failed to initialize XAxiCdma device.\n");
      return;
    }

#ifdef SCATTER_GATHER
  // Set up BD ring
  BdCount = XAxiCdma_BdRingCntCalc(XAXICDMA_BD_MINIMUM_ALIGNMENT,
                                   sizeof(bd_space),(unsigned int) bd_space);

  status = XAxiCdma_BdRingCreate(&AxiCdmaInstance, (unsigned int) bd_space,
                                 (unsigned int) bd_space,
                                 XAXICDMA_BD_MINIMUM_ALIGNMENT, BdCount);
  if (status != XST_SUCCESS) {
    printf("trigger_test: Create BD ring failed %d\r\n",status);
    return;
  }

  // Setup a BD template to copy to every BD.
  XAxiCdma_BdClear(&BdTemplate);
  status = XAxiCdma_BdRingClone(&AxiCdmaInstance, &BdTemplate);
  if (status != XST_SUCCESS) {
    printf("trigger_test: Clone BD ring failed %d\r\n",status);
    return;
  }
#endif

  // Set up interrupt system
#ifndef DMA_INTERRUPT
  // Disable DMA interrupts, we use polling mode for DMA completion
  XAxiCdma_IntrDisable(&AxiCdmaInstance, XAXICDMA_XR_IRQ_ALL_MASK);
#endif

#ifdef TRIGGER_INTERRUPT
  // Initialize the interrupt controller driver so that it is ready to use.
  IntCfgPtr = XScuGic_LookupConfig(XPAR_SCUGIC_0_DEVICE_ID);
  if (NULL == IntCfgPtr) 
    {
      printf("trigger_test: Failed to initialize interrupt controller\n");
      return;
    }
  status = XScuGic_CfgInitialize(&IntController, IntCfgPtr,
				 IntCfgPtr->CpuBaseAddress);
  if (status != XST_SUCCESS)
    {
      printf("trigger_test: Failed to initialize interrupt controller\n");
      return;
    }

  // Initialize the exception table
  Xil_ExceptionInit();

  // Connect the interrupt controller interrupt handler to the hardware
  // interrupt handling logic in the ARM processor.
  Xil_ExceptionRegisterHandler(XIL_EXCEPTION_ID_INT,
                               (Xil_ExceptionHandler) XScuGic_InterruptHandler,
                               (void *)&IntController);

  // Enable interrupts in the ARM
  Xil_ExceptionEnable();

  //	 Connect a device driver handler that will be called when an
  //	 interrupt for the device occurs, the device driver handler performs
  //	 the specific interrupt processing for the device  
  status = 
    XScuGic_Connect(&IntController,
                    XPAR_FABRIC_TRIGGER_MEMORY_BLOCK_SDE_TRIGGER_0_IRQ_INTR,
                    (Xil_ExceptionHandler)sde_trigger_handler,
                    (void *)&IntController);
  if (status != XST_SUCCESS)
    {
      printf("trigger_test: Failed to connect to sde_trigger_handler\n");
      return;
    }

  // Enable interrupts from the sde_trigger module.
  SDE_TRIGGER_EnableInterrupts((int *) SDE_TRIGGER_INTR_BASE, 
                               (1<<SHWR_TRIGGER_INTR_BIT) |
                               (1<<MUON_TRIGGER_INTR_BIT));
	
  // Enable the interrupt from the trigger at the interrupt controller
  XScuGic_Enable(&IntController,
		 XPAR_FABRIC_TRIGGER_MEMORY_BLOCK_SDE_TRIGGER_0_IRQ_INTR); 

#endif  //TRIGGER_INTERRUPT
#endif  //DMA

#ifdef SHWR_TRIGGERS
  // Enable the shower triggers for this test
  // Set trigger thresholds
  write_trig(COMPATIBILITY_SB_TRIG_THR0_ADDR,(int) TRIG_THR0);
  write_trig(COMPATIBILITY_SB_TRIG_THR1_ADDR,(int) TRIG_THR1);
  write_trig(COMPATIBILITY_SB_TRIG_THR2_ADDR,(int) TRIG_THR2);

  // Define which PMTs to include & coincidence level required
  compat_sb_trig_enab = COMPATIBILITY_SB_TRIG_INCL_PMT0 |
    COMPATIBILITY_SB_TRIG_INCL_PMT1 |
    COMPATIBILITY_SB_TRIG_INCL_PMT2 |
    (1 << COMPATIBILITY_SB_TRIG_COINC_LVL_SHIFT);
  write_trig(COMPATIBILITY_SB_TRIG_ENAB_ADDR, compat_sb_trig_enab);

  write_trig(SB_TRIG_THR0_ADDR,(int) (TRIG_THR0));
  write_trig(SB_TRIG_THR1_ADDR,(int) (TRIG_THR1));
  write_trig(SB_TRIG_THR2_ADDR,(int) (TRIG_THR2));
  write_trig(SB_TRIG_SSD_ADDR,(int) (TRIG_SSD));
 
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

  // Flush any shower stale events
  status = read_trig(SHWR_BUF_STATUS_ADDR);
  while ((SHWR_INTR_PEND_MASK & (status >> SHWR_INTR_PEND_SHIFT)) != 0)
    {
      toread_shwr_buf_num = SHWR_BUF_RNUM_MASK & 
        (status >> SHWR_BUF_RNUM_SHIFT);
      write_trig(SHWR_BUF_CONTROL_ADDR,toread_shwr_buf_num);
      status = read_trig(SHWR_BUF_STATUS_ADDR);
    }

  trigger_mask = 0;
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
  printf("\n");
  printf("Trigger_test: Shower trigger thresholds = %d\n",(int) (TRIG_THR0));

  // Flush any stale shower buffers
  shwr_status = read_trig(SHWR_BUF_STATUS_ADDR);
  while ((SHWR_INTR_PEND_MASK & (shwr_status >> SHWR_INTR_PEND_SHIFT)) != 0)
    {
      toread_shwr_buf_num = SHWR_BUF_RNUM_MASK & 
        (shwr_status >> SHWR_BUF_RNUM_SHIFT);
      write_trig(SHWR_BUF_CONTROL_ADDR, toread_shwr_buf_num);
      shwr_status = read_trig(SHWR_BUF_STATUS_ADDR);
    }
  write_trig(SHWR_BUF_TRIG_MASK_ADDR, trigger_mask);

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
    (0 << MUON_TRIG_WCD_DELAY_SHIFT) |
    (1 << MUON_TRIG_SSD_DELAY_SHIFT) |
    (3 << MUON_TRIG_COINC_OVLP_SHIFT) |
    (1 << MUON_TRIG_CONSEC_BINS_SHIFT);
     write_trig(MUON_TRIG1_ENAB_ADDR, muon_trig_enab);

muon_trigger_mask =  MUON_BUF_TRIG_SB1 | MUON_BUF_TRIG_EXT;
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
             
  enable_trigger_intr();  // Final enable of trigger interrupts (if defined)

  // Loop forever
  while (1) {

#ifdef DO_LED_PULSE
 
    disable_trigger_intr();

#ifdef DO_LED_NOW
    led_timer = led_timer+1;
    if (led_timer > 100000)
      {
        int loop_count = loop_count+1;
        write_trig(LED_CONTROL_ADDR, 0);
        led_timer = 0;
        led_pulsewid = LED_PULSWID0;
        if (loop_count % 2 == 1) led_pulsewid = LED_PULSWID1; 
        printf("Led_pulsewid = %d\n", led_pulsewid);
        led_control = (LED_DELAY << LED_DELAY_SHIFT) |
          (led_pulsewid << LED_PULSWID_SHIFT) | LED_NOW;
        write_trig(LED_CONTROL_ADDR, led_control);
      }
#else
    seconds = read_ttag(TTAG_PPS_SECONDS_ADDR);
    seconds = seconds & TTAG_SECONDS_MASK;
    if (seconds != prev_seconds) 
      {
        prev_seconds = seconds;
        int loop_count = loop_count+1;
        led_pulsewid = LED_PULSWID0;
        if (loop_count % 2 == 1) led_pulsewid = LED_PULSWID1;
        led_delay = LED_DELAY0;
        if (loop_count % 2 == 1) led_delay = LED_DELAY1; 
        printf("Led_pulsewid = %d at %d seconds\n", led_pulsewid, seconds);
        led_control = (led_delay << LED_DELAY_SHIFT) |
          (led_pulsewid << LED_PULSWID_SHIFT) | LED_ENAPPS;
        write_trig(LED_CONTROL_ADDR, led_control);
      }
#endif
    enable_trigger_intr();
#endif

#ifdef TRIGGER_POLLED
    // Is an interrupt pending?
    status = read_trig(SHWR_BUF_STATUS_ADDR);
    if ((SHWR_INTR_PEND_MASK & (status >> SHWR_INTR_PEND_SHIFT)) != 0)
      {
        toread_shwr_buf_num = SHWR_BUF_RNUM_MASK & 
          (status >> SHWR_BUF_RNUM_SHIFT);
        cur_shwr_buf_num = SHWR_BUF_WNUM_MASK & 
          (status >> SHWR_BUF_WNUM_SHIFT);
        full_shwr_bufs = SHWR_BUF_FULL_MASK & 
          (status >> SHWR_BUF_FULL_SHIFT);
        num_full = 0x7 & (status >> SHWR_BUF_NFULL_SHIFT);
        printf("Shower buf writing %d  to read %d  full %x  num full=%d\n",
               cur_shwr_buf_num,toread_shwr_buf_num,
               full_shwr_bufs,num_full);
 
        // Do readout of buffer here ....
        read_shw_buffers();  // Read buffers to local memory

        // Reset full flag
        cntrl_word = toread_shwr_buf_num;
        write_trig(SHWR_BUF_CONTROL_ADDR,cntrl_word);

        // Indicate data has been read
        Shwr_Data_Read = 1;
      }
#endif // TRIGGER_POLLED

    if (Shwr_Data_Read != 0)
      {
        unpack_shw_buffers(); // Unpack the buffers
        Shwr_Data_Read = 0;
        check_shw_buffers();  // Do sanity check of shower buffers
        print_shw_buffers();  // Print out the buffer
      }

#ifdef TRIGGER_POLLED
    // Is an interrupt pending?
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
        // write_trig(MUON_BUF_CONTROL_ADDR,toread_muon_buf_num);

        // Check that full flags are cleared
        while (num_full != 0) {
          write_trig(MUON_BUF_CONTROL_ADDR,toread_muon_buf_num);
          muon_status = read_trig(MUON_BUF_STATUS_ADDR);
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
        }

        // Indicate data has been read
        Muon_Data_Read = 1;
      }
#endif // TRIGGER_POLLED

    if (Muon_Data_Read != 0)
      {
        unpack_muon_buffers(); // Unpack the buffers
        Muon_Data_Read = 0;
        check_muon_buffers();  // Do sanity check of shower buffers
        print_muon_buffers();  // Print out the buffer
      }
  }
}


#if defined(SIMPLE) && defined(DMA)
int do_simple_polled_dma(u32 *SrcPtr, u32 *DestPtr, int length)
{
  status = 
    XAxiCdma_SimpleTransfer(&AxiCdmaInstance, (int) SrcPtr,
                            (int) DestPtr, length, NULL, NULL);
  if (status != XST_SUCCESS) return XST_FAILURE;

  // Wait until the DMA transfer is done
  while (XAxiCdma_IsBusy(&AxiCdmaInstance));

  // If the hardware has errors, this example fails
  // This is a poll example, no interrupt handler is involved.
  // Therefore, error conditions are not cleared by the driver.
  int DMA_Error = XAxiCdma_GetError(&AxiCdmaInstance);
  if (DMA_Error != 0x0) {
    printf("trigger_test: AXI DMA error\n");
    return XST_FAILURE;
  }

  // Invalidate the DestBuffer before receiving the data, in case the
  // Data Cache is enabled
  Xil_DCacheInvalidateRange((int) DestPtr, length);		

  return XST_SUCCESS;
}
#endif

#ifdef TRIGGER_INTERRUPT
void sde_trigger_handler(void *CallbackRef)
{
  int cur_shwr_buf_num = 0;
  int full_shwr_bufs = 0;
  int cntrl_word = 0;
  int num_full;

  // Is an interrupt pending?  Should be if we get here.
  status = read_trig(SHWR_BUF_STATUS_ADDR);
  if ((SHWR_INTR_PEND_MASK & (status >> SHWR_INTR_PEND_SHIFT)) != 0)
    {
      toread_shwr_buf_num = SHWR_BUF_RNUM_MASK &
        (status >> SHWR_BUF_RNUM_SHIFT);
      cur_shwr_buf_num = SHWR_BUF_WNUM_MASK & (status >> SHWR_BUF_WNUM_SHIFT);
      full_shwr_bufs = SHWR_BUF_FULL_MASK & (status >> SHWR_BUF_FULL_SHIFT);
      num_full = 0x7 & (status >> SHWR_BUF_NFULL_SHIFT);
      printf("trigger_test: Shwr intr writing %d  to read %d  full %x  num full=%d\n",
             cur_shwr_buf_num,toread_shwr_buf_num,full_shwr_bufs,num_full);

      // If room, read buffers to local memory
      if (Shwr_Data_Read == 0) 
        read_shw_buffers();
      else
        printf("trigger_test: Local memory buffer not free\n");

      // Reset full flag
      cntrl_word = toread_shwr_buf_num;
      write_trig(SHWR_BUF_CONTROL_ADDR,cntrl_word);
    }
  // Indicate the interrupt has been processed using a shared variable
  Shwr_Data_Read = 1;

  // Acknowledge the trigger to allow further interrupts
  SDE_TRIGGER_ACK((int *) SDE_TRIGGER_INTR_BASE, 1<<SHWR_TRIGGER_INTR_BIT);

  // Is an interrupt pending?  Should be if we get here.
  status = read_trig(MUON_BUF_STATUS_ADDR);
  if ((MUON_INTR_PEND_MASK & (status >> MUON_INTR_PEND_SHIFT)) != 0)
    {
      toread_muon_buf_num = MUON_BUF_RNUM_MASK &
        (status >> MUON_BUF_RNUM_SHIFT);
      cur_muon_buf_num = MUON_BUF_WNUM_MASK & (status >> MUON_BUF_WNUM_SHIFT);
      full_muon_bufs = MUON_BUF_FULL_MASK & (status >> MUON_BUF_FULL_SHIFT);
      num_full = 0x7 & (status >> MUON_BUF_NFULL_SHIFT);
      printf("trigger_test: Muon intr writing %d  to read %d  full %x  num full=%d\n",
             cur_muon_buf_num,toread_muon_buf_num,full_muon_bufs,num_full);

      // If room, read buffers to local memory
      if (Muon_Data_Read == 0) 
        read_muon_buffers();
      else
        printf("trigger_test: Local memory buffer not free\n");

      // Reset full flag
      cntrl_word = toread_muon_buf_num;
      write_trig(MUON_BUF_CONTROL_ADDR,cntrl_word);
    }
  // Indicate the interrupt has been processed using a shared variable
  Muon_Data_Read = 1;

  // Acknowledge the trigger to allow further interrupts
  SDE_TRIGGER_ACK((int *) SDE_TRIGGER_INTR_BASE, 1<<MUON_TRIGGER_INTR_BIT);
}
#endif  // TRIGGER_INTERRUPT

#ifdef SCATTER_GATHER
int check_scatter_gather_completion()
{
  int BdCount;

  /* Check whether the hardware has encountered any problems.
   * In some error cases, the DMA engine may not able to update the
   * BD that has caused the problem.
   */
  if (XAxiCdma_GetError(&AxiCdmaInstance) != 0x0) {
    printf("trigger_test: Transfer error %x\r\n",
           (unsigned int)XAxiCdma_GetError(&AxiCdmaInstance));
    DMA_Error = 1;
    return 0;
  }

  // Get all processed BDs from hardware
  BdCount = XAxiCdma_BdRingFromHw(&AxiCdmaInstance, XAXICDMA_ALL_BDS, &BdPtr);

  // Check finished BDs then release them
  if(BdCount > 0) {
    BdCurPtr = BdPtr;
    for (Index = 0; Index < BdCount; Index++) {
      // If the completed BD has error bit set,
      // then the example fails
      if (XAxiCdma_BdGetSts(BdCurPtr) &
          XAXICDMA_BD_STS_ALL_ERR_MASK)	{
        printf("trigger_test: BD error bit set\n");
        DMA_Error = 1;
        return 0;
      }

      BdCurPtr = XAxiCdma_BdRingNext(&AxiCdmaInstance, BdCurPtr);
    }

    // Release the BDs so later submission can use them
    printf("trigger_test: Releasing BDs, BdCount=%d\n", BdCount);
    status = XAxiCdma_BdRingFree(&AxiCdmaInstance, BdCount, BdPtr);
    if(status != XST_SUCCESS) {
      printf("trigger_test: Error free BD %x\r\n", status);
      DMA_Error = 1;
      return 0;
    }
    DMA_Done += BdCount;
  }
  return DMA_Done;
}

int do_scatter_gather_polled_shwr_dma()
{
  status = XAxiCdma_BdRingAlloc(&AxiCdmaInstance,
                                NUM_SHWR_BDS_TO_TRANSFER, &BdPtr);
  if (status != XST_SUCCESS) {
    printf("trigger_test: Failed bd alloc\r\n");
    return XST_FAILURE;
  }
  BdCurPtr = BdPtr;

  // Set up the BDs

  // Shower buffer 0
  mem_addr = (u32*) TRIGGER_MEMORY_SHWR0_BASE;
  mem_addr = mem_addr + toread_shwr_buf_num * SHWR_MEM_WORDS;
  status = XAxiCdma_BdSetSrcBufAddr(BdCurPtr, (unsigned int) mem_addr);
  if(status != XST_SUCCESS) {
    printf("trigger_test: Set src addr failed %d, %x/%x\r\n",
           status, (unsigned int)BdCurPtr,
           (unsigned int)mem_addr);
    return XST_FAILURE;
  }
  status = XAxiCdma_BdSetDstBufAddr(BdCurPtr, (unsigned int) shw_mem0);
  if(status != XST_SUCCESS) {
    printf("trigger_test: Set dst addr failed %d, %x/%x\r\n",
           status, (unsigned int)BdCurPtr,
           (unsigned int)shw_mem0);
    return XST_FAILURE;
  }
  status = XAxiCdma_BdSetLength(BdCurPtr, 4*SHWR_MEM_WORDS);
  if(status != XST_SUCCESS) {
    printf("trigger_test: Set BD length failed %d\r\n", status);
    return XST_FAILURE;
  }
  BdCurPtr = XAxiCdma_BdRingNext(&AxiCdmaInstance, BdCurPtr);

  // Shower buffer 1
  mem_addr = (u32*) TRIGGER_MEMORY_SHWR1_BASE;
  mem_addr = mem_addr + toread_shwr_buf_num * SHWR_MEM_WORDS;
  status = XAxiCdma_BdSetSrcBufAddr(BdCurPtr, (unsigned int) mem_addr);
  if(status != XST_SUCCESS) {
    printf("trigger_test: Set src addr failed %d, %x/%x\r\n",
           status, (unsigned int)BdCurPtr,
           (unsigned int)mem_addr);
    return XST_FAILURE;
  }
  status = XAxiCdma_BdSetDstBufAddr(BdCurPtr, (unsigned int) shw_mem1);
  if(status != XST_SUCCESS) {
    printf("trigger_test: Set dst addr failed %d, %x/%x\r\n",
           status, (unsigned int)BdCurPtr,
           (unsigned int)shw_mem1);
    return XST_FAILURE;
  }
  status = XAxiCdma_BdSetLength(BdCurPtr, 4*SHWR_MEM_WORDS);
  if(status != XST_SUCCESS) {
    printf("trigger_test: Set BD length failed %d\r\n", status);
    return XST_FAILURE;
  }
  BdCurPtr = XAxiCdma_BdRingNext(&AxiCdmaInstance, BdCurPtr);

  // Shower buffer 2
  mem_addr = (u32*) TRIGGER_MEMORY_SHWR2_BASE;
  mem_addr = mem_addr + toread_shwr_buf_num * SHWR_MEM_WORDS;
  status = XAxiCdma_BdSetSrcBufAddr(BdCurPtr, (unsigned int) mem_addr);
  if(status != XST_SUCCESS) {
    printf("trigger_test: Set src addr failed %d, %x/%x\r\n",
           status, (unsigned int)BdCurPtr,
           (unsigned int)mem_addr);
    return XST_FAILURE;
  }
  status = XAxiCdma_BdSetDstBufAddr(BdCurPtr, (unsigned int) shw_mem2);
  if(status != XST_SUCCESS) {
    printf("trigger_test: Set dst addr failed %d, %x/%x\r\n",
           status, (unsigned int)BdCurPtr,
           (unsigned int)shw_mem2);
    return XST_FAILURE;
  }
  status = XAxiCdma_BdSetLength(BdCurPtr, 4*SHWR_MEM_WORDS);
  if(status != XST_SUCCESS) {
    printf("trigger_test: Set BD length failed %d\r\n", status);
    return XST_FAILURE;
  }
  BdCurPtr = XAxiCdma_BdRingNext(&AxiCdmaInstance, BdCurPtr);

  // Shower buffer 3
  mem_addr = (u32*) TRIGGER_MEMORY_SHWR3_BASE;
  mem_addr = mem_addr + toread_shwr_buf_num * SHWR_MEM_WORDS;
  status = XAxiCdma_BdSetSrcBufAddr(BdCurPtr, (unsigned int) mem_addr);
  if(status != XST_SUCCESS) {
    printf("trigger_test: Set src addr failed %d, %x/%x\r\n",
           status, (unsigned int)BdCurPtr,
           (unsigned int)mem_addr);
    return XST_FAILURE;
  }
  status = XAxiCdma_BdSetDstBufAddr(BdCurPtr, (unsigned int) shw_mem3);
  if(status != XST_SUCCESS) {
    printf("trigger_test: Set dst addr failed %d, %x/%x\r\n",
           status, (unsigned int)BdCurPtr,
           (unsigned int)shw_mem3);
    return XST_FAILURE;
  }
  status = XAxiCdma_BdSetLength(BdCurPtr, 4*SHWR_MEM_WORDS);
  if(status != XST_SUCCESS) {
    printf("trigger_test: Set BD length failed %d\r\n", status);
    return XST_FAILURE;
  }
  BdCurPtr = XAxiCdma_BdRingNext(&AxiCdmaInstance, BdCurPtr);

  // Shower buffer 4
  mem_addr = (u32*) TRIGGER_MEMORY_SHWR4_BASE;
  mem_addr = mem_addr + toread_shwr_buf_num * SHWR_MEM_WORDS;
  status = XAxiCdma_BdSetSrcBufAddr(BdCurPtr, (unsigned int) mem_addr);
  if(status != XST_SUCCESS) {
    printf("trigger_test: Set src addr failed %d, %x/%x\r\n",
           status, (unsigned int)BdCurPtr,
           (unsigned int)mem_addr);
    return XST_FAILURE;
  }
  status = XAxiCdma_BdSetDstBufAddr(BdCurPtr, (unsigned int) shw_mem4);
  if(status != XST_SUCCESS) {
    printf("trigger_test: Set dst addr failed %d, %x/%x\r\n",
           status, (unsigned int)BdCurPtr,
           (unsigned int)shw_mem4);
    return XST_FAILURE;
  }
  status = XAxiCdma_BdSetLength(BdCurPtr, 4*SHWR_MEM_WORDS);
  if(status != XST_SUCCESS) {
    printf("trigger_test: Set BD length failed %d\r\n", status);
    return XST_FAILURE;
  }

  // Give the BDs to hardware
  status = XAxiCdma_BdRingToHw(&AxiCdmaInstance, NUM_SHWR_BDS_TO_TRANSFER,
                               BdPtr, NULL, NULL);
  if (status != XST_SUCCESS) {
    printf("trigger_test: Failed to give BDs to hw %d\r\n", status);
    return XST_FAILURE;
  } else
    printf("trigger_test: Scatter gather DMA started\n"); 

  // Wait until the DMA transfer is done or error occurs
  DMA_Done = 0;
  while ((check_scatter_gather_completion() < NUM_SHWR_BDS_TO_TRANSFER)
         && !DMA_Error) {
    /* Wait */
  }

  if (DMA_Error) {
    printf("trigger_test: Transfer has error %x\r\n",
           DMA_Error);
    return XST_FAILURE;
  } else {
    printf("trigger_test: Scatter gather DMA completed\n");
    return XST_SUCCESS;
  }
}

int do_scatter_gather_polled_muon_dma()
{
  status = XAxiCdma_BdRingAlloc(&AxiCdmaInstance, 
                                NUM_MUON_BDS_TO_TRANSFER, &BdPtr);
  if (status != XST_SUCCESS) {
    printf("trigger_test: Failed muon bd alloc\r\n");
    return XST_FAILURE;
  }
  BdCurPtr = BdPtr;

  // Set up the BDs

  // Muon buffer 0
  mem_addr = (u32*) TRIGGER_MEMORY_MUON0_BASE;
  mem_addr = mem_addr + toread_muon_buf_num * MUON_MEM_WORDS;
  status = XAxiCdma_BdSetSrcBufAddr(BdCurPtr, (unsigned int) mem_addr);
  if(status != XST_SUCCESS) {
    printf("trigger_test: Set muon src addr failed %d, %x/%x\r\n",
           status, (unsigned int)BdCurPtr,
           (unsigned int)mem_addr);
    return XST_FAILURE;
  }
  status = XAxiCdma_BdSetDstBufAddr(BdCurPtr, (unsigned int) muon_mem0);
  if(status != XST_SUCCESS) {
    printf("trigger_test: Set muon dst addr failed %d, %x/%x\r\n",
           status, (unsigned int)BdCurPtr,
           (unsigned int)muon_mem0);
    return XST_FAILURE;
  }
  status = XAxiCdma_BdSetLength(BdCurPtr, 4*MUON_MEM_WORDS);
  if(status != XST_SUCCESS) {
    printf("trigger_test: Set muon BD length failed %d\r\n", status);
    return XST_FAILURE;
  }
  BdCurPtr = XAxiCdma_BdRingNext(&AxiCdmaInstance, BdCurPtr);

  // Muon buffer 1
  mem_addr = (u32*) TRIGGER_MEMORY_MUON1_BASE;
  mem_addr = mem_addr + toread_muon_buf_num * MUON_MEM_WORDS;
  status = XAxiCdma_BdSetSrcBufAddr(BdCurPtr, (unsigned int) mem_addr);
  if(status != XST_SUCCESS) {
    printf("trigger_test: Set muon src addr failed %d, %x/%x\r\n",
           status, (unsigned int)BdCurPtr,
           (unsigned int)mem_addr);
    return XST_FAILURE;
  }
  status = XAxiCdma_BdSetDstBufAddr(BdCurPtr, (unsigned int) muon_mem1);
  if(status != XST_SUCCESS) {
    printf("trigger_test: Set muon dst addr failed %d, %x/%x\r\n",
           status, (unsigned int)BdCurPtr,
           (unsigned int)muon_mem1);
    return XST_FAILURE;
  }
  status = XAxiCdma_BdSetLength(BdCurPtr, 4*MUON_MEM_WORDS);
  if(status != XST_SUCCESS) {
    printf("trigger_test: Set muon BD length failed %d\r\n", status);
    return XST_FAILURE;
  }
  BdCurPtr = XAxiCdma_BdRingNext(&AxiCdmaInstance, BdCurPtr);

  // Give the BDs to hardware
  status = XAxiCdma_BdRingToHw(&AxiCdmaInstance, 
                               NUM_MUON_BDS_TO_TRANSFER, BdPtr, NULL, NULL);
  if (status != XST_SUCCESS) {
    printf("trigger_test: Failed to give muon BDs to hw %d\r\n", status);
    return XST_FAILURE;
  } else
    printf("trigger_test: Scatter gather muon DMA started\n");

  // Wait until the DMA transfer is done or error occurs
  DMA_Done = 0;
  while ((check_scatter_gather_completion() < NUM_MUON_BDS_TO_TRANSFER)
         && !DMA_Error) {
    /* Wait */
  }

  if (DMA_Error) {
    printf("trigger_test: Muon buffer transfer has error %x\r\n",
           DMA_Error);
    return XST_FAILURE;
  } else {
    printf("trigger_test: Muon buffer scatter gather DMA completed\n");
    return XST_SUCCESS;
  }
}
#endif

 void enable_trigger_intr()
 {
#ifdef TRIGGER_INTERRUPT
  // Enable the interrupt from the trigger at the interrupt controller
  XScuGic_Enable(&IntController,
		 XPAR_FABRIC_TRIGGER_MEMORY_BLOCK_SDE_TRIGGER_0_IRQ_INTR); 
#endif  //TRIGGER_INTERRUPT
 }

 void disable_trigger_intr()
 {
#ifdef TRIGGER_INTERRUPT
  // Disable the interrupt from the trigger at the interrupt controller
  XScuGic_Disable(&IntController,
		 XPAR_FABRIC_TRIGGER_MEMORY_BLOCK_SDE_TRIGGER_0_IRQ_INTR); 
#endif  //TRIGGER_INTERRUPT
 }
