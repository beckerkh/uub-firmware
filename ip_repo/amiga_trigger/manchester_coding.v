`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 26.05.2017 09:25:19
// Design Name: 
// Module Name: manchester_coding
// Project Name: 
// Target Devices: 
// Tool Versions: 
// Description: data transition occurs in falling clock edge.
//      DataIn: HIGH: 0 to 1
//      DataIn: LOW: 0 to 1
// Dependencies: 
// 
// Revision:
// Revision 0.01 - File Created
// Additional Comments:
// 
//////////////////////////////////////////////////////////////////////////////////


module manchester_coding (
    input DataIn,
    input clock,
    input enable,
    output reg DataOut
);

    always @(*) begin
    	if(enable)
    		DataOut = DataIn ^ clock;
    	else
    		DataOut = DataIn;
    end
endmodule
