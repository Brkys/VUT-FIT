//
// Created by brky on 12.3.18.
//

#ifndef CLION_CLIENT_SIDE_H
#define CLION_CLIENT_SIDE_H

#include "ipklib.h"

class client_side {
public:
    struct hostent *host;
    struct sockaddr_in adress;
    char buffer[BUFF_SIZE];
    int csocket = socket(AF_INET, SOCK_STREAM, 0);

    struct hostent * inicialize_host(parameters_c params, client_side myclient);

    client_side inicialize_adress(parameters_c params, client_side myclient);

    int client_connect(client_side myclient);

};


#endif //CLION_CLIENT_SIDE_H
