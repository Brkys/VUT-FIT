#include <stdio.h>
#include <stdlib.h>
#include <math.h>

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

double mypow(double x, double y) //Poèítání mocniny (podlem mì)
{
    double zaklad = x;
    double vysledek = x;
    for(int i = 1; i < y; i++)
    {
        vysledek *= zaklad;
    }
    return vysledek;
}

double cfrac_log(double x, unsigned int n)
{
    if(x > 0)
    {
        double cislo = n; //aktualní èíslo ve zlomku
        double cislopredzlomkem = ((n*2)+1) - 2; //èíslo pøed zlomkem
        double vysledek = (n*2)+1;
        x = ((x-1)/(x+1));
        double power = 1;
        double predchozi = 0.0;
        for(unsigned int i = 1; i <= n; i++)
        {
            power = power * (cislo*x);// výpoèet mocniny
            vysledek = cislopredzlomkem - ((mypow(cislo * x,2) / vysledek)); //vzorec
            cislo --;
            cislopredzlomkem -= 2;
            if(!isinf(vysledek))
            {
                predchozi = vysledek;
            }
        }
        vysledek = (2*x)/vysledek; //Poslední øádek zlomku
        if(!isinf(vysledek))
        {
            predchozi = vysledek;
        }
        return predchozi;
    }
    else if(isnan(x))
    {
        return NAN;
    }
    else if(isinf(x))
    {
        return -INFINITY;
    }
    else
    {
        return NAN;
    }
}

double taylor_log(double x, unsigned int n)
{
    double vysledek = 0.0;
    double power = 1; //výpoèet mocniny
    double predchozi = 0.0;
    if(x == 0)
    {
        return -INFINITY;
    }
    if(x > 0 && x < 1)
    {
        x = 1 - x; //Základní vzorec
        for(unsigned int i = 1; i < n; i++)
        {
            power = power * x;
            vysledek -=  (power / i); //vzorec
            if(!isinf(vysledek)) //kontrola pro nekoneènost
            {
                predchozi = vysledek;
            }
        }
    }
    else if(x >= 1)
    {
        for(unsigned int i = 1; i <= n; i++)
        {
            power = power * ((x-1)/x); //mocnina
            vysledek += (power/i); //vzorec
            if(!isinf(vysledek))
            {
                predchozi = vysledek;
            }
        }
    }
    else if(isnan(x))
    {
        return NAN;
    }
    else if(isinf(x))
    {
        return -INFINITY;
    }
    else
    {
        return NAN;
    }
    return predchozi;
}

double taylor_pow(double x, double y, unsigned int n) //Funkce je okomentována v následující funkci, funguje stejnì, akorát používá jiný algoritmus pro log
{
    if(x > 0)
    {
        double vysledek = 1.0;
        double predchozi = 1.0;
        long faktorial = 1;
        double power = 1;
        double power2 = 1;
        for(unsigned int i = 1; i <= n; i++)
        {
            power = power * y;
            power2 = power2 * taylor_log(x,n);
            faktorial = faktorial * i;
            vysledek += (power*power2)/faktorial;
            if(!isinf(vysledek))
            {
                predchozi = vysledek;
            }
        }
        return predchozi;
    }
    else if(isnan(x))
    {
        return NAN;
    }
    else if(isinf(x))
    {
        return -INFINITY;
    }
    else
    {
        return NAN;
    }
}

double taylorcf_pow(double x, double y, unsigned int n)
{
    if(x > 0)
    {
        double vysledek = 1.0;//výsledek pro jednu iteraci
        double predchozi = 1.0;//výsledek pro pøedchozí iteraci
        long faktorial = 1;//obsahuje èislo pod zlomkem
        double power = 1;//první mocnina
        double power2 = 1;//druhá mocnina
        for(unsigned int i = 1; i <= n; i++)
        {
            power = power * y; //výpoèet první mocniny
            power2 = power2 * cfrac_log(x,n); //výpoèet druhé mocniny
            faktorial = faktorial * i; //zvìtšování faktoriálu
            vysledek += (power*power2)/faktorial;
            if(!isinf(vysledek)) //Kontrola, jestli je výsledek infinity, pokud je, vrátí pøedchozí výsledek v returnu
            {
                predchozi = vysledek;
            }
        }
        return predchozi;
    }
    else if(isnan(x)) //kontrola jestli je vstup NAN
    {
        return NAN;
    }
    else if(isinf(x)) //kontrola, jestli je vstup infinity
    {
        return -INFINITY;
    }
    else //pro jiný vstup
    {
        return NAN;
    }
}

void logaritmy(double x, double n) //Funkce vypíše všechny logaritmy
{
    if(x < 0)
    {
        printf("X je mensi nez 0\n");
    }
    double vysledek = log(x);
    printf("%-7slog(%g) = %.12g\n","", x, vysledek);
    vysledek = cfrac_log(x, n);
    printf("%-1scfrac_log(%g) = %.12g\n","", x, vysledek);
    vysledek = taylor_log(x, n);
    printf("taylor_log(%g) = %.12g\n", x, vysledek);
}

void mocniny(double x, double y, double n) //Funkce vypíše všechny powy
{
    if(x < 0)
    {
        printf("X je mensi nez 0\n");
    }
    double vysledek = pow(x, y);
    printf("pow(%g,%g) = %0.12g\n", x, y, vysledek);
    vysledek = taylor_pow(x, y, n);
    printf("taylor_pow(%g,%g) = %0.12g\n", x, y, vysledek);
    vysledek = taylorcf_pow(x, y, n);
    printf("taylorcf_pow(%g,%g) = %0.12g\n", x, y, vysledek);
   // vysledek = mypow(x, y);
    //printf("mypow(%g,%g) = %0.12g\n", x, y, vysledek);
}

int main(int argc, char *argv[])
{
    if(argc > 1)
    {
        if(argc > 2)
        {
            if(porovnej(argv[1], "--log") == 0)
            {
                if(argc == 4)
                {
                    double x = strtod(argv[2], NULL);
                    double n = strtod(argv[3], NULL);
                    if(n <= 0)
                    {
                        printf("Pocet iteraci nesmi byt 0, mensi, nekonecno, nebo NAN");
                        return -1;
                    }
                    logaritmy(x, n);
                }
                else
                {
                    printf("Spatny pocet parametru");
                }
            }
            else if (porovnej(argv[1], "--pow") == 0)
            {
                if(argc == 5)
                {
                    double x = strtod(argv[2], NULL);
                    double y = strtod(argv[3], NULL);
                    double n = strtod(argv[4], NULL);
                    if(n <= 0)
                    {
                        printf("Pocet iteraci nesmi byt 0, mensi, nekonecno, nebo NAN");
                        return -1;
                    }
                    mocniny(x, y, n);
                }
                else
                {
                    fprintf(stderr,"Spatny pocet parametru");
                    return -1;
                }
            }
            else
            {
                fprintf(stderr,"Spatny parametr\n--log X N\n--pow X Y N");
                return -1;
            }
        }
        else
        {
            fprintf(stderr,"Nebylo zadano cislo");
            return -1;
        }
    }
    else
    {
        fprintf(stderr,"Nebyl zadan parametr");
        return -1;
    }
}
