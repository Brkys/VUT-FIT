/**
 * Zodpoveda za vypocet.
 * @author Simeon Borko
 */

#include "Computation.h"
#include "mainwindow.h"

namespace Gui {

Computation::Computation(QObject *parent, ModelState &mState, ModelBlockArray &blockArray, map<int, GraphicsBlock *> &modelIdToGBlock)
    : QObject(parent), mState(mState), blockArray(blockArray), modelIdToGBlock(modelIdToGBlock)
{
    connect(this, SIGNAL(next()), &blockArray, SLOT(runNextBlock()));
    connect(&blockArray, &ModelBlockArray::runResult, this, &Computation::result);
}

void Computation::run()
{
    if (initialize()) {
        mState.setState(ModelState::RUN);
        emit next();
    }
}

void Computation::debug()
{
    if (initialize()) {
        mState.setState(ModelState::DEBUG);
    }
}

void Computation::step()
{
    mState.setState(ModelState::STEP);
    emit next();
}

void Computation::continueRun()
{
    mState.setState(ModelState::RUN);
    emit next();
}

void Computation::result(bool canRunNext)
{
    if (canRunNext) {

        switch (mState.getState()) {

        case ModelState::RUN:
            emit next();
            break;

        case ModelState::STEP:
            mState.setState(ModelState::DEBUG);
            break;

        default:
            throw exception();

        }

    } else {
        mState.setState(ModelState::FINISH);
    }
}

void Computation::showWarning()
{
    qobject_cast<MainWindow*>(parent())->showWarning();
}

void Computation::showWarning(std::string message)
{
    qobject_cast<MainWindow*>(parent())->showWarning(message);
}

bool Computation::askForValues()
{
    // pre kazdy blok

    for (ModelBlock *mBlock : blockArray.emptyInputBlocks()) {

        // pre kazdy port
        for (auto it : modelIdToGBlock[mBlock->getId()]->Ports) {

            // nastavit hodnoty
            if (!it.second->setValuesDialog()) {

                // ak uzivatel odmietol zadat hodnotu
                return false;
            }
        }
    }

    return true;
}

bool Computation::initialize()
{
    if (mState != ModelState::SCHEME) {
        return false;
    }

    if (blockArray.size() == 0) {
        showWarning("V schéme musí být alespoň jeden blok");
        return false;
    }

    mState.setState(ModelState::INITIALIZING);

    if (blockArray.hasCycles()) {
        showWarning("Schéma obsahuje cykly");
        mState.setState(ModelState::SCHEME);
        return false;
    }

    if (!askForValues()) {
        mState.setState(ModelState::SCHEME);
        return false;
    }

    mState.setState(ModelState::INITIALIZED);
    return true;
}

}
