/**
 * @file recursive_descent.c
 * @author Vojtech Chadima, Simeon Borko, Zdenek Brhel
 * @date 11-12 2017
 * @brief Soubor obsahujujici algoritmus rekurzivniho sestupu syntakticke analyzy
 */

#include "recursive_descent.h"


#define tuttifruttimakro(err) { fprintf(stderr, "Chyba : %s\t: %s\t: %d\n", __FILE__, __func__, __LINE__); return err; }

// TODO abstrakna uvolnovaci funkcia?? pre next_desc_fn
#define next_desc_fn(desc_xxx, ...); {error_t err_result = desc_xxx(buffer, token_ptr, __VA_ARGS__); if (err_result != E_OK) return err_result; }

#define token_get_macro(); {error_t result = token_get(token_ptr, buffer); if (result != E_OK) return result; if(*token_ptr == NULL) return E_SYNTACTIC; }

#define incompatible_types(X, Y) ((X == 's') != (Y == 's'))


// TODO dim nemoze byt vo while

// globalni tabulka funkci

char *copy_string(char *src) {
    error_t err;
    char *dst = trash_add_calloc(strlen(src) + 1, sizeof(char), &err);
    if (dst == NULL)
        return NULL;
    return strcpy(dst, src);
}

error_t recursive_descent(buffer_t *buffer, token_t **token_ptr, ast_elem_t **root_el) {

    // vytvoreni seznamu funkci a vytvoreni (globalni) tabulky funkci
    error_t err = sem_init_list_and_symtable(&fn_list, &ht_fn);
    if (err != E_OK)
        tuttifruttimakro(err);

    // pridani vestavenych funkci do tabulky funkci
    sem_add_built_in_fns_to_ht_fn();

    // vynechani prvniho EOLu
    do {
        token_get_macro();
    } while (*token_ptr && (*token_ptr)->type == T_EOL);

    // vytvoreni korenoveho uzlu
    *root_el = NULL;
    err = ast_create_node_elem(0, AT_ROOT, root_el);
    if (err != E_OK) {
        tuttifruttimakro(err);
    }

    // prog
    next_desc_fn(desc_prog, (*root_el)->data);

    return E_OK;
}


// ten kto vola tuto funkciu ma precitat token a kdyz to je EOL, precist dalsi
error_t desc_prog(buffer_t *buffer, token_t **token_ptr, ast_node_t *root_node) {

    if (!strcmp((*token_ptr)->value, "declare")) { // pravidlo 1

        // ziskani 'function' tokenu
        token_get_macro();

        // header ocekava nacteny 'function' token
        next_desc_fn(desc_function_header, false, NULL, NULL);

        next_desc_fn(desc_prog, root_node);

    } else if (!strcmp((*token_ptr)->value, "function")) { // pravidlo 2

        hashtable_t *ht_var;
        ast_elem_t *fn_ast_el;
        ast_elem_t *stats_ast_el;

        // ocekava nacteny 'function' token
        next_desc_fn(desc_function_header, true, &fn_ast_el, &ht_var);

        // ziskani funkce
        function_t *function = ((ast_leaf_t *)(fn_ast_el->data))->value;

        // ziskani uzlu Stats
        next_desc_fn(desc_get_stats, ht_var, function, &stats_ast_el);

        if ((*token_ptr)->type != T_KEYWORD || strcmp((*token_ptr)->value, "end") != 0)
            tuttifruttimakro(E_SYNTACTIC);
        token_get_macro();

        if ((*token_ptr)->type != T_KEYWORD || strcmp((*token_ptr)->value, "function") != 0)
            tuttifruttimakro(E_SYNTACTIC);
        token_get_macro();

        if ((*token_ptr)->type != T_EOL)
            tuttifruttimakro(E_SYNTACTIC);
        token_get_macro();


        // pridani syna do korenoveho uzlu

        // vytvoreni uzlu Definice
        ast_elem_t *def_el = NULL;
        error_t err = ast_create_node_elem(2, AT_DEFINITION, &def_el);
        if (err != E_OK) {
            tuttifruttimakro(err);
        }

        // pridani odkazu na funkci jakozto syna uzlu Definice
        ast_node_t *def_node = def_el->data;
        def_node->elems[0] = fn_ast_el;

        // pridani tela funkce jakozto druheho syna uzlu Definice
        def_node->elems[1] = stats_ast_el;

        // zvacseni kapacity korenoveho uzlu
        err = ast_increase_size(root_node);
        if (err != E_OK) {
            tuttifruttimakro(err);
        }

        // pridani jednoho ditete korenovemu uzlu
        root_node->elems[root_node->cap - 1] = def_el;

        next_desc_fn(desc_prog, root_node);

    } else if (!strcmp((*token_ptr)->value, "scope")) { // pravidlo 3

        //zkontrolujeme jestli byla kazda funkce definovana (kazda funkce totiz musi byt definovana prave jednou)
        error_t err = sem_check_every_fn_defined(fn_list);
        if (err != E_OK)
            return  err;

        token_get_macro();

        // EOL
        if ((*token_ptr)->type != T_EOL)
            tuttifruttimakro(E_SYNTACTIC);
        token_get_macro();

        // Vytvoreni tabulky symbolu (promennych) pro telo scopu
        hashtable_t *ht_var = ht_init();
        if (ht_var == NULL) {
            tuttifruttimakro(E_INTERNAL); // TODO uvolnit pamet?
        }

        ast_elem_t *stats_el = NULL;
        next_desc_fn(desc_get_stats, ht_var, NULL, &stats_el); // TODO uvolnit nejaku pamet?

        if ((*token_ptr)->type != T_KEYWORD || strcmp((*token_ptr)->value, "end") != 0)
            tuttifruttimakro(E_SYNTACTIC);
        token_get_macro();

        if ((*token_ptr)->type != T_KEYWORD || strcmp((*token_ptr)->value, "scope") != 0)
            tuttifruttimakro(E_SYNTACTIC);

        err = token_get(token_ptr, buffer);
        if (err != E_OK)
            return err;

        // zaisti, aby za 'end scope' mohl byt libovolny pocet tokenu EOL (i kdyz realne se vrati nanejvys jeden), ale zadnych jinych
        while (*token_ptr != NULL) {
            if ((*token_ptr)->type != T_EOL)
                tuttifruttimakro(E_SYNTACTIC); // TODO uvolnit pamat
            error_t err = token_get(token_ptr, buffer);
            if (err != E_OK)
                return err;
        }

        // vytvorime uzel Scope
        ast_elem_t *scope_el = NULL;
        err = ast_create_node_elem(1, AT_SCOPE, &scope_el);
        if (err != E_OK) {
            tuttifruttimakro(err);
        }

        // pridame uzlu Scope jeho dva syny
        ast_node_t *scope_node = scope_el->data;
        scope_node->elems[0] = stats_el;

        // navazani uzlu Scope na korenovy uzel
        err = ast_increase_size(root_node);
        if (err != E_OK) {
            tuttifruttimakro(err);
        }

        root_node->elems[root_node->cap - 1] = scope_el;


    } else {
        tuttifruttimakro(E_SYNTACTIC);
    }

    return E_OK;
}

error_t desc_declare_function_params(buffer_t *buffer, token_t **token_ptr, list_t *params, bool first) {
    error_t err;
    if (!first) {
        // neni-li carka, funkce se konci uspesne
        if ((*token_ptr)->type != T_COMMA)
            return E_OK;
        token_get_macro();
    }

    if ((*token_ptr)->type != T_ID)
        return first ? E_OK : E_SYNTACTIC;

    token_get_macro();

    // as
    if ((*token_ptr)->type != T_KEYWORD || strcmp((*token_ptr)->value, "as") != 0) {
        tuttifruttimakro(E_SYNTACTIC);
    }
    token_get_macro();

    // type
    char data_type;
    next_desc_fn(desc_type, &data_type);

    // vytvoreni parametru
    variable_t *var = trash_add(sizeof(variable_t), &err);
    if(err != E_OK)
        return E_INTERNAL;
    if (!var) {
        tuttifruttimakro(E_INTERNAL);
    }
    var->name = NULL;
    var->data_type = data_type;

    // vlozeni parametru na seznam parametru
    if (!list_insert_last(params, var, 0)) {
        tuttifruttimakro(E_INTERNAL);
    }

    // dalsi parametr
    next_desc_fn(desc_declare_function_params, params, false);

    return E_OK;
}

error_t desc_declare_function(buffer_t *buffer, token_t **token_ptr, char *fn_name) {
    // seznam parametru
    error_t err;
    list_t *params = list_init();
    if (!params) {
        tuttifruttimakro(E_INTERNAL);
    }

    // spracovani parametru - naplneni seznamu bezejmennych parametru
    next_desc_fn(desc_declare_function_params, params, true);

    // )
    if ((*token_ptr)->type != T_BR_RIGHT) {
        tuttifruttimakro(E_SYNTACTIC);
    }
    token_get_macro();

    // as
    if ((*token_ptr)->type != T_KEYWORD || strcmp((*token_ptr)->value, "as") != 0) {
        tuttifruttimakro(E_SYNTACTIC);
    }
    token_get_macro();

    // type
    char data_type;
    next_desc_fn(desc_type, &data_type);

    // vytvoreni funkce
    function_t *fn = trash_add(sizeof(function_t), &err);
    if(err != E_OK)
        return E_INTERNAL;
    if (!fn) {
        tuttifruttimakro(E_INTERNAL);
    }
    fn->name = fn_name;
    fn->params = params;
    fn->return_type = data_type;
    fn->local_symtable = NULL;
    fn->defined = false;

    // vlozeni do tabulky funkci
    if (!ht_insert_pair(ht_fn, fn_name, fn)) {
        tuttifruttimakro(E_INTERNAL);
    }

    // vlozeni na seznam funkci
    if (!list_insert_last(fn_list, fn, 0)) {
        tuttifruttimakro(E_INTERNAL);
    }

    return E_OK;
}

error_t desc_define_function(buffer_t *buffer, token_t **token_ptr,
                             char *fn_name,
                             hashtable_t **ht_var, ast_elem_t **fn_ast_el) {
    // inicializace seznamu parametru a tabulky promennych
    list_t *params;
    error_t err = sem_init_list_and_symtable(&params, ht_var); // TODO korektni uvolneni v pripade chyb
    if (err != E_OK)
        tuttifruttimakro(err);

    // naplnit seznam parametru a vkladat do tabulky promennych
    next_desc_fn(desc_define_function_params, params, *ht_var, true);

    // )
    if ((*token_ptr)->type != T_BR_RIGHT) {
        tuttifruttimakro(E_SYNTACTIC);
    }
    token_get_macro();

    // as
    if ((*token_ptr)->type != T_KEYWORD || strcmp((*token_ptr)->value, "as") != 0) {
        tuttifruttimakro(E_SYNTACTIC);
    }
    token_get_macro();

    // type
    char data_type;
    next_desc_fn(desc_type, &data_type);

    err = sem_create_fn_leaf(ht_fn, *ht_var, params, fn_name, data_type, fn_ast_el, true, fn_list);
    if (err != E_OK) {
        tuttifruttimakro(err);
    }

    return E_OK;
}

error_t desc_check_function(buffer_t *buffer, token_t **token_ptr,
                            function_t *fn,
                            hashtable_t **ht_var, ast_elem_t **fn_ast_el
) {
    if (fn->defined) {
        tuttifruttimakro(E_SEM_DEF);
    }

    // vytvoreni tabulky promennych
    *ht_var = ht_init();
    if (*ht_var == NULL) {
        tuttifruttimakro(E_INTERNAL);
    }

    // porovnani poctu a typu parametru, pridani nazvu, pridani do tabulky promennych
    next_desc_fn(desc_check_function_params, *ht_var, fn->params->first, true);

    // )
    if ((*token_ptr)->type != T_BR_RIGHT) {
        tuttifruttimakro(E_SYNTACTIC);
    }
    token_get_macro();

    // as
    if ((*token_ptr)->type != T_KEYWORD || strcmp((*token_ptr)->value, "as") != 0) {
        tuttifruttimakro(E_SYNTACTIC);
    }
    token_get_macro();

    // type
    char data_type;
    next_desc_fn(desc_type, &data_type);

    if (fn->return_type != data_type)
        tuttifruttimakro(E_SEM_DEF);

    // ted je hlavicka funkce v poradku, nastavime na definovanou a priradime tabulku promennych
    fn->defined = true;
    fn->local_symtable = *ht_var;

    // vytvoreni AST listu
    error_t err = ast_create_leaf_elem('f', fn, fn_ast_el);
    if (err != E_OK)
        tuttifruttimakro(err);

    return E_OK;
}

// predpoklada, ze prvy token uz bol nacitany
error_t desc_function_header(buffer_t *buffer, token_t **token_ptr,
                             bool definition, // vstupne parametre
                             ast_elem_t **fn_ast_el, hashtable_t **ht_var // vystupne parametre
) {

    if (strcmp((*token_ptr)->value, "function") != 0)
        tuttifruttimakro(E_SYNTACTIC);

    token_get_macro();
    if ((*token_ptr)->type != T_ID)
        tuttifruttimakro(E_SYNTACTIC);

    char *fn_name = (*token_ptr)->value; // TODO korektni uvolneni v pripade chyby
    function_t *function = ht_read(ht_fn, fn_name);

    token_get_macro();

    if ((*token_ptr)->type != T_BR_LEFT)
        tuttifruttimakro(E_SYNTACTIC);

    token_get_macro();

	//je-li toto deklarace 
	if (!definition ){
		if (function != NULL) {    //je-li to deklarace a v tabulce je jiz zaznam pro funkci se stejnym jmenem, jedna se o sematickou chybu
            tuttifruttimakro(E_SEM_DEF);
        }
		else {
			// spracovani parametru, typu, ...
            next_desc_fn(desc_declare_function, fn_name);
        }
	}

	else{	//je-li toto definice
        if (function == NULL) {
            // definovani funkce s vytvorenim AST listu
            next_desc_fn(desc_define_function, fn_name, ht_var, fn_ast_el);
        } else {
            // skontrolovani parametru a typu funkce, vytvoreni tabulky promennych a AST listu
            next_desc_fn(desc_check_function, function, ht_var, fn_ast_el);
        }
	}

    if ((*token_ptr)->type != T_EOL)
        tuttifruttimakro(E_SYNTACTIC);

    // nacteni dalsiho tokenu
    token_get_macro();

    return E_OK;
}

error_t desc_define_function_params(buffer_t *buffer, token_t **token_ptr, list_t *params, hashtable_t *ht_var, bool first) {
    if (!first) {

        // neni-li carka, funkce sa konci uspesne
        if ((*token_ptr)->type != T_COMMA)
            return E_OK;
        token_get_macro();
    }

    if ((*token_ptr)->type != T_ID)
        return first ? E_OK : E_SYNTACTIC;

    char *var_name = copy_string((*token_ptr)->value); // TODO spravne uvolnit


    //overeni, zdali nazev parametru neni shodny s nazvem jiz definovane funkce
    function_t *fn = ht_read(ht_fn, var_name);

    if ( fn != NULL )
        return E_SEM_DEF;	//v pripade, ze je nazev parametru funkce shodny s nazvem jiz definovane funkce vrati E_SEM_DEF


    // je-li parameter jiz v tabulce, znamena to, ze by funkce mela dva parametry se shodnym jmenem, coz je chyba
    variable_t *variable = ht_read(ht_var, (*token_ptr)->value);
    if (variable != NULL)
        tuttifruttimakro(E_SEM_DEF);
    token_get_macro();

    // as
    if ((*token_ptr)->type != T_KEYWORD || strcmp((*token_ptr)->value, "as") != 0) {
        tuttifruttimakro(E_SYNTACTIC);
    }
    token_get_macro();

    // type
    char data_type;
    next_desc_fn(desc_type, &data_type);

    // vytvoreni zaznamu v tabulce promennych a ulozeni parametru na konec zaznamu
    sem_save_param(params, ht_var, var_name, data_type);

    //rekurzivni volani sebe sama (dalsi parametr)
    next_desc_fn(desc_define_function_params, params, ht_var, false);

    return E_OK;
}

error_t desc_check_function_params(buffer_t *buffer, token_t **token_ptr,
                                   hashtable_t *ht_var, list_el_ptr param_el, bool first
) {
    if (!first) {
        // neni prvni parametr

        if ((*token_ptr)->type == T_COMMA) {
            // je carka
            if (param_el == NULL) {
                tuttifruttimakro(E_SEM_DEF);
            }

            // kdyz je carka a ocekavame dalsi parametr => pokracujeme
            token_get_macro();
        } else {
            // neni carka
            if (param_el == NULL) {
                return E_OK;
            } else {
                tuttifruttimakro(E_SEM_DEF);
            }
        }
    } else if (first && param_el == NULL) {
        return E_OK;
    }

    if ((*token_ptr)->type != T_ID) {
        tuttifruttimakro(E_SYNTACTIC);
    }

    if (ht_read(ht_var, (*token_ptr)->value)) {
        // parametr s takovym jmenem jiz je v tabulce promennych
        return E_SEM_DEF;
    }

    variable_t *param_var = param_el->data;
    param_var->name = copy_string((*token_ptr)->value);
    // vlozeni do tabulky promennych
    if (!ht_insert_pair(ht_var, param_var->name, param_var)) {
        tuttifruttimakro(E_INTERNAL);
    }

    token_get_macro();

    // as
    if ((*token_ptr)->type != T_KEYWORD || strcmp((*token_ptr)->value, "as") != 0) {
        tuttifruttimakro(E_SYNTACTIC);
    }
    token_get_macro();

    // type
    char data_type;
    next_desc_fn(desc_type, &data_type);

    if (param_var->data_type != data_type) {
        tuttifruttimakro(E_SEM_DEF);
    }

    next_desc_fn(desc_check_function_params, ht_var, param_el->ptr_next, false);

    return E_OK;
}

error_t desc_type(buffer_t *buffer, token_t **token_ptr, char *data_type) {

    if ((*token_ptr)->type != T_KEYWORD)
        tuttifruttimakro(E_SYNTACTIC);

    if (strcmp((*token_ptr)->value, "integer") == 0)
        *data_type = 'i';
    else if (strcmp((*token_ptr)->value, "double") == 0)
        *data_type = 'd';
    else if (strcmp((*token_ptr)->value, "string") == 0)
        *data_type = 's';
    else
        tuttifruttimakro(E_SYNTACTIC);

    token_get_macro();

    return E_OK;
}

// ziska ast uzel pro statements
error_t desc_get_stats(buffer_t *buffer, token_t **token_ptr,
                       hashtable_t *ht_var, function_t *function,
                       ast_elem_t **stats_el
) {

    // vytvoreni uzlu Stats
    error_t err = ast_create_node_elem(0, AT_STATS, stats_el);
    if (err != E_OK) {
        return err;
    }

    next_desc_fn(desc_st_list, ht_var, (*stats_el)->data, function);

    return E_OK;
}

error_t desc_st_list(buffer_t *buffer, token_t **token_ptr, hashtable_t *ht_var, ast_node_t *stats_node, function_t *function) {

	//simulace pravidla cislo 10 (s epsilonem)
    if ((*token_ptr)->type == T_KEYWORD && 
    	(strcmp((*token_ptr)->value, "end") == 0  ||
    	 strcmp((*token_ptr)->value, "else") == 0 ||
    	 strcmp((*token_ptr)->value, "loop") == 0  ))

        return E_OK;

    ast_elem_t *stat_el = NULL;

    next_desc_fn(desc_state, ht_var, function, &stat_el);

    // Pridani elementu stat_el jakozto syna stats_node
    error_t err = ast_increase_size(stats_node);
    if (err != E_OK) {
        tuttifruttimakro(err);
    }
    stats_node->elems[stats_node->cap - 1] = stat_el;

    if ((*token_ptr)->type != T_EOL)
        tuttifruttimakro(E_SYNTACTIC);
    token_get_macro();

    next_desc_fn(desc_st_list, ht_var, stats_node, function);

    return E_OK;
}


error_t desc_state(buffer_t *buffer, token_t **token_ptr, hashtable_t *ht_var, function_t *function, ast_elem_t **stat_el) {


	//simulace pravidla 18 (podminka if-then-else)
    if ((*token_ptr)->type == T_KEYWORD && strcmp((*token_ptr)->value, "if") == 0) {
        token_get_macro();

        // Vytvoreni elementu COND_*
        ast_elem_t *condition_el = NULL;
        error_t err = precedence_algorithm(token_ptr, buffer, ht_var, true, &condition_el, NULL);
        if (err != E_OK) {
            tuttifruttimakro(err);
        }

        // then
    	if ((*token_ptr)->type != T_KEYWORD || strcmp((*token_ptr)->value, "then") != 0)
        	tuttifruttimakro(E_SYNTACTIC);
        token_get_macro();

        // EOL
    	if ((*token_ptr)->type != T_EOL)
        	tuttifruttimakro(E_SYNTACTIC);
        token_get_macro();

        // Vytvoreni elementu STATS pro vetev THEN
        ast_elem_t *stats_then_el = NULL;
        next_desc_fn(desc_get_stats, ht_var, function, &stats_then_el);

        // else
    	if ((*token_ptr)->type != T_KEYWORD || strcmp((*token_ptr)->value, "else") != 0)
        	tuttifruttimakro(E_SYNTACTIC);
        token_get_macro();

        // EOL
    	if ((*token_ptr)->type != T_EOL)
        	tuttifruttimakro(E_SYNTACTIC);
        token_get_macro();

        // Vytvoreni elementu STATS pro vetev ELSE
        ast_elem_t *stats_else_el = NULL;
        next_desc_fn(desc_get_stats, ht_var, function, &stats_else_el);

        // end
    	if ((*token_ptr)->type != T_KEYWORD || strcmp((*token_ptr)->value, "end") != 0)
        	tuttifruttimakro(E_SYNTACTIC);
        token_get_macro();

        // if
    	if ((*token_ptr)->type != T_KEYWORD || strcmp((*token_ptr)->value, "if") != 0)
        	tuttifruttimakro(E_SYNTACTIC);
        token_get_macro();

        err = ast_create_node_elem(3, AT_IF, stat_el);
        if (err != E_OK) {
            tuttifruttimakro(err);
        }
        ast_node_t *node = (*stat_el)->data;
        node->elems[0] = condition_el;
        node->elems[1] = stats_then_el;
        node->elems[2] = stats_else_el;

        return E_OK;
    }

    else if ((*token_ptr)->type == T_KEYWORD && strcmp((*token_ptr)->value, "do") == 0){
        token_get_macro();

        if ((*token_ptr)->type != T_KEYWORD || strcmp((*token_ptr)->value, "while") != 0)
        {tuttifruttimakro(E_SYNTACTIC);}
        token_get_macro();

        // Vytvoreni elementu COND_*
        ast_elem_t *condition_el = NULL;
        error_t err = precedence_algorithm(token_ptr, buffer, ht_var, true, &condition_el, NULL);
        if (err != E_OK) {
            tuttifruttimakro(err);
        }

        if ((*token_ptr)->type != T_EOL)
            tuttifruttimakro(E_SYNTACTIC);
        token_get_macro();

        // Vytvoreni elementu STATS pro telo while
        ast_elem_t *body_el = NULL;
        next_desc_fn(desc_get_stats, ht_var, function, &body_el);

        if ((*token_ptr)->type != T_KEYWORD || strcmp((*token_ptr)->value, "loop") != 0)
            tuttifruttimakro(E_SYNTACTIC);
        token_get_macro();

        err = ast_create_node_elem(2, AT_WHILE, stat_el);
        if (err != E_OK) {
            tuttifruttimakro(err);
        }
        ast_node_t *node = (*stat_el)->data;
        node->elems[0] = condition_el;
        node->elems[1] = body_el;

        return E_OK;
    }

    //simulace pravidla 20 (dim ... as...)
    else if ((*token_ptr)->type == T_KEYWORD && strcmp((*token_ptr)->value, "dim") == 0) {
    	token_get_macro();

    	if ((*token_ptr)->type != T_ID)
        	tuttifruttimakro(E_SYNTACTIC);

        // ulozeni jmena promenne
        char *var_name = copy_string((*token_ptr)->value);

        token_get_macro();

        // as
        if ((*token_ptr)->type != T_KEYWORD || strcmp((*token_ptr)->value, "as") != 0) {
            tuttifruttimakro(E_SYNTACTIC);
        }
        token_get_macro();

        // datovy typ
        char var_data_type;
        next_desc_fn(desc_type, &var_data_type);

        // pridani promenne do tabulky promennych a vytvoreni AST listu
        ast_elem_t *var_el = NULL;
        error_t err = sem_add_var_to_ht(ht_var, var_name, var_data_type, &var_el);
        if (err != E_OK) {
            tuttifruttimakro(err);
        }

        // zjisteni inicializacniho vyrazu
        ast_elem_t *init_el = NULL;
        char expr_data_type;
        next_desc_fn(desc_ival, ht_var, &init_el, &expr_data_type);

        if (init_el) {
            // ak ma promenna inicializacni hodnotu

            if (incompatible_types(var_data_type, expr_data_type)) {
                tuttifruttimakro(E_SEM_TYPE_OR_PARAMS);
            }
        }

        // vytvoreni uzlu DIM
        // kdyz neni inicializacni hodnota explicitne uvedena, bude mit uzel pouze jednoho syna
        err = ast_create_node_elem(init_el == NULL ? 1 : 2, AT_DIM, stat_el);
        if (err != E_OK) {
            tuttifruttimakro(err);
        }

        // nastaveni synu uzlu DIM
        ast_node_t *node = (*stat_el)->data;
        node->elems[0] = var_el;
        if (init_el != NULL)
            node->elems[1] = init_el;

        // dalsi token je nacteny, protoze desc_ival ho nacetl

        return E_OK;
    }

    else if ((*token_ptr)->type == T_ID){

        // vytvoreni listu pro promennou
        ast_elem_t *var_el = NULL;
        error_t err = sem_create_var_leaf_el(ht_var, (*token_ptr)->value, &var_el);
        if (err != E_OK) {
            tuttifruttimakro(err);
        }

        // zjisteni typu promenne
        char var_type = ((variable_t*)(((ast_leaf_t*)(var_el->data))->value))->data_type;

        token_get_macro();

        // =
        if ((*token_ptr)->type != T_EQUAL) {
            tuttifruttimakro(E_SYNTACTIC);
        }
        token_get_macro();

        ast_elem_t *el_1 = NULL;
        ast_elem_t *el_2 = NULL;
        char right_value_type;
        next_desc_fn(desc_right_value, ht_var, &el_1, &el_2, &right_value_type);

        if (incompatible_types(right_value_type, var_type)) {
            tuttifruttimakro(E_SEM_TYPE_OR_PARAMS);
        }

        if (el_2 == NULL) {
            // vyraz
            err = ast_create_node_elem(2, AT_ASSIGN_EXPR, stat_el);
            if (err != E_OK) {
                tuttifruttimakro(err);
            }

            ast_node_t *assign_node = (*stat_el)->data;
            assign_node->elems[0] = var_el;
            assign_node->elems[1] = el_1;
        } else {
            // volani funkce
            err = ast_create_node_elem(3, AT_ASSIGN_FN, stat_el);
            if (err != E_OK) {
                tuttifruttimakro(err);
            }

            ast_node_t *assign_node = (*stat_el)->data;
            assign_node->elems[0] = var_el;
            assign_node->elems[1] = el_1;
            assign_node->elems[2] = el_2;
        }

        return E_OK;
    }

    else if ((*token_ptr)->type == T_KEYWORD && strcmp((*token_ptr)->value, "print") == 0) {

    	token_get_macro();

        error_t err = ast_create_node_elem(1, AT_PRINT, stat_el);
        if (err != E_OK) {
            tuttifruttimakro(err);
        }

        ast_node_t *print_node = (*stat_el)->data;

        // prvni vyraz printu
        err = precedence_algorithm(token_ptr, buffer, ht_var, false, &print_node->elems[0], NULL);
        if (err != E_OK) {
            tuttifruttimakro(err);
        }

    	if ((*token_ptr)->type != T_SEMICOLON)
    		tuttifruttimakro(E_SYNTACTIC);
        token_get_macro();

    	next_desc_fn(desc_printlist, ht_var, print_node);

    	return E_OK;
    }

    else if (function != NULL && (*token_ptr)->type == T_KEYWORD && strcmp((*token_ptr)->value, "return") == 0){
    	token_get_macro();

        error_t err = ast_create_node_elem(2, AT_RETURN, stat_el);
        if (err != E_OK) {
            tuttifruttimakro(err);
        }

        ast_node_t *node = (*stat_el)->data;

        char expr_type;
        err = precedence_algorithm(token_ptr, buffer, ht_var, false, &node->elems[1], &expr_type);
        if (err != E_OK) {
            tuttifruttimakro(err);
        }

        if (incompatible_types(expr_type, function->return_type)) {
            tuttifruttimakro(E_SEM_TYPE_OR_PARAMS);
        }

        err = ast_create_leaf_elem('f', function, &node->elems[0]);
        if (err != E_OK) {
            tuttifruttimakro(err);
        }

    	return E_OK;
    }

    else if ((*token_ptr)->type == T_KEYWORD && strcmp((*token_ptr)->value, "input") == 0) {
        token_get_macro();

    	if ((*token_ptr)->type != T_ID)
        	tuttifruttimakro(E_SYNTACTIC);

        variable_t *var = ht_read(ht_var, (*token_ptr)->value);
        if (var == NULL) {
            tuttifruttimakro(E_SEM_DEF);
        }

        error_t err = ast_create_node_elem(1, AT_INPUT, stat_el);
        if (err != E_OK) {
            tuttifruttimakro(err);
        }

        // ulozit odkaz na promennou jako syna uzlu Input
        err = ast_create_leaf_elem('v', var, &((ast_node_t*)((*stat_el)->data))->elems[0]);
        if (err != E_OK) {
            tuttifruttimakro(err);
        }

        token_get_macro();

        return E_OK;
    }

    else{
    	tuttifruttimakro(E_SYNTACTIC);
    }

    return E_OK;
}

error_t desc_ival(buffer_t *buffer, token_t **token_ptr, hashtable_t *ht_var, ast_elem_t **init_el, char *data_type) {
	if ((*token_ptr)->type == T_EQUAL) {
        token_get_macro();

        error_t err = precedence_algorithm(token_ptr, buffer, ht_var, false, init_el, data_type);
        if (err != E_OK) {
            tuttifruttimakro(err);
        }
    }

    return E_OK;
}


error_t desc_printlist(buffer_t *buffer, token_t **token_ptr, hashtable_t *ht_var, ast_node_t *print_node) {

    // simulace epsilon pravidla
    if ((*token_ptr)->type == T_EOL) {
        return E_OK;
    }

    //nyni zpracovavame vyraz, musime proto zavolat precedencni analyzator (protoze ten se stara o zpracovani vyrazu)
    ast_elem_t *expr_el = NULL;
    error_t err = precedence_algorithm(token_ptr, buffer, ht_var, false, &expr_el, NULL);
    if (err != E_OK) {
        tuttifruttimakro(err);
    }

    // ak sa vyraz nasiel, pokracujeme
    if (expr_el != NULL) {
        err = ast_increase_size(print_node);
        if (err != E_OK) {
            tuttifruttimakro(err);
        }
        print_node->elems[print_node->cap - 1] = expr_el;

        if ((*token_ptr)->type != T_SEMICOLON) {
            tuttifruttimakro(E_SYNTACTIC);
        }
        token_get_macro();

        next_desc_fn(desc_printlist, ht_var, print_node);
    }

    return E_OK;
}

error_t desc_right_value(buffer_t *buffer, token_t **token_ptr, hashtable_t *ht_var,
                         ast_elem_t **el_1, ast_elem_t **el_2, char *data_type) {
	// mozna je to volani funkce
    if ((*token_ptr)->type == T_ID || (*token_ptr)->type == T_KEYWORD) {

        char *fn_name = (*token_ptr)->value;
        function_t *function = ht_read(ht_fn, fn_name);

        /* Pokud je token typu KEYWORD a zaroven se hodnota tokenu najde v tabulce funkci, urcite se jedna
         * o vestavenou funkci (length, substr, asc, chr).
         */

        // pokud se nasla vestavena nebo uzivatelska funkce
        if (function != NULL) {

            // zjisteni navratoveho typu funkce
            *data_type = function->return_type;

            // vytvoreni AST listu pro funkci
            error_t err = ast_create_leaf_elem('f', function, el_1);
            if (err != E_OK) {
                tuttifruttimakro(err);
            }

            token_get_macro();

            // (
            if ((*token_ptr)->type != T_BR_LEFT)
                tuttifruttimakro(E_SYNTACTIC);
            token_get_macro();

            // vytvoreni uzlu PARAMS
            err = ast_create_node_elem(0, AT_PARAMS, el_2);
            if (err != E_OK) {
                tuttifruttimakro(err);
            }
            ast_node_t *params_node = (*el_2)->data;

            next_desc_fn(desc_real_params, ht_var, params_node, function->params->first, true);

            // )
            if ((*token_ptr)->type != T_BR_RIGHT)
                tuttifruttimakro(E_SYNTACTIC);
            token_get_macro();

            return E_OK;
        }

        //v pripade, ze identifikator neni identifikatorem funkce, je jeste sance, ze muze jit o promennou
        else if ((*token_ptr)->type == T_ID) {
            // existuje takova promenna?
            variable_t *variable = ht_read(ht_var, (*token_ptr)->value);

            if (variable == NULL) {
                // identifikator neni ani funkce, ani promenna
                tuttifruttimakro(E_SEM_DEF);
            }
        }
    }

    // vyraz
    error_t err = precedence_algorithm(token_ptr, buffer, ht_var, false, el_1, data_type);
    if (err != E_OK) {
        tuttifruttimakro(err);
    }
    *el_2 = NULL;

    return E_OK;
}

error_t desc_real_params(buffer_t *buffer, token_t **token_ptr, hashtable_t *ht_var, ast_node_t *params_node,
                         list_el_ptr list_el, bool first) {
    if (!first) {
        if ((*token_ptr)->type == T_COMMA) {
            // mame carku

            if (list_el == NULL) {
                // ale uz nema byt zadny parametr
                tuttifruttimakro(E_SEM_TYPE_OR_PARAMS);
            }

            // else: mame carku a ocekavame dalsi parametr => pokracujeme
            token_get_macro();
        } else {
            // nemame carku

            if (list_el == NULL) {
                // a uz nema byt zadny parametr
                return E_OK;
            } else {
                // jeste ma byt dalsi parametr
                tuttifruttimakro(E_SEM_TYPE_OR_PARAMS);
            }
        }
    } else if (list_el == NULL) {
        // pokud funkce nema mit zadne parametry
        return E_OK;
    }

    // ziskani termu
    ast_elem_t *term_el = NULL;
    char data_type;
    next_desc_fn(desc_term, ht_var, &term_el, &data_type);

    // formalni parametr funkce
    variable_t *param = list_el->data;

    // kontrola typu
    if (incompatible_types(data_type, param->data_type)) {
        // nepovolena implicitna konverze, protoze string nemuzeme na nic pretypovat a zaroven nic nemuzeme pretypovat na string
        tuttifruttimakro(E_SEM_TYPE_OR_PARAMS);
    }

    // pridani termu nodu PARAMS
    error_t err = ast_increase_size(params_node);
    if (err != E_OK) {
        tuttifruttimakro(err);
    }
    params_node->elems[params_node->cap - 1] = term_el;

    next_desc_fn(desc_real_params, ht_var, params_node, list_el->ptr_next, false);

	return E_OK;
}

error_t desc_term(buffer_t *buffer, token_t **token_ptr, hashtable_t *ht_var, ast_elem_t **term_el, char *data_type) {

    if ((*token_ptr)->type == T_ID) {
        // je v tabulce promennych?
        variable_t *var = ht_read(ht_var, (*token_ptr)->value);
        if (var == NULL) {
            tuttifruttimakro(E_SEM_DEF);
        }

        // pokud je v tabulce promennych, vytvor list pro promennou
        error_t err = ast_create_leaf_elem('v', var, term_el);
        if (err != E_OK) {
            tuttifruttimakro(err);
        }

        *data_type = var->data_type;
    }

    else if ((*token_ptr)->type == T_INTEGER) {
        int *ptr = NULL;
        if (!token_to_int(*token_ptr, &ptr)) {
            tuttifruttimakro(E_INTERNAL);
        }
        error_t err = ast_create_leaf_elem('i', ptr, term_el);
        if (err != E_OK) {
            tuttifruttimakro(err);
        }

        *data_type = 'i';
    }

    else if ((*token_ptr)->type == T_DOUBLE) {
        double *ptr = NULL;
        if (!token_to_double(*token_ptr, &ptr)) {
            tuttifruttimakro(E_INTERNAL);
        }
        error_t err = ast_create_leaf_elem('d', ptr, term_el);
        if (err != E_OK) {
            tuttifruttimakro(err);
        }

        *data_type = 'd';
    }

    else if ((*token_ptr)->type == T_STRING) {
        char *ptr = NULL;
        if (!token_to_string(*token_ptr, &ptr)) {
            tuttifruttimakro(E_INTERNAL);
        }
        error_t err = ast_create_leaf_elem('s', ptr, term_el);
        if (err != E_OK) {
            tuttifruttimakro(err);
        }

        *data_type = 's';
    }

    else {
        tuttifruttimakro(E_SYNTACTIC);
    }

    token_get_macro();

	return E_OK;
}
