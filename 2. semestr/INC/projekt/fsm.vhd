-- fsm.vhd: Finite State Machine
-- Author(s): Zden�k Brhel
--
library ieee;
use ieee.std_logic_1164.all;
-- ----------------------------------------------------------------------------
--                        Entity declaration
-- ----------------------------------------------------------------------------
entity fsm is
port(
   CLK         : in  std_logic;
   RESET       : in  std_logic;

   -- Input signals
   KEY         : in  std_logic_vector(15 downto 0);
   CNT_OF      : in  std_logic;

   -- Output signals
   FSM_CNT_CE  : out std_logic;
   FSM_MX_MEM  : out std_logic;
   FSM_MX_LCD  : out std_logic;
   FSM_LCD_WR  : out std_logic;
   FSM_LCD_CLR : out std_logic
);
end entity fsm;

-- ----------------------------------------------------------------------------
--                      Architecture declaration
-- ----------------------------------------------------------------------------
architecture behavioral of fsm is
   type t_state is (TEST1, TEST2, TEST3, TEST4, TEST5, TEST6, TEST7, TEST8, TEST9, TEST10, TEST11, TEST12, TEST13, TEST14, TEST15, PRINT_MESSAGE, FAILURE, PRINT_MESSAGE_T, LASTKEY, FINISH);
   signal present_state, next_state : t_state;

begin
-- -------------------------------------------------------
sync_logic : process(RESET, CLK)
begin
   if (RESET = '1') then
      present_state <= TEST1;
   elsif (CLK'event AND CLK = '1') then
      present_state <= next_state;
   end if;
end process sync_logic;

-- -------------------------------------------------------
next_state_logic : process(present_state, KEY, CNT_OF)
begin
   case (present_state) is
   -- - - - - - - - - - - - - - - - - - - - - - -
   when TEST1 =>
      next_state <= TEST1;
      if (KEY(15) = '1') then
         next_state <= PRINT_MESSAGE; 
      end if;
      if (KEY(2) = '1') then
        next_state <= TEST2;
      elsif (KEY(14 downto 0) /= "000000000000000") then
        next_state <= FAILURE; 
      end if;
      
   -- - - - - - - - - - - - - - - - - - - - - - -
   when TEST2 =>
      next_state <= TEST2;
      if (KEY(15) = '1') then
         next_state <= PRINT_MESSAGE; 
      end if;
      if (KEY(9) = '1') then
        next_state <= TEST3;
      elsif (KEY(14 downto 0) /= "000000000000000") then
        next_state <= FAILURE;
      end if;
   ----------------------------------------------
    when TEST3 =>
      next_state <= TEST3;
      if (KEY(15) = '1') then
         next_state <= PRINT_MESSAGE; 
      end if;
      if (KEY(7) = '1') then
        next_state <= TEST4;
      elsif (KEY(14 downto 0) /= "000000000000000") then
        next_state <= FAILURE;
      end if;
   ----------------------------------------------
    when TEST4 =>
      next_state <= TEST4;
      if (KEY(15) = '1') then
         next_state <= PRINT_MESSAGE; 
      end if;
      if (KEY(6) = '1') then
        next_state <= TEST5;
      elsif (KEY(14 downto 0) /= "000000000000000") then
        next_state <= FAILURE;
      end if;
   ----------------------------------------------
    when TEST5 =>
      next_state <= TEST5;
      if (KEY(15) = '1') then
         next_state <= PRINT_MESSAGE; 
      end if;
      if (KEY(8) = '1') then
        next_state <= TEST6;
      elsif (KEY(4) = '1') then
        next_state <= TEST11;                    
      elsif (KEY(14 downto 0) /= "000000000000000") then
        next_state <= FAILURE;
      end if;
   ----------------------------------------------
    when TEST6 =>
      next_state <= TEST6;
      if (KEY(15) = '1') then
         next_state <= PRINT_MESSAGE; 
      end if;
      if (KEY(5) = '1') then
        next_state <= TEST7;
      elsif (KEY(14 downto 0) /= "000000000000000") then
        next_state <= FAILURE;
      end if;
   ----------------------------------------------
    when TEST7 =>
      next_state <= TEST7;
      if (KEY(15) = '1') then
         next_state <= PRINT_MESSAGE; 
      end if;
      if (KEY(4) = '1') then
        next_state <= TEST8;
      elsif (KEY(14 downto 0) /= "000000000000000") then
        next_state <= FAILURE;
      end if;
   ---------------------------------------------- 
    when TEST8 =>
      next_state <= TEST8;
      if (KEY(15) = '1') then
         next_state <= PRINT_MESSAGE; 
      end if;
      if (KEY(4) = '1') then
        next_state <= TEST9;
      elsif (KEY(14 downto 0) /= "000000000000000") then
        next_state <= FAILURE;
      end if;
   ----------------------------------------------
    when TEST9 =>
      next_state <= TEST9;
      if (KEY(15) = '1') then
         next_state <= PRINT_MESSAGE; 
      end if;
      if (KEY(3) = '1') then
        next_state <= TEST10;
      elsif (KEY(14 downto 0) /= "000000000000000") then
        next_state <= FAILURE;
      end if;
   ----------------------------------------------
    when TEST10 =>
      next_state <= TEST10;
      if (KEY(15) = '1') then
         next_state <= PRINT_MESSAGE; 
      end if;
      if (KEY(2) = '1') then
        next_state <= LASTKEY;
      elsif (KEY(14 downto 0) /= "000000000000000") then
        next_state <= FAILURE;
      end if;
   ----------------------------------------------
    when TEST11 =>
      next_state <= TEST11;
      if (KEY(15) = '1') then
         next_state <= PRINT_MESSAGE; 
      end if;
      if (KEY(2) = '1') then
        next_state <= TEST12;
      elsif (KEY(14 downto 0) /= "000000000000000") then
        next_state <= FAILURE;
      end if;
   ----------------------------------------------
    when TEST12 =>
      next_state <= TEST12;
      if (KEY(15) = '1') then
         next_state <= PRINT_MESSAGE; 
      end if;
      if (KEY(7) = '1') then
        next_state <= TEST13;
      elsif (KEY(14 downto 0) /= "000000000000000") then
        next_state <= FAILURE;
      end if;
   ----------------------------------------------
    when TEST13 =>
      next_state <= TEST13;
      if (KEY(15) = '1') then
         next_state <= PRINT_MESSAGE; 
      end if;
      if (KEY(2) = '1') then
        next_state <= TEST14;
      elsif (KEY(14 downto 0) /= "000000000000000") then
        next_state <= FAILURE;
      end if;
   ----------------------------------------------
    when TEST14 =>
      next_state <= TEST14;
      if (KEY(15) = '1') then
         next_state <= PRINT_MESSAGE; 
      end if;
      if (KEY(1) = '1') then
        next_state <= TEST15;
      elsif (KEY(14 downto 0) /= "000000000000000") then
        next_state <= FAILURE;
      end if;
   ----------------------------------------------
    when TEST15 =>
      next_state <= TEST15;
      if (KEY(15) = '1') then
         next_state <= PRINT_MESSAGE; 
      end if;
      if (KEY(6) = '1') then
        next_state <= LASTKEY;
      elsif (KEY(14 downto 0) /= "000000000000000") then
        next_state <= FAILURE;
      end if;
   ----------------------------------------------
    when LASTKEY =>
      next_state <= LASTKEY;
      if (KEY(15) = '1') then
         next_state <= PRINT_MESSAGE_T;
      elsif (KEY(14 downto 0) /= "000000000000000") then
        next_state <= FAILURE; 
      end if;
   ----------------------------------------------
    when FAILURE =>
      next_state <= FAILURE;
      if (KEY(15) = '1') then
         next_state <= PRINT_MESSAGE;
      elsif (KEY(14 downto 0) /= "000000000000000") then
        next_state <= FAILURE; 
      end if;
   ----------------------------------------------     
   when PRINT_MESSAGE =>
      next_state <= PRINT_MESSAGE;
      if (CNT_OF = '1') then
         next_state <= FINISH;
      end if;
   -- - - - - - - - - - - - - - - - - - - - - - -
   when PRINT_MESSAGE_T =>
      next_state <= PRINT_MESSAGE_T;
      if (CNT_OF = '1') then
         next_state <= FINISH;
      end if;
   -- - - - - - - - - - - - - - - - - - - - - - -
   when FINISH =>
      next_state <= FINISH;
      if (KEY(15) = '1') then
         next_state <= TEST1; 
      end if;
   -- - - - - - - - - - - - - - - - - - - - - - -
   when others =>
      next_state <= TEST1;
   end case;
end process next_state_logic;

-- -------------------------------------------------------
output_logic : process(present_state, KEY)
begin
   FSM_CNT_CE     <= '0';
   FSM_MX_MEM     <= '0';
   FSM_MX_LCD     <= '0';
   FSM_LCD_WR     <= '0';
   FSM_LCD_CLR    <= '0';

   case (present_state) is
   -- - - - - - - - - - - - - - - - - - - - - - -
   when TEST1 =>
      if (KEY(14 downto 0) /= "000000000000000") then
         FSM_LCD_WR     <= '1';
      end if;
      if (KEY(15) = '1') then
         FSM_LCD_CLR    <= '1';
      end if;
   -- - - - - - - - - - - - - - - - - - - - - - -
   when TEST2 =>
      if (KEY(14 downto 0) /= "000000000000000") then
         FSM_LCD_WR     <= '1';
      end if;
      if (KEY(15) = '1') then
         FSM_LCD_CLR    <= '1';
      end if;
   -- - - - - - - - - - - - - - - - - - - - - - -
   when TEST3 =>
      if (KEY(14 downto 0) /= "000000000000000") then
         FSM_LCD_WR     <= '1';
      end if;
      if (KEY(15) = '1') then
         FSM_LCD_CLR    <= '1';
      end if;
   -- - - - - - - - - - - - - - - - - - - - - - -
   when TEST4 =>
      if (KEY(14 downto 0) /= "000000000000000") then
         FSM_LCD_WR     <= '1';
      end if;
      if (KEY(15) = '1') then
         FSM_LCD_CLR    <= '1';
      end if;
   -- - - - - - - - - - - - - - - - - - - - - - -
   when TEST5 =>
      if (KEY(14 downto 0) /= "000000000000000") then
         FSM_LCD_WR     <= '1';
      end if;
      if (KEY(15) = '1') then
         FSM_LCD_CLR    <= '1';
      end if;
   -- - - - - - - - - - - - - - - - - - - - - - -
   when TEST6 =>
      if (KEY(14 downto 0) /= "000000000000000") then
         FSM_LCD_WR     <= '1';
      end if;
      if (KEY(15) = '1') then
         FSM_LCD_CLR    <= '1';
      end if;
   -- - - - - - - - - - - - - - - - - - - - - - -
   when TEST7 =>
      if (KEY(14 downto 0) /= "000000000000000") then
         FSM_LCD_WR     <= '1';
      end if;
      if (KEY(15) = '1') then
         FSM_LCD_CLR    <= '1';
      end if;
   -- - - - - - - - - - - - - - - - - - - - - - -
   when TEST8 =>
      if (KEY(14 downto 0) /= "000000000000000") then
         FSM_LCD_WR     <= '1';
      end if;
      if (KEY(15) = '1') then
         FSM_LCD_CLR    <= '1';
      end if;
   -- - - - - - - - - - - - - - - - - - - - - - -
   when TEST9 =>
      if (KEY(14 downto 0) /= "000000000000000") then
         FSM_LCD_WR     <= '1';
      end if;
      if (KEY(15) = '1') then
         FSM_LCD_CLR    <= '1';
      end if;
   -- - - - - - - - - - - - - - - - - - - - - - -
   when TEST10 =>
      if (KEY(14 downto 0) /= "000000000000000") then
         FSM_LCD_WR     <= '1';
      end if;
      if (KEY(15) = '1') then
         FSM_LCD_CLR    <= '1';
      end if;
   -- - - - - - - - - - - - - - - - - - - - - - -
   when TEST11 =>
      if (KEY(14 downto 0) /= "000000000000000") then
         FSM_LCD_WR     <= '1';
      end if;
      if (KEY(15) = '1') then
         FSM_LCD_CLR    <= '1';
      end if;
   -- - - - - - - - - - - - - - - - - - - - - - -
   when TEST12 =>
      if (KEY(14 downto 0) /= "000000000000000") then
         FSM_LCD_WR     <= '1';
      end if;
      if (KEY(15) = '1') then
         FSM_LCD_CLR    <= '1';
      end if;
   -- - - - - - - - - - - - - - - - - - - - - - -
   when TEST13 =>
      if (KEY(14 downto 0) /= "000000000000000") then
         FSM_LCD_WR     <= '1';
      end if;
      if (KEY(15) = '1') then
         FSM_LCD_CLR    <= '1';
      end if;
   -- - - - - - - - - - - - - - - - - - - - - - -
   when TEST14 =>
      if (KEY(14 downto 0) /= "000000000000000") then
         FSM_LCD_WR     <= '1';
      end if;
      if (KEY(15) = '1') then
         FSM_LCD_CLR    <= '1';
      end if;
   -- - - - - - - - - - - - - - - - - - - - - - -
   when TEST15 =>
      if (KEY(14 downto 0) /= "000000000000000") then
         FSM_LCD_WR     <= '1';
      end if;
      if (KEY(15) = '1') then
         FSM_LCD_CLR    <= '1';
      end if;
   -- - - - - - - - - - - - - - - - - - - - - - -
   when FAILURE =>
      if (KEY(14 downto 0) /= "000000000000000") then
         FSM_LCD_WR     <= '1';
      end if;
      if (KEY(15) = '1') then
         FSM_LCD_CLR    <= '1';
      end if;
   -- - - - - - - - - - - - - - - - - - - - - - -
   when LASTKEY =>
      if (KEY(14 downto 0) /= "000000000000000") then
         FSM_LCD_WR     <= '1';
      end if;
      if (KEY(15) = '1') then
         FSM_LCD_CLR    <= '1';
      end if;
   -- - - - - - - - - - - - - - - - - - - - - - -
   when PRINT_MESSAGE =>
      FSM_CNT_CE     <= '1';
      FSM_MX_LCD     <= '1';
      FSM_MX_MEM     <= '0';
      FSM_LCD_WR     <= '1';
   -- - - - - - - - - - - - - - - - - - - - - - -
   when PRINT_MESSAGE_T =>
      FSM_CNT_CE     <= '1';
      FSM_MX_LCD     <= '1';
      FSM_MX_MEM     <= '1';
      FSM_LCD_WR     <= '1';
   -- - - - - - - - - - - - - - - - - - - - - - -
   when FINISH =>
      if (KEY(15) = '1') then
         FSM_LCD_CLR    <= '1';
      end if;
   -- - - - - - - - - - - - - - - - - - - - - - -
   when others =>
   end case;
end process output_logic;

end architecture behavioral;

