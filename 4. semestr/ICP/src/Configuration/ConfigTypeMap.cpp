/**
 * @author Simeon Borko
 * Reprezentuje mnozinu typov (vsetky pouzitelne typy).
*/

#include "ConfigTypeMap.h"

namespace Configuration {

    ConfigTypeMap::~ConfigTypeMap() {
        for (auto it : typeMap) {
            delete it.second;
        }
    }

    ConfigType& ConfigTypeMap::create(string typeName, vector<string> varNames) {
        // vytvorit typ
        ConfigType *typePtr = new ConfigType(typeName);

        // pridat nazvy premennych
        for (string varName : varNames) {
            typePtr->addVarName(varName);
        }

        // overit duplicitu podla mnoziny premennych
        for (auto item : typeMap) {
            if (*item.second == *typePtr) {
                delete typePtr;
                throw exception();
            }
        }

        // overit unikatnost nazvu a vlozit
        if (!typeMap.insert(make_pair(typeName, typePtr)).second) {
            // mazat typePtr nie je potrebne, pretoze uz je v mape
            throw exception();
        }

        return *typePtr;
    }

    ConfigType& ConfigTypeMap::get(string name) {
        auto search = typeMap.find(name);
        if (search == typeMap.end()) {
            // ak sa nenasiel typ s danym nazvom
            throw exception();
        }
        return *search->second;
    }
}