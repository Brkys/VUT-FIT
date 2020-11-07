/*****************************************
 *  @file ipklib.h
 *  @author Zdeněk Brhel
 *  login: xbrhel04
 *
 *  @date 2018-04-08
 *****************************************/

#include <iostream>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <csignal>
#include <cstring>
#include <stdint-gcc.h>
#include <netdb.h>

//DEFINES
#define BUFF_SIZE 65536
#define DNS_PORT 53

using namespace std;

//STRUCTURES

enum EXIT_CODES {
    OK = 0,
    BAD_PARAMS = 2,
    QUERY_ERROR = 1,
};

enum types {
    type_A = 1,
    type_NS = 2,
    type_CNAME = 5,
    type_PTR = 12,
    type_AAAA = 28
};

typedef struct params {
    string dnsServerAddress;
    unsigned int timeOut;
    enum types type;
    bool iterative;
    string name;
    string i [50];
} params_t;

//hlavička DNS požadavku
typedef struct __dns_header {
    unsigned short xid;                     //unikátní jméno požadavku
    unsigned char recursionDesired  :1;     //rekurze jména požadavku
    unsigned char truncation  :1;           //zkrácené nebo nezkrácené jméno
    unsigned char authoritative  :1;        //je li dns server autoritativní (nadřazený)
    unsigned char opcode  :4;               //kod požadavku
    unsigned char isResponse  :1;           //požadavek, nebo odpověd
    unsigned char responseCode  :4;         //kod odpovědi
    unsigned char checkingDisabled   :1;    //kontrola povolena/zakázána na DNS resolveru
    unsigned char authenticatedData   :1;   //autentizace dat
    unsigned char reserved  :1;             //rezervováno (nepoužívat pod trestem návštěvy FBI)
    unsigned char recursionAvailable  :1;   //je podporováno rekurzivní jméno dotazu od DNS serveru?
    unsigned short questionCount;           //počet otázek v DNS zprávě... my máme jen 1 ... zatím
    unsigned short answerCount;             //počet odpovědí v DNS zprávě
    unsigned short nameServerCount;         //počet serverů, které odpověděli
    unsigned short additionalCount;         //nepoužívat pod trestem návštěvy FBI - rezervováno
} dns_header;

//konstantní pole dotazu
typedef struct _queryquestion {
    unsigned short qtype;
    unsigned short qclass;
} question;

//Constant sized fields of the resource record structure
#pragma pack(push, 1)
struct R_DATA {
    unsigned short type;
    unsigned short _class;
    unsigned int ttl;
    unsigned short data_len;
};

#pragma pack(pop)

//Pointers to resource record contents
typedef struct __resource_record {
    unsigned char *name;
    struct R_DATA *resource;
    unsigned char *rdata;
} resource_record;

//struktura dotazu
typedef struct _query {
    unsigned char *name;
    question *ques;
} query_t;

//FUNKCE
//TODO DODĚLAT
unsigned char *readAnswer(unsigned char *response, unsigned char *buff, int *count);

/**
 * Změní adresu (www.fit.vutbr.cz.) na vhodný string (\003www\003fit\005vutbr\02cz\000)
 * @param dns Zde se uloží jméno
 * @param translated Překládané jméno
 */
void changeNameToLen(unsigned char *dns, unsigned char *translated);

/**
 * Funkce vypisuje nápovědu
 */
void help();

/**
 * Přidá tečku na konec stringu
 * @param str String, do kterého má být přidána tečka
 */
void addDot(string *str);

/**
* Funkce rozřazuje parametry; určuje je na základě jejich předpony
* @param argc Počet parametrů
* @param argv Pole parametrů
* @return Vyplněná struktura params_t
*/
params_t getParams(int argc, char **argv);

/**
 * Funkce nastaví DNS header pro účely IPK-Lookup
 * @param dns Nastavovaná struktura DNS header
 */
void setDnsQuery(dns_header **dns);

/**
 * Funkce vypisuje výsledek pro neiterativní použití
 * @param dns DNS struktura
 * @param answ Odpověd (obsahuje výsledek)
 * @param par Vstupní parametry programu
 * @return
 */
int printResults(dns_header *dns, resource_record *answ, params_t par);

/**
 * Zhodnotí, jestli je string IPv6
 * @param str Vstupní string
 * @return true pokud jde o IPv6, jinak 0
 */
bool isIPv6(const string str);

/**
 * Zhodnotí, jestli je string IPv4
 * @param str Vstupní string
 * @return true pokud jde o IPv4, jinak 0
 */
bool isIPv4(const string str);

/**
 * Nastaví socket pro účely IPK-Lookup programu
 * @param sock Nastavovaný socket
 * @param address Struktura internetové adresy
 * @param par Vstupní parametry programu
 */
void setSocket(int *sock, struct sockaddr_in *address, params_t par);

/**
 * Spustí příkaz a vypíše jeho výstup
 * @param par Vstupní parametry programu
 * @param str Součást požadovaného příkazu
 * @return Výstup příkazu
 */
string execute(params_t par, unsigned char *str);

/**
 * Vykonává hlavní funkci programu (nastevní požadavku, připojení na server,
 * odeslání požadavku, příjmutí požadavku, volá funkci na výpis)
 * @param par Vstupní parametry programu
 * @return Výstupní kod programu
 */
int lookUp(params_t par);