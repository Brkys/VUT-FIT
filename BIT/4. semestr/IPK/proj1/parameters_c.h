//
// Project: IPK project - server side
// Author: Zdeněk Brhel
// Login: xbrhel04@vutbr.cz
//

#ifndef CLION_PARAMETERS_C_H
#define CLION_PARAMETERS_C_H

#include "ipklib.h"

/**
 * Trida slouzi k ulozeni a uchovani parametru
 */
class parameters_c {
public:
    char *host = nullptr, *extension = nullptr, *user = nullptr;
    int port = 0;
/**
 * Funkce zpracuje parametry
 * @param argc Pocet parametru
 * @param argv Pole parametru
 */
    void getParams(int argc, char **argv);

};
#endif //CLION_PARAMETERS_C_H
