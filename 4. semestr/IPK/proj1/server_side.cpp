//
// Project: IPK project - server side
// Author: Zdeněk Brhel
// Login: xbrhel04@vutbr.cz
//

#include "server_side.h"


int server_side::initSocket()
{
    int i  = socket(AF_INET, SOCK_STREAM, 0);
    return (i < 0) ? -1 : i;
}

server_side server_side::inicialize_adress(parameters_s params, server_side tmp)
{
    bzero((char *) &tmp.address, sizeof(tmp.address));
    tmp.address.sin_port = htons(params.port);
    tmp.address.sin_addr.s_addr = INADDR_ANY;
    tmp.address.sin_family = AF_INET;
    return tmp;
}

int server_side::bindSocket(server_side tmp, int sock)
{
    if(bind(sock, (struct sockaddr *) &tmp.address, sizeof(tmp.address)) < 0)
    {
        std::cerr << "ERROR on binding\n";
        return -1;
    }
    return 0;
}

server_side server_side::parseInfo(server_side tmp, char *buffer)
{
    for(int i = 0; i < strlen(buffer); i++)
    {
        if(i == 0)
            tmp.extension = buffer[i];
        else
            tmp.user[i-1] = buffer[i];

    }
    return tmp;
}