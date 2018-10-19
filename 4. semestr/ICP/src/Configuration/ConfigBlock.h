/**
 * @author Simeon Borko
 * @brief Reprezentuje blok (na urovni konfiguracie).
*/

#ifndef SRC_CONFIGURATION_BLOCK_H
#define SRC_CONFIGURATION_BLOCK_H

#include <iostream>
#include <map>

#include "ConfigPort.h"
#include "ConfigTypeMap.h"

using namespace std;

namespace Configuration {

    class ConfigBlock {

        string name;
        map<string, ConfigPort *> portMap;
        ConfigTypeMap &typeMap;

        /**
         * Prida port do bloku.
         * @param inOut i / o
         * @param name jedinecny nazov portu v ramci bloku
         * @param type typ portu
         */
        void addPort(char inOut, string &name, string &type);

    public:

        explicit ConfigBlock(string name, ConfigTypeMap &typeMap);

        ~ConfigBlock();

        /**
         * Prida vstupny port.
         * @param name nazov portu
         * @param type typ portu
         */
        void addInPort(string name, string type);

        /**
         * Prida vystupny port.
         * @param name nazov portu
         * @param type typ portu
         */
        void addOutPort(string name, string type);

        map<string, ConfigPort *> getPortMap();

        ConfigPort& getPort(string name);

        string getName();
    };

}


#endif //SRC_CONFIGURATION_BLOCK_H
