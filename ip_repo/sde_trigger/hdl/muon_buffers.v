// muon_buffers.v
//
// This module implements the loading of data into muon buffers.
//
// 02-Jun-2016 DFN Initial version
// 04-Oct-2016 DFN Add 2 cycle delay to MUON_BUF_WNUM to compensate for
//                 delays in time tagging module.
// 18-Nov-2016 DFN Add mode to alternate SIPM data stored in buffer
// 03-Mar-2017 DFN Fix bug that prevented SIPM cal data from being stored

`include "sde_trigger_defs.vh"

module muon_buffers(
                    input RESET,
		    input AXI_CLK,
                    input CLK120,
                    input [`ADC_WIDTH-1:0] ADC0,
		    input [`ADC_WIDTH-1:0] ADC1,
		    input [`ADC_WIDTH-1:0] ADC2,
		    input [`ADC_WIDTH-1:0] ADC_CAL,
		    input [`ADC_WIDTH-1:0] ADC_SSD,
		    input [`MUON_NUM_TRIGS-1:0] MUON_TRIG_IN,
                    input [31:0] MUON_BUF_CONTROL,
                    input [31:0] MUON_BUF_TRIG_MASK,
                    input AXI_MUON_CONTROL_WRITTEN,
                    input AXI_REG_WRITE,
		    output reg MUON_INTR,
                    output reg MUON_TRIGGER,
                    output reg MUON_ENB,
                    output reg [`MUON_BUF_NUM_WIDTH-1:0] MUON_BUF_WNUM,
                    output reg [`MUON_BUF_NUM_WIDTH-1:0] MUON_BUF_RNUM,
                    output reg [`MUON_EVT_CTR_WIDTH-1:0] MUON_EVT_CTR,
                    output reg [`MUON_MEM_WIDTH-1:0] MUON_DATA0,
                    output reg [`MUON_MEM_WIDTH-1:0] MUON_DATA1,
                    output reg [`MUON_MEM_ADDR_WIDTH-1:0] MUON_ADDR,
                    output wire [31:0] MUON_BUF_TIME_TAG_A,
                    output wire [31:0] MUON_BUF_TIME_TAG_B,
                    output wire [31:0] MUON_BUF_STATUS,
                    output wire [31:0] MUON_BUF_WORD_COUNT,
                    output reg [4:0] DEBUG
		    );

   reg [30:0]                        TIME_TAG;
   reg [31:0]                        LCL_MUON_BUF_STATUS;
   reg [31:0]                        LCL_MUON_BUF_TIME_TAG_A[0:`MUON_MEM_NBUF-1];;
   reg [31:0]                        LCL_MUON_BUF_TIME_TAG_B[0:`MUON_MEM_NBUF-1];;
   reg [`MUON_BUF_NUM_WIDTH:0]       MUON_BUF_NUM_FULL;
   reg [`MUON_MEM_NBUF-1:0]          MUON_BUF_FULL_FLAGS;
   reg [`MUON_MEM_ADDR_WIDTH-1:0]    LCL_MUON_BUF_WORD_COUNTN[0:`MUON_MEM_NBUF-1];
   reg [`MUON_MEM_ADDR_WIDTH-1:0]    LCL_MUON_BUF_WORD_COUNT;
   reg [`MUON_BUF_NUM_WIDTH-1:0]     LCL_MUON_BUF_WNUM;
   reg [`MUON_BUF_NUM_WIDTH-1:0]     TMP_MUON_BUF_WNUM;
   
   reg [31:0]                        WORD_COUNT;
   reg [7:0]                         MUON_BURST;
   reg [7:0]                         MUON_COUNT;
   wire [31:0]                       LCL_MUON_BUF_CONTROL;
   wire                              LCL_MUON_CONTROL_WRITTEN;
   reg                               PREV_MUON_CONTROL_WRITTEN;
   reg [`MUON_MEM_NBUF-1:0]          MUON_BUF_RESET;
   reg [31:0]                        DATA0_PIPELINE[0:`MUON_TRIG_PIPELINE_DLY];
   reg [31:0]                        DATA1_PIPELINE[0:`MUON_TRIG_PIPELINE_DLY];
   reg [31:0]                        MUON_DATA0_TMP0, MUON_DATA0_TMP1;
   reg [31:0]                        MUON_DATA1_TMP0, MUON_DATA1_TMP1;
   reg [`MUON_MEM_ADDR_WIDTH-1:0]    MUON_ADDRT;
   reg                               MUON_ENBT;
   reg                               NEW_BUF;
   reg 				     SIPM_CAL;
   
   integer                           INDEX;
   
   always @(posedge CLK120) begin
      if (RESET) begin
         LCL_MUON_BUF_WNUM <= 0;
         MUON_BUF_RNUM <= 0;
         MUON_BUF_FULL_FLAGS <= 0;
         MUON_BUF_NUM_FULL <= 0;
         MUON_EVT_CTR <= 0;
         MUON_ADDRT <= 0;
         MUON_DATA0 <= 0;
         MUON_DATA1 <= 0;
         MUON_ENBT <= 0;
         MUON_INTR <= 0;
         MUON_TRIGGER <= 0;
         WORD_COUNT <= 0;
         MUON_BURST <= 0;
         MUON_COUNT <= 0;
         TIME_TAG <= 0;
         LCL_MUON_BUF_TIME_TAG_A[0] <= 0;
         NEW_BUF <= 0;
	 SIPM_CAL <= 0;
      end
      else begin

         // Delay MUON_BUF_WNUM by 2 clock cycles.
         MUON_BUF_WNUM <= TMP_MUON_BUF_WNUM;
         TMP_MUON_BUF_WNUM <= LCL_MUON_BUF_WNUM;
         
         if (MUON_TRIGGER == 1) MUON_TRIGGER <= 0;  // Reset after one clock
         TIME_TAG <= TIME_TAG+1;
         LCL_MUON_BUF_WORD_COUNT <=  LCL_MUON_BUF_WORD_COUNTN[MUON_BUF_RNUM];
         DATA0_PIPELINE[0] <= ADC0 | (ADC1 << 16);
	 if (SIPM_CAL == 0)
	   DATA1_PIPELINE[0] <= ADC2 | (ADC_SSD << 16);
	 else
	   DATA1_PIPELINE[0] <= ADC2 | (ADC_CAL << 16);
         for (INDEX=1; INDEX<=`MUON_TRIG_PIPELINE_DLY; INDEX=INDEX+1) begin
            DATA0_PIPELINE[INDEX] <= DATA0_PIPELINE[INDEX-1];
            DATA1_PIPELINE[INDEX] <= DATA1_PIPELINE[INDEX-1];
         end
         MUON_DATA0_TMP0 <= DATA0_PIPELINE[`MUON_TRIG_PIPELINE_DLY];
         MUON_DATA1_TMP0 <= DATA1_PIPELINE[`MUON_TRIG_PIPELINE_DLY];
         MUON_DATA0 <= MUON_DATA0_TMP1;
         MUON_DATA1 <= MUON_DATA1_TMP1;
         MUON_ENB <= MUON_ENBT;
         MUON_ADDR <= MUON_ADDRT;
         
         // Do we have a free buffer? If not, we can't process this trigger.
         // Note that the way this is implemented here we can only fill n-1 of
         // the buffers, or we'll end up overwriting the oldest buffer. So this
         // simple logic will not work well for only 2 buffers. To use all the
         // buffers we would have to turn off the write enable to the dual
         // ported memory if all buffers were full. Let's keep the simple logic
         // for now.
         if (MUON_BUF_NUM_FULL < `MUON_MEM_NBUF-1) begin
            if (MUON_BURST == 0) begin
               if ((MUON_BUF_TRIG_MASK & MUON_TRIG_IN) != 0) begin
	          MUON_BURST <= 1;
                  MUON_ENBT <= 1;
                  MUON_DATA0_TMP1 <= TIME_TAG | 'h80000000;
                  MUON_DATA1_TMP1 <= (MUON_TRIG_IN & MUON_BUF_TRIG_MASK) |
				     ((SIPM_CAL << `MUON_BUF_SIPM_CAL_SHIFT) 
				      & MUON_BUF_TRIG_MASK) |
                                     'h80000000;
                  if (MUON_ADDRT[`MUON_MEM_BUF_SHIFT-1:0] !=  0) begin
                     MUON_ADDRT <= MUON_ADDRT+4;
                     WORD_COUNT <= WORD_COUNT+1;
                  end
                  MUON_COUNT <= MUON_COUNT+1;
                  
               end // if ((MUON_BUF_TRIG_MASK & MUON_TRIG_IN) != 0)
            end  // if (MUON_BURST == 0)

            // We are in the midst of putting muon data in the buffer 
            else if (MUON_BURST < `MUON_BURST_LEN) begin
               MUON_BURST <= MUON_BURST+1;
               MUON_ADDRT <= MUON_ADDRT+4;
               WORD_COUNT <= WORD_COUNT+1;
               
               MUON_DATA0_TMP1[`ADC_WIDTH-1:0] 
                 <= MUON_DATA0_TMP0[`ADC_WIDTH-1:0];
               MUON_DATA0_TMP1[15:`ADC_WIDTH] <=  'hf;
               MUON_DATA0_TMP1[15+`ADC_WIDTH:16] 
                 <= MUON_DATA0_TMP0[15+`ADC_WIDTH:16];
               MUON_DATA0_TMP1[31:16+`ADC_WIDTH] <=  'h7;

               MUON_DATA1_TMP1[`ADC_WIDTH-1:0] 
                 <= MUON_DATA1_TMP0[`ADC_WIDTH-1:0];
               MUON_DATA1_TMP1[15:`ADC_WIDTH] <= MUON_BURST & 'hf;
               MUON_DATA1_TMP1[15+`ADC_WIDTH:16] 
                 <= MUON_DATA1_TMP0[15+`ADC_WIDTH:16];
               MUON_DATA1_TMP1[31:16+`ADC_WIDTH] <= (MUON_BURST >> 4) & 'h7;
            end // if (MUON_BURST < `MUON_BURST_LEN)
            
            // End of muon burst
            else if (MUON_BURST >= `MUON_BURST_LEN) begin
               MUON_BURST <= 0;
               MUON_ENBT <= 0;
	       if ((MUON_BUF_TRIG_MASK & `MUON_BUF_SIPM_CAL) != 0)
		 SIPM_CAL <= ~SIPM_CAL;
               // Full muon buffer?
               if (WORD_COUNT >= `MUON_MEM_WORDS-`MUON_BURST_LEN-2) 
                 begin
                    // Mark buffer as full and switch to the next one
	            MUON_BUF_FULL_FLAGS <= MUON_BUF_FULL_FLAGS |
                                           (1<<LCL_MUON_BUF_WNUM);
 	            MUON_BUF_NUM_FULL <= MUON_BUF_NUM_FULL+1;

                    MUON_INTR <= 1;
                    MUON_TRIGGER <= 1;
                    WORD_COUNT <= 0;
                    MUON_COUNT <= 0;
                    NEW_BUF <= 1;
                    
                    // Save number of words in buffer
                    LCL_MUON_BUF_WORD_COUNTN[LCL_MUON_BUF_WNUM] <= WORD_COUNT+1;
                    // Save time at end of buffer
                    LCL_MUON_BUF_TIME_TAG_B[LCL_MUON_BUF_WNUM] <= TIME_TAG;
                    
                    LCL_MUON_BUF_WNUM <= LCL_MUON_BUF_WNUM+1;
                    MUON_ADDRT[`MUON_MEM_BUF_SHIFT-1:0] <=  0;
                    MUON_ADDRT[`MUON_MEM_BUF_SHIFT+`MUON_BUF_NUM_WIDTH-1:
                               `MUON_MEM_BUF_SHIFT] <= LCL_MUON_BUF_WNUM+1;
                end // if (WORD_COUNT >= `MUON_MEM_WORDS-`MUON_BURST_LEN-2)
            end // if (MUON_BURST >= `MUON_BURST_LEN) 

            // Beginning of new buffer
            if (NEW_BUF)
              begin
                 // Time at beginning of next buffer
                 LCL_MUON_BUF_TIME_TAG_A[LCL_MUON_BUF_WNUM] <= TIME_TAG;
                 NEW_BUF <= 0;
              end // if (NEW_BUF)
         end // if (MUON_BUF_NUM_FULL < `MUON_MEM_NBUF-1)

         // Process clearing of muon buff full flag
         PREV_MUON_CONTROL_WRITTEN <= LCL_MUON_CONTROL_WRITTEN;
         if (PREV_MUON_CONTROL_WRITTEN && !LCL_MUON_CONTROL_WRITTEN)
           begin
              MUON_BUF_RESET  
                = (1<<LCL_MUON_BUF_CONTROL) & MUON_BUF_FULL_FLAGS;
              if ((MUON_BUF_RESET != 0) 
                  && (LCL_MUON_BUF_CONTROL == MUON_BUF_RNUM))
                begin
                   MUON_BUF_FULL_FLAGS <= MUON_BUF_FULL_FLAGS & 
                                          ~MUON_BUF_RESET;
                   MUON_BUF_NUM_FULL <= MUON_BUF_NUM_FULL-1;
                   MUON_BUF_RNUM <= MUON_BUF_RNUM+1;
                   // Reset muon intr pending if resetting last filled buffer
	           if (MUON_BUF_NUM_FULL == 1) MUON_INTR <= 0;
                end
           end // if (PREV_MUON_CONTROL_WRITTEN && !LCL_MUON_CONTROL_WRITTEN)
         else
           MUON_BUF_RESET <= 0;
         

         // Process loading of status registers from internal registers
         LCL_MUON_BUF_STATUS[`MUON_BUF_WNUM_SHIFT+`MUON_BUF_NUM_WIDTH-1:
                             `MUON_BUF_WNUM_SHIFT] <= LCL_MUON_BUF_WNUM;
         LCL_MUON_BUF_STATUS[`MUON_BUF_RNUM_SHIFT+`MUON_BUF_NUM_WIDTH-1:
                             `MUON_BUF_RNUM_SHIFT] <= MUON_BUF_RNUM;
         LCL_MUON_BUF_STATUS[`MUON_BUF_FULL_SHIFT+`MUON_MEM_NBUF-1:
                             `MUON_BUF_FULL_SHIFT] <= MUON_BUF_FULL_FLAGS;
         LCL_MUON_BUF_STATUS[`MUON_INTR_PEND_SHIFT:
                             `MUON_INTR_PEND_SHIFT] <= MUON_INTR;
         LCL_MUON_BUF_STATUS[`MUON_BUF_NFULL_SHIFT+`MUON_BUF_NUM_WIDTH:
                             `MUON_BUF_NFULL_SHIFT] <= MUON_BUF_NUM_FULL;
         LCL_MUON_BUF_STATUS[31:`MUON_BUF_NOTUSED_SHIFT] <= -1;

         // Debug outputs
         DEBUG[0] <= MUON_ADDR[`MUON_MEM_BUF_SHIFT];
         DEBUG[1] <= WORD_COUNT[0];
         DEBUG[2] <= MUON_COUNT[0];
         DEBUG[3] <= MUON_DATA1[`ADC_WIDTH];
         DEBUG[4] <= MUON_INTR;
         
      end // if (RESET)

   end // always @ (posedge CLK120)

   // Synchronization with AXI registers for cases where glitches would be
   // problematic.  This should not be necessary for semi-static control 
   // registers.

   synchronizer_32bit muon_buf_control_sync(.ASYNC_IN(MUON_BUF_CONTROL),
                                            .CLK(CLK120),
                                            .SYNC_OUT(LCL_MUON_BUF_CONTROL));
   synchronizer_1bit control_written_sync(.ASYNC_IN(AXI_MUON_CONTROL_WRITTEN),
                                          .CLK(CLK120),
                                          .SYNC_OUT(LCL_MUON_CONTROL_WRITTEN));
   synchronizer_32bit muon_buf_status_sync
     (.ASYNC_IN(LCL_MUON_BUF_STATUS),
      .CLK(AXI_CLK),.SYNC_OUT(MUON_BUF_STATUS));
   synchronizer_32bit muon_buf_wc_sync
     (.ASYNC_IN(LCL_MUON_BUF_WORD_COUNT),
      .CLK(AXI_CLK),.SYNC_OUT(MUON_BUF_WORD_COUNT));
   synchronizer_32bit muon_buf_start_sync
     (.ASYNC_IN(LCL_MUON_BUF_TIME_TAG_A[MUON_BUF_RNUM]),
      .CLK(AXI_CLK),.SYNC_OUT(MUON_BUF_TIME_TAG_A));
   synchronizer_32bit muon_buf_end_sync
     (.ASYNC_IN(LCL_MUON_BUF_TIME_TAG_B[MUON_BUF_RNUM]),
      .CLK(AXI_CLK),.SYNC_OUT(MUON_BUF_TIME_TAG_B));

   // These are mostly redundant, but may be useful for debugging
   // synchronizer_32bit muon_buf_word_count0_sync
   //   (.ASYNC_IN(LCL_MUON_BUF_WORD_COUNT0),
   //    .CLK(S_AXI_ACLK),.SYNC_OUT(`MUON_BUF_WORD_COUNT0));
   // synchronizer_32bit muon_buf_word_count1_sync
   //   (.ASYNC_IN(LCL_MUON_BUF_WORD_COUNT1),
   //    .CLK(S_AXI_ACLK),.SYNC_OUT(`MUON_BUF_WORD_COUNT1));
   // synchronizer_32bit muon_buf_word_count2_sync
   //   (.ASYNC_IN(LCL_MUON_BUF_WORD_COUNT2),
   //    .CLK(S_AXI_ACLK),.SYNC_OUT(`MUON_BUF_WORD_COUNT2));
   // synchronizer_32bit muon_buf_word_count3_sync
   //   (.ASYNC_IN(LCL_MUON_BUF_WORD_COUNT3),
   //    .CLK(S_AXI_ACLK),.SYNC_OUT(`MUON_BUF_WORD_COUNT3));


endmodule

