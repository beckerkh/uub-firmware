`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date:    13:14:11 05/07/2013 
// Design Name: 
// Module Name:    ANDgate 
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
module andg(
    output signal1,
    input signal2,
    input signal3
    );


 assign  signal1 = signal2 & signal3;
					

endmodule
