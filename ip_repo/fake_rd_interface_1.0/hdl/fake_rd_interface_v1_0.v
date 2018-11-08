// Fake RD module for testing LVDS transfers
//
// Pin assignments
//  ext1_dat[0] - ENABLE_XFR_IN
//  ext1_dat[1] - SERIAL_CLK_IN
//  ext1_dat[2] - SERIAL_DATA0_IN
//  ext1_dat[3] - unused
//  ext1_dat[4] - unused
//  ext1_dat[5] - SERIAL_FAKE_OUT
//  ext1_dat[6] - FAKE_CLK_OUT
//  ext1_dat[7] - ENABLE_FAKE_XFR

`timescale 1 ns / 1 ps

	module fake_rd_interface_v1_0 #
	(
		// Users to add parameters here

		// User parameters ends
		// Do not modify the parameters beyond this line


		// Parameters of Axi Slave Bus Interface S00_AXI
		parameter integer C_S00_AXI_DATA_WIDTH	= 32,
		parameter integer C_S00_AXI_ADDR_WIDTH	= 4
	)
	(
		// Users to add ports here

         input SERIAL_DATA0_IN,
         input SERIAL_CLK_IN,
         input ENABLE_XFR_IN,
         input [31:0] FAKE_DATA,
         input LOCAL_CLK,
         
         output wire ENABLE_FAKE_XFR,
         output wire SERIAL_FAKE_OUT,
         output wire[31:0] FAKE_DATA_ADDR,
         output wire[31:0] DATA_ADDR,
         output wire[31:0] DATA_TO_MEM,
         output wire ENABLE_MEM_WRT,
         output wire DEBUG1,
         output wire DEBUG2,

		// User ports ends
		// Do not modify the ports beyond this line


		// Ports of Axi Slave Bus Interface S00_AXI
		input wire  s00_axi_aclk,
		input wire  s00_axi_aresetn,
		input wire [C_S00_AXI_ADDR_WIDTH-1 : 0] s00_axi_awaddr,
		input wire [2 : 0] s00_axi_awprot,
		input wire  s00_axi_awvalid,
		output wire  s00_axi_awready,
		input wire [C_S00_AXI_DATA_WIDTH-1 : 0] s00_axi_wdata,
		input wire [(C_S00_AXI_DATA_WIDTH/8)-1 : 0] s00_axi_wstrb,
		input wire  s00_axi_wvalid,
		output wire  s00_axi_wready,
		output wire [1 : 0] s00_axi_bresp,
		output wire  s00_axi_bvalid,
		input wire  s00_axi_bready,
		input wire [C_S00_AXI_ADDR_WIDTH-1 : 0] s00_axi_araddr,
		input wire [2 : 0] s00_axi_arprot,
		input wire  s00_axi_arvalid,
		output wire  s00_axi_arready,
		output wire [C_S00_AXI_DATA_WIDTH-1 : 0] s00_axi_rdata,
		output wire [1 : 0] s00_axi_rresp,
		output wire  s00_axi_rvalid,
		input wire  s00_axi_rready
	);
// Instantiation of Axi Bus Interface S00_AXI
	fake_rd_interface_v1_0_S00_AXI # ( 
		.C_S_AXI_DATA_WIDTH(C_S00_AXI_DATA_WIDTH),
		.C_S_AXI_ADDR_WIDTH(C_S00_AXI_ADDR_WIDTH)
	) fake_rd_interface_v1_0_S00_AXI_inst (
		.S_AXI_ACLK(s00_axi_aclk),
		.S_AXI_ARESETN(s00_axi_aresetn),
		.S_AXI_AWADDR(s00_axi_awaddr),
		.S_AXI_AWPROT(s00_axi_awprot),
		.S_AXI_AWVALID(s00_axi_awvalid),
		.S_AXI_AWREADY(s00_axi_awready),
		.S_AXI_WDATA(s00_axi_wdata),
		.S_AXI_WSTRB(s00_axi_wstrb),
		.S_AXI_WVALID(s00_axi_wvalid),
		.S_AXI_WREADY(s00_axi_wready),
		.S_AXI_BRESP(s00_axi_bresp),
		.S_AXI_BVALID(s00_axi_bvalid),
		.S_AXI_BREADY(s00_axi_bready),
		.S_AXI_ARADDR(s00_axi_araddr),
		.S_AXI_ARPROT(s00_axi_arprot),
		.S_AXI_ARVALID(s00_axi_arvalid),
		.S_AXI_ARREADY(s00_axi_arready),
		.S_AXI_RDATA(s00_axi_rdata),
		.S_AXI_RRESP(s00_axi_rresp),
		.S_AXI_RVALID(s00_axi_rvalid),
		.S_AXI_RREADY(s00_axi_rready),
                .SERIAL_DATA0_IN(SERIAL_DATA0_IN),
                .SERIAL_CLK_IN(SERIAL_CLK_IN),
                .ENABLE_XFR_IN(ENABLE_XFR_IN),
                .ENABLE_FAKE_XFR(ENABLE_FAKE_XFR),
                .FAKE_DATA(FAKE_DATA),
                .LOCAL_CLK(LOCAL_CLK),
                .SERIAL_FAKE_OUT(SERIAL_FAKE_OUT),
                .FAKE_DATA_ADDR(FAKE_DATA_ADDR),
                .DATA_ADDR(DATA_ADDR),
                .ENABLE_MEM_WRT(ENABLE_MEM_WRT),
                .DATA_TO_MEM(DATA_TO_MEM),
                .DEBUG1(DEBUG1),
                .DEBUG2(DEBUG2)
	);

	// Add user logic here


	// User logic ends

	endmodule
