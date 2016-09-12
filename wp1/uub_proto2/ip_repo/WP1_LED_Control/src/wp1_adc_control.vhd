-----------------------------------------------------------------------
-- Company: INFN
-- Roberto Assiro written 12/2015
-- LED CONTROLLER
-- Logic to controll LED pulse sincronized with PPS signal
-----------------------------------------------------------------------
library ieee;
  use ieee.std_logic_1164.all;
  use ieee.std_logic_arith.all;
  use ieee.std_logic_unsigned.all;
 
entity WP1_LED_Control is
  Port ( LED_OUT         : out  std_logic; 
         PPS             : in   std_logic;
         ASY_TRIG        : in   std_logic;
         clock           : in   std_logic;
         reset           : in   std_logic;
         data_set        : in   std_logic_vector (31 downto 0);
--         INT_TRIG        : out  std_logic;
         ENABLE_PPS      : in   std_logic);        
         
end WP1_LED_Control;

architecture Behavioral of WP1_LED_Control is
  
  signal mem_pps        : std_logic;
  signal veto_trig      : std_logic;
  signal led_pps        : std_logic;
  signal async_trig     : std_logic;
  signal veto_counter   : std_logic_vector (15 downto 0);
  
  
    begin
  
    process (reset, clock)

        begin
        if reset = '0' then
                veto_trig <= '0';
                mem_pps <= '0';
                led_pps <='0';
                veto_counter <= (others => '0');
                
        elsif (clock 'event and clock = '1') then
                if PPS = '1' then
                    mem_pps <= '1';        
                else
                    mem_pps <= mem_pps;
                end if;
        
                if ((mem_pps and PPS and ENABLE_PPS) = '1') then
                    led_pps <= '1';
                end if;
                if led_pps ='1' then
                    if (veto_counter = "1000000000000000") then --ritardo da stabilire relativo alla lunghezza dell'impulso
                        mem_pps <= '0';   
                        veto_counter <= (others => '0');
                        led_pps <='0';
                    else
                        veto_counter <= veto_counter + "0000000000000001";
                    end if;
                end if;
                    
                
        
        end if;
        
end process;

--async_trig <= data_set (31 downto 31);
LED_OUT <= ASY_TRIG or led_pps;
end Behavioral;


