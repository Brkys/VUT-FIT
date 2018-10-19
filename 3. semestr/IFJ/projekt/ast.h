/**
 * @file ast.h
 * @author Simeon Borko, Zdenek Brhel
 * @date 11-12 2017
 * @brief Soubor obsahujujici hlavicky funkci, definice vyctovych typu a struktur pro vytvareni a pracu s AST stromem
 */

#include "errors.h"
#include <stdlib.h>
#include "garbage.h"

#ifndef IFJ_AST_H
#define IFJ_AST_H

/**
 * @brief Vycet typu uzlu v AST stromu
 * Vycet vsech moznych typu uzlu
 */
typedef enum {

    // syntakticka analyza

    AT_ROOT,   
    AT_DECLARATION,
    AT_DEFINITION,
    AT_SCOPE,
    AT_STATS, // prikazy

    AT_DIM,
    AT_ASSIGN_EXPR,
    AT_ASSIGN_FN,
    AT_PARAMS, // skutecne parametry
    AT_PRINT,
    AT_RETURN,
    AT_INPUT,
    AT_IF,
    AT_WHILE,

    // precedencni analyza

    AT_PLUS,
    AT_MINUS,
    AT_MULTIPLY,
    AT_SLASH,
    AT_BACKSLASH,

    AT_COND_GT,
    AT_COND_LT,
    AT_COND_IE, // <>
    AT_COND_GE,
    AT_COND_LE,
    AT_COND_EQ

} ast_node_type_t;

/**
 * @brief Pomocny element pri praci s AST stromem
 * Element reprezentuje bud list a nebo uzel stromu, zjednodusuje tak praci mezi uzly a listy
 */
typedef struct{
    void *data;     /**< Ukazatel na list, nebo uzel stromu */
    char type;      /**< Typ elementu, reprezentuje typ dat, na ktere element ukazuje - 'l'(eaf), 'n'(ode) */
} ast_elem_t;

/**
 * @brief List AST stromu
 * List reprezentuje koncovy element abstraktniho syntaktickeho stromu
 */
typedef struct {
    char type;      /**< Typ listu, nabyva hodnot - 'v'(ariable) 'd'(ouble) 'i'(nteger) 's'(tring) 'f'(unction) 'h'(ashtable) */
    void *value;    /**< Ukazatel na hodnotu listu */
} ast_leaf_t;

/**
 * @brief Uzel AST stromu
 * Uzel reprezentuje nekoncovy element abstraktniho syntaktickeho stromu
 */
typedef struct ast_node_s {
    ast_node_type_t type;   /**< Typ uzlu */
    ast_elem_t **elems;     /**< Ukazatel na elementy, ktere uzel obsahuje */
    unsigned cap;           /**< Maximalni kapacita pole elementu*/
} ast_node_t;

/**
 * Vytvori a inicializuje abstraktni syntakticky strom.
 * @param N Pocet potomku
 * @param type Typ uzlu
 * @param **elem Element, ktery ma obsahovat uzel
 * @return E_INTERNAL v pripade chyby, jinak E_OK.
 */
error_t ast_create_node_elem(unsigned N, ast_node_type_t type, ast_elem_t **elem);

/**
 * Vytvori a inicializuje list AS stromu
 * @param ltype Typ listu
 * @param *value Pointer na hodnotu, na ktere je ulozen obsah listu
 * @param **elem Element, ktery ma obsahovat list
 * @return E_INTERNAL v pripade chyby, jinak E_OK.
 */
error_t ast_create_leaf_elem(char ltype, void *value, ast_elem_t **elem);

/**
 * Zvysi pocet potomku o 1
 * @param *node Vstupni uzel
 * @return E_INTERNAL v pripade chyby, jinak E_OK
 */
error_t ast_increase_size(ast_node_t *node);


/**
 * Znici abstraktni syntakticky strom
 * @param *root Koren stromu
 */
//void ast_destroy(ast_elem_t *root);

#endif // IFJ_AST_H