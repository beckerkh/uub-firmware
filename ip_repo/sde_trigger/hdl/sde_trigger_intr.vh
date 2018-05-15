// sde_trigger_vh.intr
//
// This file contains the main IP specific portion of the sde_trigger interrupt code.
// It is extracted as a separate .vh file to encasulate most of our specific changes
// from the standard Vivado interrupt module into one file.
//
// 26-May-2015 DFN Initial version
// 29-Feb-2016 DFN Replace C_NUM_OF_INTR by hard coded "2".  Had too many
//                 problems with C_NUM_OF_INTR being reset to 1.
// 28-Apr-2018 DFN Modify to handle just one interrupt signal.

 wire AXI_INTR;  // Trigger synchronized to AXI clock

// // Synchronize trigger signals to AXI clock

synchronizer_1bit trigger_synch(.ASYNC_IN(INTR),
                                     .CLK(S_AXI_ACLK),
                                     .SYNC_OUT(AXI_INTR));

// Generate level triggered interrupt on SHWR_INTR & MUON_INTR signals.

always @ ( posedge S_AXI_ACLK )
  begin
     if ( S_AXI_ARESETN == 1'b0)
       begin
	  intr <= 1'b0;
       end
     else
       begin
	  intr <= AXI_INTR;
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
   
   always @ ( posedge S_AXI_ACLK )
     begin
	if ( S_AXI_ARESETN == 1'b0 | reg_intr_ack[i] == 1'b1)
	  begin
	     det_intr <= 1'b0;
	  end
	else
	  begin
	     if (intr == 1'b1)
	       begin
	          det_intr <= 1'b1;
	       end
	  end
     end
      
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



