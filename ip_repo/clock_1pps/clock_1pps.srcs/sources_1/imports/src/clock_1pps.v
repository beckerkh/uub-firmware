// Module to fake 1pps signal from the GPS.
//
// 18-Dec-2015 DFN Initial version
//

module clock_1pps(
		  CLK120,
		  CLK1PPS
		  );

   input 		       CLK120;
   output reg                  CLK1PPS;
   reg [31:0]                  CLK_CTR;
   
   always @(posedge CLK120) begin
      CLK_CTR <= CLK_CTR+1;
      if (CLK_CTR >= 120000000)
        CLK_CTR <= 0;
      if (CLK_CTR >= 119000000)
        CLK1PPS <= 1;
      else
        CLK1PPS <= 0;
   end

endmodule // clock_1pps

