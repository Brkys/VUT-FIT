######################################################
#                Súbor obsahujúci triedu             #
#         pre backend uživatelského rozhrania        #
#                                                    #
#                Autor: Lukáš Letovanec              #
#                   Login: xletov00                  #
######################################################

# -*- coding: utf-8 -*-
import MathLibary
from PyQt5 import QtCore, QtGui, QtWidgets


class Backend_ui():
    operands_with_spaces = ['+', '-', '*', '/', '^', '√', '!', 'log']
    def setup_actions(self):
        """
        Metóda priradí funkcionalitu jednotlivých tlačítkam kalkulačky
        """
        # priradi funkcionalitu ciselnym tlacitkam
        self.pushButton_0.clicked.connect(lambda: self.on_click_basic("0"))
        self.pushButton_1.clicked.connect(lambda: self.on_click_basic("1"))
        self.pushButton_2.clicked.connect(lambda: self.on_click_basic("2"))
        self.pushButton_3.clicked.connect(lambda: self.on_click_basic("3"))
        self.pushButton_4.clicked.connect(lambda: self.on_click_basic("4"))
        self.pushButton_5.clicked.connect(lambda: self.on_click_basic("5"))
        self.pushButton_6.clicked.connect(lambda: self.on_click_basic("6"))
        self.pushButton_7.clicked.connect(lambda: self.on_click_basic("7"))
        self.pushButton_8.clicked.connect(lambda: self.on_click_basic("8"))
        self.pushButton_9.clicked.connect(lambda: self.on_click_basic("9"))
        # priradi funkcionalitu ostatnym tlacitkam
        self.pushButton_clear.clicked.connect(lambda: self.on_click_clear())
        self.pushButton_add.clicked.connect(lambda: self.on_click_basic("+"))
        self.pushButton_sub.clicked.connect(lambda: self.on_click_basic("-"))
        self.pushButton_dec.clicked.connect(lambda: self.on_click_basic("."))
        self.pushButton_div.clicked.connect(lambda: self.on_click_basic("/"))
        self.pushButton_mul.clicked.connect(lambda: self.on_click_basic("*"))
        self.pushButton_fact.clicked.connect(lambda: self.on_click_basic("!"))
        self.pushButton_ln.clicked.connect(lambda: self.on_click_basic("log"))
        self.pushButton_sqrt.clicked.connect(lambda: self.on_click_basic("√"))
        self.pushButton_pow.clicked.connect(lambda: self.on_click_basic("^"))
        self.pushButton_ret.clicked.connect(lambda: self.on_click_return())
        self.pushButton_eq.clicked.connect(lambda: self.on_click_equals())


    def on_click_basic(self, value):
        """
        Metóda pridá hodnotu 'value' na displej kalkulačky
        a pridá túto akciu do zoznamu operácii aby bolo možné
        vykonať undo.
        :param value: hodnota ktorá má byť pridaná na displej
        """
        self.add_to_undo(self.user_input.text()) # pridame do undo zoznamu
        if len(self.user_input.text()) < 30: # maximum znakov je 29
            if value in self.operands_with_spaces:
                self.user_input.setText("{} {} ".format(self.user_input.text(), value))
            else:
                self.user_input.setText("{}{}".format(self.user_input.text(), value))

    def on_click_clear(self):
        """
        Metóda vymaže obsah 'displeja' kalkulačky a pridá túto akciu
        do zoznamu operácii aby bolo možné vykonať undo.
        """
        self.add_to_undo(self.user_input.text())
        self.user_input.setText("")

    def on_click_return(self):
        """
        Metóda vykoná akciu undo.
        """
        if len(self.undo_deque) > 0:
            self.user_input.setText(self.undo_deque.pop())

    def on_click_equals(self):
        """
        Metóda vyhodnotí výraz ktorý sa aktuálne nachádza
        na displeji kalkulačky
        """
        tmp = ' '.join(self.user_input.text().split())
        parts = tmp.split(' ')
        parts = self.prepare_for_eval(parts)
        if len(parts) == 0:
            self.result.setText('ERR')
        else:
            res = ''.join(parts)
            try:
                result = eval(res)
                if len(str(result)) > 29:
                    self.result.setText("ERR")
                else:
                    self.result.setText("{}".format(result))
            except:
                self.result.setText('ERR')
            self.on_click_clear()

    def prepare_for_eval(self, input_lst):
        """
        Metóda upraví výraz na formát vhodný pre vyhodnotenie.
        :param input_list: list obsahujúci výraz rozdelený na operátory a operandy
        """
        try:
            while '!' in input_lst:
                index = input_lst.index('!')
                input_lst[index] = "MathLibary.fact({})".format(input_lst[index-1])
                del input_lst[index-1]
            while 'log' in input_lst:
                index = input_lst.index('log')
                input_lst[index] = "MathLibary.log({},10)".format(input_lst[index+1])
                del input_lst[index+1]
            while '^' in input_lst or '√' in input_lst:
                if '^' in input_lst:
                    if '√' in input_lst:
                        index = input_lst.index('^') if input_lst.index('^') < input_lst.index('√') else input_lst.index('√')
                    else:
                        index = input_lst.index('^')
                else:
                    index = input_lst.index('√')
                input_lst[index] = "MathLibary.pow({},{})".format(input_lst[index-1], input_lst[index+1]) if input_lst[index] == '^'\
                                    else "MathLibary.sqrt({},{})".format(input_lst[index+1], input_lst[index-1])
                del input_lst[index+1]
                del input_lst[index-1]
            return input_lst
        except Exception as e:
            return []

    def add_to_undo(self, data):
        """
        Metóda pridá 'data' do zoznamu pre undo. Ak je v undo zozname
        viac ako 15 prvkov najstarší vyradí.
        """
        # ak je ulozenych viac ako 15 operaci pre undo, zacneme popovat najstarsie
        if len(self.undo_deque) > 15:
            self.undo_deque.popleft()
        self.undo_deque.append(data)

    def keyPressEvent(self, event):
        """
        Metóda funguje ako obsluha udalosti 'stlačenie tlačítka'.
        Ak je stlačené tlačítko podporované kalkulačkou vykoná sa
        príslušná akcia.
        """
        # cisla
        if event.key() == QtCore.Qt.Key_0:
            self.on_click_basic("0")
        elif event.key() == QtCore.Qt.Key_1:
            self.on_click_basic("1")
        elif event.key() == QtCore.Qt.Key_2:
            self.on_click_basic("2")
        elif event.key() == QtCore.Qt.Key_3:
            self.on_click_basic("3")
        elif event.key() == QtCore.Qt.Key_4:
            self.on_click_basic("4")
        elif event.key() == QtCore.Qt.Key_5:
            self.on_click_basic("5")
        elif event.key() == QtCore.Qt.Key_6:
            self.on_click_basic("6")
        elif event.key() == QtCore.Qt.Key_7:
            self.on_click_basic("7")
        elif event.key() == QtCore.Qt.Key_8:
            self.on_click_basic("8")
        elif event.key() == QtCore.Qt.Key_9:
            self.on_click_basic("9")
        elif event.key() == QtCore.Qt.Key_Escape: #ESC ukonci aplikaciu
            self.close()
        elif event.key() == QtCore.Qt.Key_Enter:
            self.on_click_equals()
        elif event.key() == QtCore.Qt.Key_Slash:
            self.on_click_basic("/")
        elif event.key() == QtCore.Qt.Key_Plus:
            self.on_click_basic("+")
        elif event.key() == QtCore.Qt.Key_Minus:
            self.on_click_basic("-")
        elif event.key() == QtCore.Qt.Key_Asterisk:
            self.on_click_basic("*")

### Koniec súboru UiBackend.py ###
