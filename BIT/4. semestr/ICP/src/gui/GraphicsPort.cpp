/**
 * @author Zdeněk Brhel
 * Reprezentuje vizualizovaný port
 */

#include "GraphicsPort.h"
#include "GraphicsBlock.h"
#include "mainwindow.h"
#include "GraphicsConnect.h"

namespace Gui {

const int GraphicsPort::Space = 2;
const int GraphicsPort::Size = 10;
const int GraphicsPort::MaxPorts = 5;

GraphicsPort::GraphicsPort(GraphicsBlock *parent, ModelPort &modelPort, QGraphicsScene &scene,
               ModelState &mState, int Order, int NumberOfPorts)
    : QGraphicsEllipseItem(parent), modelPort(modelPort), scene(scene), mState(mState)
{
    setFlag(ItemIsSelectable);
    setAcceptHoverEvents(true);

    Active = false;
    Connection = nullptr;
    Hovering = false;

    basicInfo = "Název portu: ";
    basicInfo += QString::fromStdString(modelPort.getName());
    basicInfo += "\n";
    basicInfo += "Typ portu: ";
    basicInfo += QString::fromStdString(modelPort.getTypeName());
    basicInfo += "\n";
    basicInfo += "Proměnné:";

    setPos(modelPort.isInput() ? -Size : GraphicsBlock::Width,
           Order * (Size+Space) + (MaxPorts-NumberOfPorts) * ((Size+Space)/2));
    setRect(0, 0, Size, Size);

    connect(&modelPort, SIGNAL(varValueSet()), this, SLOT(varValueChanged()));
}

GraphicsPort::~GraphicsPort()
{
    if (Connection != nullptr) {
        delete Connection;
    }
}

int GraphicsPort::type() const
{
    return Type;
}

void GraphicsPort::varValueChanged()
{
    if (modelPort.hasValues()) {
        if (modelPort.isFree()) {
            QStringList values;
            for (auto it : modelPort.getVarMap()) {
                values << QString::number(it.second);
            }
            QGraphicsSimpleTextItem *textItem = new QGraphicsSimpleTextItem(
                        values.join(", "), this);
            QRectF r = textItem->boundingRect();
            qreal x = modelPort.isInput() ? - r.width() - 2 : Size + 2;
            qreal y = - (r.height() - Size) / 2;
            textItem->setPos(x, y);
        }
    } else {
        for (auto child : childItems()) {
            delete child;
        }
    }
}

QString GraphicsPort::getInfo()
{
    if (modelPort.hasValues()) {
        QStringList varInfoList;
        for (auto it : modelPort.getVarMap()) {
            QString varInfo = "\t";
            varInfo += QString::fromStdString(it.first);
            varInfo += " = ";
            varInfo += QString::number(it.second);
            varInfoList << varInfo;
        }
        return basicInfo + "\n" + varInfoList.join("\n");
    } else {
        QStringList varNamesList;
        for (auto it : modelPort.getVarNames()) {
            varNamesList << QString::fromStdString(it);
        }
        return basicInfo + " " + varNamesList.join(", ");
    }
}

int GraphicsPort::getBlockId()
{
    return (dynamic_cast<GraphicsBlock*>(this->parentItem()))->modelBlock.getId();
}

string GraphicsPort::getBlockName()
{
    return (dynamic_cast<GraphicsBlock*>(this->parentItem()))->modelBlock.getName();
}

string GraphicsPort::getPortName()
{
    return this->modelPort.getName();
}

void GraphicsPort::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    (void) option;
    (void) widget;

    QBrush brush;

    if (Active) {
        brush = QBrush(Colors::PortActive);
    } else if (Connection != nullptr) {
        brush = QBrush(Colors::PortConnected);
    } else if (Hovering) {
        brush = QBrush(Colors::PortHover);
    } else {
        brush = QBrush(Colors::Port);
    }

    painter->setBrush(brush);
    painter->drawEllipse(rect());

}

void GraphicsPort::showWarning(std::string message) {
    QMessageBox::warning(dynamic_cast<MainWindow *>(scene.parent()), QString("Upozornění"), QString::fromStdString(message), QMessageBox::Ok);
}

void GraphicsPort::connectPorts(GraphicsPort *other)
{
    if (mState != ModelState::SCHEME) {
        showWarning();
        return;
    }
    if (this == other) {
        return;
    }
    if (this->modelPort.areConnectable(other->modelPort) && this->parentItem() != other->parentItem()) {
        this->modelPort.connect(other->modelPort);
        Connection = new GraphicsConnect(*this, *other, scene, mState);
        other->Connection = Connection;
        scene.addItem(Connection);
    } else if (other->modelPort.areConnectable(this->modelPort)) {
        other->connectPorts(this);
    } else {

        if (this->modelPort.isConnected() || other->modelPort.isConnected()) {
            showWarning("Port je obsazen");
        } else if (this->modelPort.getInOut() == other->modelPort.getInOut()) {
            showWarning("Nesprávná orientace portů");
        } else if (this->parentItem() == other->parentItem()) {
            showWarning("Porty patří stejnému bloku");
        } else {
            showWarning("Různý typ portů");
        }
    }
}

bool GraphicsPort::setValuesDialog()
{
    if (mState != ModelState::INITIALIZING) {
        throw exception();
    }

    std::vector<string> varNames = modelPort.emptyInputVarNames();

    bool ok = true;

    if (varNames.size() > 0) {
        QWidget *w = dynamic_cast<QWidget *>(this->scene.parent());
        std::string title = getBlockName() + " - " + getPortName();

        // barvicky
        Active = true;
        update();


        for (std::string varName : varNames) {
            std::stringstream ss;
            ss << "Název bloku:\t" << getBlockName() << "\n";
            ss << "Název portu:\t" << getPortName() << "\n";
            ss << "Proměnná:\t" << varName;
            double val = QInputDialog::getDouble(w, QString::fromStdString(title), QString::fromStdString(ss.str()), 0, -2147483647, 2147483647, 2, &ok);

            if (!ok) {
                break;
            }

            modelPort.setValue(varName, val);
        }

        Active = false;
        update();
    }
    return ok;
}

void GraphicsPort::disconnectPorts()
{
    if (modelPort.isConnected()) {
        modelPort.disconnect();
    }
    Connection->InPort.Connection = nullptr;
    Connection = nullptr;
}

bool GraphicsPort::sceneEvent(QEvent *event)
{

    if(event->type() == QEvent::GraphicsSceneHoverEnter || event->type() == QEvent::GraphicsSceneHoverMove)
    {
        Hovering = true;
        update();
        setToolTip(getInfo());
    }
    else if (event->type() == QEvent::GraphicsSceneHoverLeave)
    {
        Hovering = false;
        update();
    }
    return QGraphicsItem::sceneEvent(event);
}

void GraphicsPort::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    if (mState == ModelState::SCHEME) {
        Active = true;
        update();
    } else {
        showWarning();
    }
    QGraphicsItem::mousePressEvent(event);
}

void GraphicsPort::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    if (mState == ModelState::SCHEME) {
        Active = false;
        update();

        QGraphicsItem *item = scene.itemAt(mapToScene(event->pos()), QTransform());
        if(item != 0 && (this->type() == item->type()))
        {
            this->connectPorts(dynamic_cast<GraphicsPort*>(item));
        }
    } else {
        showWarning();
    }

    QGraphicsItem::mouseReleaseEvent(event);
}

}
