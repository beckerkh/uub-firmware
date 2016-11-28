// led shot generator
#include <fcntl.h>
#include <stdio.h>
#include <linux/i2c-dev.h>
#include <unistd.h>
#include <sys/mman.h>
#include <stdlib.h>
#include "xparameters.h"
#include "xparameters_ps.h"
#include "sde_trigger_defs.h"
#include "sde_trigger.h"
#include <ctype.h>


#define DAC_ADDR		0x0C // DAC slave address DAC AD5316
int file;
int dac_led1, dac_led2, dac_led3, dac_led4;




volatile uint32_t *regs;
volatile uint32_t *regs1;
int reg,set;
uint32_t val,prev;


int main(int argc, char *argv[])
{

	  int c,size;
	unsigned gpio_addr = 0;
	int value = 0;
	int pmt0,pmt1,pmt2,sb_trig_mask,shower_buf_trig_mask;
	int led_addr =  0x43c203fc;

	unsigned page_addr, page_offset;
	void *ptr;
	unsigned page_size=sysconf(_SC_PAGESIZE);


	if (argc == 1) {
		 dac_led1 = 200;//argv[1];//valore canale 1 dac led
		 dac_led3 = 200;//argv[2];//valore canale 3 dac led
		// USCITA PX4
		 dac_led2 = 0;//argv[3];//valore canale 2 dac led
		 dac_led4 = 0;//argv[4];//valore canale 4 dac led
		 printf("Default LED values %d  %d  %d  %d\n",dac_led1,dac_led2,dac_led3,dac_led4);
	}
	else if (argc < 5 || argc > 6){
		usage();
	}
	else {
		dac_led1 = atoi (argv[1]);
		dac_led2 = atoi (argv[2]);
		dac_led3 = atoi (argv[3]);
		dac_led4 = atoi (argv[4]);
		printf("LED values %d  %d  %d  %d\n",dac_led1,dac_led2,dac_led3,dac_led4);
	}

	int fd, file,i,j, Status, data_trig;
	int int_trig =  0x41200000;
	void *ptrt;
	page_offset = 16;
	char buf[]={0x02,0x73,0x50};
	char filename[20];

   	printf("Initialization of I2C LED DAC..... ");
	snprintf(filename, 19, "/dev/i2c-0");
	file = open(filename, O_RDWR);
	if (file < 0) {
			exit("no open file");
	}
	if (ioctl(file, I2C_SLAVE, DAC_ADDR) < 0) {
			exit("Fail to setup slave addr!");
	}
    // Preparo i byte da inviare in buf
     // calcolo canale 1
     	buf[0] = 0x01;	//Seleziono canale del DAC
     	buf[1] = (dac_led1/64) + 112; //primi 4 bit piu' significativi di val trasferiti nei meno 4 significativi di a e aggiungo ctrl_reg=112
     	buf[2] = (dac_led1 & 0x3F)*4;
    	if (write(file, buf, sizeof(buf)) != sizeof(buf)) {
        	 	exit(3);
         	}
     	usleep(500);
     // calcolo canale 2
     	buf[0] = 0x02;	//Seleziono canale del DAC
     	buf[1] = (dac_led2/64) + 112; //primi 4 bit piu' significativi di val trasferiti nei meno 4 significativi di a e aggiungo ctrl_reg=112
     	buf[2] = (dac_led2 & 0x3F)*4;
     	if (write(file, buf, sizeof(buf)) != sizeof(buf)) {
     	       exit(3);
     	}
     	usleep(500);
     // calcolo canale 3
     	buf[0] = 0x04;	//Seleziono canale del DAC
     	buf[1] = (dac_led3/64) + 112; //primi 4 bit piu' significativi di val trasferiti nei meno 4 significativi di a e aggiungo ctrl_reg=112
     	buf[2] = (dac_led3 & 0x3F)*4;
     	if (write(file, buf, sizeof(buf)) != sizeof(buf)) {
     	       exit(3);
     	}
     	usleep(500);
     // calcolo canale 4
     	buf[0] = 0x08;	//Seleziono canale del DAC
     	buf[1] = (dac_led4/64) + 112; //primi 4 bit piu' significativi di val trasferiti nei meno 4 significativi di a e aggiungo ctrl_reg=112
     	buf[2] = (dac_led4 & 0x3F)*4;
     	if (write(file, buf, sizeof(buf)) != sizeof(buf)) {

     	    exit(3);
     	}
     	printf("Done!\n");






  	/* Open /dev/mem file */
  	/*open registers address for read/write */

  /*  regs=(uint32_t *)mmap(NULL, size,
                          PROT_READ | PROT_WRITE, MAP_SHARED, fd, XPAR_INTERFACE_UUB_DFN3_0_S00_AXI_BASEADDR);
    if(regs==MAP_FAILED){
        printf("Error - while trying to map the Registers\n");
        exit(1);
    }

*/
    	fd=open("/dev/mem",O_RDWR);

    	  if(fd<0){
    	    printf("Error - it was not possible to open the /dev/mem "
    		   "for read/write regitsters\n");
    	    exit(1);
    	  }

   	  size=0x3ff*sizeof(uint32_t);
//   	size=4096UL;
    	  if(size%sysconf(_SC_PAGE_SIZE)){
    	    size=(size/sysconf(_SC_PAGE_SIZE)+1)*sysconf(_SC_PAGE_SIZE);
    	  }

    regs1=(uint32_t *)mmap(NULL, size,PROT_READ | PROT_WRITE, MAP_SHARED, fd, SDE_TRIGGER_BASE);


    close(fd); /*it is not needed to keep opened */
  //  regs1[SHWR_BUF_TRIG_MASK_ADDR]=0x10000;


printf("\nLED Pulse is running...!\n ");

//while(1)
//     	{



	regs1[LED_CONTROL_ADDR*4]=0x03000000;
	printf("\nregs1[LED_CONTROL_ADDR*4] = %x, LED_CONTROL_ADDR = %x, BASE ADDR = %x \n ",regs1[LED_CONTROL_ADDR*4], LED_CONTROL_ADDR*4, SDE_TRIGGER_BASE);
	printf("\n regs[COMPATIBILITY_SB_TRIG_ENAB_ADDR] %x \n",regs1[COMPATIBILITY_SB_TRIG_ENAB_ADDR]);
	regs1[COMPATIBILITY_SB_TRIG_ENAB_ADDR]=0x78;
 		usleep(100000);
 		regs1[LED_CONTROL_ADDR*4]=0x03000001;
 		printf("\nregs1[LED_CONTROL_ADDR*4] = %x, LED_CONTROL_ADDR = %x, BASE ADDR = %x \n ",regs1[LED_CONTROL_ADDR*4], LED_CONTROL_ADDR*4, SDE_TRIGGER_BASE);

/*		page_addr = (led_addr & (~(page_size-1)));
      page_offset = led_addr - page_addr;
      regs1 = mmap(NULL, page_size, PROT_READ|PROT_WRITE, MAP_SHARED, fd, page_addr);









     		            value = 0x03000000; //bit 0 trigger interno a FPGA. Aquisisco segnali in BRAM (bit 1 = trigger interno)
     		            *((unsigned *)(regs1 + page_offset)) = value;// Write value to the device register
     		            usleep(1000);
     		            value = 0x03000001; //Alzo segnale trigger interno ( acquisizione continua)
     		            *((unsigned *)(regs1 + page_offset)) = value;
*/
usleep (200000);


//     	}


}

void usage(void)
{
	printf("|    led <val LED1> <val LED2> <val LED3> <val LED4>\n");
	printf("|    val is number of DAC counting <0...1023>\n");
	printf("|    example: led 500 500 300 300\n");
	printf("|    example: led PX3 PX4 PX3 PX4\n");
	exit(1);
}
