//
// Project: IPK project - server side
// Author: Zdeněk Brhel
// Login: xbrhel04@vutbr.cz
//

#ifndef CLION_SERVER_SIDE_H
#define CLION_SERVER_SIDE_H

#include "ipklib.h"
#include "parameters_s.h"


class server_side {
public:
    char extension;
    char user[BUFF_SIZE - 1];
    struct sockaddr_in address, client_address;
    int welcome_socket;
    int communication_socket;
    char buffer[BUFF_SIZE];
    socklen_t len;

    server_side parseInfo(server_side tmp, char *buffer);

    server_side inicialize_adress(parameters_s params, server_side tmp);

    int bindSocket(server_side tmp, int sock);

    int initSocket();

};


#endif //CLION_SERVER_SIDE_H
