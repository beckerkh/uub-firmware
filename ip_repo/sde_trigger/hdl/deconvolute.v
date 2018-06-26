//  deconvolute.v
//
//  This module implements one instance of a FADC trace deconvolution.
//
//  Note that the master enable and any inhibit conditions are applied
//  externally to the output of this module.
//
//
//   21-Jun-2018 DFN Initial version (ported from deconvolute.tdf)

`include "sde_trigger_defs.vh"

`define FRAC_BITS 11
`define FRAC_VALUE 2^`FRAC_BITS
`define MIN_COMPARE 4

`define FD_BITS 6
`define FN_BITS 6
`define FN_FRAC_BITS 4
`define FN_INT_BITS (`FN_BITS-`FN_FRAC_BITS)

module deconvolute(
                   input CLK,               // 120 MHz clock
                   input ENABLE40,          // Clock enable for 40 MHz
                   input [`ADC_WIDTH-1:0] ADC_IN, // Filtered ADC input
                   input [`ADC_WIDTH+`SHWR_BASELINE_EXTRA_BITS-1:0] BASELINE,
                   input [`FD_BITS-1:0] FD, // .yyyyy Fractional decay constant
                   input [`FN_BITS-1:0] FN, // xx.yyy fixed point normalizer
                   output reg [`ADC_WIDTH-1:0] ADC_OUT // Output deconvoluted value
                   );
   

   reg [`FN_INT_BITS-1:0]                  FN_INT; // integer portion of fixed point normalizer
   reg                                     FN_FRAC; // frac. portion of fixed point normalizer
   reg [11:0]                              INT_A; // Intermediate result of 1st pipeline stage
   reg [12:0]                              INT_B; // Intermediate result of 2nd pipeline stage
   reg [11:0]                              INT_C; // Intermediate result of 3rd pipeline stage
   reg [13:0]                              INT_D; // Intermediate result of 4th pipeline stage
   reg [13:0]                              INT_E; // Intermediate result of 5th pipeline stage
   reg [`FD_BITS-1:0]                      FRAC_A; // Intermediate result of 1st pipeline stage
   reg [`FD_BITS-3:0]                      FRAC_B; // Intermediate result of 2nd pipeline stage
   reg [`FD_BITS-3:0]                      FRAC_C; // Intermediate result of 3rd pipeline stage
   reg [`FN_FRAC_BITS+`FD_BITS-3:0]        FRAC_D; // Int. result of 4th pipeline stage
   reg [3:0]                               FRAC_E; // Intermediate result of 5th pipeline stage
   reg [11:0]                              ADC_AA; // Intermediate ADC val in 1st pipeline stage
   reg [11:0]                              ADC_A; // ADC synchronized output of 1st pipeline stage

   reg [11:0]                              ADC_LIMITEDA; // ADC constrained to be > pedestal
   reg [11:0]                              ADC_LIMITED; // ADC constrained to be > pedestal
   reg [11:0]                              ADC_L; // Delayed ADC

   
   always @(posedge CLK) begin

      
      // Separate integer and fractional portions of FN register
      FN_INT <= FN[`FN_BITS-1:`FN_FRAC_BITS];
      FN_FRAC <= FN[`FN_FRAC_BITS-1:0];

      // Ensure signal does not go below baseline
      if (ADC_IN <= BASELINE[`ADC_WIDTH+`SHWR_BASELINE_EXTRA_BITS-1:`SHWR_BASELINE_EXTRA_BITS])
        ADC_LIMITED <= BASELINE[`ADC_WIDTH+`SHWR_BASELINE_EXTRA_BITS-1:`SHWR_BASELINE_EXTRA_BITS];
      else
        ADC_LIMITED <= ADC_IN;

      // Multiply previous ADC value by decay factor.  
      // Result will have 6 fractional binary digits.
      {INT_A,FRAC_A} <= ADC_LIMITED * FD;
      
      // Subtract result of previous stage from current ADC value.
      // Note that the clock delays in the previous operation means this
      // calculation is taking the current ADC value and subtracting the
      // product of the previous ADC value and the decay constant - just
      // what we want.
      {INT_B,FRAC_B} <= {ADC_LIMITED,`FN_FRAC_BITS'b0} - {INT_A,FRAC_A};

      // Test if result of previous stage is less than zero.  If so, replace with zero.
      if (INT_B[12])
        {INT_C,FRAC_C} <= 0;
      else
        {INT_C,FRAC_C} <= {INT_B[11:0],FRAC_B};
      
      // Multiply what we have so far by the nomalizer
      {INT_D,FRAC_D} <= {INT_C,FRAC_C} * {FN_INT,FN_FRAC};

      // Add 1/2 to force rounding rather than truncation
      {INT_E,FRAC_E} <= {INT_D,FRAC_D} + {1,`FN_FRAC_BITS-1'b0};
                            
      // Load output value, limiting maximum value to 4095.
      if (INT_E[12] | INT_E[13])
        ADC_OUT <= 4095;
      else
        ADC_OUT <= INT_E[11:0];
   end

endmodule


// (INT_D[9+FN_INT_BITS..0],FRAC_D[FN_FRAC_BITS+FD_BITS-3..0]) =
//   lpm_mult(.dataa[]=(INT_C[9..0],FRAC_C[FD_BITS-3..0]),
//            .datab[]=(FN_INT[FN_INT_BITS-1..0],FN_FRAC[FN_FRAC_BITS-1..0]),
//            .clock=CLK)
//   WITH(LPM_WIDTHA=10+FD_BITS-2, LPM_WIDTHB=FN_BITS,
//        LPM_WIDTHP=10+FD_BITS-2+FN_BITS, LPM_PIPELINE=3)
//   RETURNS(.result[]);

// ------ Fifth pipeline stage ------
// --  Add "1/2" to force rounding rather than truncation.

// (INT_E[9+FN_INT_BITS..0],FRAC_E[3..0]) = lpm_add_sub(
//   .dataa[]=(INT_D[9+FN_INT_BITS..0],
//             FRAC_D[FN_FRAC_BITS+FD_BITS-3..FN_FRAC_BITS+FD_BITS-6]),
//   .datab[]=(GND,GND,GND,GND,GND,GND,GND,GND,GND,GND,GND,GND,VCC,GND,GND,GND),
//   .clock=CLK)
//   with (LPM_WIDTH=10+FN_INT_BITS+4,LPM_PRESENTATION="UNSIGNED",
//         LPM_DIRECTION="ADD",
//         MAXIMIZE_SPEED=10, LPM_PIPELINE=1, ONE_INPUT_CONSTANT="YES")
//   RETURNS(.result[]);

// ------ Sixth pipeline stage ------
// -- Load output value, limiting maximum value to 1023.

// FOR i in 0 to 9 GENERATE
//   ADC_OUT[i] = dff(INT_E[i] # INT_E[10] # INT_E[11],CLK,VCC,VCC);
// END GENERATE;


      
// END;
