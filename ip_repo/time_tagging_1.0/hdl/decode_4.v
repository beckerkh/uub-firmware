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
//Added two additional cases 7_16_2015
// Created decode_4 from decode_8, input was [2:0], is [1:0], added 4_7_2016
//////////////////////////////////////////////////////////////////////////////////
module decode_4( clk,reset,in,regout);
    input clk, reset;
	 input [1:0] in;
    output reg [3:0] regout;
	 
   always @(posedge clk or posedge reset)
		if (reset) begin
			regout <= 4'h0;
		end else
				case (in)
					2'b00 : regout <= 4'h1;
					2'b01 : regout <= 4'h2;
					2'b10 : regout <= 4'h4;//added 7_16_2015
					2'b11 : regout <= 4'h8;//added 7_16_2015
					default : regout <= 4'h0;
				endcase

endmodule
