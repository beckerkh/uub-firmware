/*
 * Setting trigger register
 * november 2016 by R.Assiro and G.Marsella
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

	printf("New Trigger Config!\n");
	printf("Set Pmt0 Th ( %d )\n",regs[SB_TRIG_THR0_ADDR]);
	Th = atoi(fgets(buffer, 17, stdin));
		if(Th<0||Th>4096){
			if(buffer!=""){
			//     Th = regs[SB_TRIG_THR0_ADDR];
			}
					else { regs[SB_TRIG_THR0_ADDR]=Th;
									}
			} else {
					regs[SB_TRIG_THR0_ADDR]=Th;
			}


		printf("Set Pmt1 Th ( %d )\n",regs[SB_TRIG_THR1_ADDR]);
		Th = atoi(fgets(buffer, 17, stdin));
		if(Th<0||Th>4096){
			if(buffer!=""){
				//     Th = regs[SB_TRIG_THR1_ADDR];
			}
				else { regs[SB_TRIG_THR1_ADDR]=Th;
						}
			} else {
				 regs[SB_TRIG_THR1_ADDR]=Th;
			}

		printf("Set Pmt2 Th ( %d )\n",regs[SB_TRIG_THR2_ADDR]);
		Th = atoi(fgets(buffer, 17, stdin));
		if(Th<0||Th>4096){
			if(buffer!=""){
			//     Th = regs[SB_TRIG_THR2_ADDR];
				}
				else { regs[SB_TRIG_THR2_ADDR]=Th;
				}
		 	} else {
	           regs[SB_TRIG_THR2_ADDR]=Th;
		 	 }

		printf("Set SSD Th ( %d )\n",regs[SB_TRIG_SSD_ADDR]);
		Th = atoi(fgets(buffer, 17, stdin));
		if(Th<0||Th>4096){
			if(buffer!=""){
			//     Th = regs[SB_TRIG_SSD_ADDR];
				}
				else { regs[SB_TRIG_SSD_ADDR]=Th;
				}
		 	} else {
	           regs[SB_TRIG_SSD_ADDR]=Th;
		 	 }

				printf("Set TRIG_ENAB_ADDR (%x)\n",regs[SB_TRIG_ENAB_ADDR]);
				Th = atoi(fgets(buffer, 17, stdin));
				if(Th<0||Th>4096){
					if(buffer!=""){
					//     Th = regs[SB_TRIG_THR2_ADDR];
						}
						else { regs[SB_TRIG_ENAB_ADDR]=Th;
						}
				 	} else {
				 		regs[SB_TRIG_ENAB_ADDR]=Th;
				 	 }


				printf("Set SHWR_BUF_TRIG_MASK_ADDR (%x)\n",regs[SHWR_BUF_TRIG_MASK_ADDR]);
				Th = atoi(fgets(buffer, 17, stdin));
				if(Th<=0||Th>300000){
					if(buffer!=""){
					//     Th = regs[SHWR_BUF_TRIG_MASK_ADDR];
						}
						else { regs[SHWR_BUF_TRIG_MASK_ADDR]=Th;
						}
				 	} else {
				 		regs[SHWR_BUF_TRIG_MASK_ADDR]=Th;
				 	 }

				printf("New Trigger Status: \n");
				printf("PMT0 Th = %d\n",regs[SB_TRIG_THR0_ADDR]);
				printf("PMT1 Th = %d\n",regs[SB_TRIG_THR1_ADDR]);
				printf("PMT2 Th = %d\n",regs[SB_TRIG_THR2_ADDR]);
				printf("SSD Th = %d\n",regs[SB_TRIG_SSD_ADDR]);
				printf("SB_Trigger_mask = %x   (0x11 (17) Pmt0 - 0x12 (18) Pmt1 - 0x14 (20) Pmt2 - 0x18 (24) SSD - 0x1F (31) Or all Pmts )\n",regs[SB_TRIG_ENAB_ADDR]);
				printf("SHOW_BUF_Trigger_mask = %x  (0x10 (16) external trigger - 0x20000 (131072) SB trigger - 0x20010 (131088) Or Ext SB  - \n",regs[SHWR_BUF_TRIG_MASK_ADDR]);
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

	if (argc == 1) {

	}
	else if (argc < 6 || argc > 8){
		usage();
		exit(1);
	}
	else {
		regs[SB_TRIG_THR0_ADDR] = atoi (argv[1]);
		regs[SB_TRIG_THR1_ADDR] = atoi (argv[2]);
		regs[SB_TRIG_THR2_ADDR] = atoi (argv[3]);
		regs[SB_TRIG_SSD_ADDR] = atoi (argv[4]);
		regs[SB_TRIG_ENAB_ADDR] = atoi (argv[5]);
		regs[SHWR_BUF_TRIG_MASK_ADDR] = atoi (argv[6]);
/*
		printf("New Trigger Status: \n");
		printf("SB_TRIG_THR0_ADDR = %d\n",regs[SB_TRIG_THR0_ADDR]);
		printf("SB_TRIG_THR1_ADDR = %d\n",regs[SB_TRIG_THR1_ADDR]);
		printf("SB_TRIG_THR2_ADDR = %d\n",regs[SB_TRIG_THR2_ADDR]);
		printf("SB_TRIG_SSD_ADDR = %d\n",regs[SB_TRIG_SSD_ADDR]);
		printf("SB_TRIG_ENAB_ADDR = %x \n",regs[SB_TRIG_ENAB_ADDR]);
		printf("SHWR_BUF_TRIG_MASK_ADDR = %x  \n",regs[SHWR_BUF_TRIG_MASK_ADDR]);
  */
		exit(1);
	}


	printf("Trigger Status: \n");
	printf("PMT0 Th = %d\n",regs[SB_TRIG_THR0_ADDR]);
	printf("PMT1 Th = %d\n",regs[SB_TRIG_THR1_ADDR]);
	printf("PMT2 Th = %d\n",regs[SB_TRIG_THR2_ADDR]);
	printf("SSD Th = %d\n",regs[SB_TRIG_SSD_ADDR]);
	printf("SB_Trigger_mask = %x   (0x11 (17) Pmt0 - 0x12 (18) Pmt1 - 0x14 (20) Pmt2 - 0x18 (24) SSD - 0x1F (31) Or all Pmts )\n",regs[SB_TRIG_ENAB_ADDR]);
	printf("SHOW_BUF_Trigger_mask = %x  (0x10 (16) external trigger - 0x20000 (131072) SB trigger - 0x20010 (131088) Or Ext SB  - \n",regs[SHWR_BUF_TRIG_MASK_ADDR]);

	printf("/n Change config? (y/n) \n");

	/* Parse command line arguements */
	(c = getchar());
		switch(c) {
		case 'y':
//			gpio_addr=strtoul(optarg,NULL, 0);
			getchar();//fake to clean stdin buff
			setTrigger();
			break;
		case 'n':
//			direction=IN;
			break;
		default:
			printf("invalid option: %c\n", (char)c);
			return -1;
		}

	munmap(ptr, page_size);

	return 0;
}

int usage(void)
{
    printf("____________________________\n");
    printf("|      TRIGGER SETUP       |\n");
    printf("|                          |\n");
    printf("|    written by R.Assiro   |\n");
    printf("|      and G.Marsella      |\n");
    printf("|__________________________|\n");
    exit(0);
}
