<?php
if($argc > 1) //zpracování parametrů
{
	$params = getParams($argc, $argv);
	if($params != 0)
		exit($params);
}
else
{
	$instructionnumber = 1;
    global $args;
	for($linenumber = 0; $file = fgets(STDIN); ++$linenumber) //načítá soubor po řádku
	{
		$args = [];
		//mazání komentářů a prázdných řádků
		$file = removeComments($file);
        if(ctype_space($file) || $file == "") continue;

		//kontrola správnosti kódu
		$parts = preg_split("/[ \t]+/", $file);
		if(strtoupper($parts[0]) != ".IPPCODE18" && $linenumber == 0)
			exit (21);
		if(lexical($parts) != 0) exit(21);

		//var_dump($args);
		//generování XML
		//generovani hlavicky
		if($linenumber == 0)
		{
			if(($output = xmlwriter_open_memory()) == false) exit(12);
			xmlwriter_set_indent($output, 1);
			xmlwriter_start_document($output, '1.0', 'UTF-8');
			xmlwriter_start_element($output, 'program');
            xmlwriter_start_attribute($output, 'language');
            xmlwriter_text($output, 'IPPcode18');
            xmlwriter_end_attribute($output);
		}
		else
		{
			//Zapisuji instrukci
			xmlwriter_start_element($output, 'instruction');

			//telo instrukce
			xmlwriter_start_attribute($output, 'order'); //poradi prikazu
			xmlwriter_text($output, $instructionnumber++); //atribut k orderu
			xmlwriter_end_attribute($output);
			xmlwriter_start_attribute($output, 'opcode'); //nazev instrukce
			xmlwriter_text($output, strtoupper($parts[0]));
			xmlwriter_end_attribute($output);

			//argumenty
			for($i = 1; $i < count($parts); $i++)
			{
				xmlwriter_start_element($output, "arg".$i);
				
				//atributy argumentů
				xmlwriter_start_attribute($output, "type");
				//atribut typ
				xmlwriter_text($output, $args[$i-1]);
				xmlwriter_end_attribute($output);
				if($args[$i-1] != "var" && $args[$i-1] != "label" && $args[$i-1] != "type") //pokud jde o literal, nevypisovat typ
					xmlwriter_text($output, trimliteral($parts[$i]));
				else //pokud o label, promennou, nebo typ, vypsat vse
					xmlwriter_text($output, $parts[$i]);


				xmlwriter_end_element($output);
			}

			//ukonceni instrukce
			xmlwriter_end_element($output);
		}
		
	}
    if($linenumber == 0 || $output == NULL)
        exit(21);
	xmlwriter_end_element($output);
	echo xmlwriter_output_memory($output);
}
/**
 * Funkce ořezává literál o jeho předponu
 * @param $string Vstupní string
 * @return Ořezaný literál
 */
function trimliteral($string)
{
	$result = expectSymb($string);
	if($result == "string")
		return substr($string, 7);
	elseif($result == "int")
		return substr($string, 4);
	elseif($result == "bool")
		return substr($string, 5);
}

/**
 * Funkce zajišťuje správnost názvu příkazů, funkcí a jejich pořadí v kódu
 * @param $file Vstupní string 
 * @return 21 při chybě, jinak 0
 **/
function lexical($parts)
{
	global $args;
	static $line = 0;
	$line++;
	$result = 0;
    switch (strtoupper($parts[0]))  // switch na kontrolu příkazů a parametrů
    {    	
        case ".IPPCODE18":          
        case "RETURN": 
        case "BREAK":
    	case "CREATEFRAME":
    	case "PUSHFRAME":
    	case "POPFRAME":
            if(count($parts) == 1)
                return 0;
            fprintf(STDERR, "\nNeznamy prikaz %s na radce %d\n", $parts[0], $line);
            return 21;
        
        case "POPS":
    	case "DEFVAR":
    		if(count($parts) == 2 && expectVar($parts[1]))
    		{
    			$args[0] = "var";
    			return 0;
    		}
    		fprintf(STDERR, "\nNeznamy prikaz %s na radce %d\n", $parts[0], $line);
    		return 21;

        case "AND":
        case "OR":
        case "LT":
        case "GT":      
        case "CONCAT":
        case "GETCHAR":
        case "SETCHAR":
        case "EQ":
    	case "ADD":
    	case "SUB":
        case "STRI2INT":
    	case "MUL":
    	case "IDIV":
    		if(count($parts) == 4 && expectVar($parts[1]) && ($args[1] = expectSymb($parts[2])) && ($args[2] = expectSymb($parts[3])))
    		{
    			$args[0] = "var";
    			return 0;
    		}
    		fprintf(STDERR, "\nNeznamy prikaz %s na radce %d\n", $parts[0], $line);
    		return 21;


        case "MOVE":
        case "STRLEN":
        case "TYPE":
    	case "NOT":
    	case "INT2CHAR":
    		if(count($parts) == 3 && expectVar($parts[1]) && ($args[1] = expectSymb($parts[2])))
    		{
    			$args[0] = "var";
    			return 0;
    		}
    		fprintf(STDERR, "\nNeznamy prikaz %s na radce %d\n", $parts[0], $line);
    		return 21;

    	case "READ":
    		if(count($parts) == 3 && expectVar($parts[1]) && expectType($parts[2]))
    		{
    			$args[0] = "var";
    			$args[1] = "type";
    			return 0;
    		}
    		fprintf(STDERR, "\nNeznamy prikaz %s na radce %d\n", $parts[0], $line);
    		return 21;
    
        case "PUSHS":
        case "DPRINT":
    	case "WRITE":
    		if(count($parts) == 2 && ($args[0] = expectSymb($parts[1])))
    			return 0;
    		fprintf(STDERR, "\nNeznamy prikaz %s na radce %d\n", $parts[0], $line);
    		return 21;

        case "CALL":
    	case "LABEL":
    	case "JUMP":
    		if(count($parts) == 2 && expectLabel($parts[1]))
    		{
    			$args[0] = "label";
    			return 0;
    		}
    		fprintf(STDERR, "\nNeznamy prikaz %s na radce %d\n", $parts[0], $line);
    		return 21;

    	case "JUMPIFEQ":
    	case "JUMPIFNEQ":
    		if(count($parts) == 4 && expectLabel($parts[1]) && ($args[1] = expectSymb($parts[2])) && ($args[2] = expectSymb($parts[3])))
    		{
    			$args[0] = "label";
    			return 0;
    		}
    		fprintf(STDERR, "\nNeznamy prikaz %s na radce %d\n", $parts[0], $line);
    		return 21;

    	default:
    		fprintf(STDERR, "\nNeznamy prikaz %s na radce %d\n", $parts[0], $line);
    		return 21;
    }
}

/**
 * Funkce kontroluje, zda li je string kompatibilní pro název proměnné, nebo konstanty
 * @param $file Vstupní string 
 * @return false při chybě, jinak true
 **/
function expectSymb($string)
{
	if(expectVar($string) || preg_match("/string@[^# ]*$/", $string) || preg_match("/^int@[-+]?[0-9]+$/", $string) ||
	   strtolower($string) == "bool@true" || strtolower($string) == "bool@false")
	{
		if(preg_match("/^string@.*/", strtolower($string)))
			return "string";
		else if(preg_match("/^bool@.*/", strtolower($string)))
    		return "bool";
		else if(preg_match("/^int@.*/", strtolower($string)))
    		return "int";
		else
    		return "var";
	}
	return false;
}

/**
 * Funkce kontroluje, zda li je string kompatibilní pro název proměnné
 * @param $file Vstupní string 
 * @return false při chybě, jinak true
 **/
function expectVar($string)
{
	if(expectFrame($string))
		if(preg_match("/[a-zA-Z_\$&%\*-]/", substr($string, 3, 1)))
			if(preg_match("/^[a-zA-Z_\$&%\*0-9-]*$/", substr($string, 4)))
				return true;
	return false;
}

/**
 * Funkce kontroluje, zda li je string kompatibilní pro jméno návěští
 * @param $file Vstupní string 
 * @return false při chybě, jinak true
 **/
function expectLabel($string)
{
	if(preg_match("/[a-zA-Z_\$&%\*-]/", substr($string, 0, 1)))
		if(preg_match("/^[a-zA-Z_\$&%\*0-9-]*$/", substr($string, 1)))
			return true;
	return false;
}

/**
 * Funkce kontroluje, zda li je string jméno datového typu
 * @param $file Vstupní string 
 * @return false při chybě, jinak true
 **/
function expectType($string)
{
	$string = strtolower($string);
	return ($string == "int" || $string == "string" || $string == "bool");
}

/**
 * Funkce kontroluje TF@|GF@|LF@ na začátku vstupního stringu
 * @param $file Vstupní string 
 * @return false při chybě, jinak true
**/
function expectFrame($string)
{
    if(substr($string, 0, 3) == "TF@" || substr($string, 0, 3) == "LF@" || substr($string, 0, 3) == "GF@")
        return true;
    return false;
}

/**
 * Funkce odstraňuje komentáře a následně prázdné mezery
 * @param $file Vstupní string 
 * @return String bez komentářů a prázdných znaků
**/
function removeComments($file)
{	
	return trim(preg_replace("/#.*/", "", $file));
}

/**
 * Funkce zpracuje parametry
 * @param $argc Počet parametrů
 * @param $argv Parametry
 * @return 10 v případě chyby, jinak 0
**/
function getParams($argc, $argv)
{
    if($argc == 1)
        return 0;
    else if($argc == 2 && $argv[1] == "--help")
    {
        echo "Napoveda:\n";
        echo "Skript na vstup ocekava zdrojovy soubor v jazyce .IPPcode18.\n";
        echo "Jeho vystupem je XML format na stdout.\n";
        echo "V pripade chyby v kodu vraci kod 21, v pripade chyby parametru 10.\n";
        return 0;
    }
    echo "Neplatne argumenty. Zkuste --help\n";
    return 10;
}
