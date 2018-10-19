##################################################
#                  IPP projekt                   #
#                     část 2                     #
#                                                #
# @author Zdeněk Brhel                           #
# @file interpret.py                             #
# Login: xbrhel04                                #
##################################################

import sys
import xml.etree.ElementTree as ElementTree
import re

#Návratové hodnoty

MISSING_PARAMETER = 10
UNAVAILABLE_IN_FILE = 11
UNAVAILABLE_OUT_FILE = 12
WRONG_XML_FORMAT = 31
WRONG_LEX_SYNT = 32
WRONG_SEMANTIC = 52
WRONG_OPERANDS = 53
UNEXISTING_VAR = 54
UNEXISTING_FRAME = 55
UNEXISTING_VALUE = 56
DIVISION_BY_ZERO = 57
WRONG_STR_USING = 58
INTERNAL_ERROR = 99


# Globální proměnné
globalFrame = {}
localFrame = [{}]
tempFrame = {}
canCreateFrame = False
currentIndex = 1
returnIndex = []
labels = {}
dataStack = []
instCount = 0


def VariableType(element):
    """
    Funkce zjistí typ proměnné v elementu
    :param element: Vstupní element
    :return: Hodnota v elementu přetypovaná na daný datový typ
    """
    global globalFrame, localFrame, tempFrame
    if element.attrib['type'] == 'int':
        return int(element.text)
    elif element.attrib['type'] == 'string':
        if element.text is None:
            return ""
        def replace(match):
            return chr(int(match.group(1)))
        aux = str(element.text)
        regex = re.compile(r"\\(\d{1,3})")
        new = regex.sub(replace, aux)
        return new
    elif element.attrib['type'] == 'var':
        split = str.split(element.text, '@')
        if split[0] == 'GF' and split[1] in globalFrame:
            return globalFrame[split[1]]
        elif split[0] == 'LF' and split[1] in localFrame[-1]:
            return localFrame[-1][split[1]]
        elif split[0] == 'TF' and split[1] in tempFrame:
            if canCreateFrame:
                return tempFrame[split[1]]
            else:
                exit(UNEXISTING_FRAME)
        else:
            exit(UNEXISTING_VAR)
    elif element.attrib['type'] == 'bool':
        if element.text == 'true':
            return True
        elif element.text == 'false':
            return False
    else:
        exit(WRONG_LEX_SYNT)


def help():
    """
    Vypíše nápovědu
    """
    print('Napoveda:')
    print('Soubor se spousti s parametrem --source=\'file\',')
    print('kde \'file\' je absolutni nebo relativni cesta ke vstupnimu souboru')


def arguments():
    """
    Zpracování argumentů
    :return: Při parametru --source vrátí cestu k souboru, při parametru help ukončí program s hodnotou 0, jinak 10
    """
    if (len(sys.argv) > 1) and (len(sys.argv) < 3):
        if sys.argv[1] == '--help':
            help()
            exit(0)
        elif sys.argv[1].startswith('--source'):
            return sys.argv[1][9:]
        else:
            print(sys.stderr, 'Wrong argument ', sys.argv[1])
            exit(MISSING_PARAMETER)
    else:
        print(sys.stderr, 'Wrong number of arguments', sys.argv[1])
        exit(MISSING_PARAMETER)


def CreateFrame():
    """
    Funkce umožní přiřazovat proměnné do tempFrame
    :return:
    """
    global tempFrame
    global canCreateFrame
    canCreateFrame = True
    tempFrame.clear()
    return


def PushFrame():
    """
    Přesune tempFrame na stack lokálních rámců
    """
    global tempFrame
    global canCreateFrame
    if not canCreateFrame:
        exit(UNEXISTING_FRAME)
    localFrame.append(tempFrame.copy())
    tempFrame.clear()
    canCreateFrame = False


def PopFrame():
    """
    Přesune vrchol ze zásobníku lokálních rámců do tempFrame
    """
    global tempFrame, canCreateFrame
    if not localFrame:
        exit(UNEXISTING_FRAME)
    tempFrame.clear()
    tempFrame = localFrame.pop()
    canCreateFrame = True


def DefVar(var):
    """
    Definice proměnné a její zápis do slovníku
    :param var: Název proměnné
    """
    global canCreateFrame, tempFrame

    temp = str.split(var, '@')
    if temp[0] == 'LF':
        localFrame[-1][temp[1]] = None
    elif temp[0] == 'GF':
        globalFrame[temp[1]] = None
    elif temp[0] == 'TF':
        if canCreateFrame:
            tempFrame[temp[1]] = None
        else:
            exit(UNEXISTING_FRAME)
    else:
        exit(WRONG_LEX_SYNT)


def Move(var, value):
    """
    Přiřadí hodnotu proměnné
    :param var: Jméno proměnné
    :param value: Hodnota
    """
    global canCreateFrame
    variable = str.split(var.text, "@")
    if (len(variable) <= 1):
        exit(WRONG_LEX_SYNT)
    elif variable[0] == "TF" and canCreateFrame:
        if variable[1] in tempFrame:
            tempFrame[variable[1]] = value
        else:
            sys.exit(54)
    elif variable[0] == "GF":
        if variable[1] in globalFrame:
            globalFrame[variable[1]] = value
        else:
            sys.exit(54)
    elif variable[0] == "LF":
        if localFrame:
            if variable[1] in localFrame[-1]:
                localFrame[-1][variable[1]] = value
            else:
                exit(UNEXISTING_VAR)
        else:
            exit(UNEXISTING_FRAME)
    else:
        exit(WRONG_LEX_SYNT)


def Call(element):
    """
    Nastaví aktuální řádek na řádek volaného štítku
    :param element: Volaný štítek
    """
    global returnIndex, currentIndex
    if element.attrib['type'] != 'label':
        exit(WRONG_LEX_SYNT)
    if element.text in labels:
        returnIndex.append(currentIndex)
        currentIndex = labels[element.text]
    else:
        exit(UNEXISTING_VALUE)


def Label(element):
    """
    Vytvoří štítek ve slovníku labels
    :param element: Tvořený štítek
    :return:
    """
    aux = 0
    global currentIndex
    for tmp in element:
        if aux == 1:
            exit(WRONG_LEX_SYNT)
        labels[tmp.text] = currentIndex
        aux += 1
    return


def Return():
    """
    Přiřadí na currentIndex číslo řádku, na který se má program vrátit
    """
    global returnIndex, currentIndex
    if returnIndex:
        currentIndex = returnIndex.pop()


def Pushs(element):
    """
    Pushne proměnnou na dataStack
    :param element: Pushnutá proměnná
    """
    aux = 0
    global dataStack
    for tmp in element:
        if aux == 1:
            exit(WRONG_LEX_SYNT)
        dataStack.append(VariableType(tmp))
        aux += 1
    return


def Pops(element):
    """
    Vezme hodnotu z vrcholu zásobníku dataStack a přiřadí ji do proměnné
    :param element: Proměnná
    """
    aux = 0
    for tmp in element:
        if aux == 1:
            exit(WRONG_LEX_SYNT)
        if dataStack:
            split = str.split(tmp.text, '@')
            if split[0] == 'GF' and split[1] in globalFrame:
                Move(tmp, dataStack.pop())
            elif split[0] == 'TF' and split[1] in tempFrame:
                Move(tmp, dataStack.pop())
            elif split[0] == 'LF' and split[1] in localFrame[-1]:
                Move(tmp, dataStack.pop())
            else:
                exit(UNEXISTING_VAR)
        else:
            exit(UNEXISTING_VALUE)
    return


def Math(element, operation):
    """
    Řeší matematické operace
    :param element: Element proměnné, na kterou se má výsledná hodnota přiřadit
    :param operation: Vykonávaná operace
    """
    aux = 0
    split1 = 0
    split2 = 0
    variable = ""
    for tmp in element:
        if aux == 3:
            exit(WRONG_LEX_SYNT)
        elif aux == 0:
            variable = tmp
        elif aux == 1:
            split1 = VariableType(tmp)
        elif aux == 2:
            split2 = VariableType(tmp)
        aux += 1
    if type(split1) == type(split2) == int:
        if operation == "ADD":
            Move(variable, (split1 + split2))
        elif operation == "SUB":
            Move(variable, (split1 - split2))
        elif operation == "IDIV":
            if split2 == 0:
                exit(DIVISION_BY_ZERO)
            Move(variable, int(split1 / split2))
        elif operation == "MUL":
            Move(variable, (split1 * split2))
    else:
        exit(WRONG_SEMANTIC)


def Compare(element, operation):
    """
    Porovná 2 proměnné a výsledek vypíše do proměnné
    :param element: Element proměnné
    :param operation: Vykonávaná operace
    """
    aux = 0
    split1 = 0
    split2 = 0
    variable = ""
    for tmp in element:
        if aux == 3:
            exit(WRONG_LEX_SYNT)
        elif aux == 0:
            variable = tmp
        elif aux == 1:
            split1 = VariableType(tmp)
        elif aux == 2:
            split2 = VariableType(tmp)
        aux += 1

    if type(split1) == type(split2):
        if operation == 'LT':
            Move(variable, bool(split1 < split2))
        elif operation == 'GT':
            Move(variable, bool(split1 > split2))
        elif operation == 'EQ':
            Move(variable, bool(split1 == split2))
    else:
        exit(WRONG_SEMANTIC)


def Logical(element, operation):
    """
    Vykonává logické operace nad datovým typem bool
    :param element: Element obsahující proměnnou
    :param operation: Vykonávaná operace
    """
    aux = 0
    split1 = 0
    split2 = 0
    variable = ""
    for tmp in element:
        if aux == 3:
            exit(WRONG_LEX_SYNT)
        elif aux == 0:
            variable = tmp.text
        elif aux == 1:
            split1 = tmp.text
        elif aux == 2:
            split2 = tmp.text
        aux += 1
    if operation == "NOT" and split2 != 0:
        exit(WRONG_SEMANTIC)
    if type(split1) == type(split2) == bool:
        if operation == "AND":
            Move(variable, split1 and split2)
        elif operation == "OR":
            Move(variable, split1 or split2)
    elif type(split1) == bool:
        if operation == "NOT":
            Move(variable, not split1)
    else:
        exit(WRONG_OPERANDS)


def Int2Char(element):
    """
    Hodnota v elem je převedena na char a uložena v proměnné
    :param element: Element obsahující proměnnou a hodnotu
    """
    aux = 0
    variable = ""
    split1 = 0
    for tmp in element:
        if aux == 2:
            exit(WRONG_LEX_SYNT)
        elif aux == 0:
            variable = tmp
        elif aux == 1:
            split1 = VariableType(tmp)
        aux += 1
    if type(split1) == int:
        if split1 < 1 or split1 > 127:
            sys.exit(58)
        Move(variable, chr(split1))
    else:
        sys.exit(WRONG_SEMANTIC)


def Stri2Int(element):
    """
    Funkce převede string na jeho hodnotu v ASCII a uloží do proměnné
    :param element: Element obsahující hodnotu a proměnnou
    """
    aux = 0
    variable = ""
    split1 = 0
    for tmp in element:
        if aux == 3:
            exit(WRONG_LEX_SYNT)
        elif aux == 0:
            variable = tmp
        elif aux == 1:
            split1 = VariableType(tmp)
        elif aux == 2:
            split2 = VariableType(tmp)
        aux += 1
    if type(split1) == str and type(split2) == int:
        if len(split1) < split2 - 1:
            exit(58)
        Move(variable, ord(split1[split2]))
    else:
        sys.exit(WRONG_SEMANTIC)


def Read(element):
    """
    Přečte vstup a uloží ho do proměnné
    :param element: Element obsahující proměnnou a očekáváný datový typ
    :return:
    """
    aux = 0
    variable = ""
    type = 0
    for tmp in element:
        if aux == 2:
            exit(WRONG_LEX_SYNT)
        elif aux == 0:
            variable = tmp
        elif aux == 1:
            type = tmp.text
        aux += 1
    inputString = input()
    if type == 'int':
        if inputString.isdigit():
            Move(variable, int(inputString))
        else:
            Move(variable, 0)
    elif type == 'bool':
        if inputString.lower() == 'true':
            Move(variable, True)
        else:
            Move(variable, False)
    elif type == 'string':
        Move(variable, inputString)
    else:
        exit(WRONG_SEMANTIC)


def Write(element):
    """
    Vypíše hodnotu
    :param element: Element obsahující hodnotu
    """
    aux = 0
    symb = ""
    for tmp in element:
        if aux == 1:
            exit(WRONG_LEX_SYNT)
        elif aux == 0:
            symb = VariableType(tmp)
        aux += 1
    if type(symb) == bool:
        if symb == True:
            sys.stdout.write('true')
        else:
            sys.stdout.write('false')
    sys.stdout.write(str(symb))


def Concat(element):
    """
    Spojí 2 řetězce do sebe a zapíše je do proměnné
    :param element: Element obsahující řetězce a proměnnou pro zápis
    """
    global globalFrame, tempFrame, localFrame
    aux = 0
    split1 = 0
    split2 = 0
    variable = ""
    for tmp in element:
        if aux == 3:
            exit(WRONG_LEX_SYNT)
        elif aux == 0:
            variable = tmp
        elif aux == 1:
            split1 = tmp
        elif aux == 2:
            split2 = tmp
        aux += 1

    cond1 = VariableType(split1)
    cond2 = VariableType(split2)
    if (type(cond1) == type(cond2) == str):
        Move(variable, str(cond1 + cond2))
    else:
        exit(WRONG_STR_USING)


def Strlen(element):
    """
    Spočítá délku řetězce a uloží ji do proměnné
    :param element: Obsahuje řetězec a proměnnou
    :return:
    """
    aux = 0
    split1 = 0
    variable = ""
    for tmp in element:
        if aux == 2:
            exit(WRONG_LEX_SYNT)
        elif aux == 0:
            variable = tmp
        elif aux == 1:
            split1 = VariableType(tmp)
        aux += 1

    if type(split1) == str:
        Move(variable, int(len(split1)))
    else:
        exit(WRONG_STR_USING)


def Jump(element):
    """
    Skočí na štítek v labels
    :param element: Obsahuje štítek
    """
    global returnIndex, currentIndex
    aux = 0
    for tmp in element:
        if tmp.attrib['type'] != 'label' or aux == 1:
            exit(WRONG_LEX_SYNT)
        if tmp.text in labels:
            currentIndex = labels[tmp.text]
        else:
            exit(UNEXISTING_VALUE)
        aux += 1
    return


def DPrint(element):
    """
    Vypíše hodnotu na STDERR
    :param element: Element obsahující hodnotu
    """
    global globalFrame, localFrame, tempFrame
    aux = 0
    symb = ""
    for tmp in element:
        if aux == 1:
            exit(WRONG_LEX_SYNT)
        symb = tmp
        aux += 1
    if tmp.type == 'var':
        split = str.split(tmp.text, '@')
        if split[0] == 'GF' and split[1] in globalFrame:
            print(sys.stderr, globalFrame[split[1]])
        elif split[0] == 'LF' and split[1] in localFrame[-1]:
            print(sys.stderr, localFrame[split[1]])
        elif split[0] == 'TF' and split[1] in tempFrame:
            print(sys.stderr, tempFrame[split[1]])
        else:
            exit(UNEXISTING_VAR)
    else:
        print(sys.stderr, tmp.text)


def Break():
    """
    Vypíše aktuální stav globálních proměnných na STDOUT
    """
    print(sys.stderr, "GF = " + str(globalFrame))
    print(sys.stderr, "TF = " + str(tempFrame))
    print(sys.stderr, "LF = " + str(localFrame))
    print(sys.stderr, "Instructions done = " + str(instCount))
    print(sys.stderr, "Data stack = " + str(dataStack))


def JumpIfEq(element):
    """
    Podmíněný skok (rovnost)
    :param element: Obsahuje porovnávané hodnoty
    """
    aux = 0
    symb1 = 0
    symb2 = 0
    label = ""
    for tmp in element:
        if aux == 3:
            exit(WRONG_LEX_SYNT)
        elif aux == 0:
            label = tmp
        elif aux == 1:
            symb1 = VariableType(tmp)
        elif aux == 2:
            symb2 = VariableType(tmp)
        aux += 1
    if type(symb1) == type(symb2):
        if symb1 == symb2:
            Call(label)
    else:
        exit(WRONG_OPERANDS)


def JumpIfNEq(element):
    """
    Podmíněný skok (nerovnost)
    :param element: Obsahuje porovnávané hodnoty
    """
    aux = 0
    symb1 = 0
    symb2 = 0
    label = ""
    for tmp in element:
        if aux == 3:
            exit(WRONG_LEX_SYNT)
        elif aux == 0:
            label = tmp
        elif aux == 1:
            symb1 = VariableType(tmp)
        elif aux == 2:
            symb2 = VariableType(tmp)
        aux += 1
    if type(symb1) == type(symb2):
        if symb1 != symb2:
            Call(label)
    else:
        exit(WRONG_OPERANDS)


def Type(element):
    """
    Zjistí datový typ proměnné
    :param element: Element obsahující proměnnou
    :return:
    """
    aux = 0
    symb = 0
    variable = ""
    for tmp in element:
        if aux == 2:
            exit(WRONG_LEX_SYNT)
        elif aux == 0:
            variable = tmp
        elif aux == 1:
            symb = tmp
        aux += 1
    if symb.attrib['type'] == 'int':
        symb.text = 'int'
        Move(variable, symb)
    elif symb.attrib['type'] == 'string':
        symb.text = 'string'
        Move(variable, symb)
    elif symb.attrib['type'] == 'bool':
        symb.text = 'bool'
        Move(variable, symb)
    else:
        exit(WRONG_SEMANTIC)


def GetChar(element):
    """
    Do proměnné uloží znak v řetěžci na dané pozici
    :param element: Element obsahující proměnnou, řetězec a index
    """
    split1 = ""
    split2 = ""
    variable = ""
    aux = 0
    for tmp in element:
        if aux == 3:
            exit(WRONG_LEX_SYNT)
        if aux == 0:
            variable = tmp
        if aux == 1:
            split1 = VariableType(tmp)
        if aux == 2:
            split2 = VariableType(tmp)
        aux += 1
    if type(split1) == str and type(split2) == int:
        if len(split1) < split2 - 1:
            exit(WRONG_STR_USING)
        else:
            Move(variable, str(split1[split2]))
    else:
        exit(WRONG_SEMANTIC)


def SetChar(element):
    """
    Přepíše znak v řetězci v proměnné na dané pozici na daný znak
    :param element: Element obsahující řetězec, pozici a znak
    :return:
    """
    split1 = ""
    split2 = ""
    variable = ""
    varval = ""
    aux = 0
    for tmp in element:
        if aux == 3:
            exit(WRONG_LEX_SYNT)
        if aux == 0:
            variable = tmp
            varval = VariableType(tmp)
        if aux == 1:
            split1 = VariableType(tmp)
        if aux == 2:
            split2 = VariableType(tmp)
        aux += 1
    if type(split2) == str and type(split1) == int and type(varval) == str:
        if len(varval) < split1 or len(split2) == 0:
            exit(WRONG_STR_USING)
        else:
            returnval = list(varval)
            returnval[split1] = split2[0]
            Move(variable, str(''.join(returnval)))
    else:
        exit(WRONG_SEMANTIC)


def interpret(element):
    """
    Funkce vezme data z xml a vykoná pozadovanou akci
    :param main: Obsahuje aktualni element obsahujici vsechny informace potrebne k vykonani prikazu
    """

    # print(element.attrib)

    if element.attrib['opcode'] == 'MOVE':  # nastavení hodnoty proměnné
        split1 = ""
        split2 = ""
        aux = 0
        for tmp in element:
            if aux == 2:
                exit(WRONG_LEX_SYNT)
            if aux == 1:
                split2 = VariableType(tmp)
            if aux == 0:
                split1 = tmp
            aux += 1
        Move(split1, split2)
    elif element.attrib['opcode'] == 'CREATEFRAME':  # vytvoření TF
        if (element):
            exit(WRONG_LEX_SYNT)
        CreateFrame()
    elif element.attrib['opcode'] == 'PUSHFRAME':  # pushnutí TF do LF
        if (element):
            exit(WRONG_LEX_SYNT)
        PushFrame()
    elif element.attrib['opcode'] == 'POPFRAME':  # popnuti LF do TF
        if (element):
            exit(WRONG_LEX_SYNT)
        PopFrame()
    elif element.attrib['opcode'] == 'DEFVAR':  # definování proměnné
        for tmp in element:
            if tmp.attrib['type'] != 'var':
                exit(WRONG_LEX_SYNT)
            DefVar(tmp.text)
    elif element.attrib['opcode'] == 'CALL':  # volání funkcí
        aux = 0
        var = 0
        for tmp in element:
            if aux == 1:
                exit(WRONG_LEX_SYNT)
            elif aux == 0:
                var = tmp
            Call(tmp)
    elif element.attrib['opcode'] == 'RETURN':  # návrat z funkce
        if (element):
            exit(WRONG_LEX_SYNT)
        Return()
    elif element.attrib['opcode'] == 'PUSHS':  # pushne element do datastacku
        Pushs(element)
    elif element.attrib['opcode'] == 'POPS':  # popne element z datastacku a uloží do proměnné
        Pops(element)
    elif element.attrib['opcode'] == 'ADD':  # přičíst
        Math(element, "ADD")
    elif element.attrib['opcode'] == 'SUB':  # odečíst
        Math(element, "SUB")
    elif element.attrib['opcode'] == 'MUL':  # vynásobit
        Math(element, "MUL")
    elif element.attrib['opcode'] == 'IDIV':  # vydělit
        Math(element, "IDIV")
    elif element.attrib['opcode'] == 'LT' or element.attrib['opcode'] == 'EQ' or \
            element.attrib['opcode'] == 'GT':  # porovnávání
        Compare(element, element.attrib['opcode'])
    elif element.attrib['opcode'] == 'AND' or element.attrib['opcode'] == 'NOT' or \
            element.attrib['opcode'] == 'OR':  # logické operátory
        Logical(element, element.attrib['opcode'])
    elif element.attrib['opcode'] == 'INT2CHAR':  # převedení int na char
        Int2Char(element)
    elif element.attrib['opcode'] == 'STRI2INT':  # převedení charu na int
        Stri2Int(element)
    elif element.attrib['opcode'] == 'READ':  # načíst z stdin
        Read(element)
    elif element.attrib['opcode'] == 'WRITE':  # vypsat na stdout
        Write(element)
    elif element.attrib['opcode'] == 'CONCAT':  # spojení 2 stringů
        Concat(element)
    elif element.attrib['opcode'] == 'STRLEN':  # velikost 2 stringů
        Strlen(element)
    elif element.attrib['opcode'] == 'GETCHAR':  # dostane char na pozici x ve stringu
        GetChar(element)
    elif element.attrib['opcode'] == 'SETCHAR':  # ------------------
        SetChar(element)
    elif element.attrib['opcode'] == 'TYPE':  # zjistí typ proměnné
        Type(element)
    elif element.attrib['opcode'] == 'LABEL':  # vytvoří label
        Label(element)
    elif element.attrib['opcode'] == 'JUMP':  # jumpne na label
        Jump(element)
    elif element.attrib['opcode'] == 'JUMPIFEQ':  # podmíněný skok
        JumpIfEq(element)
    elif element.attrib['opcode'] == 'JUMPIFNEQ':  # podmíněný skok
        JumpIfNEq(element)
    elif element.attrib['opcode'] == 'DPRINT':  # výstup na stderr
        DPrint(element)
    elif element.attrib['opcode'] == 'BREAK':  # break..
        Break()
    else:
        exit(WRONG_LEX_SYNT)
    return 0


# main
# zjištění, že file existuje
file = arguments()
try:
    open(file, 'r').close()
except Exception:
    print(sys.stderr, 'File doesn\'t exist or insufficient permissions')
    exit(UNAVAILABLE_IN_FILE)

# otevírání XML z file
try:
    xml = ElementTree.parse(file)
except Exception:
    print(sys.stderr, 'Wrong XML format!')
    exit(31)
main = xml.getroot()
if main.tag != 'program' or main.attrib['language'] != 'IPPcode18':
    exit(WRONG_XML_FORMAT)

try:
    labelsInCode = xml.findall("instruction[@opcode='LABEL']")
except Exception:
    exit(31)

for lab in labelsInCode:
    currentIndex = int(lab.attrib["order"])
    Label(lab)
currentIndex = 1

# procházení všeho, co je v main xml tagu <program>
while True:
    try:
        currentCommand = xml.findall('instruction[@order=\'' + str(currentIndex) + '\']')
    except Exception:
        exit(WRONG_XML_FORMAT)

    length = len(currentCommand)
    if length > 1:
        exit(WRONG_XML_FORMAT)
    elif length == 0:
        break
    interpret(currentCommand[0])
    currentIndex += 1
    instCount += 1

exit(0)
