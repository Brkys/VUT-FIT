//
// Created by brky on 12.3.18.
//

#include <stdint-gcc.h>
#include "client_side.h"

struct hostent * client_side::inicialize_host(parameters_c params, client_side myclient)
{
    struct hostent *tmp;
    if((tmp = gethostbyname(params.host)) == NULL)
    {
        std::cerr << "ERROR no such host\n";
    }
    return tmp;
}

client_side client_side::inicialize_adress(parameters_c params, client_side myclient)
{
    bzero((char *) &myclient.adress, sizeof(myclient.adress));
    myclient.adress.sin_family = AF_INET;
    myclient.adress.sin_port = htons((uint16_t)params.port);
    bcopy((char *) myclient.host->h_addr, (char *) &myclient.adress.sin_addr.s_addr, myclient.host->h_length);
    return myclient;
}

int client_side::client_connect(client_side myclient)
{
    if(connect(this->csocket, (struct sockaddr*) &myclient.adress, sizeof(myclient.adress)) < 0)
    {
        std::cerr << "ERROR connect\n" + EXIT_FAILURE << errno;
        return -1;
    }
    return 0;
}