/**
 * @author Simeon Borko
 * @brief Reprezentuje vytvoreny blok, s ktorym sa da pocitat.
 */

#ifndef SRC_RUNNING_BLOCK_H
#define SRC_RUNNING_BLOCK_H

#include <vector>
#include <algorithm>
#include <QObject>
#include "../Configuration/ConfigBlock.h"
#include "ModelPort.h"
#include "ModelState.h"

namespace Model {

    class ModelBlockArray;

    class ModelBlock : public QObject {

        Q_OBJECT

        static unsigned sleepTime;

        int id;
        Configuration::ConfigBlock &cBlock;
        map<string, ModelPort *> portMap;
        ModelBlockArray &blockArray;
        ModelState &mState;

        /**
         * Detekuje cykly rekurziou.
         * @param blockIdSet mnozina id blokov
         * @return detekoval cyklus?
         */
        bool hasCycles(set<int> &blockIdSet);

        void fnRgbToGray();

        void fnAdd();

        void fnFlip();

        void fnMultiply();

        void fnMedian5();

        static bool doubleToBool(double n);

        static double boolToDouble(bool b);

        void fnAnd();

        void fnOr();

        void fnXor();

        void fnNot();

    signals:

        void runStarted();
        void runFinished();
        void reseted();

    public:

        /**
         * Inicializuje porty pre vytvoreny blok.
         * Ak stav nie je SCHEME, vyhodi vynimku.
         * @param id cislo vytvoreneho bloku
         * @param cBlock blok konfiguracie (ako predloha)
         * @param blockArray
         * @param mState
         */
        explicit ModelBlock(int id, Configuration::ConfigBlock &cBlock, ModelBlockArray &blockArray, ModelState &mState);

        ~ModelBlock();

        map<string, ModelPort *> getPortMap();

        /**
         * Ziska vstupne porty.
         * @return vektor ukazatelov na vstupne porty
         */
        vector<ModelPort *> getInputPorts();

        /**
         * Ziska vystupne porty.
         * @return vektor ukazatelov na vystupne porty
         */
        vector<ModelPort *> getOutputPorts();

        /**
         * Ziska port podla nazvu, ak neexistuje, vyhodi vynimku.
         * @param portName nazov portu
         * @return referencia na port
         */
        ModelPort& getPort(string portName);

        /**
         * Prepoji tento blok s druhym blokom.
         * Ak stav nie je SCHEME, vyhodi vynimku.
         * @param otherBlock blok na prepojenie
         * @param thisPortName nazov vstupneho portu tohto bloku
         * @param otherPortName nazov vystupneho portu druheho bloku
         */
        void connect(ModelBlock &otherBlock, string thisPortName, string otherPortName);

        /**
         * Zisti, ci blok je korenom - teda ci ma aspon jeden vstupny port, ktory nie je zapojeny.
         * @return je blok korenom?
         */
        bool isRoot();

        /**
         * Ziska bloky, ktore su napojene vystupne porty tohto bloku.
         * @return dalsie bloky
         */
        vector<ModelBlock *> getNextBlocks();

        /**
         * Detekuje cykly.
         * @return ma cyklus?
         */
        bool hasCycles();

        int getId();

        /**
         * Ziska vstupne porty, ktore maju aspon jednu premennu nevyplnenu.
         * @return vektor portov
         */
        vector<ModelPort *> emptyInputPorts();

        /**
         * Zisti, ci blok je mozne spustit, to je ak ma vyplnene vstupne porty
         * (ci uz od uzivatela alebo z ineho bloku) a nema vyplnene vystupne porty.
         * @return je mozne tento blok spustit?
         */
        bool runnable();

        /**
         * Spusti vypocet bloku. Ak blok nie je spustitelny, vyhodi vynimku.
         * Ak stav nie je RUN alebo STEP, vyhodi vynimku.
         */
        void run();

        string getName();

        /**
         * Zmaze hodnoty portov.
         */
        void resetValues();

    private slots:

        /**
         * Odstrani mazany port z mapy.
         */
        void removePortFromMap(QObject *obj);

    };

}


#endif //SRC_RUNNING_BLOCK_H
