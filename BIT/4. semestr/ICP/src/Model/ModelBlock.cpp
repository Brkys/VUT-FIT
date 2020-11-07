/**
 * @author Simeon Borko
 * Reprezentuje vytvoreny blok, s ktorym sa da pocitat.
 */

#include "ModelBlock.h"
#include "ModelBlockArray.h"

namespace Model {

    unsigned ModelBlock::sleepTime = 1000;

    bool ModelBlock::hasCycles(set<int> &blockIdSet) {
        if (!blockIdSet.insert(id).second) {
            return true;
        }

        for (ModelBlock *nextBlock : getNextBlocks()) {
            if (nextBlock->hasCycles(blockIdSet)) {
                return true;
            }
        }

        return false;
    }

    void ModelBlock::fnRgbToGray() {
        ModelPort &p = getPort("rgb"); // I = 0.299R + 0.587G + 0.114B
        getPort("gray").setValue(
                "number",
                0.299*p.getValue("red") + 0.587*p.getValue("green") + 0.114*p.getValue("blue")
        );
    }

    void ModelBlock::fnAdd() {
        getPort("c").setValue(
                "number",
                getPort("a").getValue("number") + getPort("b").getValue("number")
        );
    }

    void ModelBlock::fnFlip() {
        getPort("b").setValue(
                "number",
                - getPort("a").getValue("number")
        );
    }

    void ModelBlock::fnMultiply() {
        getPort("c").setValue(
                "number",
                getPort("a").getValue("number") * getPort("b").getValue("number")
        );
    }

    void ModelBlock::fnMedian5() {
        vector<double> numbers {
                getPort("a").getValue("number"),
                getPort("b").getValue("number"),
                getPort("c").getValue("number"),
                getPort("d").getValue("number"),
                getPort("e").getValue("number"),
        };
        sort(numbers.begin(), numbers.end());
        getPort("median").setValue("number", numbers[2]);
    }

    bool ModelBlock::doubleToBool(double n) {
        return n != 0.0;
    }

    double ModelBlock::boolToDouble(bool b) {
        return b ? 1.0 : 0.0;
    }

    void ModelBlock::fnAnd() {
        getPort("c").setValue("bit", boolToDouble(
                doubleToBool(getPort("a").getValue("bit")) && doubleToBool(getPort("b").getValue("bit"))
        ));
    }

    void ModelBlock::fnOr() {
        getPort("c").setValue("bit", boolToDouble(
                doubleToBool(getPort("a").getValue("bit")) || doubleToBool(getPort("b").getValue("bit"))
        ));
    }

    void ModelBlock::fnXor() {
        getPort("c").setValue("bit", boolToDouble(
                doubleToBool(getPort("a").getValue("bit")) != doubleToBool(getPort("b").getValue("bit"))
        ));
    }

    void ModelBlock::fnNot() {
        getPort("b").setValue("bit", boolToDouble(
                !doubleToBool(getPort("a").getValue("bit"))
        ));
    }


    ModelBlock::ModelBlock(int id, Configuration::ConfigBlock &cBlock, ModelBlockArray &blockArray, ModelState &mState)
        : cBlock(cBlock), blockArray(blockArray), mState(mState) {
        if (mState != ModelState::SCHEME) {
            throw exception();
        }

        this->id = id;
        for (const auto &it : cBlock.getPortMap()) {
            portMap[it.first] = new ModelPort(*this, *it.second, mState);
        }
    }

    ModelBlock::~ModelBlock() {
        blockArray.removeBlockCallback(this);
    }

    map<string, ModelPort *> ModelBlock::getPortMap() {
        return portMap;
    }

    vector<ModelPort *> ModelBlock::getInputPorts() {
        vector<ModelPort *> ports;
        for (auto it : portMap) {
            if (it.second->isInput()) {
                ports.push_back(it.second);
            }
        }
        return ports;
    }

    vector<ModelPort *> ModelBlock::getOutputPorts() {
        vector<ModelPort *> ports;
        for (auto it : portMap) {
            if (it.second->isOutput()) {
                ports.push_back(it.second);
            }
        }
        return ports;
    }

    ModelPort& ModelBlock::getPort(string portName) {
        auto it = portMap.find(portName);
        if (it == portMap.end()) {
            throw exception();
        }
        return *it->second;
    }

    void ModelBlock::connect(ModelBlock &otherBlock, string thisPortName, string otherPortName) {
        ModelPort &thisPort = this->getPort(thisPortName);
        ModelPort &otherPort = otherBlock.getPort(otherPortName);
        thisPort.connect(otherPort);
    }

    bool ModelBlock::isRoot() {
        for (auto it : portMap) {
            ModelPort *port = it.second;
            if (port->isInput() && port->isFree()) {
                return true;
            }
        }
        return false;
    }

    vector<ModelBlock *> ModelBlock::getNextBlocks() {
        vector<ModelBlock *> nextBlocks;
        for (const auto &it : portMap) {
            ModelBlock *next = it.second->getNextBlock();
            if (next != nullptr) {
                nextBlocks.push_back(next);
            }
        }
        return nextBlocks;
    }

    bool ModelBlock::hasCycles() {
        set<int> blockIdSet;
        return hasCycles(blockIdSet);
    }

    int ModelBlock::getId() {
        return id;
    }

    vector<ModelPort *> ModelBlock::emptyInputPorts() {
        vector<ModelPort *> ports;
        for (auto it : portMap) {
            if (it.second->emptyInputVarNames().size() > 0) {
                ports.push_back(it.second);
            }
        }
        return ports;
    }

    bool ModelBlock::runnable() {
        for (auto it : portMap) {
            if (!it.second->runnable()) {
                return false;
            }
        }
        return true;
    }

    void ModelBlock::run() {
        if (!runnable() || (mState != ModelState::RUN && mState != ModelState::STEP)) {
            throw exception();
        }

        emit runStarted();

        QThread::msleep(sleepTime);

        string name = cBlock.getName();

        if (name == "rgbToGray") {
            fnRgbToGray();
        } else if (name == "add") {
            fnAdd();
        } else if (name == "flip") {
            fnFlip();
        } else if (name == "multiply") {
            fnMultiply();
        } else if (name == "median5") {
            fnMedian5();
        } else if (name == "and") {
            fnAnd();
        } else if (name == "or") {
            fnOr();
        } else if (name == "xor") {
            fnXor();
        } else if (name == "not") {
            fnNot();
        } else {
            throw exception();
        }

        emit runFinished();
    }

    string ModelBlock::getName() {
        return cBlock.getName();
    }

    void ModelBlock::resetValues()
    {
        for (auto it : portMap) {
            it.second->resetValues();
        }
        emit reseted();
    }

    void ModelBlock::removePortFromMap(QObject *obj)
    {
        portMap.erase(portMap.find(qobject_cast<ModelPort *>(obj)->getName()));
    }


}
