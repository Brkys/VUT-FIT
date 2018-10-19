//
// Project: IPK project - server side
// Author: Zdeněk Brhel
// Login: xbrhel04@vutbr.cz
//

#include "parameters_c.h"

void parameters_c::getParams(int argc, char **argv)
{
    if(argc < 6 || argc > 7)
    {
        fprintf(stderr, "invalid number of params\n");
        exit(-1);
    }
    for (int i = 1; i < argc; ++i)
    {
        std::string str(argv[i]);
        if(str == "-h")
        {
            host = argv[i+1];
        }
        else if(str == "-p")
        {
            port = atoi(argv[i+1]);
        }
        else if(str == "-f" || str == "-l" || str == "-n")
        {
            extension = argv[i];
            if(!strcmp(extension, "-l") && argc == 6)
                continue;
            user = argv[i+1];
        }
    }
    if(extension == NULL || port == 0 || host == NULL)
    {
        std::cerr << "invalid parameters\n";
        exit(-1);
    }
}
