// added ttagid register 7_9_15
`timescale 1 ns / 1 ps

	module time_tagging_v1_0_S00_AXI #
	(
		// Users to add parameters here

		// User parameters ends
		// Do not modify the parameters beyond this line

		// Width of S_AXI data bus
		parameter integer C_S_AXI_DATA_WIDTH	= 32,
		// Width of S_AXI address bus
		parameter integer C_S_AXI_ADDR_WIDTH	= 6
	)
	(
		// Users to add ports here
		
	    input  [31:0] teststatus,// teststatus register output
        input  [31:0] c120mout_ps,// processor c120mout register output
        input  [31:0] c120calout_ps,// processor c120calout register output
        input  [31:0] c120deadout,// dead time register output
        input  [31:0] timeseconds,// processor pps seconds register output
        input  [31:0] onanosec, // shower buffer nanoseconds register
        input  [31:0] oseconds,// shower buffer seconds register
        input  [31:0] c120mout_sb,// shower buffer c120mout register
        input  [31:0] c120calout_sb,//shower buffer c120calout register
        input  [31:0] slowtriggersec,// muon buffer slowtriggersec register
        input  [31:0] slowtriggerns,// muon buffer slowtriggerns register
        input  [31:0] c120mout_mb,// muon buffer c120mout register
        input  [31:0] c120calout_mb,// muon buffer c120calout register
		input  [31:0] ttagid,//register reads "ttag" in ascii, added 7_9_15
        output  CSEL_8R,//time tag address decode for register 8 in timetagging module
        input wire csel8rr, // time tag address decode return for register 8
        output  [31:0] SLV_REG_13, // slv_reg13 is ttagctrl write register
		output  CSEL_DFW,//time tag address write decode for default write registers
        input wire cseldfwr, // time tag address write decode return for default write registers 

		// User ports ends
		// Do not modify the ports beyond this line

		// Global Clock Signal
		input wire  S_AXI_ACLK,
		// Global Reset Signal. This Signal is Active LOW
		input wire  S_AXI_ARESETN,
		// Write address (issued by master, acceped by Slave)
		input wire [C_S_AXI_ADDR_WIDTH-1 : 0] S_AXI_AWADDR,
		// Write channel Protection type. This signal indicates the
    		// privilege and security level of the transaction, and whether
    		// the transaction is a data access or an instruction access.
		input wire [2 : 0] S_AXI_AWPROT,
		// Write address valid. This signal indicates that the master signaling
    		// valid write address and control information.
		input wire  S_AXI_AWVALID,
		// Write address ready. This signal indicates that the slave is ready
    		// to accept an address and associated control signals.
		output wire  S_AXI_AWREADY,
		// Write data (issued by master, acceped by Slave) 
		input wire [C_S_AXI_DATA_WIDTH-1 : 0] S_AXI_WDATA,
		// Write strobes. This signal indicates which byte lanes hold
    		// valid data. There is one write strobe bit for each eight
    		// bits of the write data bus.    
		input wire [(C_S_AXI_DATA_WIDTH/8)-1 : 0] S_AXI_WSTRB,
		// Write valid. This signal indicates that valid write
    		// data and strobes are available.
		input wire  S_AXI_WVALID,
		// Write ready. This signal indicates that the slave
    		// can accept the write data.
		output wire  S_AXI_WREADY,
		// Write response. This signal indicates the status
    		// of the write transaction.
		output wire [1 : 0] S_AXI_BRESP,
		// Write response valid. This signal indicates that the channel
    		// is signaling a valid write response.
		output wire  S_AXI_BVALID,
		// Response ready. This signal indicates that the master
    		// can accept a write response.
		input wire  S_AXI_BREADY,
		// Read address (issued by master, acceped by Slave)
		input wire [C_S_AXI_ADDR_WIDTH-1 : 0] S_AXI_ARADDR,
		// Protection type. This signal indicates the privilege
    		// and security level of the transaction, and whether the
    		// transaction is a data access or an instruction access.
		input wire [2 : 0] S_AXI_ARPROT,
		// Read address valid. This signal indicates that the channel
    		// is signaling valid read address and control information.
		input wire  S_AXI_ARVALID,
		// Read address ready. This signal indicates that the slave is
    		// ready to accept an address and associated control signals.
		output wire  S_AXI_ARREADY,
		// Read data (issued by slave)
		output wire [C_S_AXI_DATA_WIDTH-1 : 0] S_AXI_RDATA,
		// Read response. This signal indicates the status of the
    		// read transfer.
		output wire [1 : 0] S_AXI_RRESP,
		// Read valid. This signal indicates that the channel is
    		// signaling the required read data.
		output wire  S_AXI_RVALID,
		// Read ready. This signal indicates that the master can
    		// accept the read data and response information.
		input wire  S_AXI_RREADY
	);

	// AXI4LITE signals
	reg [C_S_AXI_ADDR_WIDTH-1 : 0] 	axi_awaddr;
	reg  	axi_awready;
	reg  	axi_wready;
	reg [1 : 0] 	axi_bresp;
	reg  	axi_bvalid;
	reg [C_S_AXI_ADDR_WIDTH-1 : 0] 	axi_araddr;
	reg  	axi_arready;
	reg [C_S_AXI_DATA_WIDTH-1 : 0] 	axi_rdata;
	reg [1 : 0] 	axi_rresp;
	reg  	axi_rvalid;

	// Example-specific design signals
	// local parameter for addressing 32 bit / 64 bit C_S_AXI_DATA_WIDTH
	// ADDR_LSB is used for addressing 32/64 bit registers/memories
	// ADDR_LSB = 2 for 32 bits (n downto 2)
	// ADDR_LSB = 3 for 64 bits (n downto 3)
	localparam integer ADDR_LSB = (C_S_AXI_DATA_WIDTH/32) + 1;
	localparam integer OPT_MEM_ADDR_BITS = 3;
	//----------------------------------------------
	//-- Signals for user logic register space example

    reg csel8r;
    (* ASYNC_REG = "TRUE" *) reg csel8rr1, csel8rr2;
    reg csel8rr3;
	reg csel8rr4;
	reg csel8rr5;
    reg cseldfr;
    reg cseldfr1;
    reg cseldfr2;
	reg cseldfr3;
    reg cseldfr4;
	reg cseldfr5;
	reg cseldfw;
	(* ASYNC_REG = "TRUE" *) reg cseldfwr1, cseldfwr2;
	reg cseldfwr3;
	reg axi_arready1;
	reg axi_arready2;
	reg axi_awready1;
	reg axi_awready2;
	reg axi_wready1;
	reg axi_wready2;
	wire slv_reg_wren2;
	wire cseldfrhs;
	wire csel8rhs;
	wire cseldfwhs;
    
    assign CSEL_8R = csel8r;// use upper case for output port name
    assign SLV_REG_13 = slv_reg13;// use upper case for output port name
	assign CSEL_DFW = cseldfw;// use upper case for output port name
	assign cseldfrhs = ~(~cseldfr4 | cseldfr5);// default read registers single clock cycle handshake for axi_rvalid generation
	assign csel8rhs = ~(~csel8rr4 | csel8rr5); // read register 8 single clock cycle handshake for axi_rvalid generation
	assign cseldfwhs = ~(~cseldfwr2 | cseldfwr3); //default write registers single clock cycle handshake for axi_bvalid generation
	//------------------------------------------------
	//-- Number of Slave Registers 16
	(* ASYNC_REG = "TRUE" *) reg [C_S_AXI_DATA_WIDTH-1:0]	slv_reg0i, slv_reg0;
	(* ASYNC_REG = "TRUE" *) reg [C_S_AXI_DATA_WIDTH-1:0]	slv_reg1i, slv_reg1;
	(* ASYNC_REG = "TRUE" *) reg [C_S_AXI_DATA_WIDTH-1:0]	slv_reg2i, slv_reg2;
	(* ASYNC_REG = "TRUE" *) reg [C_S_AXI_DATA_WIDTH-1:0]	slv_reg3i, slv_reg3;
	(* ASYNC_REG = "TRUE" *) reg [C_S_AXI_DATA_WIDTH-1:0]	slv_reg4i, slv_reg4;
	(* ASYNC_REG = "TRUE" *) reg [C_S_AXI_DATA_WIDTH-1:0]	slv_reg5i, slv_reg5;
	(* ASYNC_REG = "TRUE" *) reg [C_S_AXI_DATA_WIDTH-1:0]	slv_reg6i, slv_reg6;
	(* ASYNC_REG = "TRUE" *) reg [C_S_AXI_DATA_WIDTH-1:0]	slv_reg7i, slv_reg7;
	(* ASYNC_REG = "TRUE" *) reg [C_S_AXI_DATA_WIDTH-1:0]	slv_reg8i, slv_reg8;
	(* ASYNC_REG = "TRUE" *) reg [C_S_AXI_DATA_WIDTH-1:0]	slv_reg9i, slv_reg9;
	(* ASYNC_REG = "TRUE" *) reg [C_S_AXI_DATA_WIDTH-1:0]	slv_reg10i, slv_reg10;
	(* ASYNC_REG = "TRUE" *) reg [C_S_AXI_DATA_WIDTH-1:0]	slv_reg11i, slv_reg11;
	(* ASYNC_REG = "TRUE" *) reg [C_S_AXI_DATA_WIDTH-1:0]	slv_reg12i, slv_reg12;
	(* ASYNC_REG = "TRUE" *) reg [C_S_AXI_DATA_WIDTH-1:0]	slv_reg14i, slv_reg14;// ttagid register, added 7_9_15
	reg [C_S_AXI_DATA_WIDTH-1:0]	slv_reg13;
	//reg [C_S_AXI_DATA_WIDTH-1:0]	slv_reg14;// ttagid register, added 7_9_15
	reg [C_S_AXI_DATA_WIDTH-1:0]	slv_reg15;
	//wire	 slv_reg_rden; // not needed
	//wire	 slv_reg_wren; // replaced by slv_reg_wren2
	reg [C_S_AXI_DATA_WIDTH-1:0]	 reg_data_out;
	integer	 byte_index;

	// I/O Connections assignments

	assign S_AXI_AWREADY	= axi_awready;
	assign S_AXI_WREADY	= axi_wready;
	assign S_AXI_BRESP	= axi_bresp;
	assign S_AXI_BVALID	= axi_bvalid;
	assign S_AXI_ARREADY	= axi_arready;
	assign S_AXI_RDATA	= axi_rdata;
	assign S_AXI_RRESP	= axi_rresp;
	assign S_AXI_RVALID	= axi_rvalid;
	// Implement axi_awready generation
	// axi_awready is asserted for one S_AXI_ACLK clock cycle when both
	// S_AXI_AWVALID and S_AXI_WVALID and axi_awready1 are asserted. axi_awready is
	// de-asserted when reset is low.
	// assertion of axi_awready is delayed 2 clock cycles to allow longer address decode time

	always @( posedge S_AXI_ACLK )
	begin
	  if ( S_AXI_ARESETN == 1'b0 )
	    begin
	      axi_awready <= 1'b0;
		  axi_awready1 <= 1'b0;
		  axi_awready2 <= 1'b0;
	    end 
	  else
	    begin    
	      if (~axi_awready && S_AXI_AWVALID && S_AXI_WVALID)
	        begin
	          // slave is ready to accept write address when 
	          // there is a valid write address and write data
	          // on the write address and data bus. This design 
	          // expects no outstanding transactions. 
	          axi_awready2 <= 1'b1; // delay axi_awready assertion to stretch write address decoding time
			  axi_awready <= axi_awready1;
			  axi_awready1 <= axi_awready2;
	        end
	      else           
	        begin
	          axi_awready <= 1'b0;
			  axi_awready1 <= 1'b0;
			  axi_awready2 <= 1'b0;
	        end
	    end 
	end       

	// Implement axi_awaddr latching
	// This process is used to latch the address when both 
	// S_AXI_AWVALID and S_AXI_WVALID are valid. 

	always @( posedge S_AXI_ACLK )
	begin
	  if ( S_AXI_ARESETN == 1'b0 )
	    begin
	      axi_awaddr <= 0;
	    end 
	  else
	    begin    
	      if (~axi_awready && S_AXI_AWVALID && S_AXI_WVALID)
	        begin
	          // Write Address latching 
	          axi_awaddr <= S_AXI_AWADDR;
	        end
	    end 
	end       

	// Implement axi_wready generation
	// axi_wready is asserted for one S_AXI_ACLK clock cycle when both
	// S_AXI_AWVALID and S_AXI_WVALID and axi_wready1 are asserted. axi_wready is 
	// de-asserted when reset is low.
	// assertion of axi_aready is delayed 2 clock cycles to allow longer address decode time	

	always @( posedge S_AXI_ACLK )
	begin
	  if ( S_AXI_ARESETN == 1'b0 )
	    begin
	      axi_wready <= 1'b0;
		  axi_wready1 <= 1'b0;
		  axi_wready2 <= 1'b0;
	    end 
	  else
	    begin    
	      if (~axi_wready && S_AXI_WVALID && S_AXI_AWVALID)
	        begin
	          // slave is ready to accept write data when 
	          // there is a valid write address and write data
	          // on the write address and data bus. This design 
	          // expects no outstanding transactions. 
	          axi_wready2 <= 1'b1;// delay axi_wready assertion to stretch write address decoding time
			  axi_wready <= axi_wready1;
			  axi_wready1 <= axi_wready2;
	        end
	      else
	        begin
	          axi_wready <= 1'b0;
			  axi_wready1 <= 1'b0;
			  axi_wready2 <= 1'b0;
	        end
	    end 
	end       

	// Implement memory mapped register select and write logic generation
	// The write data is accepted and written to memory mapped registers when
	// axi_awready2, S_AXI_WVALID, axi_wready2 and S_AXI_WVALID are asserted. Write strobes are used to
	// select byte enables of slave registers while writing.
	// These registers are cleared when reset (active low) is applied.
	// Slave register write enable is asserted when valid address and data are available
	// and the slave is ready to accept the write address and write data.
	//assign slv_reg_wren = axi_wready && S_AXI_WVALID && axi_awready && S_AXI_AWVALID; // replaced by slv_reg_wren2
	assign slv_reg_wren2 = axi_wready2 && S_AXI_WVALID && axi_awready2 && S_AXI_AWVALID; // stretched version of slv_reg_wren

	always @( posedge S_AXI_ACLK )
	begin
	  if ( S_AXI_ARESETN == 1'b0 )
	    begin
	      //slv_reg0 <= 0; //slv_reg0 to slv_reg12 are read only and therefore commented out
	      //slv_reg1 <= 0;
	      //slv_reg2 <= 0;
	      //slv_reg3 <= 0;
	      //slv_reg4 <= 0;
	      //slv_reg5 <= 0;
	      //slv_reg6 <= 0;
	      //slv_reg7 <= 0;
	      //slv_reg8 <= 0;
	      //slv_reg9 <= 0;
	      //slv_reg10 <= 0;
	      //slv_reg11 <= 0;
	      //slv_reg12 <= 0;
	      slv_reg13 <= 0;
	      //slv_reg14 <= 0;// ttagid register, added 7_9_15
	      slv_reg15 <= 0;
	    end 
	  else begin
	  // address decoding for writing registers
	    if (slv_reg_wren2)
	      begin
	        case ( axi_awaddr[ADDR_LSB+OPT_MEM_ADDR_BITS:ADDR_LSB] )
	        //4'h0: slv_reg0 <= S_AXI_WDATA; //slv_reg0 to slv_reg12 are read only and therefore commented out
            //4'h1: slv_reg1 <= S_AXI_WDATA;
            //4'h2: slv_reg2 <= S_AXI_WDATA; 
            //4'h3: slv_reg3 <= S_AXI_WDATA;  
            //4'h4: slv_reg4 <= S_AXI_WDATA;
            //4'h5: slv_reg5 <= S_AXI_WDATA;
            //4'h6: slv_reg6 <= S_AXI_WDATA; 
            //4'h7: slv_reg7 <= S_AXI_WDATA;  
            //4'h8:slv_reg8 <= S_AXI_WDATA;  
            //4'h9: slv_reg9 <= S_AXI_WDATA;
            //4'hA:slv_reg10 <= S_AXI_WDATA;
            //4'hB:slv_reg11 <= S_AXI_WDATA;  
            //4'hC:slv_reg12 <= S_AXI_WDATA;  
            4'hD:slv_reg13 <= S_AXI_WDATA; 
            //4'hE:slv_reg14 <= S_AXI_WDATA; // ttagid register, added 7_9_15
            4'hF:slv_reg15 <= S_AXI_WDATA;  
            default : begin
                        //slv_reg0 <= slv_reg0;
                        //slv_reg1 <= slv_reg1;
                        //slv_reg2 <= slv_reg2;
                        //slv_reg3 <= slv_reg3;
                        //slv_reg4 <= slv_reg4;
                        //slv_reg5 <= slv_reg5;
                        //slv_reg6 <= slv_reg6;
                        //slv_reg7 <= slv_reg7;
                        //slv_reg8 <= slv_reg8;
                        //slv_reg9 <= slv_reg9;
                        //slv_reg10 <= slv_reg10;
                        //slv_reg11 <= slv_reg11;
                        //slv_reg12 <= slv_reg12;
                        slv_reg13 <= slv_reg13;
                        //slv_reg14 <= slv_reg14;// ttagid register, added 7_9_15
                        slv_reg15 <= slv_reg15;
                      end
            endcase
	      end
	  end
	end    

	// Implement write response logic generation
	// The write response and response valid signals are asserted by the slave 
	// when cseldfwhs is asserted.  
	// This marks the acceptance of address and indicates the status of 
	// write transaction.

	always @( posedge S_AXI_ACLK )
	begin
	  if ( S_AXI_ARESETN == 1'b0 )
	    begin
	      axi_bvalid  <= 0;
	      axi_bresp   <= 2'b0;
	    end 
	  else
	    begin    
	      if (~axi_bvalid && cseldfwhs) // bvalid handshake has been modified to extend time
	        begin
	          // indicates a valid write response is available
	          axi_bvalid <= 1'b1;
	          axi_bresp  <= 2'b0; // 'OKAY' response 
	        end                   // work error responses in future
	      else
	        begin
	          if (S_AXI_BREADY && axi_bvalid) 
	            //check if bready is asserted while bvalid is high) 
	            //(there is a possibility that bready is always asserted high)   
	            begin
	              axi_bvalid <= 1'b0; 
	            end  
	        end
	    end
	end   

	// Implement axi_arready generation
	// axi_arready is asserted for one S_AXI_ACLK clock cycle when
	// S_AXI_ARVALID and axi_arready1 have been asserted. axi_awready is 
	// de-asserted when reset (active low) is asserted. 
	// The read address is also latched when S_AXI_ARVALID is 
	// asserted. axi_araddr is reset to zero on reset assertion.
	// assertion of axi_arready is delayed 2 clock cycles to allow longer address decode time

	always @( posedge S_AXI_ACLK )
	begin
	  if ( S_AXI_ARESETN == 1'b0 )
	    begin
	      axi_arready <= 1'b0;
	      axi_araddr  <= 32'b0;
		  axi_arready1 <= 1'b0;
		  axi_arready2 <= 1'b0;
	    end 
	  else
	    begin    
	      if (~axi_arready && S_AXI_ARVALID)
	        begin
	          // indicates that the slave has accepted the valid read address
	          axi_arready2 <= 1'b1; //delay axi_arready assertion to stretch read address decoding time
			  axi_arready1 <= axi_arready2;
			  axi_arready <= axi_arready1;
	          // Read address latching
	          axi_araddr  <= S_AXI_ARADDR;
	        end
	      else
	        begin
	          axi_arready <= 1'b0;
			  axi_arready1 <= 1'b0;
			  axi_arready2 <= 1'b0;
	        end
	    end 
	end       

	// Implement axi_arvalid generation
	// axi_rvalid is asserted for one S_AXI_ACLK clock cycle when a combination of
	// S_AXI_ARVALID and either csel8rhs or cseldfrhs are asserted. The slave registers 
	// data are available on the axi_rdata bus at this instance. The 
	// assertion of axi_rvalid marks the validity of read data on the 
	// bus and axi_rresp indicates the status of read transaction.axi_rvalid 
	// is deasserted on reset (active low). axi_rresp and axi_rdata are 
	// cleared to zero on reset (active low).  
	always @( posedge S_AXI_ACLK )
	begin
	  if ( S_AXI_ARESETN == 1'b0 )
	    begin
	      axi_rvalid <= 0;
	      axi_rresp  <= 0;
	    end 
	  else
	    begin    
	      if (~axi_rvalid && (csel8rhs || cseldfrhs)) // read valid handshake has been modified to extend time
	        begin
	          // Valid read data is available at the read data bus
	          axi_rvalid <= 1'b1;
	          axi_rresp  <= 2'b0; // 'OKAY' response
	        end   
	      else if (axi_rvalid && S_AXI_RREADY)
	        begin
	          // Read data is accepted by the master
	          axi_rvalid <= 1'b0;
	        end                
	    end
	end    

	// Implement memory mapped register select and read logic generation
	// Slave register read enable is asserted when valid address is available
	// and the slave is ready to accept the read address.
	//assign slv_reg_rden = axi_arready & S_AXI_ARVALID & ~axi_rvalid; //not needed
	always @(*)
	begin
	  if ( S_AXI_ARESETN == 1'b0 )
	    begin
	      reg_data_out <= 0;
	    end 
	  else
	    begin    
	      // Address decoding for reading registers
	      case ( axi_araddr[ADDR_LSB+OPT_MEM_ADDR_BITS:ADDR_LSB] )
	        4'h0   : reg_data_out <= slv_reg0;
	        4'h1   : reg_data_out <= slv_reg1;
	        4'h2   : reg_data_out <= slv_reg2;
	        4'h3   : reg_data_out <= slv_reg3;
	        4'h4   : reg_data_out <= slv_reg4;
	        4'h5   : reg_data_out <= slv_reg5;
	        4'h6   : reg_data_out <= slv_reg6;
	        4'h7   : reg_data_out <= slv_reg7;
	        4'h8   : reg_data_out <= slv_reg8;
	        4'h9   : reg_data_out <= slv_reg9;
	        4'hA   : reg_data_out <= slv_reg10;
	        4'hB   : reg_data_out <= slv_reg11;
	        4'hC   : reg_data_out <= slv_reg12;
	        4'hD   : reg_data_out <= slv_reg13;
	        4'hE   : reg_data_out <= slv_reg14;
	        4'hF   : reg_data_out <= slv_reg15;
	        default : reg_data_out <= 0;
	      endcase
	    end   
	end

	// Output register or memory read data
	always @( posedge S_AXI_ACLK )
	begin
	  if ( S_AXI_ARESETN == 1'b0 )
	    begin
	      axi_rdata  <= 0;
	    end 
	  else
	    begin    
	      // (When there is a valid read address (S_AXI_ARVALID) with 
	      // acceptance of read address by the slave (axi_arready), 
	      // output the read data) this has been changed, see next line
		  // The output of data described above has been modified to coincide with the axi_rvalid handshake 
		  // condition slv_reg_rden is not needed
	      if (~axi_rvalid && (csel8rhs || cseldfrhs)) 
	        begin
	          axi_rdata <= reg_data_out;     // register read data
	        end   
	    end
	end    

	// Add user logic here
	// synchronize time tag read registers to fabric clock
    always @( posedge S_AXI_ACLK )
       begin
        if ( S_AXI_ARESETN == 1'b0 )
            begin
                slv_reg0i <= 0;
                slv_reg1i <= 0;
                slv_reg2i <= 0;
                slv_reg3i <= 0;
                slv_reg4i <= 0;
                slv_reg5i <= 0;
                slv_reg6i <= 0;
                slv_reg7i <= 0;
                slv_reg8i <= 0;
                slv_reg9i <= 0;
                slv_reg10i <= 0;
                slv_reg11i <= 0;
                slv_reg12i <= 0;
				slv_reg14i <= 0;// ttagid register, added 7_9_15
                slv_reg0 <= 0;
                slv_reg1 <= 0;
                slv_reg2 <= 0;
                slv_reg3 <= 0;
                slv_reg4 <= 0;
                slv_reg5 <= 0;
                slv_reg6 <= 0;
                slv_reg7 <= 0;
                slv_reg8 <= 0;
                slv_reg9 <= 0;
                slv_reg10 <= 0;
                slv_reg11 <= 0;
                slv_reg12 <= 0;
				slv_reg14 <= 0;// ttagid register, added 7_9_15
            end
         else
            begin
                slv_reg0i <= onanosec;
                slv_reg1i <= oseconds;
                slv_reg2i <= c120mout_sb;
                slv_reg3i <= c120calout_sb;
                slv_reg4i <= slowtriggerns;
                slv_reg5i <= slowtriggersec;
                slv_reg6i <= c120mout_mb;
                slv_reg7i <= c120calout_mb;
                slv_reg8i <= timeseconds;
                slv_reg9i <= c120mout_ps;
                slv_reg10i <= c120calout_ps;
                slv_reg11i <= c120deadout;
                slv_reg12i <= teststatus;
				slv_reg14i <= ttagid;// ttagid register, aded 7_9_15
                slv_reg0 <= slv_reg0i;
                slv_reg1 <= slv_reg1i;
                slv_reg2 <= slv_reg2i;
                slv_reg3 <= slv_reg3i;
                slv_reg4 <= slv_reg4i;
                slv_reg5 <= slv_reg5i;
                slv_reg6 <= slv_reg6i;
                slv_reg7 <= slv_reg7i;
                slv_reg8 <= slv_reg8i;
                slv_reg9 <= slv_reg9i;
                slv_reg10 <= slv_reg10i;
                slv_reg11 <= slv_reg11i;
                slv_reg12 <= slv_reg12i;
				slv_reg14 <= slv_reg14i;// ttagid register, added 7_9_15
            end            
        end
    // decode time tag read registers address selection
    always @( posedge S_AXI_ACLK )
       begin
        if ( S_AXI_ARESETN == 1'b0 )
            begin
                csel8r <= 0; //register 8 read 
                cseldfr <= 0; // default registers read
            end
        else
          if ( axi_arready2 && S_AXI_ARVALID )
            begin
              case ( axi_araddr[ADDR_LSB+OPT_MEM_ADDR_BITS:ADDR_LSB] )
                4'h0   : cseldfr <= 1'b1;
                4'h1   : cseldfr <= 1'b1;
                4'h2   : cseldfr <= 1'b1;
                4'h3   : cseldfr <= 1'b1;
                4'h4   : cseldfr <= 1'b1;
                4'h5   : cseldfr <= 1'b1;
                4'h6   : cseldfr <= 1'b1;
                4'h7   : cseldfr <= 1'b1;
                4'h8   : csel8r <= 1'b1;
                4'h9   : cseldfr <= 1'b1;
                4'hA   : cseldfr <= 1'b1;
                4'hB   : cseldfr <= 1'b1;
                4'hC   : cseldfr <= 1'b1;
                4'hD   : cseldfr <= 1'b1;
				4'hE   : cseldfr <= 1'b1;
				4'hF   : cseldfr <= 1'b1;
                default : begin
                          csel8r <= 1'b0;
                          cseldfr <= 1'b0;
                          end
              endcase
            end
          else 
            begin
              cseldfr <= 1'b0;
              csel8r <= 1'b0;
            end
      end
    //delay circuit for default read address decoding return, 
	//needed to generate axi_rvalid handshake at appropriate time
    always @( posedge S_AXI_ACLK )
       begin
        if (S_AXI_ARESETN == 1'b0)
            begin
                cseldfr1 <= 0; 
                cseldfr2 <= 0;
                cseldfr4 <= 0;
				cseldfr3 <= 0;
				cseldfr5 <= 0;
            end
        else
            begin
                cseldfr1 <= cseldfr;
                cseldfr2 <= cseldfr1;
				cseldfr3 <= cseldfr2;
                cseldfr4 <= cseldfr3;
				cseldfr5 <= cseldfr4;
            end
       end
    // delay circuit for register 8 read address decoding return,
	//needed to generate axi_rvalid handshake at appropriate time
    always @( posedge S_AXI_ACLK )
       begin
        if (S_AXI_ARESETN == 1'b0)
            begin
                csel8rr1 <= 0;
                csel8rr2 <= 0;
                csel8rr3 <= 0;
				csel8rr4 <= 0;
				csel8rr5 <= 0;
            end
        else
            begin
                csel8rr1 <= csel8rr;
                csel8rr2 <= csel8rr1;
                csel8rr3 <= csel8rr2;
				csel8rr4 <= csel8rr3;
				csel8rr5 <= csel8rr4;
            end
       end
    // decode time tag default write registers address selection
    always @( posedge S_AXI_ACLK )
       begin
        if ( S_AXI_ARESETN == 1'b0 )
            begin
                cseldfw <= 0;
            end
        else
          if ( slv_reg_wren2 )
            begin
              case ( axi_awaddr[ADDR_LSB+OPT_MEM_ADDR_BITS:ADDR_LSB] )
                4'h0   : cseldfw <= 1'b1;
                4'h1   : cseldfw <= 1'b1;
                4'h2   : cseldfw <= 1'b1;
                4'h3   : cseldfw <= 1'b1;
                4'h4   : cseldfw <= 1'b1;
                4'h5   : cseldfw <= 1'b1;
                4'h6   : cseldfw <= 1'b1;
                4'h7   : cseldfw <= 1'b1;
                4'h8   : cseldfw <= 1'b1;
                4'h9   : cseldfw <= 1'b1;
                4'hA   : cseldfw <= 1'b1;
                4'hB   : cseldfw <= 1'b1;
                4'hC   : cseldfw <= 1'b1;
                4'hD   : cseldfw <= 1'b1;
				4'hE   : cseldfw <= 1'b1;
				4'hF   : cseldfw <= 1'b1;
                default : begin
                          cseldfw <= 1'b0;
                          end
              endcase
            end
          else 
            begin
              cseldfw <= 1'b0;
            end
      end
    //delay circuit for default write address decoding,
	//needed to generate axi_bvalid handshake at appropriate time
    always @( posedge S_AXI_ACLK )
       begin
        if (S_AXI_ARESETN == 1'b0)
            begin
                cseldfwr1 <= 0;
                cseldfwr2 <= 0;
				cseldfwr3 <= 0;
            end
        else
            begin
                cseldfwr1 <= cseldfwr;
                cseldfwr2 <= cseldfwr1;
				cseldfwr3 <= cseldfwr2;
            end
       end
	 // User logic ends

	endmodule
