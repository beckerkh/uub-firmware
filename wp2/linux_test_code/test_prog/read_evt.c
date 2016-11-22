/* gcc FeShwrRead_test.c read_evt.c -I. -Ix -lrt */

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

#include <fe_lib.h> /*this include automatically the shwr_evt_defs.h */
#include "fe_kernel_interface_defs.h"
#include <read_evt.h>

#include <xparameters.h>
#include <sde_trigger_defs.h>

#define SIG_WAKEUP SIGRTMIN+14

/* for possible different control and possible distinct implementations,
   the structure below is to be used internally by the functions these
   functions. As it would be only used for these functions, 
   it many keep as local variables.
*/
   
struct read_evt_global
{
  uint32_t id_counter;
  uint32_t volatile *shwr_pt[5];
  int shwr_mem_size;

  uint32_t volatile *regs,*ttag;
  int regs_size,ttag_size;

  sigset_t sigset; /*used to wake the process periodically */
};

static struct read_evt_global gl;



/*please do not use the signal SIGRTMIN+15 because it will probaly be
  used in other part of the code ...*/

void *read_evt_map(uint32_t offset,int *size,int open_flag)
{
  int aux,fd;
  void *addr;
  fd=open("/dev/mem",open_flag);
  if(fd<0){
    *size=0;
    return(NULL);
  }
  aux=*size;
  if(aux%sysconf(_SC_PAGE_SIZE)){
    aux=((aux+sysconf(_SC_PAGE_SIZE)-1)/sysconf(_SC_PAGE_SIZE))*sysconf(_SC_PAGE_SIZE);
  }
  *size=aux;
  if(open_flag==O_RDWR){
    addr=(void *)mmap(NULL, aux,
		      PROT_READ | PROT_WRITE, MAP_SHARED,
		      fd,offset);
  } else {
    addr=(void *)mmap(NULL, aux,
		      PROT_READ , MAP_SHARED,
		      fd,offset);
  }
  if(addr==MAP_FAILED){
    printf("Error - while trying to map the Registers %08x %08x %d\n",offset,*size,fd);
    exit(1);
  }
  close(fd); /*it is not needed to keep opened */
  return addr;
}



int read_evt_init() 
{
  int fd,i;
  int size;
  struct sigevent sev;
  timer_t t_alarm;

  unsigned int shwr_addr[5]={
    TRIGGER_MEMORY_SHWR0_BASE,
    TRIGGER_MEMORY_SHWR1_BASE,
    TRIGGER_MEMORY_SHWR2_BASE,
    TRIGGER_MEMORY_SHWR3_BASE,
    TRIGGER_MEMORY_SHWR4_BASE
  };

  for(i=0;i<5;i++){
    gl.shwr_pt[i]=NULL;    
  }
  gl.regs=NULL;
  gl.ttag=NULL;
  /*open registers address for read/write */
  gl.regs_size=256*sizeof(uint32_t);
  gl.regs=read_evt_map(SDE_TRIGGER_BASE,&(gl.regs_size),O_RDWR);
  /*time tagging address */
  gl.ttag_size=16*sizeof(uint32_t);
  gl.ttag=read_evt_map(TIME_TAGGING_BASE,&(gl.regs_size),O_RDWR);  
  /*open shower buffers for read */

  for(i=0;i<5;i++){
    size=SHWR_MEM_DEPTH*SHWR_MEM_NBUF;
    gl.shwr_pt[i]=read_evt_map(shwr_addr[i],&(size),O_RDONLY);
  }
  gl.shwr_mem_size=size;  

  /*setting periodical process wakeup to check if there are event.
    It is ugly, but for now, it would work in this whay, until
    we figure a what to implement interruptions through the kernel */

  /*signal of alarm handler - it is going to be just blocked 
    to be used with sigtimedwait system call.
  */
  if(sigemptyset(&gl.sigset)!=0){
    printf("error while trying to set signals ... 1 \n");
    exit(1);
  }
  if(sigaddset(&gl.sigset,SIG_WAKEUP)!=0){
    printf("error while trying to set signals ... 2\n");
    exit(1);
  }  
  if(sigprocmask(SIG_BLOCK,&gl.sigset,NULL)!=0){
    printf("error while trying to set signals ... 3 \n");
    exit(1);
  }

  /* periodical signal generation */
  sev.sigev_notify=SIGEV_SIGNAL;
  sev.sigev_signo=SIG_WAKEUP;
  if(timer_create(CLOCK_MONOTONIC,&sev,&t_alarm)!=0){
    printf("timer creation error\n");
    exit(1);
  } else {
    struct itimerspec ts;
    ts.it_interval.tv_sec=0;
    ts.it_interval.tv_nsec=100000; /*.1 ms*/
    ts.it_value.tv_sec=0;
    ts.it_value.tv_nsec=100000;/*the next interruption would appear in .1ms*/
    if(timer_settime(t_alarm, 0, &ts, NULL)!=0){
      exit(1);
    }
  }
  gl.id_counter=0;
  return(0);
}

int read_evt_end()
{
  int i;
  void *aux;

  if(gl.regs!=NULL){
    aux=(void *)gl.regs;
    munmap(aux,gl.regs_size);
  }
  for(i=0;i<5;i++){
    if(gl.shwr_pt[i]!=NULL){
      aux=(void *)gl.shwr_pt[i];
      munmap(aux,gl.shwr_mem_size);
    }
  }
}

int read_evt_read(struct shwr_evt_raw *shwr)
{
  uint32_t volatile *st;
  int32_t ncl,ncl_pps;
  void *pt_aux;
  uint32_t aux;
  fd_set frds;
  int rd,sig,i,evid;
  int offset;
  
  st=&(gl.regs[SHWR_BUF_STATUS_ADDR]);

  aux=SHWR_BUF_NFULL_MASK<<SHWR_BUF_NFULL_SHIFT;
  sig=SIG_WAKEUP;
  
  while( ((*st) & aux)==0 && sig==SIG_WAKEUP)
    sig=sigwaitinfo(&gl.sigset,NULL);
  
  if(sig==SIG_WAKEUP){
    /*the number of ticks need to be calulated by the difference of number
      of clock counts: "shwn counter" - "pps counter".
      As the counter starts from 0 when the counter is larger than 
      TTAG_NANOSEC_MASK+1, it need calculate according that problem 
    */
    shwr->ev_gps_info.second=gl.ttag[TTAG_SHWR_SECONDS_ADDR] & TTAG_SECONDS_MASK;
    ncl=(gl.ttag[TTAG_SHWR_NANOSEC_ADDR] & TTAG_NANOSEC_MASK);
    ncl_pps=gl.ttag[TTAG_SHWR_PPS_NANOSEC_ADDR];
    if(ncl<ncl_pps){
      ncl+=TTAG_NANOSEC_MASK+1;
    }
    shwr->ev_gps_info.ticks=ncl-ncl_pps;
    /*get event identification (4 bits events counter)*/
    evid=(gl.ttag[TTAG_SHWR_NANOSEC_ADDR]>> TTAG_EVTCTR_SHIFT ) & TTAG_EVTCTR_MASK;
    if(evid==0){
      gl.id_counter+=16;
    }
    shwr->id=gl.id_counter+evid;
    shwr->ev_gps_info.ticks_prev_pps=gl.ttag[TTAG_SHWR_PPS_SECONDS_ADDR];

    
    /*Get trace information ... */
    shwr->buffer_ev_status=*st;
    rd=(((*st)>>SHWR_BUF_RNUM_SHIFT) & SHWR_BUF_RNUM_MASK);
    offset=rd*SHWR_NSAMPLES;
    for(i=0;i<5;i++){
      pt_aux=(void *)(gl.shwr_pt[i]+offset);
      memcpy(shwr->fadc_raw[i],
	     pt_aux,
	     sizeof(uint32_t)*SHWR_NSAMPLES);
    }
    shwr->id=gl.id_counter; /*just a internal counter */
    shwr->trace_start=gl.regs[SHWR_BUF_START_ADDR];
    shwr->Evt_type_1=gl.regs[SHWR_BUF_TRIG_ID_ADDR];
    shwr->Evt_type_2=0;
    
    shwr->nsamples=SHWR_NSAMPLES;
    gl.regs[SHWR_BUF_CONTROL_ADDR]=rd;
    return(0);
  }
  return(1);
}

int read_evt_read2(struct shwr_evt_raw *shwr1,struct shwr_evt_raw *shwr2)
{
  /* the idea of this function is to try to optimize the 
     data transference. It avoid the copy all the trace (twice copy
     as in the read_evt_read as implemented in the previous function).
     However, it need manage the two allocations.
  */
  uint32_t volatile *st;
  int32_t ncl,ncl_pps;
  void *pt_aux;
  uint32_t aux;
  fd_set frds;
  int rd,sig,i,evid;
  int offset;
  
  st=&(gl.regs[SHWR_BUF_STATUS_ADDR]);

  /*wait for the periodical signal and check if there is a 
    event trigger
  */
  aux=SHWR_BUF_NFULL_MASK<<SHWR_BUF_NFULL_SHIFT;
  sig=SIG_WAKEUP;

  
  
  while( ((*st) & aux)==0 && sig==SIG_WAKEUP)
    sig=sigwaitinfo(&gl.sigset,NULL);
  
  if(sig==SIG_WAKEUP){

    /*the number of ticks need to be calulated by the difference of number
      of clock counts: "shwn counter" - "pps counter".
      As the counter starts from 0 when the counter is larger than 
      TTAG_NANOSEC_MASK+1, it need calculate according that problem 
    */

    shwr1->ev_gps_info.second=gl.ttag[TTAG_SHWR_SECONDS_ADDR] & TTAG_SECONDS_MASK;
    ncl=(gl.ttag[TTAG_SHWR_NANOSEC_ADDR] & TTAG_NANOSEC_MASK);
    ncl_pps=gl.ttag[TTAG_SHWR_PPS_NANOSEC_ADDR];
    if(ncl<ncl_pps){
      ncl+=TTAG_NANOSEC_MASK+1;
    }
    shwr1->ev_gps_info.ticks=ncl-ncl_pps;
    
    /*get event identification (4 bits events counter)*/
    evid=(gl.ttag[TTAG_SHWR_NANOSEC_ADDR]>> TTAG_EVTCTR_SHIFT ) & TTAG_EVTCTR_MASK;
    if(evid==0){
      gl.id_counter+=16;
    }
    shwr1->id=gl.id_counter+evid;
    shwr1->ev_gps_info.ticks_prev_pps=gl.ttag[TTAG_SHWR_PPS_SECONDS_ADDR];
    /* rsato - end  */

    /*now it will take the event to keep stored ... */
    shwr2->buffer_ev_status=*st;
    rd=(((*st)>>SHWR_BUF_RNUM_SHIFT) & SHWR_BUF_RNUM_MASK);
    offset=rd*SHWR_NSAMPLES;
    for(i=0;i<5;i++){
      pt_aux=(void *)(gl.shwr_pt[i]+offset);
      memcpy(shwr2->fadc_raw[i],
	     pt_aux,
	     sizeof(uint32_t)*SHWR_NSAMPLES);
    }
    shwr2->id=gl.id_counter; /*just a internal counter */
    shwr2->trace_start=gl.regs[SHWR_BUF_START_ADDR];
    shwr2->Evt_type_1=gl.regs[SHWR_BUF_TRIG_ID_ADDR];
    shwr2->Evt_type_2=0;
    
    shwr2->nsamples=SHWR_NSAMPLES;
    gl.regs[SHWR_BUF_CONTROL_ADDR]=rd;

    return(0);
  }
  return(1);
}




