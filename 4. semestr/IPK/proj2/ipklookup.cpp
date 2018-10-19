/*****************************************
 *  @file ipklib.h
 *  @author Zdeněk Brhel
 *  login: xbrhel04
 *
 *  @date 2018-04-08
 *****************************************/

#include "ipklib.h"

int main(int argc, char **argv) {
    params_t parameters = getParams(argc, argv);
    int result;
    if((result = lookUp(parameters)) != 0)
        return result;
    return 0;
}
