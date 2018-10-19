# -*- coding: utf-8 -*-
######################################################
#           File containing PyUnit tests             #
#              for mathematical libary               #
#                                                    #
#                Author: Zdeněk Brhel                #
#                  Login: xbrhel04                   #
######################################################
from typing import Union
import math

e = 2.71828182845904523536028747135266249775724709369995
pi = 3.14159265358979323846264338327950288419716939937510

def add(number1: Union[int, float], number2: Union[int, float]):
    """
    Funkce vrati soucet 2 cisel
    :param number1: Cislo 1
    :param number2: Cislo 2
    :return: Soucet
    """
    return number1 + number2

def sub(number1: Union[int, float], number2: Union[int, float]):
    """
    Funkce vrati rozdil 2 cisel (odecte druhy parametr od prvniho)
    :param number1: Cislo 1
    :param number2: Cislo 2
    :return: Rozdil
    """
    return number1 - number2

def mul(number1: Union[int, float], number2: Union[int, float]):
    """
    Funkce vrati soucin 2 cisel
    :param number1: Cislo 1
    :param number2: Cislo 2
    :return: Soucin
    """
    return number1 * number2

def div(number1: Union[int, float], number2: Union[int, float]):
    """
    Funkce vrati podil 2 cisel
    :param number1: Delenec
    :param number2: Delitel
    :return: Podil
    """
    return number1 / number2

def fact(number1: int):
    """
    Funkce vrati faktorial cisla
    :param number1: Prirozene cislo
    :return: Faktorial
    """
    if number1 < 0:
        raise Exception('Parametrem musi byt prirozene cislo, nebo 0!')
    elif number1 == 0:
        return 1
    else:
        return number1 * (fact(number1-1))


def pow(number1: Union[int, float], number2: int):
    """
    Funkce vrati N2-tou mocninu cisla N
    :param number1: Cislo
    :param number2: Mocnina
    :return: Mocnina cisla
    """
    return number1**number2

def sqrt(number1: Union[int, float], number2: Union[int, float]):
    """
    Funkce vrati N2-tou odmocninu cisla N
    :param number1: Cislo
    :param number2: Odmocnina
    :return: Odmocnina cisla
    """
    if number2 == 0:
        raise Exception('Druhym parametrem nemuze byt 0!')
    return number1**(1/number2)

def log(number1: Union[int, float], number2: Union[int, float]):
    """
    Vrati logaritmus cisla N o zakladu N2
    :param number1: Cislo
    :param number2: Zaklad
    :return: Logaritmus
    """
    if number1 == number2:
        return 1
    return math.log(number1, number2)
