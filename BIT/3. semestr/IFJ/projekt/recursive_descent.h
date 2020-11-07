/**
 * @file recursive_descent.h
 * @author Vojtech Chadima, Simeon Borko, Zdenek Brhel
 * @date 11-12 2017
 * @brief Soubor obsahujujici hlavicky funkci algoritmu rekurzivniho sestupu syntakticke analyzy
 */

#include "scanner.h"
#include "errors.h"
#include "list.h"
#include "symtable.h"
#include "list.h"
#include "ast.h"
#include "precedence.h"
#include "semantic.h"
#include "garbage.h"

#ifndef IFJ_RECURSIVE_DESCENT_H
#define IFJ_RECURSIVE_DESCENT_H


hashtable_t *ht_fn;
list_t *fn_list;


/**
 * Skopiruje retezec na novou adresu.
 * @param src vstupni retezec
 * @return vystupni retezec
 */
char *copy_string(char *src);

/**
 * Syntakticka analyza zhora dolu metodou rekurzivniho sestupu vyuzivajici metodu precedencni analyzy pro vyrazy.
 * @param buffer buffer pro lexikalni analyzu
 * @param token_ptr ukazatel na token, ktery nastavuje lexikalni analyza
 * @param root_el adresa ukazatele na koren AST, ktery predame generatoru kodu
 * @return E_OK pokud probehnul syntaxi rizeny preklad v poradku, jinak chyba dle typu chyby
 */
error_t recursive_descent(buffer_t *buffer, token_t **token_ptr, ast_elem_t **root_el);

error_t desc_prog(buffer_t *buffer, token_t **token_ptr, ast_node_t *root_node);

error_t desc_function_header(buffer_t *buffer, token_t **token_ptr,
                             bool definition, // vstupne parametre
                             ast_elem_t **fn_ast_el, hashtable_t **ht_var // vystupne parametre
);

error_t desc_define_function_params(buffer_t *buffer, token_t **token_ptr, list_t *params, hashtable_t *ht_var, bool first);

error_t desc_check_function_params(buffer_t *buffer, token_t **token_ptr,
                                   hashtable_t *ht_var, list_el_ptr param_el, bool first
);

error_t desc_type(buffer_t *buffer, token_t **token_ptr, char *data_type);

error_t desc_get_stats(buffer_t *buffer, token_t **token_ptr,
                       hashtable_t *ht_var, function_t *function,
                       ast_elem_t **stats_el
);

error_t desc_st_list(buffer_t *buffer, token_t **token_ptr, hashtable_t *ht_var, ast_node_t *stats_node, function_t *function);

error_t desc_state(buffer_t *buffer, token_t **token_ptr, hashtable_t *ht_var, function_t *function, ast_elem_t **stat_el);

error_t desc_ival(buffer_t *buffer, token_t **token_ptr, hashtable_t *ht_var, ast_elem_t **init_el, char *data_type);

error_t desc_printlist(buffer_t *buffer, token_t **token_ptr, hashtable_t *ht_var, ast_node_t *print_node);

error_t desc_right_value(buffer_t *buffer, token_t **token_ptr, hashtable_t *ht_var,
                         ast_elem_t **el_1, ast_elem_t **el_2, char *data_type);

error_t desc_real_params(buffer_t *buffer, token_t **token_ptr, hashtable_t *ht_var, ast_node_t *params_node,
                         list_el_ptr list_el, bool first);

error_t desc_term(buffer_t *buffer, token_t **token_ptr, hashtable_t *ht_var, ast_elem_t **term_el, char *data_type);

#endif //IFJ_RECURSIVE_DESCENT_H