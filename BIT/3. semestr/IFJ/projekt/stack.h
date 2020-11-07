/**
 * @file stack.h
 * @author Filip Borcik
 * @date 11-12 2017
 * @brief Soubor obsahujujici hlavicky funcki pro vytvareni a pracu se zasobnikem
 */

#include "list.h"
#include "errors.h"
#include "garbage.h"

/**
 * @brief Definice elementu obsazeneho ve stacku
 */
typedef list_el_ptr stack_el_ptr;	// ukazatel na prvek zasobniku

/**
 * @brief Definice zasobniku
 */
typedef list_t stack_t;				// zasobnik

// FUNKCE

/**
* Inicializuje a vytvori zasobnik
*	
* @return Ukazatel na nove vytvoreny zasobnik
*/
stack_t *stack_init ();

/**
* Zjisti jestli je zasobnik prazdny
*
* @param stack Ukazatel na strukturu stack_t
* @return Vraci 'true' pokud je zasobnik prazdny, 'false' v opacnem pripade
*/
bool stack_empty (stack_t *stack);

/**
* Zjisti co se nachazi na vrcholu zasobniku
*	
* @param stack Ukazatel na strukturu stack_t
* @return 
*/
stack_el_ptr stack_top(stack_t *stack);

/**
* Popne prvek z vrcholu zasobniku, v pripade ze je zasobnik prazdny nedela funkce nic
*	
* @param stack Ukazatel na strukturu stack_t
*/
void stack_pop(stack_t *stack);

/**
* Popuje vsechny prvky z vrcholu zasobniku az kym nenarazi na znak '<' vcetne neho, v pripade ze je zasobnik prazdny nedela funkce nic
*	
* @param stack Ukazatel na strukturu stack_t
*/
void stack_pop_until_param(stack_t *stack);

/**
* Vlozi prvek na vrchol zasobniku
*
* @param stack Ukazatel na strukturu stack_t
* @param data Ukazatel na libovolny prvek
* @param type Typ vkladaneho prvku
* @return Funkce vraci 'true' pokud nenastane zadna chyba, v opacnem pripade funkce vraci 'false'
*/
bool stack_push(stack_t *stack, void *data, int type);

/**
* Vlozi prvek nad prvek elem
*
* @param stack Ukazatel na strukturu stack_t
* @param elem Ukazatel na prvek nad ktery se bude vkladat
* @param data Ukazatel na libovolny prvek 
* @param type Typ vkladaneho prvku
* @return Vraci 'true' pokud se operace zdarila, v opacenm pripade funkce vraci 'false'11
*/
bool stack_push_above(stack_t *stack, stack_el_ptr elem, void *data, int type);
/**
* Najde nejvrchnejsi polozku v zasobniku s danym typem
*
* @param stack Ukazatel na strukturu stack_t
* @param type Typ hledaneho prvku
* @return Vraci ukazatel na prvek zasobniku, v pripade nenalezeni funkce vraci 'NULL'
*/
stack_el_ptr stack_find_from_top_by_type(stack_t *stack, int type);
