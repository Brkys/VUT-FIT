/**
 * @author Zdeněk Brhel
 * @brief Reprezentuje vizualizovaný blok
 */

#ifndef BLOCK_H
#define BLOCK_H

#include <QPainter>
#include <QGraphicsItem>
#include <QDebug>
#include <QGraphicsView>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsScene>
#include <QObject>
#include "GraphicsConnect.h"
#include "../Model/ModelBlock.h"
#include "GraphicsPort.h"
#include "Colors.h"

namespace Ui {class GraphicsBlock;}

using namespace Configuration;
using namespace Model;

namespace Gui {

/**
 * @brief Třída GraphicsBlock reprezentuje pohled pro blok
 */
class GraphicsBlock : public QObject, public QGraphicsItem
{

    Q_OBJECT

public:
    /**
     * @brief Konstruktor třídy, nastaví nezbytné parametry
     * @param blockName Jméno bloku
     * @param scene Scéna, na které je blok vykreslovaný
     * @param mState
     */
    GraphicsBlock(Model::ModelBlock &modelBlock, QGraphicsScene &scene, Model::ModelState &mState);

    /**
     * @brief Smaže model bloku
     */
    ~GraphicsBlock();

    /**
     * @brief Nastaví a vrátí čtverec okolo portu
     * @return Čtverec
     */
    QRectF boundingRect() const;

    /**
     * @brief Text obsažený ve vizualizaci bloku
     */
    QString text;

    /**
     * @brief Vykresluje blok
     * @param painter Zprostředkovává kreslení
     * @param option Nastavení, které slouží k vykreslováín
     * @param widget Základní třída všech UI objektů
     */
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

    /**
     * @brief Slouží pro identifikaci typu tohoto objektu na scéně
     * @return Typ objektu v integeru
     */
    int type() const;

    /**
     * Anonymní enum - číslo (typ) objektu
     */
    enum {Type = UserType + 3};

    /**
     * @brief Scéna, na které se objekt nachází
     */
    QGraphicsScene &scene;

    /**
     * @brief Model bloku
     */
    Model::ModelBlock &modelBlock;

    /**
     * @brief Mapa všech portů bloku
     */
    map<std::string, GraphicsPort *> Ports;

    static const int Width;
    static const int Height;

protected:
    /**
     * @brief Událost při stisknutí tlačítka myši na tomto objektu
     * @param event Informace o události
     */
    void mousePressEvent(QGraphicsSceneMouseEvent *event);

    /**
     * @brief Událost při uvolnění tlačítka myši na tomto objektu
     * @param event Informace o události
     */
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);

private:

    /**
     * @brief Nastaví se na true, jestliže bylo na objekt kliknuto
     */
    bool Pressed;

    /**
     * @brief Nastaví se na true, jestliže je blok spočítaný
     */
    bool Computed;

    /**
     * @brief Nastaví se na true, jestliže je blok právě počítaný
     */
    bool Run;

    /**
     * @brief Pozice objektu, slouží k tomu, aby se všechny bloky nevykreslovali na stejných souřadnicích
     */
    static int Position;

    Model::ModelState &mState;

    /**
     * @brief Zobrazí varování
     * @param message
     */
    void showWarning(std::string message = "Akci nelze spustit");

private slots:

    /**
     * @brief Spustí se, jestli se spustil výpočet
     */
    void runStarted();
    /**
    * @brief Spustí se, jestli se ukončil výpočet
    */
    void runFinished();
    /**
    * @brief Spustí se, jestli se resetovalo schéma
    */
    void reseted();
};

}

#endif // BLOCK_H
