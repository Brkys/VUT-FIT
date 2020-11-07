/**
 * @brief Zodpoveda za vypocet
 * @author Simeon Borko
 */

#ifndef COMPUTATION_H
#define COMPUTATION_H

#include <QObject>
#include "../Model/ModelState.h"
#include "../Model/ModelBlockArray.h"

using namespace Model;

namespace Gui {

class GraphicsBlock;

    /**
     * @brief Třída reprezentuje výpočet
     */
class Computation : public QObject
{
    Q_OBJECT

public:
    explicit Computation(QObject *parent, ModelState &mState, ModelBlockArray &blockArray, map<int, GraphicsBlock *> &modelIdToGBlock);

    /**
     * Spusti cely vypocet.
     */
    void run();

    /**
     * Prejde do stavu DEBUG.
     */
    void debug();

    /**
     * Urobi jeden krok debugu.
     */
    void step();

    /**
     * Prejde zo stavu DEBUG to stavu RUN a spusti vypocet.
     */
    void continueRun();


signals:
    /**
     * Chceme spustit dalsi blok.
     */
    void next();

private slots:
    /**
     * Odpoved z pokusu o spustenie dalsieho bloku.
     * @param canRunNext je mozne spustit dalsi blok?
     */
    void result(bool canRunNext);

private:

    ModelState &mState;
    ModelBlockArray &blockArray;
    map<int, GraphicsBlock *> &modelIdToGBlock;

    void showWarning();
    void showWarning(std::string message);

    /**
     * Vyziada hodnoty pre nezapojene vstupne porty od uzivatela.
     * @return zadal pouzivatel vsetky hodnoty?
     */
    bool askForValues();

    /**
     * Skontroluje cykly a ziska vstupne hodnoty od uzivatela.
     * Ak nie su cykly a uzivatel zadal vsetky pozadovane hodnoty,
     * stav bude INITIALIZED, inak zostane na SCHEME.
     * @return true, pokud je stav INITIALIZED, jinak false
     */
    bool initialize();
};

}

#endif // COMPUTATION_H
