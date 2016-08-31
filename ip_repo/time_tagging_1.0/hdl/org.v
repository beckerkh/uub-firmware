`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date:    13:14:11 04/29/2014 
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
module org(
    output signal3,
    input signal1,
    input signal2
    );


 assign  signal3 = signal1 | signal2;
					

endmodule
