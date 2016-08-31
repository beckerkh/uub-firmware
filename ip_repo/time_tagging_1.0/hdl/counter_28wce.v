`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company:CWRU 
// Engineer: Bob Sobin
// 
// Create Date:    13:27:11 07/03/2014 
// Design Name: 
// Module Name:     
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
module Counter_28wce( clk, regD, res, ce);
	input clk, ce;
	input	res;
	output [27:0] regD;
	//reg [31:0] regD;
   
	reg [27:0] count;
	always @(posedge clk or posedge res)
		if (res) begin
			count <= 0;
		end else if (ce) begin
			count <= count + 1;
		end
	assign regD = count;

endmodule
