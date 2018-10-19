/**
 * @author Simeon Borko
 * Reprezentuje vsetky vytvorene bloky.
 */

#include "ModelBlockArray.h"

namespace Model {

    map<int, ModelBlock *>::iterator ModelBlockArray::getIter(int id) {
        auto search = blockArray.find(id);
        if (search == blockArray.end()) {
            throw exception();
        }
        return search;
    }

    ModelBlockArray::ModelBlockArray(Configuration::ConfigBlockMap &blockMap, ModelState &mState)
        : blockMap(blockMap), mState(mState) {

        idCounter = 0;
        cyclesAlreadyChecked = false;

        // reinicializacia pri zmene stavu na SCHEME
        connect(&mState, SIGNAL(stateChanged()), this, SLOT(reinit()));
    }

    ModelBlockArray::~ModelBlockArray() {
        for (auto it : blockArray) {
            it.second->deleteLater();
        }
    }

    int ModelBlockArray::size() {
        return (int) blockArray.size();
    }

    ModelBlock& ModelBlockArray::create(Configuration::ConfigBlock &cBlock) {
        if (mState != ModelState::SCHEME) {
            throw exception();
        }
        int id = idCounter++;  // najprv priradi do id, potom sa inkrementuje
        ModelBlock *blockPtr = new ModelBlock(id, cBlock, *this, mState);
        blockArray[id] = blockPtr;
        return *blockPtr;
    }

    ModelBlock& ModelBlockArray::create(string blockName) {
        return create(blockMap.get(blockName));
    }

    ModelBlock& ModelBlockArray::get(int id) {
        return *getIter(id)->second;
    }

    void ModelBlockArray::remove(int id) {
        if (mState != ModelState::SCHEME) {
            throw exception();
        }
        auto it = getIter(id);
        delete it->second;
        blockArray.erase(it);
    }

    bool ModelBlockArray::hasCycles() {
        for (auto it : blockArray) {
            if (it.second->hasCycles()) {
                return true;
            }
        }
        cyclesAlreadyChecked = true;
        return false;
    }

    vector<ModelBlock *> ModelBlockArray::emptyInputBlocks() {
        if (mState != ModelState::INITIALIZING || !cyclesAlreadyChecked) {
            throw exception();
        }

        vector<ModelBlock *> blocks;
        for (auto it : blockArray) {
            if (it.second->emptyInputPorts().size() > 0) {
                blocks.push_back(it.second);
            }
        }
        return blocks;
    }

    ModelBlock* ModelBlockArray::runnableBlock() {
        if (!cyclesAlreadyChecked) {
            throw exception();
        }
        for (auto it : blockArray) {
            if (it.second->runnable()) {
                return it.second;
            }
        }
        return nullptr;
    }

    void ModelBlockArray::removeBlockCallback(ModelBlock *block)
    {
        blockArray.erase(blockArray.find(block->getId()));
    }

    vector<string> ModelBlockArray::blockNames()
    {
        return blockMap.names();
    }

    void ModelBlockArray::reinit()
    {
        if (mState == ModelState::SCHEME) {
            for (auto it : blockArray) {
                it.second->resetValues();
            }
            cyclesAlreadyChecked = false;
        }
    }

    void ModelBlockArray::runNextBlock()
    {
        if (mState != ModelState::RUN && mState != ModelState::STEP) {
            throw exception();
        }

        ModelBlock *block = runnableBlock();
        if (block != nullptr) {
            block->run();
        }

        // runnableBlock sa vola znova, cize zistime, ci robime posledny krok
        emit runResult(runnableBlock() != nullptr);
    }

}
