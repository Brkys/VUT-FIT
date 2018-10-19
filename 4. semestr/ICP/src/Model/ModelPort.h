/**
 * @author Simeon Borko
 * @brief Reprezentuje port vytvoreneho bloku.
 */

#ifndef SRC_RUNNING_PORT_H
#define SRC_RUNNING_PORT_H

#include "../Configuration/ConfigPort.h"
#include "ModelState.h"

#include <limits>
#include <cmath>
#include <QObject>

using namespace std;


namespace Model {

    class ModelBlock;

    class ModelPort : public QObject {

        Q_OBJECT

        ModelBlock &block;
        Configuration::ConfigPort &cPort;
        vector<string> varNames;
        map<string, double> varMap;
        ModelPort *connected = nullptr;
        ModelState &mState;

        /**
         * Ziska premennu podla nazvu. Ak neexistuje, vyhodi vynimku.
         * @param varName nazov premennej
         * @return iterator mapy pre najdenu premennu
         */
        map<string, double>::iterator getVar(string &varName);

        /**
         * Ziska prepojeny blok.
         * @return ukazatel na prepojeny blok alebo nullptr
         */
        ModelBlock *getConnectedBlock();

    public:

        /**
         * Konstruktor inicializuje premenne portu na NaN.
         * Ak stav nie je SCHEME, vyhodi vynimku.
         * @param cPort port z konfiguracie
         */
        explicit ModelPort(ModelBlock &block, Configuration::ConfigPort &cPort, ModelState &mState);

        /**
         * Destruktor zrusi pripadne prepojenie.
         */
        ~ModelPort();

        /**
         * Zisti, ci port ma vyplnene vsetky hodnoty.
         * @return ma port vsetky hodnoty?
         */
        bool hasValues();

        /**
         * Ziska hodnotu premennej. Ak premenna neexistuje, vyhodi vynimku.
         * @param varName nazov premennej
         * @return hodnota
         */
        double getValue(string varName);

        /**
         * Nastavi novu hodnotu premennej. Ak premenna neexistuje, vyhodi vynimku.
         * Ak je tento port vystupny a je prepojeny s inym portom, hodnota sa nastavi aj pre prepojeny port.
         * Vysle signal varValueSet().
         * @param varName nazov premennej
         * @param value hodnota
         */
        void setValue(string varName, double value);

        /**
         * Zisti, ci je tento port prepojitelny so zadanym portom.
         * Tento port sa povazuje za vstupny a zadany za vystupny.
         * @param outPort vystupny port
         * @return su tento a zadany port prepojitelne?
         */
        bool areConnectable(ModelPort &inPort);

        /**
         * Prepoji tento port a zadany port. Ak nie su prepojitelne, vyhodi sa vynimka.
         * Ak stav nie je SCHEME, vyhodi vynimku.
         * @param outPort vystupny port
         */
        void connect(ModelPort &inPort);

        /**
         * Zrusi prepojenie medzi tymto portom a prepojenym portom.
         * Ak prepojenie neexistuje, vyhodi vynimku.
         * Ak stav nie je SCHEME, vyhodi vynimku.
         */
        void disconnect();

        /**
         * Ak je tento port vystupny a je napojeny na iny port, ziska sa napojeny blok.
         * @return ukazatel na napojeny blok alebo nullptr
         */
        ModelBlock *getNextBlock();

        vector<string> getVarNames();

        map<string, double> getVarMap();

        bool isInput();

        bool isOutput();

        char getInOut();

        /**
         * Zisti, ci je port volny (nie obsadeny).
         * @return je port volny?
         */
        bool isFree();

        bool isConnected();

        string getName();

        /**
         * Ziska nazvy premennych, ktore nie su vyplnene vo vstupnom porte (ktory nie je ani zapojeny).
         * @return vektor nazvov nezadanych premennych ak toto je vstupny port, inak prazdny vektor
         */
        vector<string> emptyInputVarNames();

        /**
         * Port je spustitelny, ak je vstupny a ma vyplnene vsetky premenne
         * alebo je vystupny a nema vyplnene ziadne premenne.
         * @return je port spustitelny?
         */
        bool runnable();

        string getTypeName();

        /**
         * Zmaze hodnoty premennych.
         */
        void resetValues();

    signals:
        void varValueSet();
    };

}


#endif //SRC_RUNNING_PORT_H
