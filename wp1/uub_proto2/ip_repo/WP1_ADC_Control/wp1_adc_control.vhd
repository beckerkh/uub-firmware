-----------------------------------------------------------------------
-- Company: INFN
-- Roberto Assiro, Pietro Creti
-- ADC data module
-----------------------------------------------------------------------
library ieee;
  use ieee.std_logic_1164.all;
  use ieee.std_logic_arith.all;
  use ieee.std_logic_unsigned.all;
 
entity WP1_ADC_Control is
  Port ( RSTn      : in  std_logic;
  
      -- Interface ADC AD9628 (CLK)
         FPGA_CK   : in  std_logic;
         BRAM_CK   : out  std_logic;
         
      -- Interface ADC AD9628 (Datas)
         adc0      : in  std_logic_vector(23 downto 0);
         adc1      : in  std_logic_vector(23 downto 0);
         adc2      : in  std_logic_vector(23 downto 0);
         adc3      : in  std_logic_vector(23 downto 0);
         adc4      : in  std_logic_vector(23 downto 0);

           -- Interface bus to bram
         ADC0_mem            : out  std_logic_vector(31 downto 0);
         ADC1_mem            : out  std_logic_vector(31 downto 0);
         ADC2_mem            : out  std_logic_vector(31 downto 0);
         ADC3_mem            : out  std_logic_vector(31 downto 0);
         ADC4_mem            : out  std_logic_vector(31 downto 0);
          
         ADDR_mem            : out  std_logic_vector(31 downto 0);        
            
--         ADC_data            : out  std_logic_vector(119 downto 0);   -- Interface bus to trigger      

         Stop_Addr           : buffer  std_logic_vector(15 downto 0); -- byte del valore del conteggio di indirizzo bram all'arrivo del trigger esterno ultimo valore

         wea                 : out  std_logic_vector(3 downto 0);
         ASY_TRIG            : out  std_logic;
         EXT_TRIG            : in   std_logic;
         TRIG_OUT            : out  std_logic;
         Config_Trig         : in   std_logic_vector(31 downto 0);
         ENABLE_PPS          : out  std_logic;  
         
         TRIG_TRESH          : in   std_logic_vector(31 downto 0);
         
         enable              : out  std_logic);        
                  
end WP1_ADC_Control;

architecture Behavioral of WP1_ADC_Control is

  type matrice_2x13 is array (1 downto 0) of std_logic_vector(12 downto 0);
  
--  signal TRIG_OUT_INT  : std_logic;
  signal adc0_data : std_logic_vector (31 downto 0);
  signal adc1_data : std_logic_vector (31 downto 0);
  signal adc2_data : std_logic_vector (31 downto 0);
  signal adc3_data : std_logic_vector (31 downto 0);
  signal adc4_data : std_logic_vector (31 downto 0);
  
  signal buffer1 : std_logic_vector (23 downto 0);
  signal buffer2 : std_logic_vector (23 downto 0);
  signal buffer3 : std_logic_vector (23 downto 0);
  signal buffer4 : std_logic_vector (23 downto 0);
  signal buffer5 : std_logic_vector (23 downto 0);
  
  signal threshold1 : std_logic_vector (11 downto 0); 
  signal threshold2 : std_logic_vector (11 downto 0);
  signal threshold3 : std_logic_vector (11 downto 0);
  signal trig_mode  : std_logic;

--  signal threshold4_0 : std_logic_vector (11 downto 0);
--  signal threshold5_0 : std_logic_vector (11 downto 0);
--  signal threshold1_1 : std_logic_vector (11 downto 0); 
--  signal threshold2_1 : std_logic_vector (11 downto 0);
--  signal threshold3_1 : std_logic_vector (11 downto 0);
--  signal threshold4_1 : std_logic_vector (11 downto 0);
--  signal threshold5_1 : std_logic_vector (11 downto 0);
    
  signal conteggio : std_logic_vector (15 downto 0);
  signal wea_reg : std_logic_vector (3 downto 0);
  
  signal after_trig_count : std_logic_vector (11 downto 0);
  signal more_count : std_logic;
  
  signal enable_mem : std_logic;
  signal trig_flag : std_logic;
  signal run_stop : std_logic;
  
  --signal flop_flip  : std_logic;
  signal trig0  : std_logic;
  signal trig1  : std_logic;
  signal trig2  : std_logic;
  signal trig3  : std_logic;
  signal trig_ok  : std_logic;

  signal arm_trig0  : std_logic;
  signal arm_trig1  : std_logic;
  signal arm_trig2  : std_logic;
  signal arm_trig3  : std_logic;
  signal arm_trig_ok  : std_logic;

  signal veto_trig  : std_logic;
  
--  signal data_latch  : std_logic_vector (119 downto 0); -- latch for data SDE trigger
  
begin

process(RSTn, FPGA_CK) 
 begin
   if RSTn = '0' then
       threshold1 <= "100000000000"; 
       threshold2 <= "100000000000";
       threshold3 <= "100000000000";
       trig_mode <= '0';    
   elsif rising_edge(FPGA_CK) then 

        if (TRIG_TRESH(31) = '1') then
            threshold1 <= TRIG_TRESH(9 downto 0) & "00";
            threshold2 <= TRIG_TRESH(19 downto 10) & "00";
            threshold1 <= TRIG_TRESH(29 downto 20) & "00";
            trig_mode <= TRIG_TRESH(30);
        end if;
   end if;
end process;        


 --******************************************************************
 -- Trigger interno su threshold
 --******************************************************************
 process(RSTn, FPGA_CK) 
 begin
   if RSTn = '0' then
--       threshold1_0 <= "011111111111"; -- preset a mezza scala   
       trig_flag <= '0';   
   elsif rising_edge(FPGA_CK) then 
--        threshold <= Config_Trig(19 downto 8);
--        if ((adc0(11 downto 0) < threshold1_0 or adc0(24 downto 13) < threshold1_0) 
--           or (adc1(11 downto 0) < threshold1_0 or adc1(24 downto 13) < threshold1_0)  
--           or (adc2(11 downto 0) < threshold1_0 or adc2(24 downto 13) < threshold1_0) 
--           or (adc3(11 downto 0) < threshold1_0 or adc3(24 downto 13) < threshold1_0)
--           or (adc4(11 downto 0) < threshold1_0 or adc4(24 downto 13) < threshold1_0)) then
          if (trig_mode = '1') then
                if ((adc0(11 downto 0) <  threshold1) AND 
                    (adc1(11 downto 0) <  threshold2) AND
                    (adc2(11 downto 0) <  threshold2)) then
                        trig_flag <= '1';
                else
                        trig_flag <= '0';
                end if;
                
          else
                if ((adc0(11 downto 0) <  threshold1) or 
                    (adc1(11 downto 0) <  threshold2) or
                    (adc2(11 downto 0) <  threshold2)) then
                        trig_flag <= '1';
                 else
                        trig_flag <= '0';
                 end if;      
         end if;
   end if;
end process;        

 --******************************************************************
 -- Risincronizzazione dei dati
 --******************************************************************
 process(RSTn, FPGA_CK)  
 begin
   if RSTn = '0' then
       adc0_data <= (others => '0');
       adc1_data <= (others => '0');
       adc2_data <= (others => '0');
       adc3_data <= (others => '0');
       adc4_data <= (others => '0');
--       ADC_data  <= (others => '0');          
--       flop_flip <= '0';
   elsif rising_edge(FPGA_CK) then   
--     scrittura diretta dei dati come vengono da splittaggio  il bit di overflow e' scartato
       adc0_data <= "0000" & adc0(23 downto 12) & "0000" & adc0(11 downto 0);    
       adc1_data <= "0000" & adc1(23 downto 12) & "0000" & adc1(11 downto 0);                            
       adc2_data <= "0000" & adc2(23 downto 12) & "0000" & adc2(11 downto 0);    
       adc3_data <= "0000" & adc3(23 downto 12) & "0000" & adc3(11 downto 0);
       adc4_data <= "0000" & adc4(23 downto 12) & "0000" & adc4(11 downto 0);  
        
--     ADC data to SDE trigger without over flow bit --- 10x12 bit      
--       ADC_data <= (4095 - adc4(24 downto 13)) & (4095 - adc4(11 downto 0)) & (4095 - adc3(24 downto 13)) & (4095 - adc3(11 downto 0)) & (4095 - adc2(24 downto 13)) & (4095 - adc2(11 downto 0)) & (4095 - adc1(24 downto 13)) & (4095 - adc1(11 downto 0)) & (4095 - adc0(24 downto 13)) & (4095 - adc0(11 downto 0));

  end if;
 end process;

--********************************************************************
--                 Data Addressing in BRAM 
--********************************************************************
 process(RSTn, FPGA_CK)
 begin
if RSTn = '0' then    
        conteggio <= (others => '0');
        wea_reg <= (others => '1');
        enable_mem <= '1';
        veto_trig <= '0';
        run_stop <= '0';
       
        Stop_Addr <= (others => '0');
        more_count <= '0';
        after_trig_count <= (others => '0');
        
elsif rising_edge(FPGA_CK) then
    if (Config_Trig(1) = '1') then -- se il bit 1 di config_trig e' 1 agisco sul trigger interno generato sul bit 0 di config_trig
            if ((trig_ok = '1') AND (veto_trig = '0')) then
                veto_trig <= '1';
                enable_mem <= '1';
                wea_reg <= "1111";
                conteggio <= (others => '0');     
            elsif   (conteggio = "0011111111111100") then -- mi fermo con indirizzamento a 4k per 32 bit equivalente a indirizzo 3FFC
                  conteggio <= conteggio ;
                  enable_mem <= '0';
                  wea_reg <= "1111";   
                  veto_trig <= '0';
            else
                enable_mem <= '1';
                wea_reg <= "1111";     
                conteggio <= conteggio + "0000000000000100"; -- incremento di 4 locazioni da 8 bit quindi 32 bit per riga              
            end if;    
--  TRIGGER ESTERNO SE BIT 1 di conf_trig = 0
    elsif (Config_Trig(1) = '0') then  -- se il bit 1 di config_trig e' 0 agisco sul trigger esterno immesso su ext_trig


            if (arm_trig_ok = '1') then      -- controllo bit 7 per riarmare attesa trigger esterno dopo aver letto il valore del conteggio dell'indirizzo in ram
                run_stop <= '0';
                conteggio <= (others => '0');
                Stop_Addr <= (others => '0');
                more_count <= '0';
                after_trig_count <= (others => '0');
            end if;
--- incremento indirizzo bram e dopo arrivo trigger acquisisco per altri 2048 colpi di clock per acquisire evento prima e dopo trigger ----------------------------            
            if ((run_stop = '0') or (more_count = '1'))  then
                enable_mem <= '1';
                wea_reg <= "1111";
                if   (conteggio = "0011111111111100") then 
                    conteggio <= (others => '0');
                else                
                    conteggio <= conteggio + "0000000000000100"; -- incremento di 4 locazioni da 8 bit quindi 32 bit per riga
                end if;
            end if;
            
            if ((trig_ok = '1') AND (veto_trig = '0'))  then
                 veto_trig <= '1';
                 run_stop <= '1';
                 more_count <= '1';
                 after_trig_count <= "011111111111"; -- carico contatore eventi da salvare in bram dopo arrivo trigger
                 Stop_Addr <= conteggio; 
            end if;

          if (veto_trig = '1') then
              if (after_trig_count = "000000000000")  then
                  more_count <= '0';
                  run_stop <= '1';                   
                  veto_trig <= '0';
                  enable_mem <= '0';
                  wea_reg <= "1111";    
                  Stop_Addr <= "1000000000000000" or Stop_Addr;                                  
              else
                  after_trig_count <= after_trig_count - "000000000001"; -- decremento contatore eventi da memorizzare dopo arrivo trigger
              end if;  
           end if;
                
    end if;
end if;
end process;


--------------------------------------------------------------------
-- gestione trigger interno ed esterno
process (RSTn,FPGA_CK)
begin
 if RSTn = '0' then
    trig0 <= '0';
    trig1 <= '0';
    trig2 <= '0';
    trig3 <= '0';
 elsif rising_edge(FPGA_CK) then
    trig0 <= Config_Trig(0) or (EXT_TRIG and (not Config_Trig(1)));
    trig1 <= trig0;
    trig2 <= trig1;
    trig3 <= trig2;
 end if;
end process;
trig_ok <= (not trig3) AND trig1;
-----------------------------------------------------------------------------
-- gestione riarmo trigger  ed esterno
process (RSTn,FPGA_CK)
begin
 if RSTn = '0' then
    arm_trig0 <= '0';
    arm_trig1 <= '0';
    arm_trig2 <= '0';
    arm_trig3 <= '0';
 elsif rising_edge(FPGA_CK) then
    arm_trig0 <= Config_Trig(7);
    arm_trig1 <= arm_trig0;
    arm_trig2 <= arm_trig1;
    arm_trig3 <= arm_trig2;
 end if;
end process;
arm_trig_ok <= (not arm_trig3) AND arm_trig1;



ADDR_mem <= "0000000000000000" & conteggio; -- indirizzamento continuo memoria
ADC0_mem <= adc0_data; -- collego dati su porta
ADC1_mem <= adc1_data;
ADC2_mem <= adc2_data; 
ADC3_mem <= adc3_data; 
ADC4_mem <= adc4_data; 
wea <= wea_reg;
enable <= enable_mem; -- enable sempre messo a 1, metto a zero se voglio bloccare scrittura in memoria
BRAM_CK <= not FPGA_CK;
ASY_TRIG <= Config_Trig(2);
ENABLE_PPS <= Config_Trig(3);
TRIG_OUT <= trig_flag;


end Behavioral;


