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
//Added in2 and in3 on 7_15_2016
// Created mux_4 from mux_8, input was [2:0], is [1:0], added 4_7_2016
//////////////////////////////////////////////////////////////////////////////////
module mux_4(in,in0,in1,in2,in3,out); 
	input [31:0] in0,in1,in2,in3;
    output reg [31:0] out;
	input [1:0] in; 
	 
   always @(in or in0 or in1 or in2 or in3)
				case (in)
					2'b00 : out = in0;
					2'b01 : out = in1;
					2'b10 : out = in2;//added 7_16_2015
					2'b11 : out = in3;//added 7_16_2015
					default : out = 32'h00000000;
				endcase

endmodule
