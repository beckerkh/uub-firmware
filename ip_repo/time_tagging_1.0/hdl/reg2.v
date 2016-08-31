`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date:    11:59:11 03/26/2009 
// Design Name: 
// Module Name:    Reg1 
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
module reg2( clk, reset, ce, regin, regout);
    input clk, reset, ce;
	 input [31:0] regin;
    output reg [31:0] regout;
	 
   always @(posedge clk or posedge reset)
		if (reset) begin
			regout <= 1'b0;
		end else if (ce) begin
			regout <= regin;
		end

endmodule
