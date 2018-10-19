#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

/** Funkce help-
  * promenna chyba obsahuje chybovou hlasku, ktera se vypise na prvni radek,
  * pote se vypise samotna napoveda ke kazdemu argumentu*/
void help(char chyba[]) //Hotovo
{
    fprintf(stdout, "Chyba: %s\n", chyba);
    printf("--------------------------------\n");
    printf("Zadejte parametry k programu\n");
    printf("-x (Prepise text na hexadecimalni hodnotu)\n");
    printf("<bez parametru> (Vypise pozici prvniho vypisovaneho bajtu, nasledne vypise hexadecimalni\n");
    printf("-s N -n M ");
    printf("hodnotu oddelenou mezerami a pote vypise skutecny text\n");
    printf("-S <N> (Vypise pouze slova (1 na radek), ktere jsou vetsi nez <N>\n");
    printf("-r (Prepise vstup (zapsany v hexadecimalni podobe) na text\n");
}

/** Funkce porovnej
  * char c[] nacte hodnotu argumentu, ktery byl vlozen
  * char v[] nacte hodnotu argumentu, se kterym se porovnava
  * int porovnavac prvni kontroluje, zda li ani jedna promenna
  * neni prazdna, pokud je, vraci 1.
  * pak kontroluje, jestli jsou jednotlive hodnoty rozdilne a nasledne se inkrementuje*/
int porovnej(char c[], char v[])
{
    int porovnavac = 0;
    while(c[porovnavac] != '\0')
    {
        if (c[porovnavac] == '\0')
        {
            return 1;
        }

        if (v[porovnavac] == '\0')
        {
             return 1;
        }
        if(c[porovnavac] < v[porovnavac])
        {
            return 1;
        }
        else if (c[porovnavac] > v[porovnavac])
        {
            return 1;
        }
        porovnavac++;
    }
    return 0;
}


/** Funkce texttohex
  * char temp nacte prave jedno vstupni pismeno a prepise
  * jeho hodnotu na hexadecimalni, vypisuje po parech*/
void texttohex()
{
    char temp;
    while((temp = getchar()) != EOF)
    {
        printf("%02x", temp);
    }
}

/** Funkce hextotext
  * temp - vstupní hodnota
  * i - pomocný integer pro počítání hodnoty (0, nebo 1)
  * hex - string obsahující hexadecimální hodnotu*/
void hextotext(char temp, int i, char hex[3])
{
    if(i % 2 == 0)
    {
        hex[0] = temp;
    }
    if(i % 2 == 1)
    {
        hex[1] = temp;
        hex[2] = '\0';
        printf("%c", (int)strtol(hex, NULL, 16));
    }
}


/** Funkce numtobin
  * int cislo nacte cislo a prevede ho na binarni hodnotu
  * (cislo % 2) + 10 * numtobin(cislo / 2) - vzorec pro prevod z decimalni na binarni*/
/*int numtobin(int cislo) //Hotovo
{
    int pomocna;
    if(cislo == 1)
    {
        return 1;
    }
    if(cislo == 0)
    {
        return 0;
    }
    pomocna = (cislo % 2) + 10 * numtobin(cislo / 2);
    return pomocna;
}*/

/** Funkce cisti
  * char pole[] načte pole a následně ho vyčistí*/
void cisti(char pole[])
{
    int len = 0;
    for(int i = 0; pole[i] != '\0'; i++)
    {
        len++;
    }
	for(int i = 0; i < len;i++)
	{
		pole[i]=0;
	}
}

/** Funkce noargument
  * Vypíše 8 hexadecimálních znaků aktuální pozice
  * Vypíše 16 znaků převedených do hexadecimální podoby (po dvou)
  * Vypíše skutečný string v textové podobě*/
void noargument()
{
    int pozice = 0;
    char temp;
    char word[16]; //Maximálně 16 znaků na řádek
    while((temp = getchar()) != EOF)
    {
        if(pozice % 16 == 0 && pozice != 0)
        {
            printf("|");
            for(int i = 0; i < 16; i++)
            {
                if(!isprint(word[i]))
                {
                    if(word[i] == 0)
                    {
                        printf(" ");
                    }
                    else
                    {
                        printf(".");
                    }
                }
                else
                {
                    printf("%c", word[i]);
                }
            }
            printf("|");
        }
        if(pozice % 16 == 0)
        {
            printf("\n%08x ", pozice);
            cisti(word);
        }
        word[pozice % 16] = temp;
        if(pozice % 8 == 0)
        {
            printf(" ");
        }
        printf("%02x ", temp);
        pozice++;
    }
    if(pozice % 16 != 0)
    {
        if(pozice % 16 <= 8)
        {
            printf(" ");
        }
        for(int i = 0; i < ((16-(pozice % 16))*3); i++) //Pokud není znak, doplní 3 znaky (mezery)
        {
            printf(" ");
        }
        printf("|");
        for(int i = 0; i < 16; i++)
        {
            if(!isprint(word[i]))
            {
                if(word[i] == 0)
                {
                    printf(" ");
                }
                else
                {
                    printf(".");
                }
            }
            else
            {
                printf("%c", word[i]);
            }
        }
        printf("|");
    }
}


/** Funkce arg_sn
  * dělá to samé, co funkce morethann
  * přidává do funkce proměnnou pro -s(kip) a -n(umber)*/
int arg_sn(int m, int n)
{
    char temp;
    int pozice = 0;
    int adresa = 0; //Proměnná, od které zapisuje pozice, od které se začíná vypisovat
    char word[16];
    while(adresa < m)
    {
        if((temp=getchar())==EOF)
            return 0;
        adresa++;
    }


    while((temp = getchar()) != EOF)
    {
        if(pozice % 16 == 0 && pozice != 0)
        {
            printf("|");
            for(int i = 0; i < 16; i++)
            {
                if(!isprint(word[i]))
                {
                    if(word[i] == 0)
                    {
                        printf(" ");
                    }
                    else
                    {
                        printf(".");
                    }
                }
                else
                {
                    printf("%c", word[i]);
                }
            }
            printf("|");
        }
        if(pozice % 16 == 0)
        {
            printf("\n%08x ", adresa);
            cisti(word);
        }
        word[pozice % 16] = temp;
        if(pozice % 8 == 0)
        {
            printf(" ");
        }
        printf("%02x ", temp);

        pozice++;
        adresa++;
        if(pozice >= n && n != 0)
        {
            break;
        }
    }
    if(pozice % 16 != 0)
    {
        if(pozice % 16 <= 8)
        {
            printf(" ");
        }
        for(int i = 0; i < ((16-(pozice % 16))*3); i++)
        {
            printf(" ");
        }
        printf("|");
        for(int i = 0; i < 16; i++)
        {
            if(!isprint(word[i]))
            {
                if(word[i] == 0)
                {
                    printf(" ");
                }
                else
                {
                    printf(".");
                }
            }
            else
            {
                printf("%c", word[i]);
            }
        }
        printf("|");
    }
    return 0;
}

/** Funkce morethann
  * int arg - hodnota N
  * vypíše slova se stejně a více znaky, než je N*/
int morethann(int arg)
{
    char vstup;
    while(1)
    {
        for(int i = 0; i < arg; i++) //Vypíše
        {
            vstup = getchar();
            if(vstup == EOF) //pokud je vstup prázdný, vrátí 0
            {
                return 0;
            }
            if(vstup != '\n' && vstup != '\0') //Pokud není vstup konec řádku nebo konec řetězce, vloží vstup
            {
                putchar(vstup);
            }
            else
            {
                for(int j = 0; j < i; j++) //Pokud je počet vepsaných znaků menší, než N, vymaže znaky
                {
                    putchar('\b');
                }
                break;
            }
        }
        if(vstup == '\n' || vstup == '\0') // Pokud je vstup znak nového řádku, nebo konec řetězce, ignoruje vstup a pokračuje
        {
            continue;
        }
        while((vstup = getchar()) != EOF && vstup != '\n' && vstup != '\0') //Vkládá znaky
        {
            putchar(vstup);
        }
        printf("\n"); //Vypisuje nový řádek
        if(vstup == EOF)
        {
            return 0;
        }
    }
}

/** Funkce ishex
  * int temp - načítá hodnotu ze vstupu
  * Kontroluje, zda je hodnota hexadecimální*/
int ishex(int temp)
{
    if(isdigit(temp) || (temp >= 'A' && temp <= 'F') || (temp >= 'a' && temp <= 'f'))
    {
        return 1;
    }
    return 0;
}

int main(int argc, char *argv[])
{
    int pos = 0;
    char hex[3];
    char *error;
    char vstup;
    //while((vstup = getchar()) != EOF) (Používal jsem tenhle while... pak se mi nechtělo zarovnávat)
    //{
        if(argc > 1)
        {
            if(porovnej(argv[1], "-x") == 0) //Pokud je argument -x
            {
                texttohex();
            }
            else if(porovnej(argv[1], "-r") == 0) //Pokud je argument -r
            {
                while((vstup = getchar()) != EOF)
                {
                    if(ishex(vstup) == 1)
                    {
                        hextotext(vstup, pos, hex);
                        pos++;
                        if(vstup == ' ')
                        {
                            pos = 0;
                        }
                    }
                    else if(vstup == ' ' || vstup == '\n' || isblank(vstup))
                    {
                        continue;
                    }
                    else
                    {
                        error = "Nebyla zadana hexadecimalni hodnota";
                        help(error);
                    }
                }
            }
            else if(porovnej(argv[1], "-S") == 0) //Pokud je argument -S
            {
                if(argc > 2)
                {
                    if(isdigit(*argv[2]))
                    {
                        if((int)strtol(argv[2], NULL, 10) > 0 && (int)strtol(argv[2], NULL, 10) < 200)
                        {
                            morethann((int)strtol(argv[2], NULL, 10));
                        }
                        else
                        {
                            error = "N musi byt od 1 do 199";
                            help(error);
                            return 0;
                        }
                    }
                    else
                    {
                        error = "Hodnota N neni cislo";
                        help(error);
                        return 0;
                    }
                }
                else
                {
                    error = "Nebyla zadana druha hodnota";
                    help(error);
                    return 0;
                }
            }
            else if(porovnej(argv[1], "-s") == 0 || porovnej(argv[1], "-n") == 0) //Pokud je argument -s nebo -n
            {
                if(argc < 6)
                {
                    if(argc == 3)
                    {
                        if(porovnej(argv[1], "-s") == 0)
                        {
                            arg_sn((int)strtol(argv[2], NULL, 10),0);
                        }
                        if(porovnej(argv[1], "-n") == 0)
                        {
                            arg_sn(0,(int)strtol(argv[2], NULL, 10));
                        }
                    }
                    if(argc == 5)
                    {
                        arg_sn((int)strtol(argv[2], NULL, 10), (int)strtol(argv[4], NULL, 10));
                    }
                }
                else
                {
                    error = "Nebyla zadana ciselna hodnota";
                    help(error);
                    return 0;
                }
            }
            else //pokud je neplatný argument
            {
                error = "Neplatny argument";
                help(error);
                return 0;
            }
        }
        else
        {
            noargument(); //Pokud není argument
        }
    //}
}
