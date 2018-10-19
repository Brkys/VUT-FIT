/**
 * @author Simeon Borko
 * @brief Reprezentuje mnozinu vsetkych pouzitelnych blokov.
*/

#ifndef SRC_CONFIGURATION_BLOCKMAP_H
#define SRC_CONFIGURATION_BLOCKMAP_H

#include <iostream>

#include "ConfigBlock.h"

using namespace std;

namespace Configuration {
    class ConfigBlockMap {
        map<string, ConfigBlock *> blockMap;
        ConfigTypeMap &typeMap;

    public:

        explicit ConfigBlockMap(ConfigTypeMap &typeMap);

        ~ConfigBlockMap();

        /**
         * Vytvori blok. Ak uz existuje blok s rovnakym nazvom, vyhodi vynimku.
         * @param name nazov bloku
         * @param function funkcia bloku
         * @return vytvoreny blok
         */
        ConfigBlock& create(string name);

        /**
         * Ziska blok podla nazvu. Ak blok s takym nazvom neexistuje, vyhodi vynimku.
         * @param name nazov bloku
         * @return blok
         */
        ConfigBlock& get(string name);

        /**
         * Ziska nazvy blokov.
         * @return vektor nazvov blokov
         */
        vector<string> names();
    };

}


#endif //SRC_CONFIGURATION_BLOCKMAP_H
