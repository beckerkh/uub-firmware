
// Simple standalone test routine to interact with the sde_trigger PL.
//
// 26-Feb-2016 DFN Initial (reconstructed from lost code) version
//                 The initial version does not test interrupts - 
//                 to be restored later
// 11-Apr-2016 DFN Add simple polled DMA
// 29-Apr-2016 DFN Implement TRIGGER_INTERRUPT
// 30-Apr-2018 DFN Implement split shower & muon interrupts

#include "trigger_test_options.h"
#include "trigger_test.h"

#ifndef STAND_ALONE
  volatile u32 *trig_regs;
  volatile u32 *ttag_regs;
  volatile u32 *ifc_regs;
  volatile u32 *tstctl_regs;
#endif

volatile u32 shwr_mem_ptr[5];
volatile u32 muon_mem_ptr[2];
u32 shwr_mem_addr[5];
u32 muon_mem_addr[2];
volatile static int nevents = 0;
volatile static int missed_events = 0;

// Shower memory buffers
volatile int readto_shw_buf_num = 0;
volatile int full_shw_rd_bufs[4] = {0,0,0,0};
volatile int unpack_shw_buf_num = 0;
u32 shw_mem0[4][SHWR_MEM_WORDS] __attribute__((aligned(64)));
u32 shw_mem1[4][SHWR_MEM_WORDS] __attribute__((aligned(64)));
u32 shw_mem2[4][SHWR_MEM_WORDS] __attribute__((aligned(64)));
u32 shw_mem3[4][SHWR_MEM_WORDS] __attribute__((aligned(64)));
u32 shw_mem4[4][SHWR_MEM_WORDS] __attribute__((aligned(64)));

// ADC traces & extra bits
u32 shw_mem[5][SHWR_MEM_WORDS];
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

#ifdef DMA
static XAxiCdma AxiCdmaInstance0;	// Instance of the XAxiCdma 0
static  XAxiCdma_Config *DmaCfgPtr0;
static XAxiCdma AxiCdmaInstance1;	// Instance of the XAxiCdma 1
static  XAxiCdma_Config *DmaCfgPtr1;
#endif

#ifdef TRIGGER_INTERRUPT
static XScuGic IntController;	// Instance of the Interrupt Controller
static XScuGic_Config *IntCfgPtr;  // The config. parameters of the controller
#endif

volatile static int Shwr_DMA_Error = 0;	/* Dma Error occurs */
volatile static int Muon_DMA_Error = 0;	/* Dma Error occurs */
volatile static int Shwr_Data_Read = 0;
volatile static int Muon_Data_Read = 0;
volatile static int Shwr_DMA_Done = 0;
volatile static int Muon_DMA_Done = 0;
volatile static int prev_read = -1;
volatile static int ave_num_full = 0;
volatile static int ave_num_used = 0;
volatile static int max_num_full = 0;
volatile static int max_num_used = 0;

#ifdef SCATTER_GATHER
// Needs update for separated muon/shower DMAs
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
  int thr0, thr1, thr2;
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
  int i;

  //readto_shw_buf_num = 0;
//full_shwr_rd_bufs = 0;
//unpackfrom_shw_buf_num = 0;

  // Map registers & memory buffers
  map_registers();

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
#ifdef USE_FAKE_MUON
  test_options = test_options | 4;
#endif
  if (test_options != 0)
    {
      write_tstctl(0, test_options);
      status = read_tstctl(0);
      //	  status = read_ifc(2);
      //	  printf("ifc_reg = %x  ifc_reg[2] = %x\n",ifc_regs, status);
      //	  write_ifc(2, test_options);
      //	  status = read_ifc(2);
	  
      if (status != test_options) 
	printf("trigger_test: Error setting test options, wrote %x read %x\n",
	       test_options, status);

      write_tstctl(1, 0);  // Mode=0 is used as RESET
      write_tstctl(1, FAKE_SIGNAL_MODE);
      status = read_tstctl(1);
      //	  status = read_ifc(2);
      //	  printf("ifc_reg = %x  ifc_reg[2] = %x\n",ifc_regs, status);
      //	  write_ifc(2, test_options);
      //	  status = read_ifc(2);
	  
      if (status != FAKE_SIGNAL_MODE) 
	printf("trigger_test: Error setting test mode, wrote %x read %x\n",
	       FAKE_SIGNAL_MODE, status);
    }

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
  DmaCfgPtr0 = XAxiCdma_LookupConfig(XPAR_ZYNC_BLOCK_AXI_CDMA_0_DEVICE_ID);
  if (status != XST_SUCCESS)
    {
      printf("trigger_test: Failed to lookup XAxiCdma device 0.\n");
      return;
    }
  // Initialize the XAxiCdma device.
  DmaCfgPtr1 = XAxiCdma_LookupConfig(XPAR_ZYNC_BLOCK_AXI_CDMA_1_DEVICE_ID);
  if (status != XST_SUCCESS)
    {
      printf("trigger_test: Failed to lookup XAxiCdma device 1.\n");
      return;
    }
  
  status = XAxiCdma_CfgInitialize(&AxiCdmaInstance0, DmaCfgPtr0,
                                  DmaCfgPtr0->BaseAddress);
  if (status != XST_SUCCESS) 
    {
      printf("trigger_test: Failed to initialize XAxiCdma device0.\n");
      return;
    }

  status = XAxiCdma_CfgInitialize(&AxiCdmaInstance1, DmaCfgPtr1,
                                  DmaCfgPtr1->BaseAddress);
  if (status != XST_SUCCESS) 
    {
      printf("trigger_test: Failed to initialize XAxiCdma device 1.\n");
      return;
    }

#ifdef SCATTER_GATHER
  // Need to update this for 2 DMA controllers!
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
  XAxiCdma_IntrDisable(&AxiCdmaInstance0, XAXICDMA_XR_IRQ_ALL_MASK);
  XAxiCdma_IntrDisable(&AxiCdmaInstance1, XAXICDMA_XR_IRQ_ALL_MASK);
#endif
#endif  //DMA

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

  //	 Connect device driver handler that will be called when an
  //	 interrupt for the device occurs, the device driver handler performs
  //	 the specific interrupt processing for the device  
  status = 
    XScuGic_Connect(&IntController,
                    XPAR_FABRIC_TRIGGER_MEMORY_BLOCK_SDE_TRIGGER_0_SHWR_IRQ_INTR,
                    (Xil_ExceptionHandler)sde_shwr_intr_handler,
                    (void *)&IntController);
  if (status != XST_SUCCESS)
    {
      printf("trigger_test: Failed to connect to sde_shwr_intr__handler\n");
      return;
    }

  status = 
    XScuGic_Connect(&IntController,
                    XPAR_FABRIC_TRIGGER_MEMORY_BLOCK_SDE_TRIGGER_0_MUON_IRQ_INTR,
                    (Xil_ExceptionHandler)sde_muon_intr_handler,
                    (void *)&IntController);
  if (status != XST_SUCCESS)
    {
      printf("trigger_test: Failed to connect to sde_muon_intr__handler\n");
      return;
    }

  // Enable shower interrupts from the sde_trigger module.
  SDE_TRIGGER_EnableInterrupts((int *) SDE_SHWR_TRIGGER_INTR_BASE, 1);
  SDE_TRIGGER_EnableInterrupts((int *) SDE_MUON_TRIGGER_INTR_BASE, 1); 
	
  // Enable the interrupt from the trigger at the interrupt controller
  XScuGic_Enable(&IntController,
		 XPAR_FABRIC_TRIGGER_MEMORY_BLOCK_SDE_TRIGGER_0_SHWR_IRQ_INTR); 
  XScuGic_Enable(&IntController,
		 XPAR_FABRIC_TRIGGER_MEMORY_BLOCK_SDE_TRIGGER_0_MUON_IRQ_INTR); 

#endif  //TRIGGER_INTERRUPT

  config_trigger();  // Configure triggers
             
  enable_trigger_intr();  // Final enable of trigger interrupts (if defined)

  // Loop forever
  nevents = 0;
  while (nevents < MAX_EVENTS) {

#ifdef STAND_ALONE   // Seems to be a conflict on Linux
#ifdef TOGGLE_WATCHDOG
    map_ifc();
    write_ifc(3, 3);
    write_ifc(3, 2);
#endif
#endif
#ifdef TOGGLE_ADCPWD
    printf("Toggling ADCPWD\n");
    map_ifc();
    write_ifc(1, 2);  // Toggle ADCPWD (ex P64)
    write_ifc(2, 0);  // Toggle P65 in opposite phase
    write_ifc(1, 0);
    write_ifc(2, 1);
#endif

#ifdef DO_LED_PULSE
    //     disable_trigger_intr();

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
        led_control = (LED_DELAY0 << LED_DELAY_SHIFT) |
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
    //    enable_trigger_intr();
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
#ifndef VERBOSE_BUFFERS
        if (nevents%1000 == 0)
          printf("Trigger_test: Read %d events\n");
        if (toread_shwr_buf_num != ((prev_read+1) & 0x3))
          {
            printf("Shower buf writing %d  to read %d  full %x  num full=%d",
                   cur_shwr_buf_num,toread_shwr_buf_num,
                   full_shwr_bufs,num_full);
            printf(" ******** ERROR *******");
            printf("\n");  
            exit(1);
          }
#endif
#ifdef VERBOSE_BUFFERS
        printf("Shower buf writing %d  to read %d  full %x  num full=%d",
               cur_shwr_buf_num,toread_shwr_buf_num,
               full_shwr_bufs,num_full);
        if (toread_shwr_buf_num != ((prev_read+1) & 0x3))
          {
            printf(" ******** ERROR *******");
            printf("\n");
            exit(1);
          }
        printf("\n");
#endif
        prev_read = toread_shwr_buf_num;
 
        // Do readout of buffer here ....
        read_shw_buffers();  // Read buffers to local memory
    	nevents++;

        // Reset full flag
        cntrl_word = toread_shwr_buf_num;
        write_trig(SHWR_BUF_CONTROL_ADDR,cntrl_word);

        // Indicate data has been read
        full_shw_rd_bufs[readto_shw_buf_num] = 1;
        readto_shw_buf_num = (readto_shw_buf_num+1)%4;
      }
#endif // TRIGGER_POLLED

    if (full_shw_rd_bufs[unpack_shw_buf_num] != 0)
      {
        unpack_shw_buffers(); // Unpack the buffers
        full_shw_rd_bufs[unpack_shw_buf_num] = 0;
        check_shw_buffers();  // Do sanity check of shower buffers
        print_shw_buffers();  // Print out the buffer
      }
    unpack_shw_buf_num = (unpack_shw_buf_num+1)%4;

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
int do_simple_polled_shwr_dma(u32 *SrcPtr, u32 *DestPtr, int length)
{
  status = 
    XAxiCdma_SimpleTransfer(&AxiCdmaInstance0, (int) SrcPtr,
                            (int) DestPtr, length, NULL, NULL);
  if (status != XST_SUCCESS) return XST_FAILURE;

  // Wait until the DMA transfer is done
  while (XAxiCdma_IsBusy(&AxiCdmaInstance0));

  // If the hardware has errors, this example fails
  // This is a poll example, no interrupt handler is involved.
  // Therefore, error conditions are not cleared by the driver.
  int Shwr_DMA_Error = XAxiCdma_GetError(&AxiCdmaInstance0);
  if (Shwr_DMA_Error != 0x0) {
    printf("trigger_test: AXI shwr DMA error\n");
    return XST_FAILURE;
  }

  // Invalidate the DestBuffer before receiving the data, in case the
  // Data Cache is enabled
  Xil_DCacheInvalidateRange((int) DestPtr, length);		

  return XST_SUCCESS;
}

int do_simple_polled_muon_dma(u32 *SrcPtr, u32 *DestPtr, int length)
{
  status = 
    XAxiCdma_SimpleTransfer(&AxiCdmaInstance1, (int) SrcPtr,
                            (int) DestPtr, length, NULL, NULL);
  if (status != XST_SUCCESS) return XST_FAILURE;

  // Wait until the DMA transfer is done
  while (XAxiCdma_IsBusy(&AxiCdmaInstance1));

  // If the hardware has errors, this example fails
  // This is a poll example, no interrupt handler is involved.
  // Therefore, error conditions are not cleared by the driver.
  int Muon_DMA_Error = XAxiCdma_GetError(&AxiCdmaInstance1);
  if (Muon_DMA_Error != 0x0) {
    printf("trigger_test: AXI muon DMA error\n");
    return XST_FAILURE;
  }

  // Invalidate the DestBuffer before receiving the data, in case the
  // Data Cache is enabled
  Xil_DCacheInvalidateRange((int) DestPtr, length);		

  return XST_SUCCESS;
}
#endif

#ifdef TRIGGER_INTERRUPT
void sde_shwr_intr_handler(void *CallbackRef)
{
  int cur_shwr_buf_num = 0;
  int full_shwr_bufs = 0;
  int cntrl_word = 0;
  int num_full;
  int num_used;
  double ave_full;
  double ave_used;

  // Is an interrupt pending?  Should be if we get here.
  status = read_trig(SHWR_BUF_STATUS_ADDR);
  if ((SHWR_INTR_PEND_MASK & (status >> SHWR_INTR_PEND_SHIFT)) != 0)
    {
      toread_shwr_buf_num = SHWR_BUF_RNUM_MASK &
        (status >> SHWR_BUF_RNUM_SHIFT);
      cur_shwr_buf_num = SHWR_BUF_WNUM_MASK & (status >> SHWR_BUF_WNUM_SHIFT);
      full_shwr_bufs = SHWR_BUF_FULL_MASK & (status >> SHWR_BUF_FULL_SHIFT);
      num_full = 0x7 & (status >> SHWR_BUF_NFULL_SHIFT);
      ave_num_full += num_full;
      if (num_full > max_num_full) max_num_full = num_full;
#ifndef VERBOSE_BUFFERS
        if ((nevents+missed_events)%1000 == 0)
        {
        	ave_full = (double)ave_num_full/1000.;
        	ave_num_full = 0;
                ave_used = (double)ave_num_used/1000.;
                ave_num_used = 0;
            printf("Trigger_test: Shwr intr Rd %d Msd %d events",
                   nevents, missed_events);
            printf(" Ave/max full %f %d Ave/max used %f %d\n",
                   ave_full, max_num_full, ave_used, max_num_used);
            max_num_full = 0;
            max_num_used = 0;
        }
        if (toread_shwr_buf_num != ((prev_read+1) & 0x3))
          {
            printf("Shwr intr writing %d  to read %d  full %x  num full=%d",
                   cur_shwr_buf_num,toread_shwr_buf_num,
                   full_shwr_bufs,num_full);
            printf(" ******** ERROR *******");
            printf("\n");  
            exit(1);
          }
#endif
#ifdef VERBOSE_BUFFERS
        printf("Shwr intr writing %d  to read %d  full %x  num full=%d",
               cur_shwr_buf_num,toread_shwr_buf_num,
               full_shwr_bufs,num_full);
        if (toread_shwr_buf_num != ((prev_read+1) & 0x3))
          {
            printf(" ******** ERROR *******");
            printf("\n");
            exit(1);
          }
        printf("\n");
#endif
        prev_read = toread_shwr_buf_num;

        // Keep track of local memory usage
        num_used = full_shw_rd_bufs[0] + full_shw_rd_bufs[1]
          +  full_shw_rd_bufs[2] + full_shw_rd_bufs[3];
        ave_num_used += num_used;
        if (num_used > max_num_used) max_num_used = num_used;
 
        // If room, read buffers to local memory
        if (full_shw_rd_bufs[readto_shw_buf_num] == 0)
          {
            read_shw_buffers();
            nevents++;
            // Indicate buffer available using a shared variable
            full_shw_rd_bufs[readto_shw_buf_num] = 1;
            readto_shw_buf_num = (readto_shw_buf_num+1)%4;
          }
        else
          missed_events++;

      // Reset full flag
      cntrl_word = toread_shwr_buf_num;
      write_trig(SHWR_BUF_CONTROL_ADDR,cntrl_word);
    }

  // Acknowledge the trigger to allow further interrupts
  SDE_TRIGGER_ACK((int *) SDE_SHWR_TRIGGER_INTR_BASE, 1);

}

void sde_muon_intr_handler(void *CallbackRef)
{
  int cur_muon_buf_num = 0;
  int full_muon_bufs = 0;
  int cntrl_word = 0;
  int num_full;



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
  SDE_TRIGGER_ACK((int *) SDE_MUON_TRIGGER_INTR_BASE, 1);
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
  status = XAxiCdma_BdSetDstBufAddr(BdCurPtr, (unsigned int) 
                                    &shw_mem0[readto_shw_buf_num][0]);
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
  status = XAxiCdma_BdSetDstBufAddr(BdCurPtr, (unsigned int) 
                                    &shw_mem1[readto_shw_buf_num][0]);
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
  status = XAxiCdma_BdSetDstBufAddr(BdCurPtr, (unsigned int) 
                                    &shw_mem2[readto_shw_buf_num][0]);
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
  status = XAxiCdma_BdSetDstBufAddr(BdCurPtr, (unsigned int) 
                                    &shw_mem3[readto_shw_buf_num][0]);
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
  status = XAxiCdma_BdSetDstBufAddr(BdCurPtr, (unsigned int) 
                                    &shw_mem4[readto_shw_buf_num]);
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
   // For now do both shower and muon enable together
  XScuGic_Enable(&IntController,
		 XPAR_FABRIC_TRIGGER_MEMORY_BLOCK_SDE_TRIGGER_0_SHWR_IRQ_INTR); 
  XScuGic_Enable(&IntController,
		 XPAR_FABRIC_TRIGGER_MEMORY_BLOCK_SDE_TRIGGER_0_MUON_IRQ_INTR); 
#endif  //TRIGGER_INTERRUPT
 }

 void disable_trigger_intr()
 {
#ifdef TRIGGER_INTERRUPT
   // Disable the interrupt from the trigger at the interrupt controller
   // For now do both shower and muon disable together
  XScuGic_Disable(&IntController,
		 XPAR_FABRIC_TRIGGER_MEMORY_BLOCK_SDE_TRIGGER_0_SHWR_IRQ_INTR); 
  XScuGic_Disable(&IntController,
		 XPAR_FABRIC_TRIGGER_MEMORY_BLOCK_SDE_TRIGGER_0_MUON_IRQ_INTR); 
#endif  //TRIGGER_INTERRUPT
 }
