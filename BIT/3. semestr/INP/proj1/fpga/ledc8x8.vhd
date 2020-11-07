--INP Projekt 1--------------------
--Autor: Zdenek Brhel
--Mail: xbrhel04@stud.fit.vutbr.cz

library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.std_logic_arith.all;
use IEEE.std_logic_unsigned.all;

entity ledc8x8 is
port ( -- tvorba rozhrani obvodu
        ROW     : out std_logic_vector (0 to 7);
        LED     : out std_logic_vector (0 to 7);
        RESET   : in std_logic; 
        SMCLK   : in std_logic
);
end ledc8x8;

architecture main of ledc8x8 is

    -- definice_vect vnitrnich signalu  
    signal ce_vect: std_logic;
    signal r_pos_vec: std_logic_vector(7 downto 0);
	signal l_vect: std_logic_vector(7 downto 0);
	signal cntr: std_logic_vector(7 downto 0);
    
begin

    -- Sem doplnte popis obvodu. Doporuce_vectni: pouzivejte zakladni obvodove prvky
    -- (multiplexory, registry, dekodery,...), jejich funkce_vect popisujte pomoci
    -- proce_vectsu VHDL a propojeni techto prvku, tj. komunikaci mezi proce_vectsy,
    -- realizujte pomoci vnitrnich signalu deklarovanych vyse.

    -- DODRZUJTE ZASADY PSANI SYNTETIZOVATELNEHO VHDL KODU OBVODOVYCH PRVKU,
    -- JEZ JSOU PROBIRANY ZEJMENA NA UVODNICH CVIce_vectNI INP A SHRNUTY NA WEBU:
    -- http://merlin.fit.vutbr.cz/FITkit/docs/navody/synth_templates.html.

    -- Nezapomente take doplnit mapovani signalu rozhrani na piny FPGA
    -- v souboru ledc8x8.ucf.

    
    gener_ce_vect: process(SMCLK, RESET)
    begin
        if RESET = '1' then 
            cntr <= "00000000"; -- nulovani vektoru
        elsif SMCLK'event and SMCLK = '1' then --pokud je SMCLK aktivní
            cntr <= cntr + 1; -- pricteni 1 do vektoru
        end if;
    end process gener_ce_vect;
    ce_vect <= '0' when cntr /= X"FF" else '1';

    rotace: process(RESET, ce_vect, SMCLK)
    begin   
        if RESET = '1' then 
            r_pos_vec <= "10000000"; 
        elsif (SMCLK = '1' and SMCLK'event and ce_vect = '1') then
            r_pos_vec <= r_pos_vec(0) & r_pos_vec(7 downto 1);
        end if;
    end process rotace;

    dekoder: process(r_pos_vec)
    begin
        case r_pos_vec is
            when "10000000" => l_vect <= "00000111"; -- vypsání na LCD display (0 značí rozsvíce_vectnou led)
            when "01000000" => l_vect <= "11101111"; -- pořád dokola se překresluje, posunuje se 1 v prvním vektoru
            when "00100000" => l_vect <= "11011111"; 
            when "00010000" => l_vect <= "10110001";
            when "00001000" => l_vect <= "00000110";
            when "00000100" => l_vect <= "11110001";
            when "00000010" => l_vect <= "11110110";
            when "00000001" => l_vect <= "11110001";
            when others =>     l_vect <= "11111111";
        end case;
    end process dekoder;

    ROW <= r_pos_vec; -- "číslo řádku"
    LED <= l_vect; -- ledky pro rozsvícení

end main;
