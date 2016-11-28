/*
 * This test application is to read/write data directly from/to the device
 * from userspace.
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>
#include <fcntl.h>
#include "xparameters.h"
#include "xparameters_ps.h"
#include "sde_trigger_defs.h"
#include "sde_trigger.h"

	  int fd,size,i;
	  volatile uint32_t *regs;
	  int reg,set;
	  uint32_t val,prev;



void setTrigger(void)
{
	int Th=0;
	char buffer[8];

	printf("nuovo Trigger Config!\n");
	           regs[COMPATIBILITY_SB_TRIG_THR0_ADDR]=500;
	           regs[COMPATIBILITY_SB_TRIG_THR1_ADDR]=500;
		       regs[COMPATIBILITY_SB_TRIG_THR2_ADDR]=500;

				 		regs[COMPATIBILITY_SB_TRIG_ENAB_ADDR]=0x78;
				 		regs[SHWR_BUF_TRIG_MASK_ADDR]=0x11;



				printf("New Trigger Status: \n");
				printf("PMT0 Th = %d\n",regs[COMPATIBILITY_SB_TRIG_THR0_ADDR]);
				printf("PMT1 Th = %d\n",regs[COMPATIBILITY_SB_TRIG_THR1_ADDR]);
				printf("PMT2 Th = %d\n",regs[COMPATIBILITY_SB_TRIG_THR2_ADDR]);
				printf("SB_Trigger_mask = %x   (0x48 Pmt0 - 0x50 Pmt1 - 0x60 Pmt2 - 0x78 Or all Pmts )\n",regs[COMPATIBILITY_SB_TRIG_ENAB_ADDR]);
				printf("SHOW_BUF_Trigger_mask = %x  (0x10 external trigger - 0x1 SB trigger - 0x11 Or Ext SB  - \n",regs[SHWR_BUF_TRIG_MASK_ADDR]);
	return;
}

int main(int argc, char *argv[])
{


	  int c;
	unsigned gpio_addr = 0;
	int value = 0;
	int pmt0,pmt1,pmt2,sb_trig_mask,shower_buf_trig_mask;

	unsigned page_addr, page_offset;
	void *ptr;
	unsigned page_size=sysconf(_SC_PAGESIZE);

	/* Open /dev/mem file */
	/*open registers address for read/write */
	  fd=open("/dev/mem",O_RDWR);
	  if(fd<0){
	    printf("Error - it was not possible to open the /dev/mem "
		   "for read/write regitsters\n");
	    exit(1);
	  }
	  size=256*sizeof(uint32_t);
	  if(size%sysconf(_SC_PAGE_SIZE)){
	    size=(size/sysconf(_SC_PAGE_SIZE)+1)*sysconf(_SC_PAGE_SIZE);
	  }
	  regs=(uint32_t *)mmap(NULL, size,
				PROT_READ | PROT_WRITE, MAP_SHARED, fd, SDE_TRIGGER_BASE);
	  if(regs==MAP_FAILED){
	    printf("Error - while trying to map the Registers\n");
	    exit(1);
	  }
	  close(fd); /*it is not needed to keep opened */

/*	  pmt0=regs[COMPATIBILITY_SB_TRIG_THR0_ADDR];
	  pmt1=regs[COMPATIBILITY_SB_TRIG_THR1_ADDR];
	  pmt2=regs[COMPATIBILITY_SB_TRIG_THR2_ADDR];
	  sb_trig_mask=regs[COMPATIBILITY_SB_TRIG_ENAB_ADDR];
	  shower_buf_trig_mask=regs[SHWR_BUF_TRIG_MASK_ADDR];

	  prev=regs[reg];
	  if(set){
	    regs[reg]=val;
	  }
	  printf("reg: %d -- %08x ... %08x\n",reg,prev,regs[reg]);*/



	printf("Trigger Status: \n");
	printf("PMT0 Th = %d\n",regs[COMPATIBILITY_SB_TRIG_THR0_ADDR]);
	printf("PMT1 Th = %d\n",regs[COMPATIBILITY_SB_TRIG_THR1_ADDR]);
	printf("PMT2 Th = %d\n",regs[COMPATIBILITY_SB_TRIG_THR2_ADDR]);
	printf("SB_Trigger_mask = %x   (0x48 Pmt0 - 0x50 Pmt1 - 0x60 Pmt2 - 0x78 Or all Pmts )\n",regs[COMPATIBILITY_SB_TRIG_ENAB_ADDR]);
	printf("SHOW_BUF_Trigger_mask = %x  (0x10 external trigger - 0x1 SB trigger - 0x11 Or Ext SB  - \n",regs[SHWR_BUF_TRIG_MASK_ADDR]);

			setTrigger();







	munmap(ptr, page_size);

	return 0;
}
