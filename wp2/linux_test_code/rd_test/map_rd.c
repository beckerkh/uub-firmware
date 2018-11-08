// map_rd
//
// Map the firmware registers
//
// 03-Nov-2018 DFN Original version

#include "rd_test.h"

void map_rd()
{
  int fd;
  int size, imem;
  int i;

  fake_rd_mem_addr[0] = FAKE_RD_EVENT0_BASE;
  rd_mem_addr[0] = RD_EVENT0_BASE;

  // Open register addresses for read/write

  fd = open("/dev/mem",O_RDWR);
  if (fd < 0){
    printf("Error - /dev/mem open to read/write registers failed\n");
    exit(1);
  }
  size=256*sizeof(u32);
  if (size%sysconf(_SC_PAGE_SIZE)){
    size=(size/sysconf(_SC_PAGE_SIZE)+1)*sysconf(_SC_PAGE_SIZE);
  }
  rd_regs=(u32 *)mmap(NULL, size,
                      PROT_READ | PROT_WRITE, MAP_SHARED,
                      fd, FAKE_RD_BASE);
  if (rd_regs==MAP_FAILED){
    printf("Error - while trying to map rd registers\n");
    exit(1);
  }

  printf("Finished mapping rd registers\n");
  fflush(stdout);
  sleep(1);

  // Now map shared memory buffers
  size =RD_MEM_DEPTH*RD_MEM_NBUF;
  rd_mem_ptr[0] = (u32)mmap(NULL, size,
                               PROT_WRITE|PROT_READ, MAP_SHARED,
                               fd,rd_mem_addr[0]);
  if (rd_mem_ptr[0] == (u32)MAP_FAILED){
    printf("Error - while trying to map rd memory %d\n", 0);
    exit(1);
  }
  fake_rd_mem_ptr[0] = (u32)mmap(NULL, size,
                                    PROT_WRITE|PROT_READ, MAP_SHARED,
                                    fd,fake_rd_mem_addr[0]);
  if (fake_rd_mem_ptr[0] == (u32)MAP_FAILED){
    printf("Error - while trying to map fake rd memory %d\n", 0);
    exit(1);
  }

  printf("Finished mapping fake rd buffers\n");
  fflush(stdout);
  sleep(1);

  close(fd); // Can close fd now
}
