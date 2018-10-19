# -*- coding: utf-8 -*-
######################################################
#           File containing PyUnit tests             #
#              for mathematical libary               #
#                                                    #
#                                                    #
# @author Zdeněk Brhel                               #
# @file tests.py                                     #
# Login: xbrhel04                                    #
######################################################

import math
import unittest
from MathLibary import add, sub, mul, div, fact, pow, sqrt, log, e

class testConstructor(unittest.TestCase):
    def setUp(self):
        self.zero = 0
        self.num = 4
        self.num2 = 8
        self.neg = -15
        self.neg2 = -16
        self.nat = 0.23
        self.nat2 = 4.2

class testAddition(testConstructor):
    def runTest(self):
        self.assertEqual(self.zero + self.num, add(self.zero, self.num))
        self.assertEqual(self.num + self.num2, add(self.num, self.num2))
        self.assertEqual(self.neg + self.num2, add(self.neg, self.num2))
        self.assertEqual(self.neg2 + self.nat, add(self.neg2, self.nat))
        self.assertEqual(self.num2 + self.nat2, add(self.num2, self.nat2))
        print('\033[92m[OK]\u001b[37m - Addition')

class testSubstraction(testConstructor):
    def runTest(self):
        self.assertEqual(self.zero - self.num, sub(self.zero, self.num))
        self.assertEqual(self.num - self.num2, sub(self.num, self.num2))
        self.assertEqual(self.neg - self.num2, sub(self.neg, self.num2))
        self.assertEqual(self.neg2 - self.nat, sub(self.neg2, self.nat))
        self.assertEqual(self.num2 - self.nat2, sub(self.num2, self.nat2))
        print('\033[92m[OK]\u001b[37m - Substraction')

class testMultiplication(testConstructor):
    def runTest(self):
        self.assertEqual(self.zero * self.num, mul(self.zero, self.num))
        self.assertEqual(self.num * self.num2, mul(self.num, self.num2))
        self.assertEqual(self.neg * self.num2, mul(self.neg, self.num2))
        self.assertEqual(self.neg2 * self.nat, mul(self.neg2, self.nat))
        self.assertEqual(self.num2 * self.nat2, mul(self.num2, self.nat2))
        print('\033[92m[OK]\u001b[37m - Multiplication')

class testDivision(testConstructor):
    def runTest(self):
        self.assertRaises(Exception, div, self.num2, self.zero)
        self.assertEqual(self.zero / self.num, div(self.zero, self.num))
        self.assertEqual(self.num / self.num2, div(self.num, self.num2))
        self.assertEqual(self.neg / self.num2, div(self.neg, self.num2))
        self.assertEqual(self.neg2 / self.nat, div(self.neg2, self.nat))
        self.assertEqual(self.num2 / self.nat2, div(self.num2, self.nat2))
        print('\033[92m[OK]\u001b[37m - Division')


class testFactorial(testConstructor):
    def runTest(self):
        self.assertEqual(math.factorial(self.zero), fact(self.zero))
        self.assertEqual(math.factorial(self.num), fact(self.num))
        self.assertRaises(Exception, fact, self.neg)
        print('\033[92m[OK]\u001b[37m - Factorial')


class testSquareRoot(testConstructor):
    def runTest(self):
        self.assertRaises(Exception, sqrt, self.zero)
        self.assertEqual(math.sqrt(self.num), sqrt(self.num, 2))
        self.assertEqual(self.num2**(1/3), sqrt(self.num2,3))
        self.assertRaises(Exception, sqrt, self.neg)
        self.assertEqual(self.nat2**(1/2), sqrt(self.nat2, 2))
        print('\033[92m[OK]\u001b[37m - Square root')


class testPower(testConstructor):
    def runTest(self):
        self.assertEqual(self.zero**0, pow(self.zero, 0))
        self.assertEqual(self.num2**(3), pow(self.num2,3))
        self.assertEqual(self.neg**(3), pow(self.neg, 3))
        self.assertEqual(self.neg2**(4), pow(self.neg2, 4))
        self.assertEqual(self.nat2**(2), pow(self.nat2, 2))
        print('\033[92m[OK]\u001b[37m - Power')


class testLogarithm(testConstructor):
    def runTest(self):
        self.assertEqual(math.log(e,e), log(e,e))
        self.assertRaises(Exception, log, self.neg, e)
        self.assertRaises(Exception, log, 0, e)
        self.assertEqual(math.log(self.num, self.num2), log(self.num, self.num2))
        self.assertEqual(math.log(self.num, self.num), log(self.num, self.num))
<<<<<<< HEAD
        print('\033[92m[OK]\u001b[37m - Logarithm')

=======
>>>>>>> documentation
