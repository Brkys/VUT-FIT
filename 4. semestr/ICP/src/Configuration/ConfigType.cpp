/**
 * @author Simeon Borko
 * Reprezentuje typ portu.
 * Typ moze mat viacero premennych urcenych nazvami.
*/

#include "ConfigType.h"

namespace Configuration {

    ConfigType::ConfigType(string name) {
        this->name = name;
    }

    ConfigType::ConfigType(string name, vector<string> varNames) {
        this->name = name;
        for (string varName : varNames) {
            addVarName(varName);
        }
    }

    void ConfigType::addVarName(string varName) {
        if (varNameSet.count(varName)) {
            throw exception();
        }
        varNameSet.insert(varName);
    }

    bool ConfigType::operator==(ConfigType &other) {
        return this == &other || varNameSet == other.varNameSet;
    }

    bool ConfigType::operator!=(ConfigType &other) {
        return !(this->operator==(other));
    }

    string ConfigType::getName() {
        return name;
    }

    vector<string> ConfigType::getVarNames() {
        vector<string> names;
        for (auto s : varNameSet) {
            names.push_back(s);
        }
        sort(names.begin(), names.end());
        return names;
    }

}
