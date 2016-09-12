-----------------------------------------------------------------------
-- Company: INFN
-- Roberto Assiro
-- LED CONTROLLER
-----------------------------------------------------------------------
library ieee;
  use ieee.std_logic_1164.all;
  use ieee.std_logic_arith.all;
  use ieee.std_logic_unsigned.all;
 
entity WP1_LED_Control is
  Port ( LED_ASY      : in  std_logic;
  
         INT_TRIG     : in  std_logic;
         enable       : out  std_logic);        
         
end WP1_LED_Control;

architecture Behavioral of WP1_LED_Control is
   
  signal conteggio : std_logic_vector (15 downto 0);
  signal wea_reg : std_logic_vector (3 downto 0);

  signal enable_mem : std_logic;
  
  signal flop_flip  : std_logic;
  signal trig0  : std_logic;
  signal trig1  : std_logic;
  signal trig2  : std_logic;
  signal trig3  : std_logic;
  signal trig_ok  : std_logic;
  signal veto_trig  : std_logic;
  
begin
            


-- gestione trigger interno
--process (RSTn,FPGA_CK)
--begin
-- if RSTn = '0' then
--    trig0 <= '0';
--    trig1 <= '0';
--    trig2 <= '0';
--    trig3 <= '0';
-- elsif rising_edge(FPGA_CK) then
--    trig0 <= INT_TRIG;
--    trig1 <= trig0;
--    trig2 <= trig1;
--    trig3 <= trig2;
-- end if;
--end process;




end Behavioral;


