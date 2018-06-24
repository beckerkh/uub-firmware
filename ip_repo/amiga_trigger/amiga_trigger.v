`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 24.04.2017 13:39:05
// Design Name: 
// Module Name: amiga_trigger
// Project Name: 
// Target Devices: 
// Tool Versions: 
// Description: latch lts_in when trigger is active, then hamming encode and send
// via shift_reg. This module is capable of retain one extra lts_in when shif_reg 
// is busy and send as soon as possible.
// -padding bits are used to send 24 bits to the shift register. The use of this bits
// is optional
// 
// Dependencies: 
// 
// Revision:
// Revision 0.01 - File Created
// Additional Comments:
// 
//////////////////////////////////////////////////////////////////////////////////


module amiga_trigger #(parameter DENOMINATOR=12) (
    input clock_120M,
    input resetn,
    input trigger,
    input [15:0] lts_in,
    output reg busy,
    output clock_out,
    output lts_out
);

    localparam LTS_LEN = 16;
    localparam HAMM_ENC_LEN = LTS_LEN + 6; //LTS bits + 6 PARITY bits
//	localparam PADDING_BITS_LEN = 2;
	localparam PADDING_BITS_LEN = 0;
	localparam PADDING_BITS = 1'b0;
			   
    localparam WAIT_DATA = 2'b00,
               LOAD_SR   = 2'b01,
               SEND_DATA = 2'b10;


    function [HAMM_ENC_LEN-1:0] hamming_encoder;
        input [LTS_LEN-1:0] in;
        reg P1, P2, P4, P8, P16, P;
        reg [20:0] aux;
        begin
            P1 = in[0]^in[1]^in[3]^in[4]^in[6]^in[8]^in[10]^in[11]^in[13]^in[15];
            P2 = in[0]^in[2]^in[3]^in[5]^in[6]^in[9]^in[10]^in[12]^in[13];
            P4 = in[1]^in[2]^in[3]^in[7]^in[8]^in[9]^in[10]^in[14]^in[15];
            P8 = in[4]^in[5]^in[6]^in[7]^in[8]^in[9]^in[10];
            P16 = in[11]^in[12]^in[13]^in[14]^in[15];
            aux = {in[15:11], P16, in[10:4], P8, in[3:1], P4, in[0], P2, P1}; 
            P = ^aux[20:0];
            hamming_encoder = {P, aux[20:0]};
        end
    endfunction


    reg [1:0] state = WAIT_DATA;
    reg [1:0] next_state;
    reg next_load_sr;
    reg next_busy;
    reg [LTS_LEN-1:0] latch;
    reg [LTS_LEN-1:0] last_lts;
    reg load_sr;

    wire busy_sr;


    // instantiate shift register module 
    shift_register #(.NBITS(HAMM_ENC_LEN+PADDING_BITS_LEN), .DENOMINATOR(DENOMINATOR)) SR (
        .clock(clock_120M),
        .aresetn(resetn),
        .load(load_sr),
        .lts_in({{PADDING_BITS_LEN{PADDING_BITS}} , hamming_encoder(latch)}),
        .lts_out(lts_out),
        .busy(busy_sr),
        .clock_out(clock_out)
    );

    always @(posedge trigger or negedge resetn) begin
        if (!resetn)
            latch <= 0;
        else
            if(trigger) begin
                latch <= lts_in;
            end
    end

    always @(posedge clock_120M) begin
        if(!resetn) begin
            state <= WAIT_DATA;
            load_sr <= 0;
            busy <= 0;
            last_lts <= 0;
        end
        else begin
            state <= next_state;
            busy <= next_busy;
            load_sr <= next_load_sr;
            if (!busy && (last_lts != latch)) //only update last_lts when not sending data
                last_lts <= latch;
        end
    end
    
    always @(*) begin
        next_state = state;
        next_load_sr = load_sr;
        next_busy = busy;
        case (state)
            WAIT_DATA: begin
                if (last_lts != latch) begin 
                    next_state = LOAD_SR;
                    next_busy = 1;
                end
            end
            LOAD_SR: begin
                if (!busy_sr) begin
                    next_state = LOAD_SR;
                    next_load_sr = 1;
                end
                else begin
                    next_state = SEND_DATA;
                    next_load_sr = 0;
                end
            end
            SEND_DATA: begin
                if (busy_sr) begin
                    next_state = SEND_DATA;
                end
                else begin
                    next_state = WAIT_DATA;
                    next_busy = 0;
                end
            end
            default: begin
                next_state = WAIT_DATA;
                next_load_sr = 0;
                next_busy = 0;
            end
        endcase
    end


endmodule
