//
// Project: IPK project - server side
// Author: Zdeněk Brhel
// Login: xbrhel04@vutbr.cz
//

#include "parameters_s.h"

void parameters_s::getParams(int argc, char **argv)
{
    if(argc != 3)
    {
        fprintf(stderr, "invalid number of params\n");
        exit(-1);
    }
    std::string str(argv[1]);
    if(str != "-p")
    {
        printf("invalid parameter\n");
        exit(-1);
    }
    port = atoi(argv[2]);
    if(port == 0)
    {
        std::cerr << "invalid parameters\n";
        exit(-1);
    }
}