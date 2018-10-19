/**
 * @author Zdeněk Brhel
 * Reprezentuje vizualizovaný blok
 */

#include "GraphicsBlock.h"
#include "mainwindow.h"

namespace Gui {

int GraphicsBlock::Position = 0;

const int GraphicsBlock::Width = 100;
const int GraphicsBlock::Height = 58;

GraphicsBlock::GraphicsBlock(Model::ModelBlock &modelBlock, QGraphicsScene &scene, Model::ModelState &mState)
    : scene(scene), modelBlock(modelBlock), mState(mState)
{
    text = QString::fromStdString(modelBlock.getName());

    Pressed = false;
    Computed = false;
    Run = false;

    setFlag(ItemIsMovable);
    setFlag(ItemSendsScenePositionChanges);
    this->setPos(Position,Position);
    this->setOpacity(0.9);
    Position+=10;

    int counter = 0;
    vector<ModelPort *> ports = modelBlock.getInputPorts();
    for (auto port : ports) {
        Ports[port->getName()] = new GraphicsPort(this, *port, scene, mState, counter++, ports.size());
    }

    counter = 0;
    ports = modelBlock.getOutputPorts();
    for (auto port : ports) {
        Ports[port->getName()] = new GraphicsPort(this, *port, scene, mState, counter++, ports.size());
    }

    QObject::connect(&modelBlock, SIGNAL(runStarted()), this, SLOT(runStarted()));
    QObject::connect(&modelBlock, SIGNAL(runFinished()), this, SLOT(runFinished()));
    QObject::connect(&modelBlock, SIGNAL(reseted()), this, SLOT(reseted()));

    setZValue(1);
}

GraphicsBlock::~GraphicsBlock()
{
    delete &modelBlock;
}

int GraphicsBlock::type() const
{
    return Type;
}

QRectF GraphicsBlock::boundingRect() const
{
    return QRectF(0, 0, Width, Height);
}

void GraphicsBlock::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    (void) option;
    (void) widget;

    QBrush brush;

    if (Pressed) {
        brush = QBrush(Colors::BlockPressed);
    } else if (Run) {
        brush = QBrush(Colors::BlockRun);
    } else if (Computed) {
        brush = QBrush(Colors::BlockComputed);
    } else {
        brush = QBrush(Colors::Block);
    }

    QRectF rect = boundingRect();
    painter->fillRect(rect, brush);
    painter->drawText(rect, Qt::AlignCenter, text.toUpper());
    painter->drawRect(rect);
}


void GraphicsBlock::showWarning(string message)
{
    QMessageBox::warning(dynamic_cast<MainWindow *>(scene.parent()), QString("Upozornění"), QString::fromStdString(message), QMessageBox::Ok);
}


/* sloty */

void GraphicsBlock::runStarted()
{
    Run = true;
    update();
}

void GraphicsBlock::runFinished()
{
    Run = false;
    Computed = true;
    update();
}

void GraphicsBlock::reseted()
{
    Computed = false;
    update();
}


/* eventy */

void GraphicsBlock::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    static int zValue = 1;
    Pressed = true;
    if(event->button() == Qt::RightButton)
    {
        if (mState == ModelState::SCHEME) {
            //zařídí, že se smažou i děti a destruktor dětí zajistí, že se zruší connection
            this->deleteLater();
        } else {
            showWarning();
            Pressed = false;
        }
        return;
    }
    this->setZValue(++zValue);
    QGraphicsItem::mousePressEvent(event);
}

void GraphicsBlock::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    Pressed = false;
    update();
    QGraphicsItem::mouseReleaseEvent(event);
}

}
