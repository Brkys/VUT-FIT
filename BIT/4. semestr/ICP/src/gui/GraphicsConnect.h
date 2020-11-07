/**
 * @author Zdeněk Brhel
 * @brief Reprezentuje propoj mezi dvěma porty
 */

#ifndef CONNECTION_H
#define CONNECTION_H

#include <QPainter>
#include <QGraphicsItem>
#include <QDebug>
#include <QGraphicsView>
#include <QObject>
#include <QToolTip>
#include <QGraphicsSceneMouseEvent>
#include "../Model/ModelState.h"

namespace Ui {class GraphicsConnect;}

namespace Gui {

class GraphicsPort;

/**
 * @brief Třída reprezentuje propoj
 */
class GraphicsConnect : public QGraphicsLineItem
{
public:
    /**
     * @brief Nastavuje propoj
     * @param OutPort Port ze kterého byla tažena myš
     * @param InPort Port na kterém byla puštěna myš
     * @param scene Scéna, na které se vykresluje propoj
     * @param mState Aktuální stav
     */
    GraphicsConnect(GraphicsPort &OutPort, GraphicsPort &InPort, QGraphicsScene &scene, Model::ModelState &mState);

    /**
     * @brief Smaže propojení i v portech
     */
    ~GraphicsConnect();

    std::map<std::string, double> Value;

    /**
     * @brief Vykreslí propoj
     * @param painter
     * @param option
     * @param widget
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
    enum {Type = UserType + 2};

    GraphicsPort &OutPort;
    GraphicsPort &InPort;

protected:
    bool sceneEvent(QEvent *event);

    void mousePressEvent(QGraphicsSceneMouseEvent *event);

private:

    QGraphicsScene &scene;
    Model::ModelState &mState;

    /**
     * Posun pozice portu
     */
    static const QPointF deltaPoint;

    void showWarning(std::string message = "Akci nelze spustit");

};

}

#endif // CONNECTION_H
