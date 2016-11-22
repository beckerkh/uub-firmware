// adc_filt_delay_block.vh
// 
// This is the portion of the sde_trigger code that handles the 
// adc filter scrubbing, adc delays, & shower memory loading.
//
// 22-Nov-2016 DFN Initial version split off from sde_trigger_code.vh because
//                 that file was getting too large.

        // Clip filtered signals to remain within 12 bit range
        if (FILT_PMT0[`ADC_WIDTH+1] == 1)
          FILTB_PMT0 <= 0;
        else if (FILT_PMT0[`ADC_WIDTH] == 1)
          FILTB_PMT0 <= (1 << `ADC_WIDTH) -1;
        else
          FILTB_PMT0 = FILT_PMT0[`ADC_WIDTH-1:0];

        if (FILT_PMT1[`ADC_WIDTH+1] == 1)
          FILTB_PMT1 <= 0;
        else if (FILT_PMT1[`ADC_WIDTH] == 1)
          FILTB_PMT1 <= (1 << `ADC_WIDTH) -1;
        else
          FILTB_PMT1 = FILT_PMT1[`ADC_WIDTH-1:0];

        if (FILT_PMT2[`ADC_WIDTH+1] == 1)
          FILTB_PMT2 <= 0;
        else if (FILT_PMT2[`ADC_WIDTH] == 1)
          FILTB_PMT2 <= (1 << `ADC_WIDTH) -1;
        else
          FILTB_PMT2 = FILT_PMT2[`ADC_WIDTH-1:0];

        // Keep a copy of unclipped filtered signal in sync with clipped one.
        FILTD_PMT0 <= FILT_PMT0;
        FILTD_PMT1 <= FILT_PMT1;
        FILTD_PMT2 <= FILT_PMT2;

	// Delay ADC data to re-time it with filtered ADC data
	ADC0_DLY[0] <= ADC0;
	ADC1_DLY[0] <= ADC1;
	ADC2_DLY[0] <= ADC2;
	ADC3_DLY[0] <= ADC3;
	ADC4_DLY[0] <= ADC4;
        for (DLY_IDX=1; DLY_IDX<=`ADC_FILT_DELAY; DLY_IDX=DLY_IDX+1) begin
           ADC0_DLY[DLY_IDX] <= ADC0_DLY[DLY_IDX-1];
           ADC1_DLY[DLY_IDX] <= ADC1_DLY[DLY_IDX-1];
           ADC2_DLY[DLY_IDX] <= ADC2_DLY[DLY_IDX-1];
           ADC3_DLY[DLY_IDX] <= ADC3_DLY[DLY_IDX-1];
           ADC4_DLY[DLY_IDX] <= ADC4_DLY[DLY_IDX-1];
        end
	ADCD0 <= ADC0_DLY[`ADC_FILT_DELAY];
	ADCD1 <= ADC1_DLY[`ADC_FILT_DELAY];
	ADCD2 <= ADC2_DLY[`ADC_FILT_DELAY];
	ADCD3 <= ADC3_DLY[`ADC_FILT_DELAY];
	ADCD4 <= ADC4_DLY[`ADC_FILT_DELAY];

	ADCD[1] <= ADCD0[2*`ADC_WIDTH-1:`ADC_WIDTH];
	ADCD[3] <= ADCD1[2*`ADC_WIDTH-1:`ADC_WIDTH];
	ADCD[5] <= ADCD2[2*`ADC_WIDTH-1:`ADC_WIDTH];
	ADCD[7] <= ADCD3[2*`ADC_WIDTH-1:`ADC_WIDTH];
	ADCD[9] <= ADCD4[2*`ADC_WIDTH-1:`ADC_WIDTH];
	ADCD[0] <= ADCD0[`ADC_WIDTH-1:0];
	ADCD[2] <= ADCD1[`ADC_WIDTH-1:0];
	ADCD[4] <= ADCD2[`ADC_WIDTH-1:0];
	ADCD[6] <= ADCD3[`ADC_WIDTH-1:0];
	ADCD[8] <= ADCD4[`ADC_WIDTH-1:0];
	
        // Send shower data to memory
        
        SHWR_ADDR1[`SHWR_MEM_BUF_SHIFT-1:0]
          <= (SHWR_ADDR1+4) & (`SHWR_MEM_DEPTH-1);
        SHWR_ADDR1[`SHWR_MEM_BUF_SHIFT+`SHWR_BUF_NUM_WIDTH-1:
                   `SHWR_MEM_BUF_SHIFT] <= LCL_SHWR_BUF_WNUM;
        SHWR_ADDR <= SHWR_ADDR1;
        SHWR_DATA0[`ADC_WIDTH-1:0] <= ADCD0[`ADC_WIDTH-1:0];
        SHWR_DATA0[`ADC_WIDTH+15:16] <= ADCD0[2*`ADC_WIDTH-1:`ADC_WIDTH];
        SHWR_DATA1[`ADC_WIDTH-1:0] <= ADCD1[`ADC_WIDTH-1:0];
        SHWR_DATA1[`ADC_WIDTH+15:16] <= ADCD1[2*`ADC_WIDTH-1:`ADC_WIDTH];
        SHWR_DATA2[`ADC_WIDTH-1:0] <= ADCD2[`ADC_WIDTH-1:0];
        SHWR_DATA2[`ADC_WIDTH+15:16] <= ADCD2[2*`ADC_WIDTH-1:`ADC_WIDTH];
//        SHWR_DATA3[`ADC_WIDTH-1:0] <= ADCD3[`ADC_WIDTH-1:0];
//        SHWR_DATA3[`ADC_WIDTH+15:16] <= ADCD3[2*`ADC_WIDTH-1:`ADC_WIDTH];
//        SHWR_DATA4[`ADC_WIDTH-1:0] <= ADCD4[`ADC_WIDTH-1:0];
//        SHWR_DATA4[`ADC_WIDTH+15:16] <= ADCD4[2*`ADC_WIDTH-1:`ADC_WIDTH];

        SHWR_DATA0[15:`ADC_WIDTH] <= FILTB_PMT0 & 'hf;
        SHWR_DATA0[31:`ADC_WIDTH+16] <= (FILTB_PMT0 >> 4) & 'hf;
        SHWR_DATA1[15:`ADC_WIDTH] <= (FILTB_PMT0 >> 8) & 'hf;
        SHWR_DATA1[31:`ADC_WIDTH+16] <= FILTB_PMT1 & 'hf;
        SHWR_DATA2[15:`ADC_WIDTH] <= (FILTB_PMT1 >> 4) & 'hf;
        SHWR_DATA2[31:`ADC_WIDTH+16] <= (FILTB_PMT1 >> 8) & 'hf;
//        SHWR_DATA3[15:`ADC_WIDTH] <=  FILTB_PMT2 & 'hf;
//        SHWR_DATA3[31:`ADC_WIDTH+16] <= (FILTB_PMT2 >> 4) & 'hf;
//        SHWR_DATA4[15:`ADC_WIDTH] <=  (FILTB_PMT2 >> 8) & 'hf;
//        SHWR_DATA4[31:`ADC_WIDTH+16] <= ADC_EXTRA;

// Debugging information for integral
	SHWR_DATA3 <= (BASELINE[3] >> `SHWR_AREA_FRAC_WIDTH);
	SHWR_DATA4 <= (AREA[3] >> `SHWR_AREA_FRAC_WIDTH) | 
		      (PEAK[3] << `SHWR_PEAK_SHIFT) |
		      (SATURATED[3] << `SHWR_SATURATED_SHIFT);
	
	// Some debugging for now in ADC_EXTRA in place of seq. number
	ADC_EXTRA = COMPATIBILITY_SB_TRIG |
		    (SB_TRIG << 1) |
		    (LED_TRG_FLAG << 2) |
		    (TRIGGERED << 3);
       
     // Put sequence number in ADC_EXTRA
//        if (ADC_EXTRA == 14)
//          ADC_EXTRA <= 0;
//        else
//          ADC_EXTRA <= ADC_EXTRA+1;
        
