// This module implements the 40 MHz compatibility mode ToTd trigger.
//
// 21-June-2018 DFN Initial version

`include "sde_trigger_defs.vh"

`define TRG_DLY 4
`define WIDTH 122
`define WIDTH_SIZE 7
  
module totd_40mhz(
		  input [1:0] ENABLE40,
		  input CLK120,
                  input RESET,
		  input [`ADC_WIDTH-1:0] ADC0,
		  input [`ADC_WIDTH-1:0] ADC1,
		  input [`ADC_WIDTH-1:0] ADC2,
                  input [`ADC_WIDTH+`SHWR_BASELINE_EXTRA_BITS-1:0] BASELINE0,
                  input [`ADC_WIDTH+`SHWR_BASELINE_EXTRA_BITS-1:0] BASELINE1,
                  input [`ADC_WIDTH+`SHWR_BASELINE_EXTRA_BITS-1:0] BASELINE2,
		  input [`ADC_WIDTH-1:0] THRES0,
		  input [`ADC_WIDTH-1:0] THRES1,
		  input [`ADC_WIDTH-1:0] THRES2,
		  input [2:0] TRIG_ENABLE,
		  input [1:0] MULTIPLICITY,
                  input [`WIDTH_SIZE-1:0] OCCUPANCY,
                  input [`FD_BITS-1:0] FD,
                  input [`FN_BITS-1:0] FN,
		  output reg TRIG,
                  output reg [15:0] DEBUG
		  );

   reg                      SB_TRIG[2:0];
   reg [`WIDTH-1:0]         WINDOW[2:0];
   reg [`WIDTH_SIZE-1:0]    OCC_COUNTER[2:0];
   reg [2:0]                PMT_TRIG;
   reg [1:0]                SUM_PMT_TRIGS;
   reg [`ADC_WIDTH-1:0]     THRES[2:0];
   reg [`ADC_WIDTH-1:0]     ADCD[2:0];
   wire [`ADC_WIDTH-1:0]    ADCD0;
   wire [`ADC_WIDTH-1:0]    ADCD1;
   wire [`ADC_WIDTH-1:0]    ADCD2;

   integer                  INDEX;
   
// Deconvolute each filtered FADC trace
   deconvolute deconv0(
                       .CLK(CLK120),
                       .ENABLE40(ENABLE40),
                       .ADC_IN(ADC0),
                       .BASELINE(BASELINE0),
                       .FD(FD),
                       .FN(FN),
                       .ADC_OUT(ADCD0)
                       );
   deconvolute deconv1(
                       .CLK(CLK120),
                       .ENABLE40(ENABLE40),
                       .ADC_IN(ADC1),
                       .BASELINE(BASELINE1),
                       .FD(FD),
                       .FN(FN),
                       .ADC_OUT(ADCD1)
                       );
   deconvolute deconv2(
                       .CLK(CLK120),
                       .ENABLE40(ENABLE40),
                       .ADC_IN(ADC2),
                       .BASELINE(BASELINE2),
                       .FD(FD),
                       .FN(FN),
                       .ADC_OUT(ADCD2)
                       );
   
   always @(posedge CLK120) begin
      if (RESET) begin
         WINDOW[0] <= 0;
         OCC_COUNTER[0] <= 0;
         WINDOW[1] <= 0;
         OCC_COUNTER[1] <= 0;
         WINDOW[2] <= 0;
         OCC_COUNTER[2] <= 0;
         TRIG <= 0;
      end
      else begin
         if (ENABLE40 == 0) begin
            
            // First do a simple single bin trigger on each bin
            THRES[0] <= THRES0;
            THRES[1] <= THRES1;
            THRES[2] <= THRES2;
            ADCD[0] <= ADCD0;
            ADCD[1] <= ADCD1;
            ADCD[2] <= ADCD2;
            
            for (INDEX=0; INDEX<3; INDEX=INDEX+1) begin
	       if ((THRES[INDEX] < ADCD[INDEX]) && (TRIG_ENABLE[INDEX] == 1)) 
	         PMT_TRIG[INDEX] <= 1;
	       else 
	         PMT_TRIG[INDEX] <= 0;
               WINDOW[INDEX] <= {WINDOW[INDEX][`WIDTH-2:0],PMT_TRIG[INDEX]};
               if (WINDOW[INDEX][`WIDTH-1] && !PMT_TRIG[INDEX])
                 OCC_COUNTER[INDEX] <= OCC_COUNTER[INDEX]-1;
               else if (!WINDOW[INDEX][`WIDTH-1] && PMT_TRIG[INDEX])
                 OCC_COUNTER[INDEX] <= OCC_COUNTER[INDEX]+1;
               if (OCC_COUNTER[INDEX] > OCCUPANCY) SB_TRIG[INDEX] <= 1;
               else SB_TRIG[INDEX] <= 0;
            end
         end  // Downsampled loop

         SUM_PMT_TRIGS <= SB_TRIG[0] + SB_TRIG[1] + SB_TRIG[2];
         if ((SUM_PMT_TRIGS >= MULTIPLICITY) && (MULTIPLICITY != 0)) begin
	    TRIG <= 1;
            WINDOW[0] <= 0;
            OCC_COUNTER[0] <= 0;
            WINDOW[1] <= 0;
            OCC_COUNTER[1] <= 0;
            WINDOW[2] <= 0;
            OCC_COUNTER[2] <= 0;
         end
         else
	   TRIG <= 0;
       end

      // Debug outputs
      DEBUG[11:0] <= ADCD0;
      DEBUG[12] <= SB_TRIG[0];
      DEBUG[13] <= SB_TRIG[1];
      DEBUG[14] <= PMT_TRIG[0];
      DEBUG[15] <= PMT_TRIG[1];
   end
   
endmodule
