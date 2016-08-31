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
module Counter_27wld( clk, regD, res, load);
	input clk;
	input	res;
	input load;
	output [26:0] regD;
	//reg [31:0] regD;
   
	reg [26:0] count;
	always @(posedge clk or posedge res)
		if (res)
			count <= 0;
		else if (load)
			count <= 1;
		else
			count <= count + 1;
	assign regD = count;

endmodule
