//
// Project: IPK project - server side
// Author: Zdeněk Brhel
// Login: xbrhel04@vutbr.cz
//

#include "server_side.h"

int main(int argc, char **argv)
{
    parameters_s params;
    params.getParams(argc, argv);
    //params.printAll();

    //inicializace serveru a získání cesty k /etc/passwd
    server_side myserver;
    myserver.welcome_socket = myserver.initSocket(); //inicializace socketu
    if(myserver.welcome_socket == -1) { //ošetření chyby socketeu
        std::cerr << "ERROR socket\n";
        exit(-1);
    }
    myserver = myserver.inicialize_adress(params, myserver); //inicializace struktury sockaddr
    if(myserver.bindSocket(myserver, myserver.welcome_socket) == -1) //napojení socketu
        exit(-1);
    listen(myserver.welcome_socket, 1); //poslouchá jednoho klienta
    myserver.len = sizeof(myserver.client_address);

    while(true)
    {
        int incommingsocket = accept(myserver.welcome_socket, NULL, 0);
        if (incommingsocket < 0) {
            std::cerr << "ERROR on accept\n";
            exit(-1);
        }
        //získání informací z klienta
        bzero(myserver.buffer, BUFF_SIZE-1);
        if(recv(incommingsocket, myserver.buffer,BUFF_SIZE-1, 0) < 0){
            std::cerr << "ERROR reading from socket\n";
            exit(-1);
        }
        //parsování klienta
        myserver = myserver.parseInfo(myserver, myserver.buffer);
        //vyhledávání v souboru
        setpwent();
        struct passwd *user = NULL;
        std::string information = "";
        if(myserver.extension == 'l')
        {
            while (user = getpwent()) {
                if(myserver.user[0] == '\0') //bez jména
                {
                    information = information.append(user->pw_name);
                    information = information.append("\n");
                }
                else    //se jménem
                {
                    if(strncmp(myserver.user, user->pw_name, strlen(myserver.user)) == 0)
                    {
                        information = information.append(user->pw_name);
                        information = information.append("\n");
                    }
                }

            }
        }
        else if(myserver.extension == 'n')
        {
            struct passwd *tmp;
            if ((tmp = getpwnam(myserver.user)) != NULL) {
                information = information.append(tmp->pw_gecos);
                information = information.append("\n");
            }
            else
            {
                information = "user not found\n";
            }
        }
        else if(myserver.extension == 'f') {
            struct passwd *tmp;
            if ((tmp = getpwnam(myserver.user)) != NULL) {
                information = information.append(tmp->pw_dir);
                information = information.append("\n");
            }
            else
            {
                information = "user not found\n";
            }
        }
        endpwent();
        information = information.substr(0, information.size()-1);
        printf("Incomming message: %s\n", myserver.buffer);
        send(incommingsocket, information.c_str(), strlen(information.c_str()), 0);
        information = "";
        myserver.extension = ' ';
        bzero(myserver.user, BUFF_SIZE-1);
        bzero(myserver.buffer, BUFF_SIZE-1);
        //TESTOVÁNÍ KONEC
        close(incommingsocket);
    }
}