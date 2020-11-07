/**
 * @file symtable.c
 * @author Vojtech Chadima, Simeon Borko
 * @date 11-12 2017
 * @brief Soubor obsahujujici vytvareni a praci s tabulkou symbolu
 */

#include "symtable.h"

#define HT_SIZE 131

hashtable_t *ht_init ()
{
	int size = HT_SIZE;	//prvocislo, nami vhodne vybrane
	//Alokace vlastni tabulky
	error_t err;
	hashtable_t *hashtable = trash_add(sizeof(hashtable_t), &err);
	if(err != E_OK)
		return NULL;
	if (hashtable == NULL)
		return NULL;

	hashtable->size  = size;	//nastaveni velikosti tabulky

	//Alokace vsech ukazatelu na jednotliva mista tabulky
	hashtable->table = trash_add(sizeof(element_t *) * size, &err);
	if(err != E_OK)
		return NULL;
	if (hashtable->table == NULL)
		return NULL;

	//Nastaveni hodnot jednotlivych mist tabulky na NULL
	for (int i = 0; i < size; i++)
	{
		hashtable->table[i] = NULL;
	}

	return hashtable;
}

//algoritmus djb2
unsigned long ht_hash_function ( char *key )
{
    unsigned long hash_value = 5381;
    int c;

    while ((c = *key++) != 0)
        hash_value = ((hash_value << 5) + hash_value) + c; // hash * 33 + c

    return hash_value % HT_SIZE;
}


bool ht_insert_pair ( hashtable_t *hashtable, char *key, void *ptr )
{
	element_t *item = ht_search( hashtable, key );
	if (  item != NULL ){
		return false;
	}

	//tabulka neobsahuje prvek se stejnym klicem:
	error_t err;
	element_t *new_item = trash_add(sizeof(element_t), &err);
	if(err !=E_OK)
		return E_INTERNAL;
	if ( new_item == NULL ){	//byl malloc uspesny?
		return false;
	}

	//inicializace nove vkladaneho prvku
	new_item->key = key;
	new_item->ptr = ptr;
	new_item->next = NULL;

	unsigned long hash_code = ht_hash_function(key);	//z klice ziskame hashovaci funkci hashovaci kod

	//je-li polozka v hash. tabulce na indexu hash_code zatim volna (tj. neni-li na zatim zacatek zadneho seznamu)
	if ( hashtable->table[hash_code] == NULL ){
		hashtable->table[hash_code] = new_item;
	}

	//v tabulce na indexu hash_code jiz je nejaky zretezeny seznam, tudiz vlozime novy prvek na jeho zacatek
	else {
		new_item->next = hashtable->table[hash_code];
        hashtable->table[hash_code] = new_item;	//na pozici hashtable[has_code] bude nyni ukazatel na nami vlozeny nocy prvek
	}
	
	return true;
}


element_t *ht_search ( hashtable_t *hashtable, char *key )
{
	unsigned long hash_code = ht_hash_function(key);

	//je-li misto hashtable[hash_code] rovne NULL, na tomto miste se nenachazi zadny prvek ani seznam, tudiz prvekm nalezen nebyl
	if ( hashtable->table[hash_code] == NULL )
		return NULL;
	
	//na miste hashtable[hash_code] se nachazi seznam o alespon jednom prvku
	else{
		element_t *item_tmp = hashtable->table[hash_code];

		while ( item_tmp != NULL ){
			if ( strcmp(item_tmp->key, key) == 0 )
				return item_tmp;	//hledany prvek byl nalezen, ukazatel na nej je vracen
		
			else {	//hledany prvek nalezen nebyl, 
				item_tmp = item_tmp->next;	//posouvame se na dalsi prvek
			}
	    }
	}	

	return NULL;	//v pripade, ze v cyklu while nebyl nalezen zadny prvek s onim vyhledavanym klicem, vyhledavani nebylo uspesne a vratime NULL
}


void *ht_read ( hashtable_t* hashtable, char *key ) {
	element_t *item_tmp = ht_search(hashtable, key);
    return item_tmp == NULL ? NULL : item_tmp->ptr;
}