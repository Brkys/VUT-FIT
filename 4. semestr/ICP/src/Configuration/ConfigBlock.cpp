/**
 * @author Simeon Borko
 * Reprezentuje blok (na urovni konfiguracie).
*/

#include "ConfigBlock.h"

namespace Configuration {

    void ConfigBlock::addPort(char inOut, string &name, string &type) {
        ConfigPort *portPtr = new ConfigPort(inOut, name, typeMap.get(type));
        if (!portMap.insert(make_pair(name, portPtr)).second) {
            delete portPtr;
            throw exception();
        }
    }

    ConfigBlock::ConfigBlock(string name, ConfigTypeMap &typeMap) : typeMap(typeMap) {
        this->name = name;
    }

    ConfigBlock::~ConfigBlock() {
        for (auto it : portMap) {
            delete it.second;
        }
    }

    void ConfigBlock::addInPort(string name, string type) {
        addPort('i', name, type);
    }

    void ConfigBlock::addOutPort(string name, string type) {
        addPort('o', name, type);
    }

    map<string, ConfigPort *> ConfigBlock::getPortMap() {
        return portMap;
    }

    ConfigPort& ConfigBlock::getPort(string name) {
        auto search = portMap.find(name);
        if (search == portMap.end()) {
            throw exception();
        }
        return *search->second;
    }

    string ConfigBlock::getName() {
        return name;
    }
}