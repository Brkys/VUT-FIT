#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <semaphore.h>
#include <signal.h>
#include <sys/ipc.h>
#include <sys/mman.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <sys/wait.h>

//Globální proměnné

sem_t *adult, *child, *exitsem, *prior, *out, *ecla; //Semafory pro synchronizaci

int *counter = NULL; //Počítadlo řádků

FILE *output; //Výstupní soubor

int adultCount, childCount, CGT, AGT, CWT, AWT; //Parametry programu

int *adultsincenter, *childsincenter, *adultstoleave, *childstoenter = 0; //Globální hodnoty

int *remAdults, *remChilds; //Zbývající procesy na spuštění

char *ERR = "OK"; //Zpráva erroru



//Funkce

void help(); //Vypíše nápovědu

int saveParams(int cntofparams, char *locparams[]); //Uloží parametry

int getMem(); //Inicializuje a alokuje paměť

void release(); //Pustí jednoho childa a jednoho adulta (pokud čekají)

void sigcatch(); //Zachytí SIGTERM

void clearMem(); //Dealokuje a uvolní zdroje

int adultProc(int order); //Proces s logikou pro adulty

int childProc(int order); //Proces s logikou pro childy 
