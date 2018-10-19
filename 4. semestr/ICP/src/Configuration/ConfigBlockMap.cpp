/**
 * @author Simeon Borko
 * Reprezentuje mnozinu vsetkych pouzitelnych blokov.
*/

#include "ConfigBlockMap.h"

namespace Configuration {

    ConfigBlockMap::ConfigBlockMap(ConfigTypeMap &typeMap) : typeMap(typeMap) {}

    ConfigBlockMap::~ConfigBlockMap() {
        for (auto it : blockMap) {
            delete it.second;
        }
    }

    ConfigBlock& ConfigBlockMap::create(string name) {
        ConfigBlock *blockPtr = new ConfigBlock(name, typeMap);
        if (!blockMap.insert(make_pair(name, blockPtr)).second) {
            delete blockPtr;
            throw exception();
        }
        return *blockPtr;
    }

    ConfigBlock& ConfigBlockMap::get(string name) {
        auto it = blockMap.find(name);
        if (it == blockMap.end()) {
            throw exception();
        }
        return *it->second;
    }

    vector<string> ConfigBlockMap::names()
    {
        vector<string> v;
        for (auto it : blockMap) {
            v.push_back(it.first);
        }
        return v;
    }

}
