// Map register & buffers of sde_trigger
//
// 21-Nov-2016 DFN Original version

#include "uub_sipm_cal.h"

map_regs()
{
  int flag, fd;
  int offset, size;

  // Open register addresses for read/write
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

  // Open muon buffers for read only
  fd = open("/dev/mem",O_RDONLY);
  if(fd < 0){
    printf("Error - /dev/mem open to read only buffers failed\n");
    exit(1);
  }
  size=MUON_MEM_WORDS*MUON_MEM_NBUF*sizeof(uint32_t);
  if(size%sysconf(_SC_PAGE_SIZE)){
    size=(size/sysconf(_SC_PAGE_SIZE)+1)*sysconf(_SC_PAGE_SIZE);
  }
  muon_buf0 = (uint32_t *)mmap(NULL, size,
			PROT_READ, MAP_SHARED,
			fd,TRIGGER_MEMORY_MUON0_BASE);
  if(muon_buf0==MAP_FAILED){
    printf("Error - while trying to map muon_mem0\n");
    exit(1);
  }
  muon_buf1 = (uint32_t *)mmap(NULL, size,
			PROT_READ, MAP_SHARED,
			fd,TRIGGER_MEMORY_MUON1_BASE);
  if(muon_buf1==MAP_FAILED){
    printf("Error - while trying to map muon_mem1\n");
    exit(1);
  }
  close(fd); // Can close fd now
}
