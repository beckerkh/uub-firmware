//this code would be interesting to get evts directly reading the
// event data from the Programmable Logic

#include <stdio.h>

#include "read_evt.h"

//struct shwr_gps_info
//{
//  uint32_t second;
//  uint32_t ticks;
//  uint32_t ticks_prev_pps;
//};

//struct shwr_evt_raw
//{
//  uint32_t id;
//  uint32_t Evt_type_1; /* primary trigger event type (SB, TOT, ...).
//			  which kind of trigger the event appeared.*/
//  uint32_t Evt_type_2; /* secunday trigger event type (SB, ToT, ...),
//			  Which list of trigger it might be considered */
//  int32_t trace_start; /* The fadc_raw is just a copy the memory block,
//			  but the element ...[0] is not when the ADC 
//			  trace really starts. This parameter
//			  will contain that information.
//			*/
//  struct shwr_gps_info ev_gps_info;
//  int32_t micro_off;
//  int32_t nsamples;
//  uint32_t fadc_raw[SHWR_RAW_NCH_MAX][SHWR_NSAMPLES];
//
//  uint32_t buffer_ev_status;
//};

main()
{
  uint32_t v1,v2;
  struct shwr_evt_raw ev;
  int flag,n,i,j;
  int offset;
  if(read_evt_init()==0){
    n=0;
    while(n<10){
      flag=read_evt_read(&ev);
      if(flag==0){
	printf("%d   %10d %10d %10d  %08x %4d %08x\n",ev.id,
	       ev.ev_gps_info.second,
	       ev.ev_gps_info.ticks,
	       ev.ev_gps_info.ticks_prev_pps,
	       ev.buffer_ev_status,
	       ev.trace_start,ev.Evt_type_1);
//	for(i=0;i<ev.nsamples;i++){
//	  printf("%4d  ",i);
//	  for(j=0;j<5;j++){
//	    offset=(i+ev.trace_start) & (SHWR_NSAMPLES - 1);
//	    v1=ev.fadc_raw[j][offset] & 0xFFF;
//	    v2=(ev.fadc_raw[j][offset]>>16) & 0xFFF;
//	    printf("%4d %4d ",v1,v2);
//	  }
//	  printf("\n");
//	}
      }
    }
  } 
}    
