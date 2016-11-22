// shwr_integral.v
//
// This keeps track of the integral of a shower signal.
//
// 18-Nov-2016 DFN Initial version

`include "sde_trigger_defs.vh"

module shwr_integral(
		     input RESET,
		     input CLK120,
		     input [`ADC_WIDTH-1:0] ADC,
		     input TRIGGERED,
		     output reg[`SHWR_AREA_WIDTH+`SHWR_AREA_FRAC_WIDTH-1:0] INTEGRAL,
		     output reg[`ADC_WIDTH+`SHWR_AREA_FRAC_WIDTH-1:0] BASELINE,
		     output reg[`ADC_WIDTH-1:0] PEAK,
		     output reg SATURATED
		     );
   
   reg [7:0] 					BIN_COUNT;
   reg [`SHWR_AREA_WIDTH+`SHWR_AREA_FRAC_WIDTH-1:0] INTEGRALA;
   reg [`ADC_WIDTH-1:0] 			    ADCL;
   reg [`ADC_WIDTH-1:0] 			    LPEAK;
   
   always @(posedge CLK120) begin
      if (RESET) begin
         INTEGRAL <= 0;
	 INTEGRALA <= 0;
	 BASELINE <= 250 << `SHWR_AREA_FRAC_WIDTH;
	 BIN_COUNT <= 0;
	 LPEAK <= 0;
	 SATURATED <= 0;
      end
      else begin

	 if (!TRIGGERED) begin
	    BIN_COUNT <= 0;
	    INTEGRAL <= 0;
	    INTEGRALA <= 0;
	    LPEAK <= 0;
	    // Keep running track of Baseline with FRAC_WIDTH time constant
	    if (ADC > BASELINE[`ADC_WIDTH+`SHWR_AREA_FRAC_WIDTH-1:
			       `SHWR_AREA_FRAC_WIDTH])
	      BASELINE <= BASELINE+1;
	    else
	      BASELINE <= BASELINE-1;
	 end

	 if (TRIGGERED) begin
	    BIN_COUNT <= BIN_COUNT+1;
	    if (BIN_COUNT <= `SHWR_AREA_BINS) begin

	       // Look for peak
	       if (ADC > LPEAK) LPEAK <= ADC;
	       if (ADC >= `SHWR_SATURATED_LEVEL) SATURATED <= 1;
	       PEAK <= LPEAK - BASELINE;
	       
	       // Accumulate integral
	       INTEGRALA[`SHWR_AREA_WIDTH+`SHWR_AREA_FRAC_WIDTH-1:
			 `SHWR_AREA_FRAC_WIDTH] 
		 <= INTEGRAL[`SHWR_AREA_WIDTH+`SHWR_AREA_FRAC_WIDTH-1:
			     `SHWR_AREA_FRAC_WIDTH] + ADC;
	       ADCL <= ADC;
//	       if (INTEGRALA <= BASELINE)
//		 INTEGRAL <= 0;
//	       else
//	       INTEGRAL <= INTEGRALA - BASELINE;
	       if (ADCL > BASELINE)
		 INTEGRAL <= INTEGRALA - BASELINE;
	    end	      
	 end
      end // if (RESET)
   end // always @ (posedge CLK120)
   
endmodule

