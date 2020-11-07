/**
 * @author Simeon Borko
 * @brief Reprezentuje typ portu. Typ moze mat viacero premennych urcenych nazvami.
*/

#ifndef SRC_CONFIGURATION_TYPE_H
#define SRC_CONFIGURATION_TYPE_H

#include <iostream>
#include <set>
#include <map>
#include <vector>
#include <algorithm>

using namespace std;

namespace Configuration {

    class ConfigType {
        string name;
        set<string> varNameSet;

    public:

        explicit ConfigType(string name);

        explicit ConfigType(string name, vector<string> varNames);

        /**
         * Prida novy nazov premennej. Vyhodi vynimku, ak nazov uz existuje.
         * @param varName nazov premennej
         */
        void addVarName(string varName);

        /**
         * Porovnanie typov podla mnoziny premennych.
         * @param other typ na porovnanie
         * @return ci su tieto typy rovnake
         */
        bool operator==(ConfigType &other);

        bool operator!=(ConfigType &other);

        string getName();

        vector<string> getVarNames();
    };

}


#endif //SRC_CONFIGURATION_TYPE_H
