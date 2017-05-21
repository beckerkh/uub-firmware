// shwr_integral.v
//
// This keeps track of the integral of a shower signal.
//
// 18-Nov-2016 DFN Initial version
// 03-Mar-2017 DFN Update to allow different time constants for lo/hi gain

`include "sde_trigger_defs.vh"

module shwr_integral(
		     input RESET,
		     input CLK120,
		     input [`ADC_WIDTH-1:0] ADC,
		     input TRIGGERED,
		     input HILO,
		     output reg[`SHWR_AREA_WIDTH-1:0] INTEGRAL,
		     output reg[`ADC_WIDTH+`SHWR_BASELINE_EXTRA_BITS-1:0] BASELINE,
		     output reg[`ADC_WIDTH+`SHWR_BASELINE_EXTRA_BITS-1:0] SBASELINE,
		     output reg[`ADC_WIDTH-1:0] PEAK,
		     output reg SATURATED,
		     output reg [`ADC_WIDTH-1:0] ADCD
		     );
   
// Count of how many bins included so far in integral
   reg [11:0] 					BIN_COUNT;
// First pipeline stage of integral calculation
   reg [`SHWR_AREA_WIDTH+`BASELINE_FRAC_WIDTH-1:0] INTEGRALA;
// ADC delay chain
   reg [`ADC_WIDTH-1:0] 			    DLYD_ADC[0:`SHWR_AREA_ADC_DLY];
   // Delayed ADC value
//   reg [`ADC_WIDTH-1:0] 			    ADCD;
// Peak signal before baseline subtraction
   reg [`ADC_WIDTH-1:0] 			    LPEAK;
// Computed baseline keeping fractional precision
   reg [`ADC_WIDTH+`BASELINE_FRAC_WIDTH-1:0] 	    LBASELINE;
   // Baseline corrected for sag
   reg [`ADC_WIDTH+`BASELINE_FRAC_WIDTH-1:0] 	    CBASELINE;
// Baseline truncated to integer value
//   reg [`ADC_WIDTH-1:0] 			    IBASELINE;
// Baseline rounded to nearest integer value
   reg [`ADC_WIDTH-1:0] 			    RBASELINE;
   // Extended precision ADC value
   reg [`ADC_WIDTH+`BASELINE_FRAC_WIDTH-1:0] 	    ADCLONG;
   // Delayed extended precision ADC value
   reg [`ADC_WIDTH+`BASELINE_FRAC_WIDTH-1:0] 	    ADCLONGD;
   reg [`ADC_WIDTH+`BASELINE_FRAC_WIDTH-1:0] 	    ADCLONGD1;
   // Accumulated baseline sag
   reg [`ADC_WIDTH+`BASELINE_FRAC_WIDTH-1:0] 	    SAG;
   // Intermediate sag calculations
   reg [`ADC_WIDTH+`BASELINE_FRAC_WIDTH-1:0] 	    SAG1;
   reg [`ADC_WIDTH+`BASELINE_FRAC_WIDTH-1:0] 	    SAG2;
   reg [`ADC_WIDTH+`BASELINE_FRAC_WIDTH-1:0] 	    SAG3;
 	
   integer 					    DLY_IDX;

`define ONE_HALF (1 << `BASELINE_FRAC_WIDTH-1)
`define THREE_QUARTERS (`ONE_HALF + (1 << `BASELINE_FRAC_WIDTH-2)) 
    
   always @(posedge CLK120) begin
      if (RESET) begin
         INTEGRAL <= 0;
	 INTEGRALA <= 0;
	 LBASELINE <= 250 << `BASELINE_FRAC_WIDTH;
	 BIN_COUNT <= 0;
	 LPEAK <= 0;
	 PEAK <= 0;
	 SATURATED <= 0;
	 ADCLONG <= 0;
	 ADCLONGD <= 0;
	 ADCLONGD1 <= 0;
	 SAG <= 0;
      end
      else begin

	 // We need to delay the data so that we start looking at the traces
	 // before the trigger.  Otherwise we'll start looking too late in
	 // the trace.
	 DLYD_ADC[0] <= ADC;
	 for (DLY_IDX=1; DLY_IDX<=`SHWR_AREA_ADC_DLY; DLY_IDX=DLY_IDX+1) begin
	    DLYD_ADC[DLY_IDX] <= DLYD_ADC[DLY_IDX-1];
	 end
	 ADCD <= DLYD_ADC[`SHWR_AREA_ADC_DLY];
	 ADCLONG[`ADC_WIDTH+`BASELINE_FRAC_WIDTH-1:`BASELINE_FRAC_WIDTH] 
	   <= DLYD_ADC[`SHWR_AREA_ADC_DLY];
	 
	 if (!TRIGGERED) begin
	    BIN_COUNT <= 0;
	    INTEGRAL <= 0;
	    INTEGRALA <= 0;
	    LPEAK <= 0;
	    PEAK <= 0;
	    SATURATED <= 0;
	    SAG <= 0;
	    
	    // Keep running track of Baseline with FRAC_WIDTH time constant
	    // Needs to do more than just integral test here
	    // if (ADCLONG > LBASELINE)
	    //   LBASELINE <= LBASELINE+1;
	    // else if (ADCLONG < LBASELINE)
	    //   LBASELINE <= LBASELINE-1;
	    // CBASELINE <= LBASELINE;

	    if (ADCLONG > LBASELINE + `ONE_HALF)
	      LBASELINE <= LBASELINE +
			   (2 << `BASELINE_FRAC_WIDTH-`BASELINE_FRAC_INCR);
	    else if (ADCLONG > LBASELINE)
	      LBASELINE <= LBASELINE +
			   (1 << `BASELINE_FRAC_WIDTH-`BASELINE_FRAC_INCR);
	    else if (ADCLONG < LBASELINE - `ONE_HALF)
	      LBASELINE <= LBASELINE - 
			   (2 << `BASELINE_FRAC_WIDTH-`BASELINE_FRAC_INCR);
	    else if  (ADCLONG < LBASELINE)
	      LBASELINE <= LBASELINE - 
			   (1 << `BASELINE_FRAC_WIDTH-`BASELINE_FRAC_INCR);
	    CBASELINE <= LBASELINE;
	    
	    // Round returned baseline to nearest integer for peak calc.
	    // Return baseline with SHWR_BASELINE_EXTR_BITS.
	    RBASELINE <= (LBASELINE + `ONE_HALF) >> `BASELINE_FRAC_WIDTH;
	    BASELINE <= (LBASELINE + 
			(1 << (`BASELINE_FRAC_WIDTH
			       - `SHWR_BASELINE_EXTRA_BITS-1)))
	      >> (`BASELINE_FRAC_WIDTH-`SHWR_BASELINE_EXTRA_BITS);
	    SBASELINE <= (LBASELINE + 
			 (1 << (`BASELINE_FRAC_WIDTH
				- `SHWR_BASELINE_EXTRA_BITS-1)))
	      >> (`BASELINE_FRAC_WIDTH-`SHWR_BASELINE_EXTRA_BITS);
	 end
	 
	 if (TRIGGERED) begin
	    BIN_COUNT <= BIN_COUNT+1;
	    if (BIN_COUNT <= `SHWR_AREA_BINS) begin

	       // Accumulate baseline sag -- need to do this one step
	       // also ignore recovery.
	       if (ADCLONG > LBASELINE) begin
		  if (HILO == 0)  // Low gain channel
		    begin
		       SAG1 <= (ADCLONG - LBASELINE) >> `BASELINEL_SAG_SHIFT1;
		       SAG2 <= (ADCLONG - LBASELINE) >> `BASELINEL_SAG_SHIFT2;
		       SAG3 <= (ADCLONG - LBASELINE) >> `BASELINEL_SAG_SHIFT3;
		    end 
		  else begin  // High gain channel
		     SAG1 <= (ADCLONG - LBASELINE) >> `BASELINEH_SAG_SHIFT1;
		     SAG2 <= (ADCLONG - LBASELINE) >> `BASELINEH_SAG_SHIFT2;
		     SAG3 <= (ADCLONG - LBASELINE) >> `BASELINEH_SAG_SHIFT3;
		  end
		  SAG <= SAG + (SAG1 + SAG2 + SAG3);
	       end
               CBASELINE <= LBASELINE - SAG; // Simple version if speed problem.
	       // CBASELINE <= LBASELINE  
	       //  	    - (SAG + (((ADCLONG - LBASELINE) 
	       //  		       >> `BASELINE_SAG_SHIFT1)
	       //  		      - ((ADCLONG - LBASELINE) 
	       //  			 >> `BASELINE_SAG_SHIFT2)));
	       SBASELINE <= (CBASELINE +
			     (1 << (`BASELINE_FRAC_WIDTH
				    - `SHWR_BASELINE_EXTRA_BITS-1)))
		 >> (`BASELINE_FRAC_WIDTH-`SHWR_BASELINE_EXTRA_BITS);
	       
	       // Look for peak.
	       if ((ADCD > LPEAK) && (ADCD > RBASELINE)) begin
		  LPEAK <= ADCD;
		  if (ADCD >= `SHWR_SATURATED_LEVEL) SATURATED <= 1;
//		  PEAK <= ADCD - RBASELINE;  // Simple version if speed prob.
		  PEAK <= ADCD 
			  - ((LBASELINE + `ONE_HALF - SAG)
			     >> `BASELINE_FRAC_WIDTH);
	       end
		   
	       // Accumulate integral, making sure it can't go negative.
               ADCLONGD1 <= ADCLONG; // Delay to keep in sync with CBASELINE
               ADCLONGD <= ADCLONGD1;
		 INTEGRALA <= INTEGRALA + (ADCLONGD - CBASELINE);
	       if (INTEGRALA[`SHWR_AREA_WIDTH+`BASELINE_FRAC_WIDTH-1] == 0)
		 INTEGRAL <= INTEGRALA >> `BASELINE_FRAC_WIDTH;
	       else
		 INTEGRAL <= 0;
	    end	      
	 end
      end // if (RESET)
   end // always @ (posedge CLK120)
   
endmodule

