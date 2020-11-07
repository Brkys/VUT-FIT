/**
 * @author Zdeněk Brhel
 * @brief Reprezentuje vizualizovaný port
 */


#ifndef PORT_H
#define PORT_H

#include <QPainter>
#include <QGraphicsItem>
#include <QDebug>
#include <QGraphicsView>
#include <QGraphicsSceneMouseEvent>
#include <QInputDialog>
#include <QGraphicsScene>
#include <QMessageBox>
#include <QGraphicsSimpleTextItem>
#include <vector>
#include <sstream>
#include "../Model/ModelPort.h"
#include "GraphicsConnect.h"
#include "Colors.h"

namespace Ui{class GraphicsPort;}

using namespace Model;

namespace Gui {

class GraphicsBlock;

/**
 * @brief Třída GraphicsPort zprostředkovává pohled pro porty
 */
class GraphicsPort : public QObject, public QGraphicsEllipseItem
{
    Q_OBJECT

public:

    /**
     * @brief Specifikuje model portu
     */
    ModelPort &modelPort;

    /**
     * @brief Konstruktor třídy, nastaví nutné atributy funkce
     * @param parent Blok, kterému tento port patří
     * @param modelPort model portu
     * @param scene Scéna, na které se port vykresluje
     * @param mState Stav modelu
     * @param Order Aktuální pořadí vykreslovaného portu - nutné pro správné odsazení
     * @param NumberOfPorts Pocet portu dane orientace
     */
    GraphicsPort(GraphicsBlock *parent, ModelPort &modelPort, QGraphicsScene &scene,
                 ModelState &mState, int Order, int NumberOfPorts);

    /**
      * @brief Destruktor třídy smaže případně vytvořené propojení
      */
    ~GraphicsPort();

    /**
     * @brief Zjistí id bloku (rodiče)
     * @return Id bloku
     */
    int getBlockId();

    std::string getBlockName();

    /**
     * @brief Zjistí název portu
     * @return Název portu
     */
    std::string getPortName();

    /**
     * @brief Vykresluje port
     * @param painter Zprostředkovává kreslení
     * @param option Nastavení, které slouží k vykreslování
     * @param widget Základní třída všech UI objektů
     */
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

    /**
     * Anonymní enum - číslo (typ) objektu
     */
    enum {Type = UserType + 1};

    /**
     * @brief Propojí tento port s portem předaným v parametru
     * @param other Propojovaný port
     */
    void connectPorts(GraphicsPort *other);

    /**
     * @brief Nastaví počáteční hodnoty nevyplněných vstupních portů
     * @return zadal uzivatel vsetky pozadovane hodnoty?
     */
    bool setValuesDialog();

    void disconnectPorts();

    /**
     * @brief Vertikalna medzera medzi portami
     */
    static const int Space;

    /**
     * @brief Velkost kruznice portu.
     */
    static const int Size;

    /**
     * @brief Maximalny pocet portov
     */
    static const int MaxPorts;

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

    /**
     * @brief Událost při najetí myši nad objekt
     * @param event Informace o události
     * @return true pokud byl spuštěn nějaký event, jinak false
     */
    bool sceneEvent(QEvent *event);

    /**
     * @brief Slouží pro identifikaci typu tohoto objektu na scéně
     * @return Typ objektu v integeru
     */
    int type() const;

private slots:

    /**
     * Spusti sa po zmene hodnoty premennej portu.
     * Zabezpeci zobrazenie hodnoty.
     * Ak nie su vyplnene vsetky premenne, nerobi nic.
     */
    void varValueChanged();

private:

    /**
     * @brief Scéna, na které se objekt nachází
     */
    QGraphicsScene &scene;

    Model::ModelState &mState;

    bool Hovering;

    /**
     * @brief Nastaví se na true v případě propojování mezi bloky a pocas zadavani hodnoty
     */
    bool Active;

    /**
     * @brief Pointer na relaci mezi tímto a dalším portem. Pokud není port spojen, tak nullptr
     */
    GraphicsConnect *Connection;

    QString basicInfo;

    /**
     * Ziska data o porte pre tooltip.
     * @return data pre tooltip
     */
    QString getInfo();

    void showWarning(std::string message = "Akci nelze spustit");
};

}

#endif // PORT_H
