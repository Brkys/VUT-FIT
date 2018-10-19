/**
 * @author Zdeněk Brhel
 * Reprezentuje propoj mezi dvěma porty
 */

#include "GraphicsConnect.h"
#include "GraphicsPort.h"
#include "mainwindow.h"

namespace Gui {

const QPointF GraphicsConnect::deltaPoint (GraphicsPort::Size / 2, GraphicsPort::Size / 2);

GraphicsConnect::GraphicsConnect(GraphicsPort &Port1, GraphicsPort &Port2, QGraphicsScene &scene, Model::ModelState &mState)
    : OutPort(Port1), InPort(Port2), scene(scene), mState(mState)
{
    setFlag(ItemIsSelectable);
    setAcceptHoverEvents(true);

    QPen pen (Qt::black);
    pen.setWidth(3);
    setPen(pen);

    setPos(deltaPoint);

    new QGraphicsRectItem(boundingRect(), this);
}

GraphicsConnect::~GraphicsConnect()
{
    OutPort.disconnectPorts();
}

void GraphicsConnect::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    (void) option;
    (void) widget;
    setLine(QLineF(InPort.scenePos(), OutPort.scenePos()));
    painter->setPen(pen());
    painter->drawLine(line());
}

int GraphicsConnect::type() const
{
    return Type;
}

void GraphicsConnect::showWarning(std::string message) {
    QMessageBox::warning(dynamic_cast<MainWindow *>(scene.parent()), QString("Upozornění"), QString::fromStdString(message), QMessageBox::Ok);
}

bool GraphicsConnect::sceneEvent(QEvent *event)
{
    if(event->type() == QEvent::GraphicsSceneHoverEnter || event->type() == QEvent::GraphicsSceneHoverMove)
    {
        if (OutPort.modelPort.hasValues()) {
            QStringList list;
            for (auto it : OutPort.modelPort.getVarMap()) {
                list << (QString::fromStdString(it.first) + " = " + QString::number(it.second));
            }
            setToolTip(list.join("\n"));
        } else {
            setToolTip("Žádné data");
        }
    }
    return QGraphicsLineItem::sceneEvent(event);
}

void GraphicsConnect::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    if(event->button() == Qt::RightButton)
    {
        if (mState == ModelState::SCHEME) {
            delete this;
        } else {
            showWarning();
        }
    }
}

}
