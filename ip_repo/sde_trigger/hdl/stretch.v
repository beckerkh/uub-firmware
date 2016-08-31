// Module to stretch logic high signal
//
// 26-May-2015 DFN Initial version

module stretch #( parameter STRETCH = 5 )
   (
    input IN,
    input CLK,
    output reg OUT                          
    );

   (* ASYNC_REG = "TRUE", SHREG_EXTRACT="NO" *) 
   reg [STRETCH-1:0] stretch_regs = {STRETCH{1'b0}};

   always @(posedge CLK)
     begin
        stretch_regs <= {stretch_regs[STRETCH-2:0], IN};
        OUT = | stretch_regs;     
     end
endmodule

