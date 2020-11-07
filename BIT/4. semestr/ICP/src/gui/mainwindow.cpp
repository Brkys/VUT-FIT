/**
 * @author Zdeněk Brhel, Simeon Borko
 * Reprezentuje hlavní okno programu
 */


#include "mainwindow.h"
#include "ui_mainwindow.h"

using namespace Configuration;
using namespace Model;

namespace Gui {

QString MainWindow::fileNameFilter = "ICP Soubor (*.icp);;Všechny soubory (*)";

MainWindow::MainWindow(ModelState &mState, ModelBlockArray &blockArray, QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow), mState(mState), blockArray(blockArray)
{
    ui->setupUi(this);

    // inicializace scény
    scene = new QGraphicsScene(this);
    ui->graphicsView->setScene(scene);

    connect(&mState, SIGNAL(stateChanged()), this, SLOT(updateButtons()));

    computation = new Computation(this, mState, blockArray, modelIdToGBlock);
}

MainWindow::~MainWindow()
{
    mState.setState(ModelState::SCHEME, true); // pri ruseni objektov treba mat stav SCHEME
    scene->clear();
    delete ui;
}

void MainWindow::showWarning(std::string message) {
    QMessageBox::warning(this, QString("Upozornění"), QString::fromStdString(message), QMessageBox::Ok);
}

void MainWindow::show()
{
    //přidá do listwidgetu všechny bloky
    for(string name : blockArray.blockNames())
    {
        QListWidgetItem *wi = new QListWidgetItem(name.c_str());
        wi->setTextAlignment(Qt::AlignCenter);
        ui->blockList->addItem(wi);
    }

    QMainWindow::show();
}

GraphicsBlock* MainWindow::addBlock(string blockName)
{
    Model::ModelBlock &modelBlock = blockArray.create(blockName);
    GraphicsBlock *block = new GraphicsBlock(modelBlock, *scene, mState);
    scene->addItem(block);
    modelIdToGBlock[modelBlock.getId()] = block;
    return block;
}

GraphicsPort* MainWindow::getPort(int blockId, string portName)
{
    GraphicsBlock *block = modelIdToGBlock[blockId];
    return block->Ports[portName];
}

/* Sloty */

void MainWindow::on_blockList_doubleClicked(const QModelIndex &index)
{
    (void) index;
    if (mState == ModelState::SCHEME) {
        std::string name = ui->blockList->currentItem()->text().toUtf8().constData();
        addBlock(name);
    } else {
        showWarning();
    }
}

void MainWindow::on_actionSave_triggered()
{
    if (mState != ModelState::SCHEME) {
        showWarning();
        return;
    }

    QFileDialog dialog(this, "Uložit soubor");
    dialog.setAcceptMode(QFileDialog::AcceptSave);
    dialog.setFileMode(QFileDialog::AnyFile);
    dialog.setNameFilter(fileNameFilter);
    dialog.setViewMode(QFileDialog::Detail);
    dialog.setDefaultSuffix("icp");
    if (!dialog.exec()) {
        return;
    }

    QString filename = dialog.selectedFiles()[0];

    QVariantList list;
    for(auto *item : scene->items())
    {
        QVariantHash hash;
        if(item->type() == GraphicsBlock::Type)
        {
            GraphicsBlock *block = dynamic_cast<GraphicsBlock*>(item);
            hash["type"] = block->type();
            hash["position"] = block->pos();
            hash["blockName"] = QString::fromStdString(block->modelBlock.getName());
            hash["blockId"] = block->modelBlock.getId();
            list << hash;
        }
        else if(item->type() == GraphicsConnect::Type)
        {
            GraphicsConnect *connection = dynamic_cast<GraphicsConnect*>(item);
            hash["type"] = connection->type();
            hash["outBlockId"] = connection->OutPort.getBlockId();
            hash["outPortName"] = QString::fromStdString(connection->OutPort.getPortName());
            hash["inBlockId"] = connection->InPort.getBlockId();
            hash["inPortName"] = QString::fromStdString(connection->InPort.getPortName());
            list << hash;
        }
    }

    QFile file(filename);
    file.open(QIODevice::WriteOnly);
    QDataStream out(&file);
    out << list;
    file.flush();
    file.close();

    QMessageBox::information(this, "Uloženo", "Vaše schéma bylo úspěšně uloženo", QMessageBox::Ok);
}

void MainWindow::on_actionLoad_triggered()
{
    if (mState != ModelState::SCHEME) {
        showWarning();
        return;
    }

    if(!scene->items().isEmpty())
    {
        QMessageBox::StandardButton res = QMessageBox::question(this,"Upozornění",
                                                                "Chcete načíst schéma bez uložení stávajícího?",
                                                                QMessageBox::Yes|QMessageBox::No);
        if(res != QMessageBox::Yes)
            return;

        scene->clear();
    }

    QFileDialog dialog(this, "Načíst soubor");
    dialog.setFileMode(QFileDialog::ExistingFile);
    dialog.setNameFilter(fileNameFilter);
    dialog.setViewMode(QFileDialog::Detail);
    if (!dialog.exec()) {
        return;
    }

    QFile file(dialog.selectedFiles()[0]);
    file.open(QIODevice::ReadOnly);
    QDataStream in(&file);
    QVariantList list;
    std::map<int, int> idMap;
    in >> list;

    for(QVariant variant : list)
    {
        QVariantHash data = variant.toHash();
        if(data["type"].toInt() == GraphicsBlock::Type)
        {
            GraphicsBlock *block = addBlock(data["blockName"].toString().toUtf8().constData());
            idMap[data["blockId"].toInt()] = block->modelBlock.getId();
            block->setPos(data["position"].toPointF());
        }
    }

    for(QVariant variant : list)
    {
        QVariantHash data = variant.toHash();
        if(data["type"].toInt() == GraphicsConnect::Type)
        {
            GraphicsPort *outPort = getPort(idMap[data["outBlockId"].toInt()], data["outPortName"].toString().toUtf8().constData());
            GraphicsPort *inPort = getPort(idMap[data["inBlockId"].toInt()], data["inPortName"].toString().toUtf8().constData());
            outPort->connectPorts(inPort);
        }
    }

    file.close();
}

void MainWindow::on_actionNew_triggered()
{
    if (mState != ModelState::SCHEME) {
        showWarning();
        return;
    }
    if(!scene->items().isEmpty())
    {
        QMessageBox::StandardButton res = QMessageBox::question(this,"Upozornění",
                                                                "Chcete vytvořit nové schéma bez uložení stávajícího?",
                                                                QMessageBox::Yes|QMessageBox::No);
        if(res == QMessageBox::Yes)
            scene->clear();
        else
            return;
    }
}

void MainWindow::on_actionHelp_triggered()
{
    QMessageBox::information(this, "Nápověda",
                             "Program slouží na vytváření a výpočet blokových schémat. "
                             "Blok se do programu dá přidat dvojtým kliknutím na jeho jméno, "
                             "Vazba mezi porty se dá přidat kliknutím a následným přetáhnutím "
                             "z portu na jiný (kompatibilní) port.\n\n"
                             "Název, typ a proměnné portu se dá zjistit najetím kurzotu na daný port "
                             "Informace o propojení portů pak najetím kurzoru na propoj.\n\n"
                             "Při spuštění/krokování výpočtu program přejde z návrhového stavu "
                             "do výpočetního, a proto všechny operace měnící schéma jsou zakázány.\n\n"
                             "Zkratky:\n"
                             "CTRL+N - nové schéma\n"
                             "CTRL+S - uložení schématu\n"
                             "CTRL+O - otevření schématu\n"
                             "CTRL+R - spuštění krokování\n"
                             "F5 - spuštění krokování\n"
                             "F1 - tato nápověda");
}

void MainWindow::on_actionAuthors_triggered()
{
    QMessageBox::information(this, "Autoři",
                             "Zdeněk Brhel - xbrhel04@stud.fit.vutbr.cz\n"
                             "Simeon Borko - xborko02@stud.fit.vutbr.cz");
}

void MainWindow::on_controlEndButton_clicked()
{
    switch (mState.getState()) {
    case ModelState::DEBUG:
    case ModelState::FINISH:
        mState.setState(ModelState::SCHEME);
        break;

    default:
        showWarning();
    }
}

void MainWindow::on_controlNextStepButton_clicked()
{
    computation->step();
}

void MainWindow::on_actionRun_triggered()
{
    computation->run();
}

void MainWindow::on_actionDebug_triggered()
{
    computation->debug();
}

void MainWindow::on_controlContinueButton_clicked()
{
    computation->continueRun();
}

void MainWindow::updateButtons()
{
    switch (mState.getState()) {
    case ModelState::SCHEME:
        ui->controlRunButton->setEnabled(true);
        ui->controlDebugButton->setEnabled(true);
        ui->controlEndButton->setEnabled(false);
        ui->controlContinueButton->setEnabled(false);
        ui->controlNextStepButton->setEnabled(false);
        break;

    case ModelState::DEBUG:
        ui->controlRunButton->setEnabled(false);
        ui->controlDebugButton->setEnabled(false);
        ui->controlEndButton->setEnabled(true);
        ui->controlContinueButton->setEnabled(true);
        ui->controlNextStepButton->setEnabled(true);
        break;

    case ModelState::FINISH:
        ui->controlRunButton->setEnabled(false);
        ui->controlDebugButton->setEnabled(false);
        ui->controlEndButton->setEnabled(true);
        ui->controlContinueButton->setEnabled(false);
        ui->controlNextStepButton->setEnabled(false);
        break;

    default:
        ui->controlRunButton->setEnabled(false);
        ui->controlDebugButton->setEnabled(false);
        ui->controlEndButton->setEnabled(false);
        ui->controlContinueButton->setEnabled(false);
        ui->controlNextStepButton->setEnabled(false);
    }
}

}
