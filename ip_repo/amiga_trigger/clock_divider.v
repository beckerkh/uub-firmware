`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 02.05.2017 15:16:08
// Design Name: 
// Module Name: clock_divider
// Project Name: 
// Target Devices: 
// Tool Versions: 
// Description: 
// 
// Dependencies: 
// 
// Revision:
// Revision 0.01 - File Created
// Additional Comments:
// 
//////////////////////////////////////////////////////////////////////////////////


module clock_divider (
    input clock,
    input resetn,
    input enable,
    output reg clock_out
    );

    parameter DENOMINATOR = 12; /* DENOMINATOR: should be an even number */
    parameter OUT_PULSES_QTY = 24;

    localparam HALF_CYCLE = DENOMINATOR/2;
    localparam COUNT_BITS = HALF_CYCLE;

    reg [COUNT_BITS-1:0] counter;    

    always @(posedge clock) begin
        if(!resetn) begin
            counter <= 0;
            clock_out <= 1'b0;
        end
        else begin
            if(enable == 1) begin
                counter <= counter + 1;
                if(counter == 0) begin 
                    clock_out <= ~clock_out;
                end
                if(counter == HALF_CYCLE) begin
                    counter <= 1;
                    clock_out <= ~clock_out;
                end
            end
            else begin
                counter <= 0;
                clock_out <= 1'b0;
            end
        end
    end

endmodule