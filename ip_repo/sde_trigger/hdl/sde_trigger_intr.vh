// sde_trigger_vh.intr
//
// This file contains the main IP specific portion of the sde_trigger interrupt code.
// It is extracted as a separate .vh file to encasulate most of our specific changes
// from the standard Vivado interrupt module into one file.
//
// 26-May-2015 DFN Initial version
// 29-Feb-2016 DFN Replace C_NUM_OF_INTR by hard coded "2".  Had too many
//                 problems with C_NUM_OF_INTR being reset to 1.

 wire AXI_SHWR_INTR;  // Shower trigger synchronized to AXI clock
 wire AXI_MUON_INTR;  // Muon trigger synchronized to AXI clock

// // Synchronize trigger signals to AXI clock

synchronizer_1bit muon_trigger_synch(.ASYNC_IN(MUON_INTR),
                                     .CLK(S_AXI_ACLK),
                                     .SYNC_OUT(AXI_MUON_INTR));
synchronizer_1bit shwr_trigger_synch(.ASYNC_IN(SHWR_INTR),
                                     .CLK(S_AXI_ACLK),
                                     .SYNC_OUT(AXI_SHWR_INTR));

// Generate level triggered interrupt on SHWR_INTR & MUON_INTR signals.

always @ ( posedge S_AXI_ACLK )
  begin
     if ( S_AXI_ARESETN == 1'b0)
       begin
	  intr <= 2'b0;
       end
     else
       begin
	  intr[`SHWR_TRIGGER_INTR_BIT] <= AXI_SHWR_INTR;
	  intr[`MUON_TRIGGER_INTR_BIT] <= AXI_MUON_INTR;
       end

     // detects interrupt in any intr input 
     // Note that interrupts are merged here
     // detects intr ack in any reg_intr_ack reg bits
     if ( S_AXI_ARESETN == 1'b0 )
       begin
	  intr_all <= 1'b0;
       end
     else
       begin
	  intr_all <= | reg_intr_pending;
	  intr_ack_all <= | reg_intr_ack;
       end
  end
	   
 
   
   //---------------------------------------------------------------------
   // Hardware interrupt detection
   //---------------------------------------------------------------------
   
   // Detect interrupts for user selected number of interrupts
   // For clarity only positive level interrupt is implemented.  Please
   // see sde_trigger_v1_2_S_AXI_INTR.v prior to svn version 41 for other
   // possibilities.
   
   generate
      for(i=0; i<= 1; i=i+1)
        begin: irq_level_high
	   always @ ( posedge S_AXI_ACLK )
	     begin
	        if ( S_AXI_ARESETN == 1'b0 | reg_intr_ack[i] == 1'b1)
	          begin
	             det_intr[i] <= 1'b0;
	          end
	        else
	          begin
	             if (intr[i] == 1'b1)
	               begin
	                  det_intr[i] <= 1'b1;
	               end
	          end
	     end
        end
   endgenerate
      
      // IRQ generation logic

      always @ ( posedge S_AXI_ACLK )
        begin
           if ( (S_AXI_ARESETN == 1'b0) || (intr_ack_all == 1'b1))
             begin
	        s_irq <= 1'b0;
             end
           else if (intr_all == 1'b1 && reg_global_intr_en[0] ==1'b1)
             begin
	        s_irq <= 1'b1;
             end
        end	

      assign irq = s_irq;



