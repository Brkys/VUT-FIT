import MathLibary

def standardDeviation(values):
    '''
    Funkcia vypočíta výberovú smerodatnú odchylku pre zadané čísla
    :param values: zoznam čísiel
    '''
    N = len(values)
    priemer = 0
    
    for x in values: #vypocet priemeru vsetkych cisiel
        priemer = MathLibary.add(priemer, x)
        
    priemer = MathLibary.div(priemer, N)
    suma = 0
    
    for x in values: #prenasobenie druhou mocninou
        suma = MathLibary.add(suma, MathLibary.pow(x, 2))

    suma = MathLibary.sub(suma, MathLibary.mul(N, MathLibary.pow(priemer, 2)))
    suma = MathLibary.sqrt(MathLibary.div(suma, MathLibary.sub(N, 1)), 2) #ziskanie vyslednej odmocniny
    print(suma)

values = [int(x) for x in input().split()] #Ziskanie hodnot zo vstupu   
standardDeviation(values) #Vypocet odchylky a jej vypis