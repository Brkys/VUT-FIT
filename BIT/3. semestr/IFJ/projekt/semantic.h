/**
 * @file semantic.h
 * @author Simeon Borko, Vojtech Chadima
 * @date 11-12 2017
 * @brief Soubor obsahujujici hlavicky pomocnych funkci pro semanticku analyzu
 */

#include "errors.h"
#include "symtable.h"
#include "ast.h"
#include "garbage.h"

#ifndef IFJ_SEMANTIC_H
#define IFJ_SEMANTIC_H

extern hashtable_t *ht_fn;

/**
 * Vytvori novou funkci, vlozi ji do tabulky funkci, prida funkci do seznamu funkci, vytvori AST element s odkazem na funkci.
 * @param ht_fn tabulka funkci
 * @param ht_var tabulka promennych
 * @param params seznam parametru funkce
 * @param function_name nazev funkce
 * @param fn_return_type navratovy typ funkce
 * @param fn_ast_el adresa na ulozeni AST elementu
 * @param defined byla funkce jiz definovana
 * @param fn_list seznam funkci
 * @return E_OK | E_INTERNAL
 */
error_t sem_create_fn_leaf(hashtable_t *ht_fn, hashtable_t *ht_var, list_t *params, char *function_name,
                           char fn_return_type, ast_elem_t **fn_ast_el, bool defined, list_t *fn_list);

/**
 * Inicializuje seznam a vytvori symtable.
 * @param list_ptr adresa seznamu
 * @param ht_ptr adresa tabulky
 * @return E_OK | E_INTERNAL
 */
error_t sem_init_list_and_symtable(list_t **list_ptr, hashtable_t **ht_ptr);

/**
 * Vytvori promennou pro parametr, vlozi ji do tabulky promennych a prida na konec seznamu parametru.
 * @param list seznam parametru funkce
 * @param ht_var tabulka promennych funkce
 * @param var_name nazev promenne, ktery nesmi byt v ramci tokenu, ale z tokenu jsme ho extrahovali a ulozili do pameti zvlast
 * @param data_type datovy typ promenne
 * @return E_OK | E_INTERNAL
 */
error_t sem_save_param(list_t *list, hashtable_t *ht_var, char *var_name, char data_type);

/**
 * Skontroluje, zda vsechny funkce byly definovany.
 * @param list_fn seznam funkci
 * @return E_OK | E_SEM_DEF
 */
error_t sem_check_every_fn_defined(list_t *list_fn);

/**
 * Zkontroluje, zda promenna jiz neni v tabulce promennych. Pokud ne, tak vytvori zaznam promenne a prida ho
 * do tabulky promennych. Taky vytvori AST element pro promennou.
 * @param ht_var tabulka promennych
 * @param var_name nazev promenne
 * @param data_type datovy typ promenne
 * @param var_el adresa na AST element promenne
 * @return E_OK | E_INTERNAL | E_SEM_DEF
 */
error_t sem_add_var_to_ht(hashtable_t *ht_var, char *var_name, char data_type, ast_elem_t **var_el);

/**
 * Zkontroluje, zda promenna je v tabulce promennych. Pokud je, vytvori AST element pro promennou.
 * @param ht_var tabulka promennych
 * @param var_name nazev promenne
 * @param var_el adresa na AST element promenne
 * @return E_OK | E_INTERNAL | E_SEM_DEF
 */
error_t sem_create_var_leaf_el(hashtable_t *ht_var, char *var_name, ast_elem_t **var_el);

/**
 * Prida jednu vestavenou funkci do tabulky funkci.
 * @param params seznam parametru funkce
 * @param fn_name nazev funkce
 * @param return_type navratovy typ funkce
 * @return E_OK | E_INTERNAL
 */
error_t sem_add_one_fn_to_ht_fn(list_t *params, char *fn_name, char return_type);

/**
 * Prida vsechny vestavene funkce do tabulky funkci.
 * @return E_OK | E_INTERNAL
 */
error_t sem_add_built_in_fns_to_ht_fn();

#endif //IFJ_SEMANTIC_H
