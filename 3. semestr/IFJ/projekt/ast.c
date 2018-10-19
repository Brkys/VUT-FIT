/**
 * @file ast.c
 * @author Simeon Borko, Zdenek Brhel
 * @date 11-12 2017
 * @brief Soubor obsahujujici vytvareni a pracu s AST stromem
 */

#include "ast.h"

error_t ast_create_node_elem(unsigned N, ast_node_type_t type, ast_elem_t **elem)
{
    // alokovat misto na element
    error_t err;
    *elem = trash_add(sizeof(ast_elem_t), &err); //alokuje element ukazujici na uzel
    if(err != E_OK)
        return E_INTERNAL;

    // alokovat misto na uzel
    ast_node_t *node = trash_add(sizeof(ast_node_t), &err); //alokuje uzel
    if(err != E_OK)
    {
        return E_INTERNAL;
    }

    // napojit uzel na element
    (*elem)->data = node;
    (*elem)->type = 'n';

    // nastavit typ uzlu a pocet synu
    node->type = type;
    node->cap = N;

    // alokovat prostor pro syny
    if (N) {
        node->elems = trash_add_calloc(N, sizeof(ast_elem_t *), &err);
        if (err != E_OK) {
            return E_INTERNAL;
        }
    } else {
        node->elems = NULL;
    }

    return E_OK;
}

error_t ast_create_leaf_elem(char ltype, void *value, ast_elem_t **elem)
{
    //listy se musi rovnat 'v(ariable)', 'd(ouble)', 'i(nteger)', 's(tring)', 'f(unction)', 'h'(ashtable)
    if(ltype != 'v' && ltype != 'd' && ltype != 'i' && ltype != 's' && ltype != 'f' && ltype != 'h')
        return E_INTERNAL;

    //alokuje pamet elementu ukazujici na list
    error_t err;
    *elem = trash_add(sizeof(ast_elem_t), &err);
    if(err != E_OK)
        return E_INTERNAL;


    ast_leaf_t *leaf = trash_add(sizeof(ast_leaf_t), &err); //alokuje list
    if(err != E_OK)
    {
        return E_INTERNAL;
    }

    leaf->type = ltype; //nastavi hodnoty listu
    leaf->value = value;

    (*elem)->type = 'l'; //nastavy typ elementu na list a propoji element s listem
    (*elem)->data = leaf;

    return E_OK;
}

error_t ast_increase_size(ast_node_t *node)
{
    if(node == NULL) 
        return E_INTERNAL;

    error_t err;

    if (node->cap > 0)
    {
        node->elems = trash_add_realloc(node->elems, sizeof(ast_elem_t *) * (node->cap + 1), &err);
        if(err != E_OK)
            return E_INTERNAL;
    }
    else
        node->elems = trash_add(sizeof(ast_elem_t *), &err);

    if(err != E_OK)
        return E_INTERNAL;

    // inicializace noveho syna
    node->elems[node->cap] = NULL;

    // inkrementace poctu synu
    node->cap++;

    return E_OK;
}
