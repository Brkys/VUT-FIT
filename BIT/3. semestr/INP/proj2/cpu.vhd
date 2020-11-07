-- cpu.vhd: Simple 8-bit CPU (BrainLove interpreter)
-- Copyright (C) 2017 Brno University of Technology,
--                    Faculty of Information Technology
-- Author(s): Zdeněk Brhel xbrhel04
--

library ieee;
use ieee.std_logic_1164.all;
use ieee.std_logic_arith.all;
use ieee.std_logic_unsigned.all;

-- ----------------------------------------------------------------------------
--                        Entity declaration
-- ----------------------------------------------------------------------------
entity cpu is
 port (
   CLK   : in std_logic;  -- hodinovy signal
   RESET : in std_logic;  -- asynchronni reset procesoru
   EN    : in std_logic;  -- povoleni cinnosti procesoru
 
   -- synchronni pamet ROM
   CODE_ADDR : out std_logic_vector(11 downto 0); -- adresa do pameti
   CODE_DATA : in std_logic_vector(7 downto 0);   -- CODE_DATA <- rom[CODE_ADDR] pokud CODE_EN='1'
   CODE_EN   : out std_logic;                     -- povoleni cinnosti
   
   -- synchronni pamet RAM
   DATA_ADDR  : out std_logic_vector(9 downto 0); -- adresa do pameti
   DATA_WDATA : out std_logic_vector(7 downto 0); -- mem[DATA_ADDR] <- DATA_WDATA pokud DATA_EN='1'
   DATA_RDATA : in std_logic_vector(7 downto 0);  -- DATA_RDATA <- ram[DATA_ADDR] pokud DATA_EN='1'
   DATA_RDWR  : out std_logic;                    -- cteni z pameti (DATA_RDWR='0') / zapis do pameti (DATA_RDWR='1')
   DATA_EN    : out std_logic;                    -- povoleni cinnosti
   
   -- vstupni port
   IN_DATA   : in std_logic_vector(7 downto 0);   -- IN_DATA obsahuje stisknuty znak klavesnice pokud IN_VLD='1' a IN_REQ='1'
   IN_VLD    : in std_logic;                      -- data platna pokud IN_VLD='1'
   IN_REQ    : out std_logic;                     -- pozadavek na vstup dat z klavesnice
   
   -- vystupni port
   OUT_DATA : out  std_logic_vector(7 downto 0);  -- zapisovana data
   OUT_BUSY : in std_logic;                       -- pokud OUT_BUSY='1', LCD je zaneprazdnen, nelze zapisovat,  OUT_WE musi byt '0'
   OUT_WE   : out std_logic                       -- LCD <- OUT_DATA pokud OUT_WE='1' a OUT_BUSY='0'
 );
end cpu;


-- ----------------------------------------------------------------------------
--                      Architecture declaration
-- ----------------------------------------------------------------------------
architecture behavioral of cpu is

 -- zde dopiste potrebne deklarace signalu
	
	--pc
	signal increasePc: std_logic;
	signal decreasePc: std_logic;
	signal dataPc: std_logic_vector(11 downto 0);
	
	--cnt
	signal dataCnt: std_logic_vector(7 downto 0);
	signal increaseCnt: std_logic;
	signal decreaseCnt: std_logic;
	signal cntdatatmp: std_logic;
	
	--ptr
	signal increasePtr: std_logic;
	signal decreasePtr: std_logic;
	signal dataPtr: std_logic_vector(9 downto 0);
	
	-- tmp
	signal selector: std_logic_vector(1 downto 0);
	signal rdatam: std_logic_vector(7 downto 0);
	signal rdatap: std_logic_vector(7 downto 0);
	signal rdatatmp: std_logic;
	signal setcnt: std_logic;
	
	-- states
	type t_FSM is (startfsm, decodefsm, incPtrfsm, decPtrfsm, UNKNWN, incValfsm, decValfsm, read2fsm,
			   incVal2fsm, decVal2fsm, whileSfsm, whileEfsm, whileS2fsm, whileS3fsm, whileS4fsm,
			   whileE2fsm, whileE3fsm, whileE4fsm, whileE5fsm, readfsm, writefsm, endProgfsm, 
			   write2fsm, whileS5fsm, incVal3fsm, fetchfsm, fetch2fsm, decVal3fsm, 
			   breakfsm, break2fsm, break3fsm, break4fsm);
	signal nstate: t_FSM;
	signal pstate: t_FSM;
	

begin
	
	----------------------
	-- COUNTER REGISTER --
	----------------------
	CNTREG: process(RESET, CLK, setcnt) -- pocitadlo cyklu
	begin
		if(RESET = '1') then
			dataCnt <= "00000000";
		elsif rising_edge(CLK) then
			if(decreaseCnt = '1') then
				dataCnt <= dataCnt - '1';
			elsif (increaseCnt = '1') then
				dataCnt <= dataCnt + '1';
			end if;
		end if;
		if(setcnt = '1') then
			dataCnt <= "00000001";
		end if;
	end process;
	
	
	rdatap <= DATA_RDATA + '1';
	rdatam <= DATA_RDATA - '1'; 
	-----------------
	-- PC REGISTER --
	-----------------
	PCREG: process(RESET, CLK) -- pocitadlo ROM
	begin
		if(RESET = '1') then
			dataPc <= (others => '0');
		elsif rising_edge(CLK) then 
			if(decreasePc = '1') then
				dataPc <= dataPc - '1';
			elsif(increasePc = '1') then
				dataPc <= dataPc + '1';
			end if;
		end if;
	CODE_ADDR <= dataPc;
	end process;
	
	
	----------------------
	-- POINTER REGISTER --
	----------------------
	PTRREG: process(RESET, CLK) -- pocitadlo pointeru
	begin
		if(RESET = '1') then
			dataPtr <= (others => '0');
		elsif rising_edge(CLK) then
			if(decreasePtr = '1') then
				dataPtr <= dataPtr - '1';
			elsif(increasePtr = '1') then
				dataPtr <= dataPtr + '1';
			end if;
		end if;
	DATA_ADDR <= dataPtr;
	end process;
	
	-----------------
	-- MULTIPLEXOR --
	-----------------
	MUX: process (selector)
	begin
		case (selector) is
			when "00" =>
				DATA_WDATA <= IN_DATA;
			when "01" =>
				DATA_WDATA <= rdatam; -- DATA_WDATA - 1
			when "10" =>
				DATA_WDATA <= rdatap; -- DATA_WDATA + 1
			when others =>
				DATA_WDATA <= (others => '0');
		end case;
	end process;
	
	cntdatatmp <= '0' when (dataCnt = "00000000") else '1';  -- pomocne promenne pro while
	rdatatmp <= '0' when (DATA_RDATA = "00000000") else '1';
	
	
	----------------------
	-- NEXT STATE LOGIC --
	----------------------
	PSTATEP: process(CLK, RESET) -- nastavi n(ext)state na p(resent)state
	begin
		if(RESET = '1') then
			pstate <= startfsm;
		elsif rising_edge(CLK) then
			if( EN = '1' ) then
				pstate <= nstate;
			end if;
		end if;
	end process;
	
	STATELOGIC: process(CLK, EN, CODE_DATA, DATA_RDATA, IN_DATA, IN_VLD, OUT_BUSY, rdatatmp, pstate, cntdatatmp)
	begin
		--inicializace proměnných
		increaseCnt <= '0';
		decreaseCnt <= '0';
		increasePc <= '0';
		decreasePc <= '0';
		increasePtr <= '0';
		decreasePtr <= '0';
		-- default selector (nedela nic)
		selector <= "11";
		-- vychozi nastaveni
		CODE_EN <= '0';
		OUT_WE <= '0';
		IN_REQ <= '0';
		DATA_RDWR <= '0';
		DATA_EN <= '0';
		--nastaveni jednicky u cnt
		setcnt <= '0';
		
		case (pstate) is
			when startfsm => -- vychozi konfigurace
				nstate <= fetchfsm;
				
			when fetchfsm =>
				CODE_EN <= '1';
				nstate <= fetch2fsm;
			
			when fetch2fsm =>
				CODE_EN <= '1';
				nstate <= decodefsm;
				
			when decodefsm => -- dekodovani instrukci
				case (CODE_DATA) is
					when X"3C" =>
						nstate <= decPtrfsm;
					when X"3E" =>
						nstate <= incPtrfsm;
					when X"2D" =>
						nstate <= decValfsm;
					when X"2B" => 
						nstate <= incValfsm;
					when X"5B" =>
						nstate <= whileSfsm;
					when X"5D" =>
						nstate <= whileEfsm;
					when X"2E" =>
						nstate <= writefsm;
					when X"2C" =>
						nstate <= readfsm;
					when X"7E" =>
						nstate <= breakfsm;
					when X"00" =>
						nstate <= endProgfsm;
					when others =>
						nstate <= UNKNWN;
				end case;
				
			when decPtrfsm => --snizeni pointeru
				decreasePtr <= '1';
				increasePc <= '1';
				nstate <= fetchfsm;
			
			when incPtrfsm => -- zvyseni pointeru
				increasePtr <= '1';
				increasePc <= '1';
				nstate <= fetchfsm;
			
			when decValfsm => -- snizeni hodnoty
				DATA_EN <= '1';
				DATA_RDWR <= '0';
				increasePc <= '1';
				nstate <= decVal2fsm;
				
			when decVal2fsm =>
				DATA_EN <= '1';
				nstate <= decVal3fsm;
				
			when decVal3fsm =>
				DATA_EN <= '1';
				selector <= "01";
				DATA_RDWR <= '1';
				nstate <= fetchfsm;
			
			when incValfsm => -- zvyseni hodnoty
				DATA_EN <= '1';
				DATA_RDWR <= '0';
				increasePc <= '1';
				nstate <= incVal2fsm;
				
			when incVal2fsm =>
				DATA_EN <= '1';
				nstate <= incVal3fsm;
			
			when incVal3fsm =>
				selector <= "10";
				DATA_EN <= '1';
				DATA_RDWR <= '1';
				nstate <= fetchfsm;
				
			when writefsm => -- vypis hodnoty
			DATA_RDWR <= '0';
			DATA_EN <= '1'; -- povoleni cteni
			nstate <= write2fsm;
			
			when write2fsm =>
				if(OUT_BUSY = '0') then
					DATA_RDWR <= '0';
					OUT_DATA <= DATA_RDATA; -- zapis
					increasePc <= '1';
					OUT_WE <= '1';
					nstate <= fetchfsm;
				else
					nstate <= writefsm;
				end if;
				
			when readfsm =>  -- nacteni hodnoty
				IN_REQ <= '1';
				if(IN_VLD = '0') then --overeni validnosti dat
					nstate <= readfsm;
				else
					nstate <= read2fsm;
				end if;
			
			when read2fsm =>
				DATA_EN <= '1';
				DATA_RDWR <= '1';
				selector <= "00"; -- nacitani
				increasePc <= '1'; 
				nstate <= fetchfsm;
			
			when endProgfsm =>
				nstate <= endProgfsm; -- return
			
			when UNKNWN =>
				increasePc <= '1'; --neznamy prikaz
				nstate <= fetchfsm;
			
			-- WHILE BEGIN -- -----------------------------------------------------
			when whileSfsm =>
				increasePc <= '1';
				DATA_EN <= '1';
				DATA_RDWR <= '0';
				nstate <= whileS2fsm;
			
			when whileS2fsm =>
			if(rdatatmp = '0') then
				setcnt <= '1';
				nstate <= whileS3fsm;
			else
				nstate <= fetchfsm;
			end if;
				
			when whileS3fsm =>
				if(cntdatatmp = '1') then
					CODE_EN <= '1';
					nstate <= whileS4fsm;
				else
					nstate <= fetchfsm;
				end if;
			
			when whileS4fsm =>
				if(CODE_DATA = X"5B") then
					increaseCnt <= '1';
				elsif (CODE_DATA = X"5D") then
					decreaseCnt <= '1';
				end if;
				nstate <= whileS5fsm;
			
			when whileS5fsm =>
				increasePc <= '1';
				nstate <= whileS3fsm;
				
			 --- while end ---
			when whileEfsm =>
				DATA_EN <= '1';
				DATA_RDWR <= '0';
				nstate <= whileE2fsm;
			
			when whileE2fsm =>
				if(rdatatmp = '0') then
					nstate <= fetchfsm;
					increasePc <= '1';
				else
					setcnt <= '1';
					nstate <= whileE3fsm;
					decreasePc <= '1';
				end if;
			
			when whileE3fsm =>
				if(cntdatatmp = '1') then
					CODE_EN <= '1';
					nstate <= whileE4fsm;
				else
					nstate <= fetchfsm;
				end if;
				
			when whileE4fsm =>
				if(CODE_DATA = X"5D") then
					increaseCnt <= '1';
				elsif (CODE_DATA = X"5B") then
					decreaseCnt <= '1';
				end if;
				nstate <= whileE5fsm;
				
			when whileE5fsm =>
				if(cntdatatmp = '0') then
					increasePc <= '1';
				else
					decreasePc <= '1';
				end if;
				nstate <= whileE3fsm;
			
			-- break start
			when breakfsm =>
				setcnt <= '1';
				increasePc <= '1';
				nstate <= break2fsm;
			
			when break2fsm =>
				if (cntdatatmp = '1') then
					CODE_EN <= '1';
					nstate <= break3fsm;
				else
					nstate <= fetchfsm;
				end if;
			
			when break3fsm =>
				if(CODE_DATA = X"5B") then
					increaseCnt <= '1';
				elsif (CODE_DATA = X"5D") then
					decreaseCnt <= '1';
				end if;
				nstate <= break4fsm;
				
			when break4fsm =>
				increasePc <= '1';
				nstate <= break2fsm;
			
			when others => NULL;
		end case;
	end process;				
end behavioral;
 
