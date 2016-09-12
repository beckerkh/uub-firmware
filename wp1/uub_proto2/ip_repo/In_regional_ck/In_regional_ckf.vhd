
 --
 -------------------------------------------------------------------------------
 -- Filename:        In_regional_ck.vhd
 --
 -- Description:     
 --                  
 -- VHDL-Standard:   VHDL'93
 -------------------------------------------------------------------------------
 -- Structure:   
 --              In_regional_ck.vhd
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
 -- Date
 --  20141219 - la sortie Clock est inversée pour prendre en compte le 
 --             changement de phase de l'horloge de l'ADC AD9628
 -------------------------------------------------------------------------------
 
 library IEEE;
 use IEEE.std_logic_1164.all;
 use IEEE.numeric_std.all;
 
 library UNISIM;
 use UNISIM.VComponents.all;
 
 entity In_regional_ck is
   port (
     -- ports for differential signaling input buffer
     IBUF_DS_P  : in    std_logic;
     IBUF_DS_N  : in    std_logic;
     IBUF_OUT   : out   std_logic );
 end In_regional_ck;
 
 architecture IMP of In_regional_ck is
   signal OUT_BUF : std_logic;
 begin
  
   IBUFDS_I : IBUFGDS
     port map ( I  => IBUF_DS_P, 
                IB => IBUF_DS_N,
--                O  => IBUF_OUT);
                O  => OUT_BUF);
                
   IBUF_OUT <= not OUT_BUF;
            
--    BUFMR_I : BUFG
--      port map ( I => OUT_BUF,
--                 O => IBUF_OUT);
               
 end IMP;
 
 