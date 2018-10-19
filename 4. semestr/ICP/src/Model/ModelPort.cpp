/**
 * @author Simeon Borko
 * Reprezentuje port vytvoreneho bloku.
 */

#include "ModelPort.h"
#include "ModelBlock.h"

namespace Model {

    map<string, double>::iterator ModelPort::getVar(string &varName) {
        auto var = varMap.find(varName);
        if (var == varMap.end()) {
            throw exception();
        }
        return var;
    }

    ModelBlock* ModelPort::getConnectedBlock() {
        return connected != nullptr ? &(connected->block) : nullptr;
    }

    ModelPort::ModelPort(ModelBlock &block, Configuration::ConfigPort &cPort, ModelState &mState)
        : QObject(&block), block(block), cPort(cPort), mState(mState) {

        if (mState != ModelState::SCHEME) {
            throw exception();
        }

        varNames = cPort.getVarNames();
        resetValues();
    }

    ModelPort::~ModelPort() {
        if (isConnected()) {
            disconnect();
        }
    }

    bool ModelPort::hasValues()
    {
        for (auto it : varMap) {
            if (isnan(it.second)) {
                return false;
            }
        }
        return true;
    }

    double ModelPort::getValue(string varName) {
        return getVar(varName)->second;
    }

    void ModelPort::setValue(string varName, double value) {
        getVar(varName)->second = value;
        if (isOutput() && isConnected()) {
            connected->setValue(varName, value);
        }
        emit varValueSet();
    }

    bool ModelPort::areConnectable(ModelPort &inPort) {
        return this->cPort.areCompatible(inPort.cPort) && this->isFree() && inPort.isFree();
    }

    void ModelPort::connect(ModelPort &inPort) {
        if (!areConnectable(inPort) || mState != ModelState::SCHEME) {
            throw exception();
        }
        this->connected = &inPort;
        inPort.connected = this;
    }

    void ModelPort::disconnect() {
        if (!isConnected() || mState != ModelState::SCHEME) {
            throw exception();
        }
        connected->connected = nullptr;
        connected = nullptr;
    }

    ModelBlock* ModelPort::getNextBlock() {
        return isOutput() ? getConnectedBlock() : nullptr;
    }

    vector<string> ModelPort::getVarNames() {
        return varNames;
    }

    map<string, double> ModelPort::getVarMap() {
        return varMap;
    }

    bool ModelPort::isInput() {
        return cPort.isInput();
    }

    bool ModelPort::isOutput() {
        return cPort.isOutput();
    }

    char ModelPort::getInOut() {
        return cPort.getInOut();
    }

    bool ModelPort::isFree() {
        return connected == nullptr;
    }

    bool ModelPort::isConnected() {
        return connected != nullptr;
    }

    string ModelPort::getName() {
        return cPort.getName();
    }

    vector<string> ModelPort::emptyInputVarNames() {
        vector<string> names;
        if (isInput() && isFree()) {
            for (auto it : varMap) {
                if (isnan(it.second)) {
                    names.push_back(it.first);
                }
            }
        }
        return names;
    }

    bool ModelPort::runnable() {
        if (isInput()) {
            for (auto it : varMap) {
                if (isnan(it.second)) {
                    return false;
                }
            }
        } else if (isOutput()) {
            for (auto it : varMap) {
                if (!isnan(it.second)) {
                    return false;
                }
            }
        } else {
            throw exception();
        }
        return true;
    }

    string ModelPort::getTypeName() {
        return cPort.getTypeName();
    }

    void ModelPort::resetValues()
    {
        for (const string &varName : varNames) {
            varMap[varName] = numeric_limits<double>::quiet_NaN();
            emit varValueSet();
        }
    }

}
