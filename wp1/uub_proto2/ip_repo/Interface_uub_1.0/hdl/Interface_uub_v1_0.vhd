
 library ieee;
 use ieee.std_logic_1164.all;
 use ieee.std_logic_arith.all;
 use ieee.std_logic_unsigned.all;
 
 entity Interface_uub_v1_0 is
   generic (
     -- Users to add parameters here
 
     -- User parameters ends
     -- Do not modify the parameters beyond this line
 
     -- Parameters of Axi Slave Bus Interface S00_AXI
     C_S00_AXI_DATA_WIDTH  : integer := 32;
     C_S00_AXI_ADDR_WIDTH  : integer := 6 );
   port ( -- Users to add ports here
       -- Interface ADC AD9628 (CLK)
          ADC0_CK       : in  std_logic;
          ADC1_CK       : in  std_logic;
          ADC2_CK       : in  std_logic;
          ADC3_CK       : in  std_logic;
          ADC4_CK       : in  std_logic;
       -- Interface Horloge
          FPGA_CK       : in  std_logic;
       -- Interface ADC AD9628 (Datas)
          adc0          : in  std_logic_vector(25 downto 0);
          adc1          : in  std_logic_vector(25 downto 0);
          adc2          : in  std_logic_vector(25 downto 0);
          adc3          : in  std_logic_vector(25 downto 0);
          adc4          : in  std_logic_vector(25 downto 0);
       -- Interface switches
          hconf         : in  std_logic_vector(7 downto 0);
       -- Interface External connector
          ext0_dat      : in  std_logic_vector(7 downto 0);
          ext0_ctl      : out std_logic_vector(7 downto 0);
          ext1_dat      : in  std_logic_vector(7 downto 0);
          ext1_ctl      : out std_logic_vector(7 downto 0);
       -- Interface Trigger
          TRIG_IN       : in  std_logic;
          TRIG_OUT      : out std_logic;
       -- Interface Radio
          GPS_PPS       : in  std_logic;
       -- Interface autre 
          WATCHDOG      : in  std_logic;
          RADIO_RST_IN  : in  std_logic;
          RADIO_RST_OUT : out std_logic;
          USB_IFAULT    : in  std_logic;
          LED_FLG       : in  std_logic;
          LED_ASY       : out std_logic;
          tp            : out std_logic_vector(6 downto 2);
     -- User ports ends
     -- Do not modify the ports beyond this line
 
     -- Ports of Axi Slave Bus Interface S00_AXI
     s00_axi_aclk    : in  std_logic;
     s00_axi_aresetn : in  std_logic;
     s00_axi_awaddr  : in  std_logic_vector(C_S00_AXI_ADDR_WIDTH-1 downto 0);
     s00_axi_awprot  : in  std_logic_vector(2 downto 0);
     s00_axi_awvalid : in  std_logic;
     s00_axi_awready : out std_logic;
     s00_axi_wdata   : in  std_logic_vector(C_S00_AXI_DATA_WIDTH-1 downto 0);
     s00_axi_wstrb   : in  std_logic_vector((C_S00_AXI_DATA_WIDTH/8)-1 downto 0);
     s00_axi_wvalid  : in  std_logic;
     s00_axi_wready  : out std_logic;
     s00_axi_bresp   : out std_logic_vector(1 downto 0);
     s00_axi_bvalid  : out std_logic;
     s00_axi_bready  : in  std_logic;
     s00_axi_araddr  : in  std_logic_vector(C_S00_AXI_ADDR_WIDTH-1 downto 0);
     s00_axi_arprot  : in  std_logic_vector(2 downto 0);
     s00_axi_arvalid : in  std_logic;
     s00_axi_arready : out std_logic;
     s00_axi_rdata   : out std_logic_vector(C_S00_AXI_DATA_WIDTH-1 downto 0);
     s00_axi_rresp   : out std_logic_vector(1 downto 0);
     s00_axi_rvalid  : out std_logic;
     s00_axi_rready  : in  std_logic );
 end Interface_uub_v1_0;
 
 architecture arch_imp of Interface_uub_v1_0 is
 
   -- component declaration
   component Interface_uub_v1_0_S00_AXI is
     generic ( C_S_AXI_DATA_WIDTH  : integer := 32;
               C_S_AXI_ADDR_WIDTH  : integer := 6 );
     port    ( 
            -- Interface ADC AD9628 (Clk)
               ADC0_CK       : in  std_logic;
               ADC1_CK       : in  std_logic;
               ADC2_CK       : in  std_logic;
               ADC3_CK       : in  std_logic;
               ADC4_CK       : in  std_logic;
            -- Interface Horloge
               FPGA_CK       : in  std_logic;
            -- Interface ADC AD9628 (Datas)
               adc0          : in  std_logic_vector(25 downto 0);
               adc1          : in  std_logic_vector(25 downto 0);
               adc2          : in  std_logic_vector(25 downto 0);
               adc3          : in  std_logic_vector(25 downto 0);
               adc4          : in  std_logic_vector(25 downto 0);
            -- Interface switches
               hconf         : in  std_logic_vector(7 downto 0);
            -- Interface External connector
               ext0_dat      : in  std_logic_vector(7 downto 0);
               ext0_ctl      : out std_logic_vector(7 downto 0);
               ext1_dat      : in  std_logic_vector(7 downto 0);
               ext1_ctl      : out std_logic_vector(7 downto 0);
            -- Interface Trigger
               TRIG_IN       : in  std_logic;
               TRIG_OUT      : out std_logic;
            -- Interface Radio
               GPS_PPS       : in  std_logic;
            -- Interface autre 
               WATCHDOG      : in  std_logic;
               RADIO_RST_IN  : in  std_logic;
               RADIO_RST_OUT : out std_logic;
               USB_IFAULT    : in  std_logic;
               LED_FLG       : in  std_logic;
               LED_ASY       : out std_logic;
               tp            : out std_logic_vector(6 downto 2);

               S_AXI_ACLK    : in std_logic;
               S_AXI_ARESETN : in std_logic;
               S_AXI_AWADDR  : in std_logic_vector(C_S_AXI_ADDR_WIDTH-1 downto 0);
               S_AXI_AWPROT  : in std_logic_vector(2 downto 0);
               S_AXI_AWVALID : in std_logic;
               S_AXI_AWREADY : out std_logic;
               S_AXI_WDATA   : in std_logic_vector(C_S_AXI_DATA_WIDTH-1 downto 0);
               S_AXI_WSTRB   : in std_logic_vector((C_S_AXI_DATA_WIDTH/8)-1 downto 0);
               S_AXI_WVALID  : in std_logic;
               S_AXI_WREADY  : out std_logic;
               S_AXI_BRESP   : out std_logic_vector(1 downto 0);
               S_AXI_BVALID  : out std_logic;
               S_AXI_BREADY  : in std_logic;
               S_AXI_ARADDR  : in std_logic_vector(C_S_AXI_ADDR_WIDTH-1 downto 0);
               S_AXI_ARPROT  : in std_logic_vector(2 downto 0);
               S_AXI_ARVALID : in std_logic;
               S_AXI_ARREADY : out std_logic;
               S_AXI_RDATA   : out std_logic_vector(C_S_AXI_DATA_WIDTH-1 downto 0);
               S_AXI_RRESP   : out std_logic_vector(1 downto 0);
               S_AXI_RVALID  : out std_logic;
               S_AXI_RREADY  : in std_logic );
   end component Interface_uub_v1_0_S00_AXI;
 
 begin
 
 -- Instantiation of Axi Bus Interface S00_AXI
 Interface_uub_v1_0_S00_AXI_inst : Interface_uub_v1_0_S00_AXI
   generic map ( C_S_AXI_DATA_WIDTH  => C_S00_AXI_DATA_WIDTH,
                 C_S_AXI_ADDR_WIDTH  => C_S00_AXI_ADDR_WIDTH )
   port map    ( 
                 ADC0_CK       => ADC0_CK, 
                 ADC1_CK       => ADC1_CK, 
                 ADC2_CK       => ADC2_CK, 
                 ADC3_CK       => ADC3_CK, 
                 ADC4_CK       => ADC4_CK, 
                 FPGA_CK       => FPGA_CK,
                 adc0          => adc0,
                 adc1          => adc1,
                 adc2          => adc2,
                 adc3          => adc3,
                 adc4          => adc4,
                 hconf         => hconf,       
                 ext0_dat      => ext0_dat,  
                 ext0_ctl      => ext0_ctl,  
                 ext1_dat      => ext1_dat,  
                 ext1_ctl      => ext1_ctl,  
                 TRIG_IN       => TRIG_IN,  
                 TRIG_OUT      => TRIG_OUT, 
                 GPS_PPS       => GPS_PPS,   
                 WATCHDOG      => WATCHDOG,
                 RADIO_RST_IN  => RADIO_RST_IN,
                 RADIO_RST_OUT => RADIO_RST_OUT,
                 USB_IFAULT    => USB_IFAULT,
                 LED_FLG       => LED_FLG,
                 LED_ASY       => LED_ASY,
                 tp            => tp,

                 S_AXI_ACLK    => s00_axi_aclk,
                 S_AXI_ARESETN => s00_axi_aresetn,
                 S_AXI_AWADDR  => s00_axi_awaddr,
                 S_AXI_AWPROT  => s00_axi_awprot,
                 S_AXI_AWVALID => s00_axi_awvalid,
                 S_AXI_AWREADY => s00_axi_awready,
                 S_AXI_WDATA   => s00_axi_wdata,
                 S_AXI_WSTRB   => s00_axi_wstrb,
                 S_AXI_WVALID  => s00_axi_wvalid,
                 S_AXI_WREADY  => s00_axi_wready,
                 S_AXI_BRESP   => s00_axi_bresp,
                 S_AXI_BVALID  => s00_axi_bvalid,
                 S_AXI_BREADY  => s00_axi_bready,
                 S_AXI_ARADDR  => s00_axi_araddr,
                 S_AXI_ARPROT  => s00_axi_arprot,
                 S_AXI_ARVALID => s00_axi_arvalid,
                 S_AXI_ARREADY => s00_axi_arready,
                 S_AXI_RDATA   => s00_axi_rdata,
                 S_AXI_RRESP   => s00_axi_rresp,
                 S_AXI_RVALID  => s00_axi_rvalid,
                 S_AXI_RREADY  => s00_axi_rready );
 
   -- Add user logic here
   -- User logic ends
 
 end arch_imp;
 