#include <stdlib.h>
#include <sys/select.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <signal.h>
#include <time.h>
#include <sys/mman.h>
#include <unistd.h>
#include <string.h>

#include <stdint.h>
#include <xparameters.h>
#include <xparameters_ps.h>
#include <sde_trigger_defs.h>
int main(int argc,char *argv[])
{
  int fd,size,i;
  volatile uint32_t *regs;
  int reg,set;
  uint32_t val,addr,prev;
  uint16_t ret;

  FILE *arq;

  
  if(argc<2){
    printf("use as: %s <file>\n",argv[0]);
    printf("<file> is a file with a list of register address and value to be set \n");
    return(1);
  }
  arq=fopen(argv[1],"r");
  if(arq==NULL){
    printf("Not possible to open the file: %s\n",argv[1]);
    return(1);
  }
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
			PROT_READ | PROT_WRITE, MAP_SHARED,
			fd,SDE_TRIGGER_BASE);
  if(regs==MAP_FAILED){
    printf("Error - while trying to map the Registers\n");
    exit(1);
  }
  close(fd); /*it is not needed to keep opened */

  while(fscanf(arq,"%d %x",&addr,&val)==2){
    if(addr<0 || addr>256){
      printf("addr out of range: %d %x\n",addr,val);
      exit(0);
    } 
    prev=regs[addr];
    regs[addr]=val;
    printf("reg: %d -- %08x ... %08x\n",addr,prev,regs[addr]);
  }
  fclose(arq);
  return(0);
}
