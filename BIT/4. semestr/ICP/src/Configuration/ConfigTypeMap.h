/**
 * @author Simeon Borko
 * @brief Reprezentuje mnozinu typov (vsetky pouzitelne typy).
*/

#ifndef SRC_CONFIGURATION_TYPEMAP_H
#define SRC_CONFIGURATION_TYPEMAP_H

#include <iostream>
#include <map>
#include <vector>

#include "ConfigType.h"

using namespace std;

namespace Configuration {

    class ConfigTypeMap {
        map<string, ConfigType *> typeMap;

    public:

        /**
         * Destruktor zmaze alokovane typy.
         */
        ~ConfigTypeMap();

        /**
         * Vytvori novy typ. Ak taky typ uz existuje, vyhodi vynimku.
         * @param typeName nazov typu
         * @param varNames vektor nazvov premennych
         * @return referencia na novy typ
         */
        ConfigType& create(string typeName, vector<string> varNames);

        /**
         * Ziska typ podla nazvu alebo vyhodi vynimku ak typ neexistuje.
         * @param name nazov typu
         * @return referencia na typ
         */
        ConfigType& get(string name);
    };

}


#endif //SRC_CONFIGURATION_TYPEMAP_H
