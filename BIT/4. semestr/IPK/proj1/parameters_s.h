//
// Project: IPK project - server side
// Author: Zdeněk Brhel
// Login: xbrhel04@vutbr.cz
//

#ifndef CLION_PARAMETERS_S_H
#define CLION_PARAMETERS_S_H

#include "ipklib.h"

/**
 *  Trida pro zpracovani a uchovani serveru
 */
class parameters_s {
public:
    int port = 0;

/**
 * Funkce zpracuje parametry
 * @param argc Pocet argumentu
 * @param argv Pole argumentu
 */
    void getParams(int argc, char **argv);
};

#endif //CLION_PARAMETERS_S_H
