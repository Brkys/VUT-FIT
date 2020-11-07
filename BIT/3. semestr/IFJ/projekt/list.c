/**
 * @file list.c
 * @author Filip Borcik
 * @date 11-12 2017
 * @brief Soubor obsahujujici pracu s listem
 */

// TODO skontrolovat a dispose

#include "list.h"

list_t *list_init (){
	//inicializace listu a osetreni proti chybe pameti
	error_t err;
	list_t *list = (list_t*) trash_add(sizeof(list_t), &err);

    if(err != E_OK)
    {
        return NULL;
    }

	if(list == NULL)
	{
		return NULL;
	}


	list->first = NULL; //nastaveni implicitnich hodnot
	list->last = NULL;

	return list;
}

bool list_insert_first (list_t *list, void *data, int type) { // deklarace a alokace mista pro novy prvek
	error_t err;
	list_el_ptr new = trash_add(sizeof(struct list_el), &err);
    if(err != E_OK)
    {
        return false;
    }

	if (new != NULL)					// pokud se alokacia zdarila tak 
	{					
		new->data = data;				// ulozime hodnotu prvku 
		new->type = type;
		new->ptr_prev = NULL;			// nastavime ukazatel na predchazajici (zadny) prvek
		new->ptr_next = list->first;	// a na nasledujici (bejvali prvni) prvek
		if (list->last != NULL)			// pokud nebyl seznam prazdny zmenime predchadzajici 
		{								// prvek bejvaleho prvniho prvku na novy prvek
			list->first->ptr_prev = new;
			list->first = new;
		}else{							// inak, nastavime prvni a posledni prvek na novy prvek
			list->last = new;
			list->first = new;
		}
		return true;
	}else								// jestli se alokace nezdarila vracime false
		return false;
}

bool list_insert_last(list_t *list, void *data, int type) {
										// deklarace a alokace mista pro novy prvek	
	error_t err;
	list_el_ptr new = trash_add(sizeof(struct list_el), &err);
    if(err != E_OK)
    {	
        return E_INTERNAL;
    }

	if (new != NULL)					// pokud se alokace zdarila pak 
	{
		new->data = data;				// ulozime hodnotu prvku 
		new->type = type;
		new->ptr_prev = list->last;		// nastavime ukazatel na predchazajici (bejvali posledni) prvek
		new->ptr_next = NULL;			// nastavime ukazatel na dalsi (zadny) prvek
		if (list->first != NULL)
		{								// pokud nebyl seznam prazdny zmenime nasledujici prvek 
			list->last->ptr_next = new;	// byvaleho posledniho prvku na novy prvek
			list->last = new;			// a poslednim se stava novy prvek
		}else{
			list->last = new;			// jinak, nastavime prvni a posledni prvek na novy prvek
			list->first = new;
		}

		return true;
	}else								// pokud se alokace nezdarila vracime false
		return false;
}
/*
void *list_copy_first (list_t *list) {
	if (list->first == NULL)
		return NULL;
	else
		return list->first->data;
}

void *list_copy_last (list_t *list) {
	if (list->first == NULL)
		return NULL
	else
		return list->last->data;
}
*/
void list_delete_first (list_t *list) {
	if (list->first == NULL)			// pokud byl seznam prazdny nedelame nic
		return;				 
	else
	{	
		list_el_ptr second = list->first->ptr_next;
		if (second != NULL)				// zmena ukazatele predchazajiciho prvniho prvku na NULL
		{
			second->ptr_prev = NULL;
		}
		list->first = second;			// prvnim prvkem se stava bejvali druhy prvek
	}	
}

void list_delete_last (list_t *list) {
	if (list->first == NULL)			// pokud byl seznam prazdny nedelame nic
		return;
	else
	{
		list_el_ptr penultimate = list->last->ptr_prev;
		if (penultimate != NULL)		// zmena ukazatele naslednika posledniho prvku na NULL
		{
			penultimate->ptr_next = NULL;
		} else {
			list->first = NULL;
		}			// odstraneni posledniho prvku
		list->last = penultimate;		// poslednim prvkem se stava predposledni
	}	
}

void list_post_delete (list_t *list, list_el_ptr elem) {
	if (elem == NULL || elem->ptr_next == NULL)
		return;
	else
	{
		list_el_ptr next = elem->ptr_next;		// prvek next je naslednik prvku elem

		if(list->last != next)					// pokud neni jeho naslednik zaroven posledni prvek,
			next->ptr_next->ptr_prev = elem;	// pak jeho naslednik bude ukazovat na prvek elem
		else
			list->last = elem;	

		elem->ptr_next = next->ptr_next;		// a naslednikem prvku elem se stane naslednik prvku next
	}					
}

void list_pre_delete (list_t *list, list_el_ptr elem) {
	if (elem == NULL || elem->ptr_prev == NULL)
		return;
	else
	{
		list_el_ptr prev = elem->ptr_prev;		// prvek prev je predchodce aktivniho prvku

		if(list->first != prev)					// pokud neni jeho predchodce prvni prvek,
			prev->ptr_prev->ptr_next = elem;	// pak jeho predchodce bude ukazovat na aktivni prvek
		else
			list->first = elem;
		
		elem->ptr_prev = prev->ptr_prev;		// a predchodcem aktivniho se stane predchodce prvku prev
	}
}

bool list_post_insert (list_t *list, list_el_ptr elem, void *data, int type) {
	if(elem == NULL)	
	{						// v pripade neexistujiciho prvku vrat false (chyba)
		return false;
	}
												// deklarace a alokace mista pro novy prvek
	error_t err;
	list_el_ptr new = trash_add(sizeof(struct list_el), &err);
    if(err != E_OK)
    {
        return E_INTERNAL;
    }
	if (new == NULL)
	{
		return false;							// vraceni false (chyba) v pripade neuspesnosti alokacie
	}
	else
	{											// pokud ma prvek elem nasledovnika pak jeho ukazatele
        new->type = type;
        new->data = data;
		if(elem->ptr_next != NULL)				// na predchozi prvek prestavime na novy prvek
			elem->ptr_next->ptr_prev = new;
												// nastavime ukazatele noveho prvku na nasledujici a predchozi prvek
		new->ptr_next = elem->ptr_next;
		new->ptr_prev = elem;
		elem->ptr_next = new;					// nasledujici prvek aktivniho prvku je novy prvek

        // je-li aktivni prvek (elem) poslednim prvkem, nastavime novy prvek jako posledni seznamu
        if (list->last == elem) {
            list->last = new;
        }
	}

	return true;
}

list_el_ptr list_find_from_end_by_type(list_t *list, int type) {
	list_el_ptr current = list->last;

	while(current){
		if (current->type == type)
			return current;
		
		current = current->ptr_prev;
	}

	return NULL;
}