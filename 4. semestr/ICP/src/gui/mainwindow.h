/**
 * @author Zdeněk Brhel, Simeon Borko
 * @brief Reprezentuje hlavní okno programu
 */

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtCore>
#include <QtGui>
#include "../Configuration/ConfigBlockMap.h"
#include "GraphicsBlock.h"
#include "GraphicsPort.h"
#include <QMessageBox>
#include <QFileDialog>
#include "../Model/ModelBlockArray.h"
#include "../Model/ModelState.h"
#include "Computation.h"

namespace Ui {class MainWindow;}

namespace Gui {

    /**
     * @brief Třída reprezentuje hlavní okno programu
     */
class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    /**
     * @brief Nastaví MainWindow
     * @param mState
     * @param blockArray
     * @param parent
     */
    explicit MainWindow(ModelState &mState, ModelBlockArray &blockArray, QWidget *parent = 0);

    ~MainWindow();

    /**
     * @brief Zobrazí okno
     */
    void show();

    /**
     * @brief Zobrazí upozornění
     * @param message
     */
    void showWarning(std::string message = "Akci nelze spustit");

private slots:
    /**
     * @brief Spustí ukládání schématu
     */
    void on_actionSave_triggered();

    /**
     * @brief Přidá do scény blok, na který bylo kliknuto
     * @param index
     */
    void on_blockList_doubleClicked(const QModelIndex &index);

    /**
     * @brief Spustí vytvoření nového schématu
     */
    void on_actionNew_triggered();

    /**
     * @brief Otevře nápovědu
     */
    void on_actionHelp_triggered();

    /**
     * @brief Otevře autory
     */
    void on_actionAuthors_triggered();

    /**
     * @brief Ukončí výpočet schématu
     */
    void on_controlEndButton_clicked();

    /**
     * @brief Přejde na další krok výpočtu
     */
    void on_controlNextStepButton_clicked();

    /**
     * Ak je stav SCHEME, pokusi sa spustit vypocet.
     * Ak sa vypocet podaril, zmeni stav na FINISH.
     * Ak na zaciatku nebol stav SCHEME, vypise sa varovanie.
     */
    void on_actionRun_triggered();

    /**
     * @brief Spustí načtení schématu
     */
    void on_actionLoad_triggered();


    /**
     * @brief Spustí krokování schématu
     */
    void on_actionDebug_triggered();

    /**
     * @brief Spustí pokračování výpočtu (výpočet se automaticky dokončí)
     */
    void on_controlContinueButton_clicked();

    /**
     * Na zaklade stavu povoli / zakaze tlacidla.
     */
    void updateButtons();

private:
    Ui::MainWindow *ui;

    /**
     * @brief Scéna, na které se objekty vykreslují
     */
    QGraphicsScene *scene;

    /**
     * Mapuje id bloku na ukazatel na graficky blok.
     * Ak je blok vymazany, zaznam sa nezmaze; ukazatel je neplatny.
     */
    map<int, GraphicsBlock *> modelIdToGBlock;

    Model::ModelState &mState;
    ModelBlockArray &blockArray;
    Computation *computation;

    static QString fileNameFilter;

    /**
     * @brief Slouží k načítání a přiřazování portu
     * @param blockId
     * @param portName Jméno portu
     * @return Pointer na port
     */
    GraphicsPort * getPort(int blockId, std::string portName);

    /**
     * @brief Přidá blok do scény
     * @param blockName Jméno bloku
     * @return Pointer na přidaný blok
     */
    GraphicsBlock * addBlock(string blockName);

};

}

#endif // MAINWINDOW_H
