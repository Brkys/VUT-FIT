/**
 * @author Simeon Borko
 * Reprezentuje port bloku (na urovni konfiguracie).
*/

#include "ConfigPort.h"

namespace Configuration {

    ConfigPort::ConfigPort(char inOut, string name, ConfigType& type) : type(type) {
        if (inOut != 'i' && inOut != 'o') {
            throw exception();
        }
        this->inOut = inOut;
        this->name = name;
    }

    bool ConfigPort::areCompatible(ConfigPort &outPort) {
        return this->isOutput() && outPort.isInput() && this->type == outPort.type;
    }

    bool ConfigPort::isInput() {
        return inOut == 'i';
    }

    bool ConfigPort::isOutput() {
        return inOut == 'o';
    }

    char ConfigPort::getInOut() {
        return inOut;
    }

    string ConfigPort::getName() {
        return name;
    }

    vector<string> ConfigPort::getVarNames() {
        return type.getVarNames();
    }

    string ConfigPort::getTypeName() {
        return type.getName();
    }

}
