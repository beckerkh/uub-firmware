// single_bin_40mhz.v
//
// This module implements the 40 MHz compatibility mode single bin trigger.
//
// 21-Mar-2014 DFN Initial version
// 21-May-2015 DFN Add requirement that multiplicity > 0
// 19-Sep-2016 DFN Correct width of THRES & ADC arrays.

`include "sde_trigger_defs.vh"

module single_bin_40mhz(
			input [1:0] ENABLE40,
			input CLK120,
			input [`ADC_WIDTH-1:0] ADC0,
			input [`ADC_WIDTH-1:0] ADC1,
			input [`ADC_WIDTH-1:0] ADC2,
			input [`ADC_WIDTH-1:0] THRES0,
			input [`ADC_WIDTH-1:0] THRES1,
			input [`ADC_WIDTH-1:0] THRES2,
			input [2:0] TRIG_ENABLE,
			input [1:0] MULTIPLICITY,
			output reg TRIG
			);
   
   reg [2:0]                       PMT_TRIG;
   reg [1:0]                       SUM_PMT_TRIGS;
   reg                             ITRIG;
   reg                             PREV_ITRIG;
   reg [`ADC_WIDTH-1:0]            THRES[2:0];
   reg [`ADC_WIDTH-1:0]            ADC[2:0];
   
   integer                         INDEX;
   
   always @(posedge CLK120) begin
      if (ENABLE40 == 0) begin
	 THRES[0] <= THRES0;
	 THRES[1] <= THRES1;
	 THRES[2] <= THRES2;
	 ADC[0] <= ADC0;
 	 ADC[1] <= ADC1;
	 ADC[2] <= ADC2;
         
	 for (INDEX=0; INDEX<3; INDEX=INDEX+1) begin
	    if ((THRES[INDEX] < ADC[INDEX]) && (TRIG_ENABLE[INDEX] == 1)) 
	      PMT_TRIG[INDEX] <= 1;
	    else 
	      PMT_TRIG[INDEX] <= 0;
	 end
	 SUM_PMT_TRIGS <= PMT_TRIG[0] + PMT_TRIG[1] + PMT_TRIG[2];
	 if ((SUM_PMT_TRIGS >= MULTIPLICITY) && (MULTIPLICITY != 0))
	   ITRIG <= 1;
	 else
	   ITRIG <= 0;
         PREV_ITRIG <= ITRIG;
      end
      if (ITRIG && !PREV_ITRIG)
        TRIG <= 1;
      else
        TRIG <= 0;
   end

endmodule
