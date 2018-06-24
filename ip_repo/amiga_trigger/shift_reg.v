`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 03.04.2017 13:40:40
// Design Name: 
// Module Name: ShiftReg
// Project Name: 
// Target Devices: 
// Tool Versions: 
// Description: data stream is DUMMY_BITS HEADER_BITS DATA_BITS
// *DUMMY bits: used to avoid line transient before reading HEADER and DATA bits
// Dependencies: 
// 
// Revision:
// Revision 0.01 - File Created
// Additional Comments:
// 
//////////////////////////////////////////////////////////////////////////////////

module shift_register #(parameter NBITS=16, parameter DENOMINATOR = 12) (
    input clock,
    input aresetn,
    input load,
    input [NBITS-1:0] lts_in,
    output wire lts_out,
    output reg busy,
    output clock_out
);

    localparam AMIGA_HDR = 5'b11000;
    localparam AMIGA_HDR_LEN = 5;
    localparam DUMMY_BITS_LEN = 0;
    localparam DUMMY_BITS = 1'b0;

    localparam HALF_CYCLE = DENOMINATOR/2;
    localparam CLOCK_OUT_IDLE = 1'b1;

    reg [AMIGA_HDR_LEN-1:0] header = AMIGA_HDR;
    reg [NBITS-1:0] shift_reg;

    localparam WAIT_AND_LOAD = 1'b0,
               SEND_DATA     = 1'b1;

    reg state = WAIT_AND_LOAD, next_state;
    reg next_busy;
    reg clock_10M, next_clock_10M;
    reg [3:0] counter, next_counter;
    reg [5:0] out_counter, next_out_counter;
    reg datain_mc, next_datain_mc;
    reg enable_mc = 1'b0; // 1: enabled, 0: disabled
    wire dataout_mc;
    
    assign lts_out = (dataout_mc & busy) | !busy;
    assign clock_out = clock_10M;

    // instantiate manchester coding module     
    manchester_coding MC (
        .DataIn(datain_mc),
        .clock(clock_10M),
        .enable(enable_mc),
        .DataOut(dataout_mc)
    );
    
    always @(posedge clock) begin
        if(!aresetn) begin
            state <= WAIT_AND_LOAD;
            busy <= 1'b0;
            clock_10M <= CLOCK_OUT_IDLE;
            datain_mc <= 1'b1;
            counter <= 0;
            out_counter <= 0;
        end
        else begin
            state <= next_state;
            datain_mc <= next_datain_mc;
            busy <= next_busy;
            clock_10M <= next_clock_10M;
            counter <= next_counter;
            out_counter <= next_out_counter;
            if (load && !busy)
                shift_reg <= lts_in;
        end
    end

    always @(*) begin
        next_state = state;
        next_datain_mc = datain_mc;
        next_busy = busy;
        next_clock_10M = clock_10M;
        next_counter = counter;
        next_out_counter = out_counter;
        case(state)
            WAIT_AND_LOAD: begin
                if(load) begin
                    next_state = SEND_DATA;
                    next_busy = 1'b1;
					if (DUMMY_BITS_LEN)
                    	next_datain_mc = DUMMY_BITS;
                    else
                   		next_datain_mc = header[AMIGA_HDR_LEN-1];
                    next_clock_10M = !CLOCK_OUT_IDLE;
                end
                else begin
                    next_busy = 1'b0;
                    next_out_counter = 0;
                end
            end
            SEND_DATA: begin
                next_counter = counter + 1;
                if (counter == (HALF_CYCLE-1)) begin // toggle clock_10M
                    next_counter = 0;
                    if (out_counter < NBITS+AMIGA_HDR_LEN+DUMMY_BITS_LEN) begin
                        next_clock_10M = !clock_10M;
                        if (next_clock_10M == CLOCK_OUT_IDLE) begin 
							next_out_counter = out_counter + 1; // increment counter for next data bit
						end
						else begin //update output on falling edges
							if (out_counter < DUMMY_BITS_LEN) begin
								next_datain_mc = DUMMY_BITS;
							end
							else begin
								if (out_counter < AMIGA_HDR_LEN+DUMMY_BITS_LEN)
									next_datain_mc = header [(AMIGA_HDR_LEN-1) - (out_counter-DUMMY_BITS_LEN)]; // HEADER
								else
									next_datain_mc = shift_reg [(NBITS-1) - (out_counter-AMIGA_HDR_LEN-DUMMY_BITS_LEN)]; // DATA
							end
						end
					end
					else begin
						next_state = WAIT_AND_LOAD;
						next_clock_10M = CLOCK_OUT_IDLE; 
						next_busy = 1'b0;
					end
                end
            end
            default: begin
                next_state = WAIT_AND_LOAD;
                next_busy = 1'b0;
            end
        endcase
    end    
endmodule
