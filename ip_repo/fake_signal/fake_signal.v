// Module to intject an internal digital fake signal before the filter and
// trigger modules.
//
// 14-Sep-2016 DFN Initial version
// 21-Sep-2016 DFN Modify to send a burst of 2 pulses.
//

`define CLOCK_FREQ 120        // Clock frequency in Mhz
`define DESIRED_DELAY 5000000 // Desired pulse spacing in usec
`define DESIRED_DELAY2 1000000 // Delay of 2nd pulse after first
`define MAX_DLYCOUNT (`CLOCK_FREQ * `DESIRED_DELAY)
`define MAX_DLYCOUNT2 (`CLOCK_FREQ * `DESIRED_DELAY2)
`define PEDESTAL 200         // Signal pedestal
`define MAX_SIGNAL 2047      // Maximum signal size
`define SIGNAL_BINS (`MAX_SIGNAL-`PEDESTAL)

module fake_signal 
  (
   input USE_FAKE,
   input[23:0] ADC0_IN,
   input[23:0] ADC1_IN,
   input[23:0] ADC2_IN,
   input[23:0] ADC3_IN,
   input[23:0] ADC4_IN,
   input CLK,
   output reg[23:0] ADC0_OUT,
   output reg[23:0] ADC1_OUT,
   output reg[23:0] ADC2_OUT,
   output reg[23:0] ADC3_OUT,
   output reg[23:0] ADC4_OUT
   );

   reg [11:0] FAKE_SIGNALHG;
   reg [11:0] FAKE_SIGNALLG;
   reg [23:0] FAKE_SIGNAL;
   reg [31:0] PULSE_DELAY;
   reg [11:0] PULSE;

initial begin
   PULSE_DELAY <= 0;
   PULSE <= 0;
end
   
always @(posedge CLK)
     begin

	// Define basic pulse interval
	if (PULSE_DELAY >= `MAX_DLYCOUNT) 
	  PULSE_DELAY <= 0;
        else
	  PULSE_DELAY <= PULSE_DELAY+1;
	if (PULSE_DELAY < `SIGNAL_BINS)
	  PULSE <= PULSE+1;
	else if ((PULSE_DELAY >= `MAX_DLYCOUNT2) && 
		 (PULSE_DELAY <`MAX_DLYCOUNT2+`SIGNAL_BINS))
	  PULSE <= PULSE+1;
	else if ((PULSE_DELAY >= 2*`MAX_DLYCOUNT2) && 
		 (PULSE_DELAY <2*`MAX_DLYCOUNT2+`SIGNAL_BINS))
	  PULSE <= PULSE+1;
	else
	  PULSE <= 0;

	FAKE_SIGNALHG <= PULSE + `PEDESTAL;
	FAKE_SIGNALLG <= (PULSE >> 5) + `PEDESTAL;
	FAKE_SIGNAL <= (FAKE_SIGNALHG << 12) | FAKE_SIGNALLG;
	
	// If USE_FAKE is set, replace signal from ADCs with a fake.
	if (USE_FAKE) begin
	   ADC0_OUT <= FAKE_SIGNAL;
	   ADC1_OUT <= FAKE_SIGNAL;
	   ADC2_OUT <= FAKE_SIGNAL;
	   ADC3_OUT <= FAKE_SIGNAL;
	   ADC4_OUT <= FAKE_SIGNAL;
	end

	// Otherwise just pass ADC signals through to output
	else begin
	   ADC0_OUT <= ADC0_IN;
	   ADC1_OUT <= ADC1_IN;
	   ADC2_OUT <= ADC2_IN;
	   ADC3_OUT <= ADC3_IN;
	   ADC4_OUT <= ADC4_IN;
	end
     end
	
endmodule

