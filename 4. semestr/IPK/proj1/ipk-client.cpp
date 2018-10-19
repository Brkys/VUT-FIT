//
// Project: IPK project - client side
// Author: Zdeněk Brhel
// Login: xbrhel04@vutbr.cz
//

#include "client_side.h"

int main(int argc, char **argv)
{
    parameters_c params; //parsování parametrů
    params.getParams(argc, argv);
    //params.printAll(); //debug na vypsání parametrů

    //deklarace a inicializace serveru
    client_side myserver;
    myserver.host = myserver.inicialize_host(params, myserver); //inicializace struktury hostent
    myserver = myserver.inicialize_adress(params, myserver); //inicializace struktury sockaddr
    if(myserver.client_connect(myserver) == -1) //připojení k serveru
        return -1;
    bzero(myserver.buffer, BUFF_SIZE); //vynulování bufferu
    if(params.user != NULL) { //načtení stringu do bufferu a jeho odeslání
        for(int i = 0; i < strlen(params.user)+1; i++) {
            if(i == 0)
                myserver.buffer[i] = params.extension[1];
            else
                myserver.buffer[i] = params.user[i-1];
            myserver.buffer[i+1] = '\0';
        }
    } else {
        if(strcmp(params.extension, "-l")) {
            std::cerr << "ERROR parameters\n";
            exit(-1);
        }
        myserver.buffer[0] = 'l';
        myserver.buffer[1] = '\0';
    }
    send(myserver.csocket, myserver.buffer, strlen(myserver.buffer), 0);
    //write(myserver.csocket, myserver.buffer, strlen(myserver.buffer)); //odeslání serveru parametry
    bzero(myserver.buffer, BUFF_SIZE); //vynulování bufferu, aby mohl přijmout info
    read(myserver.csocket, myserver.buffer, BUFF_SIZE-1); //čtení infa ze serveru
    printf("%s\n",myserver.buffer); //vypsání info o uživateli
    close(myserver.csocket); //uzavření socketu
    return 0;
}