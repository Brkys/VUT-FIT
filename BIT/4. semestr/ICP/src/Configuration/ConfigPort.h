/**
 * @author Simeon Borko
 * @brief Reprezentuje port bloku (na urovni konfiguracie).
*/

#ifndef SRC_CONFIGURATION_PORT_H
#define SRC_CONFIGURATION_PORT_H

#include "ConfigType.h"

#include <iostream>

using namespace std;

namespace Configuration {
    class ConfigPort {
        char inOut;
        string name;
        ConfigType& type;

    public:

        /**
         * Konstruktor Port.
         * @param inOut 'i' pre input port, 'o' pre output port
         * @param name nazov portu
         * @param type typ portu
         */
        explicit ConfigPort(char inOut, string name, ConfigType& type);

        /**
         * Rozhodne, ci tento a zadany port su kompatibilne,
         * cize ci tento je vystupny a zadany vstupny a maju rovnaky typ.
         * @param outPort
         * @return je tento port kompatibilny so zadanym?
         */
        bool areCompatible(ConfigPort &outPort);

        bool isInput();

        bool isOutput();

        char getInOut();

        string getName();

        vector<string> getVarNames();

        string getTypeName();

    };
}


#endif //SRC_CONFIGURATION_PORT_H
