// single_bin_120mhz.v
//
// This module implements the full bandwidth single bin trigger.
//
// 17-Sep-2016 DFN Initial version
// 19-Nov-2016 DFN Remove unneccessary WCD delay

`include "sde_trigger_defs.vh"

module single_bin_120mhz(
			 input RESET,
			 input CLK120,
			 input [`ADC_WIDTH-1:0] ADC0,
			 input [`ADC_WIDTH-1:0] ADC1,
			 input [`ADC_WIDTH-1:0] ADC2,
			 input [`ADC_WIDTH-1:0] ADC_SSD,
			 input [`ADC_WIDTH-1:0] TRIG_THR0,
			 input [`ADC_WIDTH-1:0] TRIG_THR1,
			 input [`ADC_WIDTH-1:0] TRIG_THR2,
			 input [`ADC_WIDTH-1:0] TRIG_SSD,
			 input [31:0] TRIG_ENAB,
			 output reg TRIG,
			 output reg [4:0] DEBUG
			 );

   reg [2:0] 				  SUM_PMT_TRIGS;

   reg [`ADC_WIDTH-1:0] 		  THRES[3:0];
   reg [`SB_TRIG_COINC_LVL_WIDTH-1:0] 	  MULTIPLICITY;

   reg [`ADC_WIDTH-1:0] 		  ADC0_DELAY;
   reg [`ADC_WIDTH-1:0] 		  ADC1_DELAY;
   reg [`ADC_WIDTH-1:0] 		  ADC2_DELAY;
   reg [`ADC_WIDTH-1:0] 		  ADCSSD_DELAY[0:`SB_TRIG_DELAY_MAX];

   reg [`ADC_WIDTH-1:0] 		  ADC0_PIPELINE[0:`SB_TRIG_CONSEC_BINS_MAX];
   reg [`ADC_WIDTH-1:0] 		  ADC1_PIPELINE[0:`SB_TRIG_CONSEC_BINS_MAX];
   reg [`ADC_WIDTH-1:0] 		  ADC2_PIPELINE[0:`SB_TRIG_CONSEC_BINS_MAX];
   reg [`ADC_WIDTH-1:0] 		  ADCSSD_PIPELINE[0:`SB_TRIG_CONSEC_BINS_MAX];

   reg [`SB_TRIG_CONSEC_BINS_MAX:0] 	  ADC0_TRIG0;
   reg [`SB_TRIG_CONSEC_BINS_MAX:0] 	  ADC1_TRIG0;
   reg [`SB_TRIG_CONSEC_BINS_MAX:0] 	  ADC2_TRIG0;
   reg [`SB_TRIG_CONSEC_BINS_MAX:0] 	  ADCSSD_TRIG0;

   reg 					  ADC0_TRIG1;
   reg 					  ADC1_TRIG1;
   reg 					  ADC2_TRIG1;
   reg 					  ADCSSD_TRIG1;
   
   reg 					  ADC0_TRIG1L;
   reg 					  ADC1_TRIG1L;
   reg 					  ADC2_TRIG1L;
   reg 					  ADCSSD_TRIG1L;
   
   reg 					  ADC0_TRIG2;
   reg 					  ADC1_TRIG2;
   reg 					  ADC2_TRIG2;
   reg 					  ADCSSD_TRIG2;
   
   reg [`SB_TRIG_COINC_OVLP_MAX:0] 	  ADC0_TRIG3;
   reg [`SB_TRIG_COINC_OVLP_MAX:0] 	  ADC1_TRIG3;
   reg [`SB_TRIG_COINC_OVLP_MAX:0] 	  ADC2_TRIG3;
   reg [`SB_TRIG_COINC_OVLP_MAX:0] 	  ADCSSD_TRIG3;

   reg 					  ADC0_TRIG4;
   reg 					  ADC1_TRIG4;
   reg 					  ADC2_TRIG4;
   reg 					  ADCSSD_TRIG4;
   
   reg [`SB_TRIG_COINC_OVLP_WIDTH-1:0] 	  COINC_OVLP;
   reg [`SB_TRIG_CONSEC_BINS_WIDTH-1:0]   CONSEC_BINS; // # consec. bins - 1
   reg [`SB_TRIG_DELAY_WIDTH-1:0] 	  SSD_DELAY;
   
   integer                                DLY_IDX;
   integer                                CONSEC_IDX;
   integer                                CONSEC_IDX2;
   integer                                OVLP_IDX;
   
   always @(posedge CLK120) begin
      if (RESET) begin
         TRIG <= 0;
      end
      else begin
         THRES[0] <= TRIG_THR0;
	 THRES[1] <= TRIG_THR1;
	 THRES[2] <= TRIG_THR2;
	 THRES[3] <= TRIG_SSD;
         MULTIPLICITY <= TRIG_ENAB[`SB_TRIG_COINC_LVL_SHIFT+
                                   `SB_TRIG_COINC_LVL_WIDTH-1:
                                   `SB_TRIG_COINC_LVL_SHIFT];
         COINC_OVLP <= TRIG_ENAB[`SB_TRIG_COINC_OVLP_SHIFT+
                                 `SB_TRIG_COINC_OVLP_WIDTH-1:
                                 `SB_TRIG_COINC_OVLP_SHIFT];
         CONSEC_BINS <= TRIG_ENAB[`SB_TRIG_CONSEC_BINS_SHIFT+
                                  `SB_TRIG_CONSEC_BINS_WIDTH-1:
                                  `SB_TRIG_CONSEC_BINS_SHIFT];
         SSD_DELAY <= TRIG_ENAB[`SB_TRIG_SSD_DELAY_SHIFT+
                                `SB_TRIG_DELAY_WIDTH-1:
                                `SB_TRIG_SSD_DELAY_SHIFT];
         
         // Delay signals to allow compensation of timing difference between
         // WCD & SSD.
         ADC0_DELAY <= ADC0;
         ADC1_DELAY <= ADC1;
         ADC2_DELAY <= ADC2;
         ADCSSD_DELAY[0] <= ADC_SSD;
         for (DLY_IDX=1; DLY_IDX<=`SB_TRIG_DELAY_MAX; DLY_IDX=DLY_IDX+1) begin
            ADCSSD_DELAY[DLY_IDX] <= ADCSSD_DELAY[DLY_IDX-1];
         end

         // Store signals in pipeline to allow multi time bin operations
         
	 ADC0_PIPELINE[0] <= ADC0_DELAY;
	 ADC1_PIPELINE[0] <= ADC1_DELAY;
	 ADC2_PIPELINE[0] <= ADC2_DELAY;
	 ADCSSD_PIPELINE[0] <= ADCSSD_DELAY[SSD_DELAY];

         for (CONSEC_IDX=1; CONSEC_IDX<=`SB_TRIG_CONSEC_BINS_MAX;
              CONSEC_IDX=CONSEC_IDX+1) begin
            ADC0_PIPELINE[CONSEC_IDX] <= ADC0_PIPELINE[CONSEC_IDX-1];
            ADC1_PIPELINE[CONSEC_IDX] <= ADC1_PIPELINE[CONSEC_IDX-1];
            ADC2_PIPELINE[CONSEC_IDX] <= ADC2_PIPELINE[CONSEC_IDX-1];
            ADCSSD_PIPELINE[CONSEC_IDX] <= ADCSSD_PIPELINE[CONSEC_IDX-1];
         end
         
         // Apply first trigger comparision against all required consecutive bins
         // Note that CONSEQ_BINS=0 means one bin, CONSEQ_BINS=1 means 2 bins, etc.
         for (CONSEC_IDX2=0; CONSEC_IDX2<=`SB_TRIG_CONSEC_BINS_MAX;
              CONSEC_IDX2=CONSEC_IDX2+1) begin
            ADC0_TRIG0[CONSEC_IDX2] <= ((ADC0_PIPELINE[CONSEC_IDX2] > THRES[0])
					|| (CONSEC_IDX2 > CONSEC_BINS)) && TRIG_ENAB[0];
            ADC1_TRIG0[CONSEC_IDX2] <= ((ADC1_PIPELINE[CONSEC_IDX2] > THRES[1])
					|| (CONSEC_IDX2 > CONSEC_BINS)) && TRIG_ENAB[1];
            ADC2_TRIG0[CONSEC_IDX2] <= ((ADC2_PIPELINE[CONSEC_IDX2] > THRES[2])
					|| (CONSEC_IDX2 > CONSEC_BINS)) && TRIG_ENAB[2];
            ADCSSD_TRIG0[CONSEC_IDX2] <= ((ADCSSD_PIPELINE[CONSEC_IDX2] > THRES[3])
					  || (CONSEC_IDX2 > CONSEC_BINS)) && TRIG_ENAB[3];
         end // for (CONSEC_IDX2=0; CONSEC_IDX2<=`SB_TRIG_CONSEQ_BINS_MAX;...

         // Now require that all consecutive bins have satistifed the ph condition
         ADC0_TRIG1 <= &ADC0_TRIG0;
         ADC1_TRIG1 <= &ADC1_TRIG0;
         ADC2_TRIG1 <= &ADC2_TRIG0;
         ADCSSD_TRIG1 <= &ADCSSD_TRIG0;

         // Require transition from not trig to trig to avoid retrigger of slow pulses
         ADC0_TRIG1L <= ADC0_TRIG1;
         ADC1_TRIG1L <= ADC1_TRIG1;
         ADC2_TRIG1L <= ADC2_TRIG1;
         ADCSSD_TRIG1L <= ADCSSD_TRIG1;

         ADC0_TRIG2 <= ADC0_TRIG1 && !ADC0_TRIG1L;
         ADC1_TRIG2 <= ADC1_TRIG1 && !ADC1_TRIG1L;
         ADC2_TRIG2 <= ADC2_TRIG1 && !ADC2_TRIG1L;
         ADCSSD_TRIG2 <= ADCSSD_TRIG1 && !ADCSSD_TRIG1L;

         // Widen pulse to increase overlap
         for (OVLP_IDX=0; OVLP_IDX<=`SB_TRIG_COINC_OVLP_MAX;
              OVLP_IDX=OVLP_IDX+1) begin
            ADC0_TRIG3[OVLP_IDX] <= ADC0_TRIG2 && (OVLP_IDX <= COINC_OVLP);
            ADC1_TRIG3[OVLP_IDX] <= ADC1_TRIG2 && (OVLP_IDX <= COINC_OVLP);
            ADC2_TRIG3[OVLP_IDX] <= ADC2_TRIG2 && (OVLP_IDX <= COINC_OVLP);
            ADCSSD_TRIG3[OVLP_IDX] <= ADCSSD_TRIG2 && (OVLP_IDX <= COINC_OVLP);
         end
         ADC0_TRIG4 <= |ADC0_TRIG3;
         ADC1_TRIG4 <= |ADC1_TRIG3;
         ADC2_TRIG4 <= |ADC2_TRIG3;
         ADCSSD_TRIG4 <= |ADCSSD_TRIG3;
         
         // Finally, apply multiplicity condition
   	 SUM_PMT_TRIGS <= ADC0_TRIG4 + ADC1_TRIG4 + ADC2_TRIG4 + ADCSSD_TRIG4;
	 if ((SUM_PMT_TRIGS >= MULTIPLICITY) && (MULTIPLICITY != 0) && !TRIG)
           TRIG <= 1;
         else
           TRIG <= 0;      
         
         // Debugging returns
         DEBUG[0] <= ADC0_TRIG1;
         DEBUG[1] <= ADC0_TRIG2;
         DEBUG[2] <= ADC0_TRIG3;
         DEBUG[3] <= ADC0_TRIG4;
         DEBUG[4] <= TRIG;
         
      end // if (RESET)
   end // always @ (posedge CLK120)
   
endmodule

