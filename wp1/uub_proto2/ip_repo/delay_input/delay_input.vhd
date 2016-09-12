
 -------------------------------------------------------------------------------
 -- Filename:        delay_input.vhd
 --
 -- Description:     
 --                  
 -- VHDL-Standard:   VHDL'93
 -------------------------------------------------------------------------------
 -- Structure:   
 --              delay_input.vhd
 --
 --
 -------------------------------------------------------------------------------
 -- Naming Conventions:
 --      active low signals:                     "*_n"
 --      clock signals:                          "clk", "clk_div#", "clk_#x" 
 --      reset signals:                          "rst", "rst_n" 
 --      generics:                               "C_*" 
 --      user defined types:                     "*_TYPE" 
 --      state machine next state:               "*_ns" 
 --      state machine current state:            "*_cs" 
 --      combinatorial signals:                  "*_com" 
 --      pipelined or register delay signals:    "*_d#" 
 --      counter signals:                        "*cnt*"
 --      clock enable signals:                   "*_ce" 
 --      internal version of output port         "*_i"
 --      device pins:                            "*_pin" 
 --      ports:                                  - Names begin with Uppercase 
 --      processes:                              "*_PROCESS" 
 --      component instantiations:               "<ENTITY_>I_<#|FUNC>
 -------------------------------------------------------------------------------
 --
 --  20141204 - The ADc data data is now adjustable by ADC ( DAT_DLY_ADC0,
 --             DAT_DLY_ADC1,DAT_DLY_ADC2,DAT_DLY_ADC3,DAT_DLY_ADC4). it be 
 --             calculated to have a constant delay between the ADC data and 
 --             the FPGA equal to 1,049ns)
 --  20160415 - The delay between the input and the output data is removed.
 --             With the clokinverter which is removed in the In_regional_ck
 --             project, the design is now more simple, use less elements and 
 --             have a more valid external ADC delay  setting range
 ---------------------------------------------------------------------------
 library ieee;
   use ieee.std_logic_1164.all;
   use ieee.numeric_std.all;
 
 library unisim;
   use unisim.vcomponents.all;
 
 entity delay_input is
   port (     
     REF_CLOCK    : in  std_logic;
     IO_RESET     : in  std_logic;
     IBUF_0_P     : in  std_logic_vector(12 downto 0); -- positive diff. in buffer
     IBUF_0_N     : in  std_logic_vector(12 downto 0); -- negagitve diff. in buffer
     IBUF_1_P     : in  std_logic_vector(12 downto 0); -- positive diff. in buffer
     IBUF_1_N     : in  std_logic_vector(12 downto 0); -- negagitve diff. in buffer
     IBUF_2_P     : in  std_logic_vector(12 downto 0); -- positive diff. in buffer
     IBUF_2_N     : in  std_logic_vector(12 downto 0); -- negagitve diff. in buffer
     IBUF_3_P     : in  std_logic_vector(12 downto 0); -- positive diff. in buffer
     IBUF_3_N     : in  std_logic_vector(12 downto 0); -- negagitve diff. in buffer
     IBUF_4_P     : in  std_logic_vector(12 downto 0); -- positive diff. in buffer
     IBUF_4_N     : in  std_logic_vector(12 downto 0); -- negagitve diff. in buffer
                  
     DDELAY_0     : out std_logic_vector(12 downto 0); --output dat with delay
     DDELAY_1     : out std_logic_vector(12 downto 0); --output dat with delay
     DDELAY_2     : out std_logic_vector(12 downto 0); --output dat with delay
     DDELAY_3     : out std_logic_vector(12 downto 0); --output dat with delay
     DDELAY_4     : out std_logic_vector(12 downto 0); --output dat with delay
     DELAY_LOCKED : out std_logic );
 end delay_input;
 
 architecture beh of delay_input is
   type matrice_5x13 is array(4 downto 0) of std_logic_vector(12 downto 0);
   type matrice_5xN is array(4 downto 0) of natural;
   signal inbuf           : matrice_5x13;
   signal outbuf          : matrice_5x13;
   -- the formula to have the adc data delay  is
   --    DAT_DLY_ACDx = ( 1.049 ns - DATA_DELAY_NET_ADCx) / 78 ps
   -- it must be equal to 1 for the EVAL_UUB board
   -- DATA_DELAY_NET_ADC0 =
   -- DATA_DELAY_NET_ADC1 =
   -- DATA_DELAY_NET_ADC2 =
   -- DATA_DELAY_NET_ADC3 =
   -- DATA_DELAY_NET_ADC4 =
   constant DAT_DLY_ADC0  : natural := 30;
   constant DAT_DLY_ADC1  : natural := 30;
   constant DAT_DLY_ADC2  : natural := 30;
   constant DAT_DLY_ADC3  : natural := 30;
   constant DAT_DLY_ADC4  : natural := 30;
   constant int_delay_val : matrice_5xN :=(DAT_DLY_ADC4,
                                           DAT_DLY_ADC3,DAT_DLY_ADC2,
                                           DAT_DLY_ADC1,DAT_DLY_ADC0);
--   signal ref_clock_bufg : std_logic;
 begin
 
 --********************************************************* 
 --               Input Buffer declaration
 --********************************************************* 
 
   gen_in_buffer : for i in 0 to 12 generate
     IBUFDS_0 : IBUFDS
       generic map ( DIFF_TERM  => FALSE,        -- Differential termination
                     IOSTANDARD => "LVDS_25")
       port map    ( I  => IBUF_0_P(i), 
                     IB => IBUF_0_N(i),
                     O  => INBUF(0)(i) );
                  
     IBUFDS_1 : IBUFDS
       generic map ( DIFF_TERM  => FALSE,        -- Differential termination
                     IOSTANDARD => "LVDS_25")
       port map    ( I  => IBUF_1_P(i), 
                     IB => IBUF_1_N(i),
                     O  => INBUF(1)(i) );
                  
     IBUFDS_2 : IBUFDS
       generic map ( DIFF_TERM  => FALSE,        -- Differential termination
                     IOSTANDARD => "LVDS_25")
       port map    ( I  => IBUF_2_P(i), 
                     IB => IBUF_2_N(i),
                     O  => INBUF(2)(i) );
                  
     IBUFDS_3 : IBUFDS
       generic map ( DIFF_TERM  => FALSE,        -- Differential termination
                     IOSTANDARD => "LVDS_25")
       port map    ( I  => IBUF_3_P(i), 
                     IB => IBUF_3_N(i),
                     O  => INBUF(3)(i) );
                  
     IBUFDS_4 : IBUFDS
       generic map ( DIFF_TERM  => FALSE,        -- Differential termination
                     IOSTANDARD => "LVDS_25")
       port map    ( I  => IBUF_4_P(i), 
                     IB => IBUF_4_N(i),
                     O  => INBUF(4)(i) );
   end generate;

 --******************************************************
 --          IDELAYCTRL is needed for calibration
 --******************************************************
 
 delayctrl : IDELAYCTRL
   port map ( RDY    => delay_locked,
              REFCLK => ref_clock,
              RST    => IO_RESET );
 
                
 --******************************************************
 --             Instantiate the delay primitive
 --******************************************************

  gen_delaye2 : for j in 0 to 4 generate
    gen_del_chip : for i in 0 to 12 generate
      idelaye2_bus : IDELAYE2
        generic map ( CINVCTRL_SEL           => "FALSE",  -- TRUE, FALSE
                      DELAY_SRC              => "IDATAIN",-- IDATAIN, DATAIN
                      HIGH_PERFORMANCE_MODE  => "FALSE",  -- TRUE, FALSE
                      IDELAY_TYPE            => "FIXED",  -- FIXED, VARIABLE, VAR_LOADABLE
                      IDELAY_VALUE           => int_delay_val(j),-- 0 to 31
                      REFCLK_FREQUENCY       => 200.0,
                      PIPE_SEL               => "FALSE",
                      SIGNAL_PATTERN         => "DATA")   -- CLOCK, DATA
        port map    ( DATAIN                 => '0',     -- Data from FPGA logic
                      C                      => '0',
                      CE                     => '0',
                      INC                    => '0',
                      IDATAIN                => inbuf(j)(i), -- Driven by IOB
                      DATAOUT                => outbuf(j)(i),
                      LD                     => '0',
                      REGRST                 => '0',
                      LDPIPEEN               => '0',
                      CNTVALUEIN             => "00000",
                      CNTVALUEOUT            => open,
                      CINVCTRL               => '0' );
      end generate gen_del_chip;
  end generate gen_delaye2;
  
  DDELAY_0 <= INBUF(0); -- outbuf(0);
  DDELAY_1 <= INBUF(1); -- outbuf(1);
  DDELAY_2 <= INBUF(2); -- outbuf(2);
  DDELAY_3 <= INBUF(3); -- outbuf(3);
  DDELAY_4 <= INBUF(4); -- outbuf(4);

 end beh;
 
 