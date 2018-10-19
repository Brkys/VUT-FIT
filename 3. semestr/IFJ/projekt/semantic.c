/**
 * @file semantic.c
 * @author Simeon Borko, Vojtech Chadima
 * @date 11-12 2017
 * @brief Soubor obsahujujici pomocne funkce pro semanticku analyzu
 */

#include "semantic.h"

error_t sem_create_fn_leaf(hashtable_t *ht_fn, hashtable_t *ht_var, list_t *params, char *function_name,
                            char fn_return_type, ast_elem_t **fn_ast_el, bool defined, list_t *fn_list) {
    error_t err;
    function_t *function = trash_add(sizeof(struct function_s), &err);
    if(err != E_OK)
    {
        return E_INTERNAL;
    }
	if (function == NULL) {//doslo-li k chybe mallocu
		return E_INTERNAL;
	}

	function->name = function_name;
    function->params = params;
	function->return_type = fn_return_type;
    function->local_symtable = ht_var;
	function->defined = defined;

	if (!ht_insert_pair(ht_fn, function_name, function)) {
        //nastala interni chyba
        return E_INTERNAL;
    }

    //vlozime novou funkci do seznamu funkci (ten vyuzijeme pote ke kontrole toho, zdali byla kazda funkce definovana)
    if(!list_insert_last(fn_list, function, 0)) {
        return E_INTERNAL;
    }

    // vytvoreni AST listu
    err = ast_create_leaf_elem('f', function, fn_ast_el);
    if (err != E_OK)
        return err;

    return E_OK;
}

error_t sem_init_list_and_symtable(list_t **list_ptr, hashtable_t **ht_ptr) {
	// inicializace seznamu
	*list_ptr = list_init();
	if (*list_ptr == NULL) {
        return E_INTERNAL;
    }

	// vytvoreni symtable
	*ht_ptr = ht_init();
	if (*ht_ptr == NULL) {
		return E_INTERNAL;
	}

    return E_OK;
}

error_t sem_save_param(list_t *list, hashtable_t *ht_var, char *var_name, char data_type) {
    error_t err;
    variable_t *var = trash_add(sizeof(variable_t), &err);
    if(err != E_OK)
        return E_INTERNAL;
    if (var == NULL)
        return E_INTERNAL;

    var->name = var_name;
    var->data_type = data_type;

    // vlozeni do tabulky promennych
    if (ht_var != NULL) {
        if (!ht_insert_pair(ht_var, var_name, var)) {
            return E_INTERNAL;
        }
    }

    // vlozeni na konec seznamu parametru
    if (!list_insert_last(list, var, 0)) {
        return E_INTERNAL;
    }

    return E_OK;
}

//zkotroluje vsechny funkce (ulozene v seznamu dunkci) zdali jsou definovane, pokud ne, vrati semantickou chybu E_SEM_DEF
error_t sem_check_every_fn_defined(list_t *list_fn){

    // prvek seznamu
    list_el_ptr elem_ptr = list_fn->first;

    //dokud jsme nedosli na konec seznamu
    while (elem_ptr != NULL){
        //nebyla-li funkce definovana, ukonci se s chybou

        function_t *function = elem_ptr->data;
        if (function->defined == false)
            return E_SEM_DEF;

        //funkce byla definovana -> presouvame se na dalsi funkci v seznamu funkci
        else
            elem_ptr = elem_ptr->ptr_next;
    }
    //vse ok
    return E_OK;
}


error_t sem_add_var_to_ht(hashtable_t *ht_var, char *var_name, char data_type, ast_elem_t **var_el) {

    // kontrola, zda neexistuje funkce se stejnym nazvem
    if (ht_read(ht_fn, var_name))
        return E_SEM_DEF;

    // kontrola zda promenna uz neni v tabulce promennych
    variable_t *var = ht_read(ht_var, var_name);
    if (var != NULL) {
        return E_SEM_DEF;
    }

    // vytvorime promennou a nastavime jeji vlastnosti
    error_t err;
    var = trash_add(sizeof(variable_t), &err);
    if(err != E_OK)
        return E_INTERNAL;
    if (var == NULL) {
        return E_INTERNAL;
    }
    var->data_type = data_type;
    var->name = var_name;

    // vlozeni do tabulky promennych
    if(!ht_insert_pair(ht_var, var_name, var)) {
        return E_INTERNAL;
    }

    err = ast_create_leaf_elem('v', var, var_el);
    if (err != E_OK) {
        return err;
    }

    return E_OK;
}

error_t sem_create_var_leaf_el(hashtable_t *ht_var, char *var_name, ast_elem_t **var_el) {
    // kontrola zda promenna je v tabulce promennych
    variable_t *var = ht_read(ht_var, var_name);
    if (var == NULL) {
        return E_SEM_DEF;
    }

    error_t err = ast_create_leaf_elem('v', var, var_el);
    if (err != E_OK) {
        return err;
    }

    return E_OK;
}

error_t sem_add_one_fn_to_ht_fn(list_t *params, char *fn_name, char return_type) {
    error_t err;
    function_t *function = trash_add(sizeof(function_t), &err);
    if(err != E_OK)
        return E_INTERNAL;
    if (function == NULL) {
        return E_INTERNAL;
    }

    function->name = fn_name;
    function->params = params;
    function->defined = true;
    function->local_symtable = NULL;
    function->return_type = return_type;

    if (!ht_insert_pair(ht_fn, fn_name, function)) {
        return E_INTERNAL;
    }

    return E_OK;
}

error_t sem_add_built_in_fns_to_ht_fn() {

    list_t *params;

    // funkce length
    if ((params = list_init()) == NULL ||
        sem_save_param(params, NULL, "s", 's') != E_OK ||
        sem_add_one_fn_to_ht_fn(params, "length", 'i') != E_OK) {
        return E_INTERNAL;
    }

    // funkce substr
    if ((params = list_init()) == NULL ||
        sem_save_param(params, NULL, "s", 's') != E_OK ||
        sem_save_param(params, NULL, "i", 'i') != E_OK ||
        sem_save_param(params, NULL, "n", 'i') != E_OK ||
        sem_add_one_fn_to_ht_fn(params, "substr", 's') != E_OK) {
        return E_INTERNAL;
    }

    // asc
    if ((params = list_init()) == NULL ||
        sem_save_param(params, NULL, "s", 's') != E_OK ||
        sem_save_param(params, NULL, "i", 'i') != E_OK ||
        sem_add_one_fn_to_ht_fn(params, "asc", 'i') != E_OK) {
        return E_INTERNAL;
    }

    // chr
    if ((params = list_init()) == NULL ||
        sem_save_param(params, NULL, "i", 'i') != E_OK ||
        sem_add_one_fn_to_ht_fn(params, "chr", 's') != E_OK) {
        return E_INTERNAL;
    }

    return E_OK;

}