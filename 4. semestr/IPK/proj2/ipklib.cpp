/*****************************************
 *  @file ipklib.h
 *  @author Zdeněk Brhel
 *  login: xbrhel04
 *
 *  @date 2018-04-08
 *****************************************/


#ifndef PROJ2_IPKLIB_H
#define PROJ2_IPKLIB_H

#include "ipklib.h"

bool called = false;

//Převzatá funkce readAnswer
//Author : Silver Moon (m00n.silv3r@gmail.com)
//Dated : 29/4/2009
unsigned char *readAnswer(unsigned char *response, unsigned char *buff, int *count) {
    unsigned char *name;
    unsigned int p = 0, jumped = 0, offset;
    int i, j;

    *count = 1;
    name = (unsigned char *) malloc(256);

    name[0] = '\0';

    //read the names in 3www6google3com format
    while (*response != 0) {
        if (*response >= 192) {
            offset = (*response) * 256 + *(response + 1) - 49152; //49152 = 11000000 00000000 ;)
            response = buff + offset - 1;
            jumped = 1; //we have jumped to another location so counting wont go up!
        } else {
            name[p++] = *response;
        }

        response = response + 1;

        if (jumped == 0) {
            *count = *count + 1; //if we havent jumped to another location then we can count up
        }
    }

    name[p] = '\0'; //string complete
    if (jumped == 1) {
        *count = *count + 1; //number of steps we actually moved forward in the packet
    }

    //now convert 3www6google3com0 to www.google.com
    for (i = 0; i < (int) strlen((const char *) name); i++) {
        p = name[i];
        for (j = 0; j < (int) p; j++) {
            name[i] = name[i + 1];
            i = i + 1;
        }
        name[i] = '.';
    }
    name[i - 1] = '\0'; //remove the last dot
    return name;
}

//pozměněná funkce changeNameToLen
//Author : Silver Moon (m00n.silv3r@gmail.com)
//Dated : 29/4/2009
void changeNameToLen(unsigned char *dns, unsigned char *translated) {
    if(string((char*)translated) == ".")
    {
        *dns++='\000';
        *dns++='\0';
    }
    auto len = (int)strlen((char *) translated), aux = 0, i = 0;
    while(i < len)
    {
        if (translated[i] == '.') {
            *dns++ = i - aux;
            while(aux < i)
            {
                *dns++ = translated[aux++];
            }
            aux++;
        }
        ++i;
    }
    *dns++ = '\0';
}

void help() {
    cout << "Help for IPK LookUp" << endl;
    cout << "\"./ipk-lookup -h\" - Prints out this help" << endl;
    cout << "\"./ipk-lookup -s {ip address} [-T {seconds}] [-t {type}] [-i] {name}\"" << endl;
    cout << "-s - requied argument, expect {ip adress} after" << endl;
    cout << "-T - optional argument, specifies timeout value in seconds, expect {seconds} after" << endl;
    cout << "-t - optional argument, specifies type of queried record, expect {type} after" << endl;
    cout << "-i - optional argument, force iterative way of resolution" << endl;
    cout << "{ip address} - IP address of server" << endl;
    cout << "{seconds} - number of seconds" << endl;
    cout << "{type} - type of DNS record" << endl;
    cout << "{name} - translated domain name, in case of -t PTR program expect IPv4 or IPv6 address" << endl;
    exit(OK);
}

void addDot(string *str) {
    char *ptr = (char*)str->c_str();
    if (ptr[str->length() - 1] != '.') //pokud na konci není tečka, přidá ji
        str->append(".");
}

params_t getParams(int argc, char **argv) {
    if (argc < 2 || argc > 9)
        exit(BAD_PARAMS);
    //inicializace proměnných
    params_t tmp;
    tmp.dnsServerAddress = "";
    tmp.timeOut = 5;
    tmp.iterative = false;
    tmp.type = type_A;
    tmp.name = "";

    //rozřazování parametrů
    for (int i = 1; i < argc; i++) {
        string str(argv[i]);
        if (str == "-h" && argc == 2) { //help parametr
            help();
        } else if (str == "-s" && argc >= 3) { //parametr serveru + jeho kontrola
            tmp.dnsServerAddress = string(argv[++i]);
            struct sockaddr_in si;
            if (inet_pton(AF_INET, tmp.dnsServerAddress.c_str(), &(si.sin_addr)) == 0) {
                cerr << "Expected valid IP address after parameter -s" << endl;
                exit(BAD_PARAMS);
            }
        } else if (str == "-T" && argc >= 5) { //parametr timeoutu + konverze na int
            tmp.timeOut = (unsigned int) stoi(string(argv[++i]));
        } else if (str == "-t" && argc >= 5) { //parametr typu záznamu + kontrola správnosti
            string a = string(argv[++i]);
            if (a != "A" && a != "AAAA" && a != "NS" && a != "PTR" && a != "CNAME") {
                cerr << "Expected valid DNS query after parameter -t" << endl;
                exit(BAD_PARAMS);
            }
            if (a == "AAAA")
                tmp.type = type_AAAA;
            else if (a == "NS")
                tmp.type = type_NS;
            else if (a == "PTR")
                tmp.type = type_PTR;
            else if (a == "CNAME")
                tmp.type = type_CNAME;
            else
                tmp.type = type_A;
        } else if (str == "-i" && argc >= 4) { //parametr -i a jeho rozdělení na samostatné servery
            str = argv[argc-1];
            if(!isIPv4(tmp.name) && !isIPv6(tmp.name))
                addDot(&str);
            int dotCount = 0;
            char *ptr = (char *)str.c_str();
            bool first = true;
            for(int j = 0; j < (int)(str.length()); j++) //konverze na samostatné servery
            {
                if(first)
                {
                    first = false;
                    tmp.i[dotCount] = string(ptr);
                } else {
                    if(ptr[j] == '.')
                    {
                        dotCount++;
                        j++;
                        if(j == (int)(str.length()))
                            j--;
                        char *tmpstr = &ptr[j];
                        tmp.i[dotCount] = string(tmpstr);
                    }
                }
            }
            tmp.iterative = true;
            i++;
        }
        string aux = string(argv[argc-1]);
        if (aux == "-i" || aux == "-t" ||aux == "-T") { //kontrola posledního parametru
            cerr << "Missing last argument {name}, type \"./ipk-lookup -h\" for help" << endl;
            exit(BAD_PARAMS);
        }
    }
    if(tmp.name == "") {
        tmp.name = string(argv[argc - 1]);
        if(!isIPv4(tmp.name) && !isIPv6(tmp.name))
            addDot(&tmp.name); //přídání tečky nakonec - pokud tam není
    }
    return tmp;
}

void setDnsQuery(dns_header **dns) {
    (*dns)->xid = (unsigned short) htons((uint16_t )getpid()); //nastavení ID
    (*dns)->isResponse = 0; //jde o dotaz
    (*dns)->opcode = 0; //jde o standartní dotaz
    (*dns)->authoritative = 0; //nejde o autoritativní
    (*dns)->truncation = 0; //zpráva není zkrácená
    (*dns)->recursionDesired = 1; //rekurze je potřeba
    (*dns)->recursionAvailable = 0; //rekurzi nemáme? CO TADY???
    (*dns)->reserved = 0; //rezervováno :(
    (*dns)->authenticatedData = 0; //neautentizováno :(
    (*dns)->checkingDisabled = 0; //nekontrolováno :(
    (*dns)->responseCode = 0; //kod odpovědi
    (*dns)->questionCount = htons(1); //počet otázek na které se ptáme
    (*dns)->answerCount = 0; //počet odpovědí je 0
    (*dns)->nameServerCount = 0; //počet serverů je 0
    (*dns)->additionalCount = 0; //zase něco rezervováno :(
}

int printResults(dns_header *dns, resource_record *answ, params_t par) {
    int cname = 0, a=0, ptr=0, ns=0, aaaa=0; //počet záznamů
    for (int i = 0; i < ntohs(dns->answerCount); i++) {
        long long *pom = (long long *) answ[i].rdata;
        string aux = string((char *)answ[i].name);
        addDot(&aux); //přidá tečku na konec stringu
        switch (ntohs(answ[i].resource->type)) {
            case type_A: //typ A
                a++;
                struct sockaddr_in tmp; //vytvoření pomocné struktury pro uložení a konvertování adresy
                tmp.sin_addr.s_addr = (in_addr_t) (*pom); //konvertování
                cout << aux << " IN A " << inet_ntoa(tmp.sin_addr) << endl;
                break;
            case type_CNAME:
                cname++;
                cout << aux << " IN CNAME " << answ[i].rdata << "." << endl;
            case type_PTR:
                ptr++;
                if (par.type == type_PTR)
                    cout << aux << " IN PTR " << answ[i].rdata << "." << endl;
                break;
            case type_NS:
                ns++;
                cout << aux << " IN NS " << answ[i].rdata << endl;
                break;
            case type_AAAA:
                aaaa++;
                struct in6_addr tmp2;
                char buf[41];
                memcpy(tmp2.s6_addr, answ[i].rdata, INET6_ADDRSTRLEN);
                cout << answ[i].name << " IN AAAA " << inet_ntop(AF_INET6, tmp2.s6_addr, buf, INET6_ADDRSTRLEN) << endl; //konvertovat answ[i].rdata na IPv6?
                if(called == true)
                    exit(0);
                break;

            default:
                break;
        }
    };
    //kontrola, jestli byl požadovaný typ nalezen
    if (par.type == type_A && a == 0) {
        cerr << "No query of that type found!" << endl;
        return QUERY_ERROR;
    } else if (par.type == type_NS && ns == 0) {
        cerr << "No query of that type found!" << endl;
        return QUERY_ERROR;
    } else if (par.type == type_PTR && ptr == 0) {
        cerr << "No query of that type found!" << endl;
        return QUERY_ERROR;
    } else if (par.type == type_CNAME && cname == 0) {
        cerr << "No query of that type found!" << endl;
        return QUERY_ERROR;
    } else if (par.type == type_AAAA && aaaa == 0) {
        cerr << "No query of that type found!" << endl;
        return QUERY_ERROR;
    }
    return 0;
}

bool isIPv6(const string str) {
    struct sockaddr_in sa;
    return inet_pton(AF_INET6, str.c_str(), &(sa.sin_addr)) != 0;
}

bool isIPv4(const string str) {
    struct sockaddr_in6 sa;
    return inet_pton(AF_INET, str.c_str(), &(sa.sin6_addr)) != 0;
}

void setSocket(int *sock, struct sockaddr_in *address, params_t par) {
    //Nastavení socketu pro UDP, komunikace probíhá na IPv4
    *sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    //Nastavení adresy
    address->sin_family = AF_INET;
    address->sin_port = htons(DNS_PORT);
    address->sin_addr.s_addr = inet_addr(par.dnsServerAddress.c_str());
}

string execute(params_t par, unsigned char *str) {
    const char *cmd = "./ipk-lookup -s ";
    string tmp = "";
    tmp += cmd;
    tmp += par.dnsServerAddress += " ";
    tmp += string((char *)str);

    const char *command = tmp.c_str();
    char buffer[128];
    string result = "";
    FILE* pipe = popen(command, "r");
    while (!feof(pipe)) {
        if (fgets(buffer, 128, pipe) != NULL)
            result += buffer;
    }
    pclose(pipe);
    return result;
}

int lookUp(params_t par) {
    unsigned char buffer[BUFF_SIZE];
    struct sockaddr_in ipv4;


    //Překládaní jména (obrácení čísel..) (z nějakého důvodu nelze hodit do funkce :( )
    if (par.type == type_PTR) {
        if (isIPv4(par.name)) {
            char reversed_ip[16];
            in_addr_t addr;
            inet_pton(AF_INET, par.name.c_str(), &addr);
            //otočení adresy a přidání .in-addr.arpa
            addr = ((addr & 0xff000000) >> 24) |
                   ((addr & 0x00ff0000) >> 8) |
                   ((addr & 0x0000ff00) << 8) |
                   ((addr & 0x000000ff) << 24); //Posuv bitů, reversnutí IP
            inet_ntop(AF_INET, &addr, reversed_ip, sizeof(reversed_ip));
            par.name = strcat(reversed_ip, ".in-addr.arpa.");
            //Nastavení na rozdělení IPv4 adresy
            const char*pom = par.name.c_str();
            int iterator = 5;
            for(int i = (int)strlen(pom); i >= 0; i--)
            {
                if(strstr(&pom[i],".") && iterator == 5) {
                    par.i[iterator] = ".";
                    iterator--;
                }
                else if(strstr(&pom[i],"in-addr.arpa.") && iterator == 4) {
                    par.i[iterator] = &pom[i];
                    iterator--;
                }
                else
                {
                    if(strstr(&pom[i], ".in-addr.arpa.") && pom[i] == '.')
                    {
                        par.i[iterator] = &pom[i+1];
                        iterator--;
                    }
                    if(iterator == 0)
                    {
                        par.i[iterator] = &pom[i];
                    }
                }
            }
        } else if (isIPv6(par.name)) {
            struct in6_addr addr;
            inet_pton(AF_INET6, par.name.c_str(), &addr);
            char str[32], str2[32], str3[63];

            //Otočení IPv6 adresy a přidání .ip6.arpa
            sprintf(str, "%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x",
                    addr.s6_addr[0], addr.s6_addr[1], addr.s6_addr[2], addr.s6_addr[3],
                    addr.s6_addr[4], addr.s6_addr[5], addr.s6_addr[6], addr.s6_addr[7],
                    addr.s6_addr[8], addr.s6_addr[9], addr.s6_addr[10], addr.s6_addr[11],
                    addr.s6_addr[12], addr.s6_addr[13], addr.s6_addr[14], addr.s6_addr[15]);
            for (int i = 0; i < (int) strlen(str); i++) {
                str2[i] = str[31 - i];
            }
            for (int i = 0; i < 63; i++) {
                str3[i] = str2[i / 2];
                str3[++i] = '.';
            }
            par.name = strcat(str3, "ip6.arpa.");
            const char*pom = par.name.c_str();
            int iterator = 32;
            for(int i = (int)strlen(pom); i >= 0; i--)
            {
                if(strstr(&pom[i],".") && iterator == 32) {
                    par.i[iterator] = ".";
                    iterator--;
                }
                else if(strstr(&pom[i],"ip6.arpa.") && iterator == 32) {
                    par.i[iterator] = &pom[i];
                    iterator--;
                }
                else
                {
                    if(strstr(&pom[i], ".ip6.arpa.") && pom[i] == '.')
                    {
                        par.i[iterator] = &pom[i+1];
                        iterator--;
                    }
                    if(iterator == 0)
                    {
                        par.i[iterator] = &pom[i];
                    }
                }
            }
        }
    }


    //nastavení timeoutu
    struct timeval timeout;
    timeout.tv_sec = par.timeOut;

    if(par.iterative)
    {
        string previous = "";
        int last = 0;
        //spočítání prvků v poli
        enum types tmptype = par.type;
        par.type = type_NS;
        for(int i = 0; par.i[i] != ""; i++)
            last++;

        auto *queryname = &buffer[sizeof(dns_header)];
        for(int i = last-1; i >= 0; i--) {
            if(i == 0) {
                par.type = tmptype;
                par.iterative = false;
                called = true;
                lookUp(par);
                return 0;
            }
            auto *translatedName = (unsigned char*) par.i[i].c_str(); //překládané jméno
            changeNameToLen(queryname, translatedName); //předělání jména na přívětivou formu

            //Nastavení socketu a timeoutu
            int sock;
            setSocket(&sock, &ipv4, par);
            if (setsockopt (sock, SOL_SOCKET, SO_RCVTIMEO, (char *)&timeout, sizeof(timeout)) < 0)
                cerr << "Setsockopt function failed!" << endl;
            if (setsockopt (sock, SOL_SOCKET, SO_SNDTIMEO, (char *)&timeout, sizeof(timeout)) < 0)
                cerr << "Setsockopt function failed!" << endl;

            //nastavení DNS hlavičky
            auto *dns = (dns_header *) &buffer;
            setDnsQuery(&dns);

            //nastavení dotazu
            auto *queryinfo = (question *) &buffer[sizeof(dns_header) + (strlen((const char*) queryname) +1)];

            queryinfo->qtype = htons(par.type);
            queryinfo->qclass = htons(1);

            //odeslání socketu
            if(sendto(sock, (char *) buffer, sizeof(dns_header) + (strlen((const char*) queryname) +1) + sizeof(question),
                      0, (struct sockaddr *) &ipv4, sizeof(ipv4)) < 0)
            {
                cerr << "Sending failed, timeout!" << endl;
                return QUERY_ERROR;
            }

            //přijetí socketu
            int size = sizeof(ipv4);
            if(recvfrom(sock, (char *) buffer, BUFF_SIZE, 0, (struct sockaddr *) &ipv4, (socklen_t *) &size) < 0)
            {
                cerr << "Receiving failed, timeout!" << endl;
                return QUERY_ERROR;
            }

            auto *readResponse = &buffer[sizeof(dns_header) + (strlen((const char *) queryname) +1) + sizeof(queryinfo)];

            //čtení
            int aux;
            resource_record answ[50];

            for(size = 0; size < ntohs(dns->answerCount); size++)
            {
                answ[size].name = readAnswer(readResponse, buffer, &aux);
                readResponse += aux;

                answ[size].resource = (R_DATA *) readResponse;
                readResponse += sizeof(R_DATA);

                if(ntohs(answ[size].resource->type) == 1)
                {
                    answ[size].rdata = (unsigned char*) malloc(ntohs(answ[size].resource->data_len));

                    for(int k = 0; k < ntohs(answ[size].resource->data_len); k++)
                    {
                        answ[size].rdata[k] = readResponse[k];
                    }

                    answ[size].rdata[ntohs(answ[size].resource->data_len)] = '\0';

                    readResponse += ntohs(answ[i].resource->data_len);
                }
                else if(ntohs(answ[size].resource->type) == 28)
                {

                    answ[size].rdata = (unsigned char*) malloc(ntohs(answ[size].resource->data_len));

                    for(int k = 0; k < ntohs(answ[size].resource->data_len); k++)
                    {
                        answ[size].rdata[k] = readResponse[k];
                    }

                    answ[size].rdata[ntohs(answ[size].resource->data_len)] = '\0';

                    readResponse += ntohs(answ[i].resource->data_len);
                } else {
                    answ[size].rdata = readAnswer(readResponse, buffer, &aux);
                    readResponse += aux;
                }

                string str = string((char *)answ[size].name);

                addDot(&str);
                long long *pom = (long long *) answ[i].rdata;
                switch (ntohs(answ[size].resource->type)) {
                    case type_A:
                        struct sockaddr_in tmp; //vytvoření pomocné struktury pro uložení a konvertování adresy
                        tmp.sin_addr.s_addr = (in_addr_t) (*pom); //konvertování
                        cout << aux << " IN A " << inet_ntoa(tmp.sin_addr) << endl;
                        break;
                    case type_CNAME:
                        cout << str << " IN CNAME " << answ[size].rdata << "." << endl;
                    case type_PTR:
                        if (par.type == type_PTR)
                            cout << str << " IN PTR " << answ[size].rdata << "." << endl;
                        break;
                    case type_NS:
                        if(previous != str) {
                            cout << str << " IN NS " << answ[size].rdata << endl;
                            string aux = execute(par, answ[size].rdata); //reku
                            cout << aux;
                            /*char *result = (char *)aux.c_str();
                            result = strstr(result, "IN A ");
                            result = &result[5];
                            result[strlen(result)-1] = '\0';
                            par.dnsServerAddress = string(result);*/
                        }
                        break;
                    case type_AAAA:
                        struct in6_addr tmp2;
                        char buf[41];
                        memcpy(tmp2.s6_addr, answ[i].rdata, INET6_ADDRSTRLEN);
                        cout << answ[i].name << " IN AAAA " << inet_ntop(AF_INET6, tmp2.s6_addr, buf, INET6_ADDRSTRLEN) << endl;
                        break;

                    default:
                        if(size != 1)
                            continue;
                        break;
                }
                previous = str;
            }
        }
        return 0;
    }




    //NEITERATIVNÍ ŘEŠENÍ
    auto *translatedName = (unsigned char *) par.name.c_str();
    //Nastavení query
    auto *queryname = &buffer[sizeof(dns_header)];
    changeNameToLen(queryname, translatedName);

    //Nastavení socketu a adresy
    int sock;

    setSocket(&sock, &ipv4, par);
    if (setsockopt (sock, SOL_SOCKET, SO_RCVTIMEO, (char *)&timeout, sizeof(timeout)) < 0)
        cerr << "Setsockopt function failed!" << endl;
    if (setsockopt (sock, SOL_SOCKET, SO_SNDTIMEO, (char *)&timeout, sizeof(timeout)) < 0)
        cerr << "Setsockopt function failed!" << endl;

    //Nastavení DNS hlavičky
    dns_header *dns = (dns_header *) &buffer;
    setDnsQuery(&dns);

    //Nastavení dotazu
    auto *queryinfo = (question *) &buffer[sizeof(dns_header) + (strlen((const char *) queryname) + 1)]; //fill it
    queryinfo->qtype = htons(par.type); //type požadavku o jaký žádáme
    queryinfo->qclass = htons(1);

    //Odeslání požadavku
    if (sendto(sock, (char *) buffer, sizeof(dns_header) + (strlen((const char *) queryname) + 1) +
                                      sizeof(question), 0, (struct sockaddr *) &ipv4, sizeof(ipv4)) < 0) {
        cerr << "Sending failed, timeout!" << endl;
        return QUERY_ERROR;
    }

    //Navrácení požadavku
    int i = sizeof(ipv4);
    if (recvfrom(sock, (char *) buffer, BUFF_SIZE, 0, (struct sockaddr *) &ipv4, (socklen_t *) &i) < 0) {
        cerr << "Receiving failed, timeout!" << endl;
        return QUERY_ERROR;
    }

    dns = (dns_header *) buffer;

    //Nastavení čtení
    auto readResponse = &buffer[sizeof(dns_header) + (strlen((const char *) queryname) + 1) + sizeof(question)];

    //Čtení odpovědí
    int aux = 0;
    resource_record answ[50];

    for (i = 0; i < ntohs(dns->answerCount); i++) {
        answ[i].name = readAnswer(readResponse, buffer, &aux);
        readResponse = readResponse + aux;

        answ[i].resource = (struct R_DATA *) (readResponse);
        readResponse = readResponse + sizeof(struct R_DATA);

        if (ntohs(answ[i].resource->type) == 1) //Pokud jde o IPv4 adresu
        {
            answ[i].rdata = (unsigned char *) malloc(ntohs(answ[i].resource->data_len));

            for (int j = 0; j < ntohs(answ[i].resource->data_len); j++) {
                answ[i].rdata[j] = readResponse[j];
            }

            answ[i].rdata[ntohs(answ[i].resource->data_len)] = '\0';

            readResponse = readResponse + ntohs(answ[i].resource->data_len);
        }
        else if(ntohs(answ[i].resource->type) == 28)
        {

            answ[i].rdata = (unsigned char*) malloc(ntohs(answ[i].resource->data_len));

            for(int k = 0; k < ntohs(answ[i].resource->data_len); k++)
            {
                answ[i].rdata[k] = readResponse[k];
            }

            answ[i].rdata[ntohs(answ[i].resource->data_len)] = '\0';

            readResponse += ntohs(answ[i].resource->data_len);
        } else {
            answ[i].rdata = readAnswer(readResponse, buffer, &aux);
            readResponse = readResponse + aux;
        }
    }


    //Vypíše výsledky
    return printResults(dns, answ, par);
}

#endif //PROJ2_IPKLIB_H