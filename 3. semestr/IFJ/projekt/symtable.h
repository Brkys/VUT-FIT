/**
 * @file symtable.h
 * @author Vojtech Chadima, Simeon Borko
 * @date 11-12 2017
 * @brief Soubor obsahujujici definice struktur a hlavicky funkci pro vytvareni a praci s tabulkou symbolu
 */

#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include "list.h"
#include "errors.h"
#include "garbage.h"

#ifndef IFJ_SYMTABLE_H
#define IFJ_SYMTABLE_H

//TODO !!!!!!!!!!!!!!!!!!!!!! KOMENTÁŘE !!!!!!!!!!!!!!!!!!!!!!!!!!
typedef struct element_s
{ 
    char *key; /**< ?????? */
    struct element_s *next;	/**< Ukazatel na dalsi prvek */
    void *ptr;	/**< Ukazatel na promennou nebo funkci */
} element_t;


typedef struct hashtable_s
{
    int size;	//velikost tabulky-idealne prvocislo
    struct element_s **table;	//

} hashtable_t;



typedef struct variable_s
{
    char *name;
    char data_type;	//datovy typ promenne
} variable_t;

typedef struct function_s
{	
	char *name;
    list_t *params;		//seznam parametru funkce
	char return_type;	//navratovy typ funkce
	hashtable_t *local_symtable;	//lokalni tabulka symbolu funkce
	bool defined;	//byla funkce definovana?
} function_t;


/**
 * Vytvori a inicializuje hashovaci tabulku (tabulku s rozptylenymi polozkami).
 *
 * @return Ukazatel na nove vytvorenou tabulku.
 */
hashtable_t *ht_init ();

/**
 * Vlozi novou polozku do tabulky symbolu. V pripade existence polozky s rovnakym klicem vrati 'false'.
 * @param hashtable Tabulka funkci
 * @param key Klic zaznamu
 * @param ptr Data zaznamu
 * @return 'false' v pripade nalezeni polozky se stejnym klicem nebo v pripade chyby alokace pamete, jinak 'true'
 */
bool ht_insert_pair ( hashtable_t *hashtable, char *key, void *ptr );


/**
 * Funkce vyhleda prvek v tabulce podle klice.
 *
 * @param hashtable Konkretni hashovaci tabulka.
 * @param key Klic konkretniho prvku tabulky.
 * @return Ukazatel na funkci nebo promennou.
 */
element_t *ht_search ( hashtable_t *hashtable, char *key );


/**
 * Funkce zjisti hodnotu dat prvku v tabulce.
 *
 * @param hashtable Konkretni hashovaci tabulka.
 * @param key Klic konkretniho prvku tabulky.
 * @return Ukazatel na funkci nebo promennou.
 */
void *ht_read ( hashtable_t *hashtable, char *key );

//void ht_destroy(hashtable_t *ht, void (*destroy_el_data)(void *));

//void ht_destroy_function(void *ptr);

#endif // IFJ_SYMTABLE_H