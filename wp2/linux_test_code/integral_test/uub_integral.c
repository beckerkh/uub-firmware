
// uub_integral

// Test program to get events directly by reading the event data
// from the programmable logic and produce an output file for checking
// the integral of the traces.

// 19-Nov-2016 DFN Original version based upon test0.c from Ricardo Sato

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include "read_evt.h"
#include "sde_trigger_defs.h"

#define MAX_EVTS 100
#define TRIG_THR0 600
#define TRIG_THR1 600
#define TRIG_THR2 600

// Macros
#define write_trig(RegNumber, Data) regs[RegNumber] = Data
#define read_trig(RegNumber) regs[RegNumber]

main()
{
  uint32_t v1, v2;
  struct shwr_evt_raw ev;
  int flag, n, i, j, fd;
  int status, rnum;
  int ar, pk;
  int offset, size;
  volatile uint32_t *regs;
  uint32_t v[10];
  int saturated[10], peak[10], area[10];
  int baseline[10];

  // Open register addresses for read/write

  printf("Opening /dev/mem\n");
  fd = open("/dev/mem",O_RDWR);
  if(fd < 0){
    printf("Error - /dev/mem open to read/write registers failed\n");
    exit(1);
  }
  size=256*sizeof(uint32_t);
  if(size%sysconf(_SC_PAGE_SIZE)){
    size=(size/sysconf(_SC_PAGE_SIZE)+1)*sysconf(_SC_PAGE_SIZE);
  }
  regs=(uint32_t *)mmap(NULL, size,
			PROT_READ | PROT_WRITE, MAP_SHARED,
			fd,SDE_TRIGGER_BASE);
  if(regs==MAP_FAILED){
    printf("Error - while trying to map the Registers\n");
    exit(1);
  }
  close(fd); // Can close fd now
  printf("Finished open of /dev/mem\n");

  // Set up triggers for this test

  // Reset trigger
  write_trig(COMPATIBILITY_GLOBAL_CONTROL_ADDR,1);

  // Set trigger parameters
  write_trig(COMPATIBILITY_SB_TRIG_THR0_ADDR,(int) TRIG_THR0);
  write_trig(COMPATIBILITY_SB_TRIG_THR1_ADDR,(int) TRIG_THR1);
  write_trig(COMPATIBILITY_SB_TRIG_THR2_ADDR,(int) TRIG_THR2);
  write_trig(COMPATIBILITY_SB_TRIG_ENAB_ADDR, 
	     COMPATIBILITY_SB_TRIG_INCL_PMT0 |
	     COMPATIBILITY_SB_TRIG_INCL_PMT1 |
	     COMPATIBILITY_SB_TRIG_INCL_PMT2 |
	     (1 << COMPATIBILITY_SB_TRIG_COINC_LVL_SHIFT));
  write_trig(SHWR_BUF_TRIG_MASK_ADDR, 
	     COMPATIBILITY_SHWR_BUF_TRIG_SB |
	     COMPATIBILITY_SHWR_BUF_TRIG_EXT);
  status = read_trig(COMPATIBILITY_SB_TRIG_ENAB_ADDR);
  printf("Trigger enable = %x\n",status);
  status = read_trig(SHWR_BUF_TRIG_MASK_ADDR);
  printf("Shwr buf trig mask = %x\n",status);
  while(1 == 1)
    {
  status = read_trig(SHWR_BUF_STATUS_ADDR);
  printf("Shwr buf status = %x\n",status);
  sleep(2);
    }
      
  // Now loop reading events
  if(read_evt_init()==0){
    n=0;
    // Get a fixed number of events
    while(n<MAX_EVTS){
      n++;
      flag=read_evt_read(&ev);
      if(flag==0){
	printf("Got a trigger\n");
	// Get FPGA calculated values of baseline, peak, and area.
	/* v[0] = read_trig(SHWR_PEAK_AREA0_ADDR); */
	/* v[1] = read_trig(SHWR_PEAK_AREA1_ADDR); */
	/* v[2] = read_trig(SHWR_PEAK_AREA2_ADDR); */
	/* v[3] = read_trig(SHWR_PEAK_AREA3_ADDR); */
	/* v[4] = read_trig(SHWR_PEAK_AREA4_ADDR); */
	/* v[5] = read_trig(SHWR_PEAK_AREA5_ADDR); */
	/* v[6] = read_trig(SHWR_PEAK_AREA6_ADDR); */
	/* v[7] = read_trig(SHWR_PEAK_AREA7_ADDR); */
	/* v[8] = read_trig(SHWR_PEAK_AREA8_ADDR); */
	/* v[9] = read_trig(SHWR_PEAK_AREA9_ADDR); */
	/* for (i=0; i<10; i++) */
	/*   { */
	/*     peak[i] = (v[i] >> SHWR_PEAK_SHIFT) & SHWR_PEAK_MASK; */
	/*     area[i] = (v[i] & SHWR_AREA_MASK); */
	/*     saturated[i] = (v[i] >> SHWR_SATURATED_SHIFT); */
	/*   } */

	/* v[0] = read_trig(SHWR_BASELINE0_ADDR); */
	/* v[1] = read_trig(SHWR_BASELINE1_ADDR); */
	/* v[2] = read_trig(SHWR_BASELINE2_ADDR); */
	/* v[3] = read_trig(SHWR_BASELINE3_ADDR); */
	/* v[4] = read_trig(SHWR_BASELINE4_ADDR); */
	/* for (i=0; i<5; i++) */
	/*   { */
	/*     baseline[2*i] = v[i] & 0xffff; */
	/*     baseline[2*i+1] = (v[i] >> 16) & 0xffff; */
	/*   } */

	// Reset shower buffer
	status = read_trig(SHWR_BUF_STATUS_ADDR);
	rnum = (status >> SHWR_BUF_RNUM_SHIFT) & SHWR_BUF_RNUM_MASK;
	write_trig(SHWR_BUF_CONTROL_ADDR,rnum);

	printf("\n>>>>>>>>>> BEGINNING OF EVENT >>>>>>>>>>\n");

	// Output a few lines header with the FPGA calculated area, peak, etc.
	/* for (i=0; i<10; i++) */
	/*   { */
	/*     printf("%1d %1d %4d %4d %d %d\n",  */
	/* 	   i, saturated[i], baseline[i], peak[i], area[i], rnum); */
	/*   } */

	// This is still test code here.
	for(i=0;i<ev.nsamples;i++){
	  printf("%4d  ",i);
	  offset=(i+ev.trace_start) & (SHWR_NSAMPLES - 1);

	  // Nominal ADC trace for high gain PMT2
	  for(j=1;j<2;j++){
	    v2=(ev.fadc_raw[j][offset]>>16) & 0xFFF;
	    printf("%4d ",v2);
	  }
	  // Delayed ADC trace for high gain PMT2 used in integral calc.
	  for(j=2;j<3;j++){
	    v2=ev.fadc_raw[j][offset] & 0xFFF;
	    printf("%4d ",v2);
	  }
	  // Calculated baseline & baseline with RC sag.
	  for(j=3;j<4;j++) {
	    v1 = ev.fadc_raw[j][offset] & 0xffff;
	    v2 = (ev.fadc_raw[j][offset]>>16) & 0xffff;
	    printf("%d %d ",v1,v2);
	  }

	  // Calculated area & peak
	  for(j=4;j<5;j++) {
	    v1 = ev.fadc_raw[j][offset];
            ar = v1 & 0x7ffff;
            pk = (v1 >> 19) & 0xfff;
	    printf("%d %d ",ar,pk);
	  }
	  printf("\n");
	}
	printf("<<<<<<<<<< END OF EVENT <<<<<<<<<<\n\n");
      }
    }
  } 
}    
