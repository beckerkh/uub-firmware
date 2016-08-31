`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date:    14:03:49 03/26/2009 
// Design Name: 
// Module Name:    DQ1 
// Project Name: 
// Target Devices: 
// Tool versions: 
// Description: 
//
// Dependencies: 
//
// Revision: 
// Revision 0.01 - File Created
// Additional Comments: 
//
//////////////////////////////////////////////////////////////////////////////////
module dff( clk, d1, q1, reset);
	input clk, d1;
	input	reset;
	output reg q1;
	//reg Q1;
	
	// Reset on low, asynchronous
   always @(posedge clk or posedge reset)
      if (reset) begin
         q1 <= 1'b0;
      end else begin
         q1 <= d1;
      end
	
endmodule
