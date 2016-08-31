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
module reg1( clk, reset, regin, regout);
    input clk, reset;
	 input [31:0] regin;
    output reg [31:0] regout;
	 
   always @(posedge clk or posedge reset)
		if (reset) begin
			regout <= 1'b0;
		end else begin
			regout <= regin;
		end

endmodule
