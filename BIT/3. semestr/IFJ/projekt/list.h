/**
 * @file list.c
 * @author Filip Borcik
 * @date 11-12 2017
 * @brief Soubor obsahujujici definice struktur a hlavicky funkci pro pracu s listem
 */

#include <stdlib.h>
#include <stdbool.h>
#include "errors.h"
#include "garbage.h"

#ifndef IFJ_LIST_H
#define IFJ_LIST_H

/**
 * @brief Ukazatel na prvek seznamu
 */
typedef struct list_el {	
	void *data;					/**< Ukazatel na cokoliv */
	int type;					/**< Typ ukazatelu */
	struct list_el *ptr_next;	/**< Ukazatel na nasledujici prvek */
	struct list_el *ptr_prev;	/**< Ukazatel na predchozi prvek */
} *list_el_ptr;

/**
 * @brief Definice listu
 */
typedef struct {
	list_el_ptr first; /**< Ukazatel na prvni prvek seznamu */
	list_el_ptr last;  /**< Ukazatel na posledni prvek seznamu */
} list_t;


/**
* Vytvori a inicializuje obousmerne vazany seznam
*
* @return Ukazatel na nove vytvoreny seznam, v pripade chyby funkce vraci 'NULL'
*/
list_t *list_init ();

/**
* Zrusi vsechny prvky seznamu
*
* @param list Ukazatel na ruseny seznam
*/
//void list_dispose (list_t *list);

/**
* Vlozi prvek na zacatek seznamu
*
* @param list Ukazatel na seznam do ktereho se bude prvek vkladat
* @param data Ukazatel na libovolny prvek
* @param type Typ vkladaneho prvku
* @return Vraci 'true' ak se operace probehla bez problemu, v opacnem pripade funkce vraci 'false'
*/
bool list_insert_first (list_t *list, void *data, int type);

/**
* Vlozi prvek na konec seznamu
*
* @param list Ukazatel na seznam do ktereho se bude prvek vkladat
* @param data Ukazatel na libovolny prvek
* @param type Typ vkladaneho prvku
* @return Vraci 'true' ak se operace probehla bez problemu, v opacnem pripade funkce vraci 'false'
*/
bool list_insert_last(list_t *list, void *data, int type);

/**
* Odstrani prvek se zacatku seznamu
*
* @param list Ukazatel na seznam do ktereho se bude prvek vkladat
*/
void list_delete_first (list_t *list);

/**
* Odstrani prvek se zacatku seznamu
*
* @param list Ukazatel na seznam do ktereho se bude prvek vkladat
*/
void list_delete_last (list_t *list);

/**
* Odstrani nasledujici prvek prvku
*
* @param list Ukazatel na seznam
* @param elem Ukazatel na prvek seznamu ktereho nasledovnik se bude mazat
*/
void list_post_delete (list_t *list, list_el_ptr elem);

/**
* Odstrani predchozi prvek prvku
*
* @param list Ukazatel na seznam
* @param elem Ukazatel na prvek seznamu ktereho predchudce se bude mazat
*/
void list_pre_delete (list_t *list, list_el_ptr elem);

/**
* Vlozi prvek za prvek elem
*
* @param list Ukazatel na seznam
* @param elem Ukazatel na prvek za ktery se bude vkladat
* @param data Ukazatel na libovolny prvek 
* @param type Typ vkladaneho prvku
* @return Vraci 'true' pokud se operace zdarila, v opacnem pripade funkce vraci 'false'
*/
bool list_post_insert (list_t *list, list_el_ptr elem, void *data, int type);

/**
* Najde prvni prvek od konce seznamu s danym typem
*
* @param list Ukazatel na strukturu list_t
* @param type typ hledaneho prvku
* @return Vraci ukazatel na prvek seznamu, v pripade nenalezeni vraci funkce 'NULL'
*/
list_el_ptr list_find_from_end_by_type(list_t *list, int type);

#endif // IFJ_LIST_H