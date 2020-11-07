/**
 * @file generator.c
 * @author Simeon Borko, Filip Borcik, Vojtech Chadima
 * @date 11-12 2017
 * @brief Soubor obsahujujici vysledne generovani kodu IFJcode17
 */
#include "generator.h"

#define code_stat(X,Y) X(Y, frame, list)

#define tuttifruttimakro(err) { fprintf(stderr, "Chyba : %s\t: %s\t: %d\n", __FILE__, __func__, __LINE__); return err; }

error_t code_generator(ast_node_t *program_root){
	fprintf(stdout, ".IFJcode17\n" );
    fprintf(stdout, "DEFVAR GF@&l\nDEFVAR GF@&r\n"); // deklarace pomocnych promennych pro porovnavaci vyrazy
	fprintf(stdout, "JUMP $$main\n" );

    code_builtin_length();
    code_builtin_substr();
    code_builtin_asc();
    code_builtin_chr();

	ast_elem_t *act_elem;		// aktualny element na kterem je generator
	ast_node_t *act_node;		// node aktualnyho elementu
	error_t err;

    code_buffer = buffer_init();

	for (unsigned i = 0; i < program_root->cap; i++)
	{
		act_elem = program_root->elems[i];
		if (act_elem->type == 'l')
		{
			tuttifruttimakro(E_INTERNAL);
		}

		act_node = act_elem->data;
        if (act_node->type == AT_DEFINITION) {
            ast_leaf_t *fn_leaf = act_node->elems[0]->data;
            err = code_function(fn_leaf->value, act_node->elems[1]->data);
            if (err != E_OK) {
                return err;
            }
        } else if (act_node->type == AT_SCOPE) {
            fprintf(stdout, "LABEL $$main # main body\n" );
            list_t *list = list_init();
            if (!list) {
                tuttifruttimakro(E_INTERNAL);
            }
            err = code_stats(act_node->elems[0]->data, "GF", list);
            if (err != E_OK) {
                return err;
            }

            code_stats_gen("GF", list);

            fprintf(stdout, "# end of main body\n" );
        }
	}

	return E_OK;
}

error_t code_function(function_t *function, ast_node_t *stats_node) {

    list_t *list = list_init();
    if (!list)
        tuttifruttimakro(E_INTERNAL);

    fprintf(stdout, "LABEL $%s # start of %s function\n", function->name, function->name );
    fprintf(stdout, "PUSHFRAME\n" );
    fprintf(stdout, "DEFVAR LF@%%retval\n");
    char rettype = function->return_type;
    switch(rettype){
        case 'i':
            fprintf(stdout, "MOVE LF@%%retval int@0\n");
            break;
        case 'd':
            fprintf(stdout, "MOVE LF@%%retval float@0.0\n");
            break;
        case 's':
            fprintf(stdout, "MOVE LF@%%retval string@\n");
            break;
        default:
            tuttifruttimakro(E_INTERNAL);
    }

	error_t err = code_stats(stats_node, "LF", list);
    if (err != E_OK) {
        return err;
    }

    err = code_stats_gen("LF", list);

    fprintf(stdout, "LABEL $%s$epilog\n", function->name );
	fprintf(stdout, "POPFRAME\n" );
	fprintf(stdout, "RETURN\n" );

	return err;
}

// --------------------------------------------------------------------------------

error_t code_stats(ast_node_t *stats, char *frame, list_t *list){
	ast_elem_t *act_elem;
	ast_node_t *act_node;
	ast_node_type_t node_type;
	error_t err = E_OK;

	for (unsigned i = 0; i < stats->cap; i++)
	{
		act_elem = stats->elems[i];
		if (act_elem->type != 'n')
			tuttifruttimakro(E_INTERNAL);

		act_node = (ast_node_t *)(act_elem->data);
		node_type = act_node->type;
		
		switch(node_type){
			case AT_IF:
				err = code_stat(code_if, act_node);
				break;
		    case AT_WHILE:
		    	err = code_stat(code_while, act_node);
		    	break;		    
		    case AT_DIM:
		    	err = code_stat(code_dim, act_node);
		    	break;
		    case AT_ASSIGN_EXPR:
		    	err = code_stat(code_ass_exp, act_node);
		    	break;
		    case AT_ASSIGN_FN:
		    	err = code_stat(code_ass_fc, act_node);
		    	break;		    
		    case AT_PRINT:
		    	err = code_stat(code_print, act_node);
		    	break;
		    case AT_RETURN:
		    	err = code_stat(code_return, act_node);
		    	break;
		    case AT_INPUT:
		    	err = code_stat(code_input, act_node);
		    	break;
		    default:
		    	err = E_INTERNAL;
		}
        if (err != E_OK) {
            tuttifruttimakro(err);
        }
	}

	return err;
}

error_t code_if(ast_node_t *if_node, char *frame, list_t *list){
	int label = code_label_gen(); // cislo nazvu labelu pro vetvu else a pro konec konstrukce if

	ast_elem_t *act_elem;		// aktualne spracovavan element z troch
	ast_node_t *act_node;		// aktualne spracovavan uzel
	
	error_t err;

	if(if_node->cap != 3)		// overeni poctu elementov.. melo by jich bejt 3 (condition, then, else)
		tuttifruttimakro(E_INTERNAL);


	// Nacteni uzlu a Spracovani podminky  
	act_elem = if_node->elems[0];
	if (act_elem->type != 'n')
		tuttifruttimakro(E_INTERNAL);
	act_node = (ast_node_t *)(act_elem->data);

    // Skoci na label ELSE
	err = code_cond(act_node, frame, list, label, "_$ELSE");
    if (err != E_OK) {
        return err;
    }

	// Nacteni uzlu a Spracovani kodu THEN 
	act_elem = if_node->elems[1];
	if (act_elem->type != 'n')
		tuttifruttimakro(E_INTERNAL);
	act_node = (ast_node_t *)(act_elem->data);
    
	err = code_stat(code_stats, act_node);
    if (err != E_OK) {
        return err;
    }

    {
        
        code_statement_t *statement = trash_add(sizeof(code_statement_t), &err);
        if(err != E_OK)
            return E_INTERNAL;
        statement->name = "JUMP";
        statement->a = trash_add(sizeof(code_operand_t), &err);
        statement->a->frame = NULL;
        statement->a->name = "_$ENDIF";
        statement->a->numb = label;

        statement->b = NULL;
        statement->c = NULL;

        list_insert_last(list, statement, 's');
    }

    {
        code_statement_t *statement = trash_add(sizeof(code_statement_t), &err);
        if(err != E_OK)
            return E_INTERNAL;
        statement->name = "LABEL";

        statement->a = trash_add(sizeof(code_operand_t), &err);
        if (err != E_OK)
            return E_INTERNAL;
        statement->a->frame = NULL;
        statement->a->name = "_$ELSE";
        statement->a->numb = label;

        statement->b = NULL;
        statement->c = NULL;

        list_insert_last(list, statement, 's');
    }

	// Nacteni uzlu a Spracovani kodu ELSE 
	act_elem = if_node->elems[2];
	if (act_elem->type != 'n')
		tuttifruttimakro(E_INTERNAL);
	act_node = (ast_node_t *)(act_elem->data);
	err = code_stat(code_stats, act_node);
    if (err != E_OK) {
        return err;
    }

    // Vytvori label ENDIF
    {
        code_statement_t *statement = trash_add(sizeof(code_statement_t), &err);
        if(err != E_OK)
            return E_INTERNAL;
        statement->name = "LABEL";

        statement->a = trash_add(sizeof(code_operand_t), &err);
        if(err != E_OK)
            return E_INTERNAL;
        statement->a->frame = NULL;
        statement->a->name = "_$ENDIF";
        statement->a->numb = label;

        statement->b = NULL;
        statement->c = NULL;

        list_insert_last(list, statement, 's');
    }

	return err;
}

error_t code_while(ast_node_t *while_node, char *frame, list_t *list){
    int label = code_label_gen(); // cislo nazvu labelu pro zacatek whilu a pro konec whilu

	ast_elem_t *act_elem;		// aktualne spracovavan element z troch
	ast_node_t *act_node;		// aktualne spracovavan uzel

    error_t err;

	if(while_node->cap != 2)		// overeni poctu elementov.. melo by jich bejt 2 (condition, do)
		tuttifruttimakro(E_INTERNAL);


	// Nacteni uzlu a Spracovani podminky  
	act_elem = while_node->elems[0];
	if (act_elem->type != 'n')
		tuttifruttimakro(E_INTERNAL);
	act_node = (ast_node_t *)(act_elem->data);

    {
        code_statement_t *statement = trash_add(sizeof(code_statement_t), &err);
        if(err != E_OK)
            return E_INTERNAL;
        statement->name = "LABEL";

        statement->a = trash_add(sizeof(code_operand_t), &err);
        if(err != E_OK)
            return E_INTERNAL;
        statement->a->frame = NULL;
        statement->a->name = "_$WHILESTART";
        statement->a->numb = label;

        statement->b = NULL;
        statement->c = NULL;

        list_insert_last(list, statement, 's');
    }

    err = code_cond(act_node, frame, list, label, "_$WHILEEND");
    if (err != E_OK)
        return err;

	// Nacteni uzlu a Spracovani kodu DO 
    act_elem = while_node->elems[1];
    if (act_elem->type != 'n')
        tuttifruttimakro(E_INTERNAL);
    act_node = (ast_node_t *)(act_elem->data);
    err = code_stat(code_stats, act_node);
    if (err != E_OK)
        return err;

    // po vykonani z cyklu skok na opatovne vyhodnoceni podminky
    {
        code_statement_t *statement = trash_add(sizeof(code_statement_t), &err);
        if(err != E_OK)
            return E_INTERNAL;
        statement->name = "JUMP";

        statement->a = trash_add(sizeof(code_operand_t), &err);
        if(err != E_OK)
            return E_INTERNAL;
        statement->a->frame = NULL;
        statement->a->name = "_$WHILESTART";
        statement->a->numb = label;

        statement->b = NULL;
        statement->c = NULL;

        list_insert_last(list, statement, 's');
    }

    // Navesti pro vyskoceni z cyklu
    {
        code_statement_t *statement = trash_add(sizeof(code_statement_t), &err);
        if(err != E_OK)
            return E_INTERNAL;

        statement->name = "LABEL";

        statement->a = trash_add(sizeof(code_operand_t), &err);
        if(err != E_OK)
            return E_INTERNAL;
        statement->a->frame = NULL;
        statement->a->name = "_$WHILEEND";
        statement->a->numb = label;

        statement->b = NULL;
        statement->c = NULL;

        list_insert_last(list, statement, 's');
    }

	return err;
}

error_t code_dim(ast_node_t *dim_node, char *frame, list_t *list){

    // deklarace
    ast_leaf_t *var_leaf = dim_node->elems[0]->data;
    variable_t *var = var_leaf->value;
    fprintf(stdout, "DEFVAR %s@%s\n", frame, var->name);

    if (dim_node->cap == 1) {
        // pokud nema explicitne definovanou inicializacni hodnotu
        switch (var->data_type) {
            case 'i':
                fprintf(stdout, "MOVE %s@%s int@0\n", frame, var->name);
                break;

            case 'd':
                fprintf(stdout, "MOVE %s@%s float@0.0\n", frame, var->name);
                break;

            case 's':
                fprintf(stdout, "MOVE %s@%s string@\n", frame, var->name);
                break;

            default:
                tuttifruttimakro(E_INTERNAL);
        }
    } else {
        code_stat(code_ass_exp, dim_node);
    }

	return E_OK;
}

error_t code_ass_exp(ast_node_t *ass_exp_node, char *frame, list_t *list){

    // ziskani listu promenne, do ktere prirazujeme
    ast_leaf_t *var_leaf = ass_exp_node->elems[0]->data;
    variable_t *var = var_leaf->value;
    error_t err;

    // vyhodnoceni vyrazu na prave strane prirazeni
    int var_number;
    char expr_type;

    err = code_expr(ass_exp_node->elems[1], frame, &var_number, &expr_type, list);
    if (err != E_OK)
        return err;

    if (expr_type == 's') {
        {
            code_statement_t *statement = trash_add(sizeof(code_statement_t), &err);
            if(err != E_OK)
                return E_INTERNAL;
            statement->name = "MOVE";

            statement->a = trash_add(sizeof(code_operand_t), &err);
            if(err != E_OK)
                return E_INTERNAL;
            statement->a->frame = frame;
            statement->a->name = var->name;
            statement->a->numb = -1;

            statement->b = trash_add(sizeof(code_operand_t), &err);
            if(err != E_OK)
                return E_INTERNAL;
            statement->b->frame = frame;
            statement->b->name = NULL; // &ExprTmp
            statement->b->numb = var_number;

            statement->c = NULL;

            list_insert_last(list, statement, 's');
        }
    } else {
        code_cast(frame, frame, var->data_type, var->name, expr_type, var_number, list);
    }

    return E_OK;
}

error_t code_ass_fc(ast_node_t *ass_fc_node, char *frame, list_t *list){

    error_t err;
    {
        code_statement_t *statement = trash_add(sizeof(code_statement_t), &err);
        if(err != E_OK)
            return E_INTERNAL;
        statement->name = "CREATEFRAME";

        statement->a = NULL;
        statement->b = NULL;
        statement->c = NULL;

        list_insert_last(list, statement, 's');
    }

    ast_leaf_t *fn_leaf = ass_fc_node->elems[1]->data;
    function_t *function = fn_leaf->value;

    // naplnit docasny ramec parametry funkce
    err = code_params(ass_fc_node->elems[2]->data, frame, list, function->params);
    if (err != E_OK) {
        return err;
    }

    {
        code_statement_t *statement = trash_add(sizeof(code_statement_t), &err);
        if(err != E_OK)
            return E_INTERNAL;
        statement->name = "CALL";

        statement->a = trash_add(sizeof(code_operand_t), &err);
        if(err != E_OK)
            return E_INTERNAL;
        statement->a->frame = NULL;
        statement->a->name = function->name;
        statement->a->numb = -1;

        statement->b = NULL;
        statement->c = NULL;

        list_insert_last(list, statement, 's');
    }

    ast_leaf_t *v_el = ass_fc_node->elems[0]->data;
    variable_t *var = v_el->value;

    if (function->return_type == 's') {
        //fprintf(stdout, "MOVE %s@%s TF@%%retval\n", frame, var->name);
        {
            code_statement_t *statement = trash_add(sizeof(code_statement_t), &err);
            if(err != E_OK)
                return E_INTERNAL;
            statement->name = "MOVE";

            statement->a = trash_add(sizeof(code_operand_t), &err);
            if(err != E_OK)
                return E_INTERNAL;
            statement->a->frame = frame;
            statement->a->name = var->name;
            statement->a->numb = -1;

            statement->b = trash_add(sizeof(code_operand_t), &err);
            statement->b->frame = "TF";
            statement->b->name = "%retval";
            statement->b->numb = -1;

            statement->c = NULL;

            list_insert_last(list, statement, 's');
        }
    } else {
        int tmp = code_varname_gen(frame);
        //fprintf(stdout, "MOVE %s@&ExprTmp%d TF@%%retval\n", frame, tmp);
        {
            code_statement_t *statement = trash_add(sizeof(code_statement_t), &err);
            if(err != E_OK)
                return E_INTERNAL;
            statement->name = "MOVE";

            statement->a = trash_add(sizeof(code_operand_t), &err);
            if(err != E_OK)
                return E_INTERNAL;
            statement->a->frame = frame;
            statement->a->name = NULL;
            statement->a->numb = tmp;

            statement->b = trash_add(sizeof(code_operand_t), &err);
            if(err != E_OK)
                return E_INTERNAL;
            statement->b->frame = "TF";
            statement->b->name = "%retval";
            statement->b->numb = -1;

            statement->c = NULL;

            list_insert_last(list, statement, 's');
        }
        code_cast(frame, frame, var->data_type, var->name, function->return_type, tmp, list);
    }

    return E_OK;
}

error_t code_print(ast_node_t *print_node, char *frame, list_t *list){
    for (unsigned i = 0; i < print_node->cap; i++) {
        int tmp;
        char null;
        error_t err = code_expr(print_node->elems[i], frame, &tmp, &null, list);
        if (err != E_OK)
            return err;
        {
            error_t err;
            code_statement_t *statement = trash_add(sizeof(code_statement_t), &err);
            if(err != E_OK)
                return E_INTERNAL;
            statement->name = "WRITE";

            statement->a = trash_add(sizeof(code_operand_t), &err);
            if(err != E_OK)
                return E_INTERNAL;
            statement->a->frame = frame;
            statement->a->name = NULL;
            statement->a->numb = tmp;

            statement->b = NULL;
            statement->c = NULL;

            list_insert_last(list, statement, 's');
        }
        //fprintf(stdout, "WRITE %s@&ExprTmp%d\n", frame, tmp);
    }
    return E_OK;
}

error_t code_return(ast_node_t *return_node, char *frame, list_t *list){
    int tmp;
    char data_type;
    error_t err = code_expr(return_node->elems[1], frame, &tmp, &data_type, list);
    if (err != E_OK)
        return err;

    function_t *function = ((ast_leaf_t *)(return_node->elems[0]->data))->value;

    if (data_type == 's') {
        //fprintf(stdout, "MOVE LF@%%retval LF@&ExprTmp%d\n", tmp);
        {
            code_statement_t *statement = trash_add(sizeof(code_statement_t), &err);
            if(err != E_OK)
                return E_INTERNAL;
            statement->name = "MOVE";

            statement->a = trash_add(sizeof(code_operand_t), &err);
            if(err != E_OK)
                return E_INTERNAL;
            statement->a->frame = frame;
            statement->a->name = "%retval";
            statement->a->numb = -1;

            statement->b = trash_add(sizeof(code_operand_t), &err);
            if(err != E_OK)
                return E_INTERNAL;
            statement->b->frame = frame;
            statement->b->name = NULL;
            statement->b->numb = tmp;

            statement->c = NULL;

            list_insert_last(list, statement, 's');
        }
    } else {
        code_cast(frame, frame, function->return_type, "%retval", data_type, tmp, list);
    }

    {
        code_statement_t *statement = trash_add(sizeof(code_statement_t), &err);
        if(err != E_OK)
            return E_INTERNAL;
        statement->name = "JUMP";

        statement->a = trash_add(sizeof(code_operand_t), &err);
        if(err != E_OK)
            return E_INTERNAL;
        statement->a->frame = NULL;
        statement->a->name = function->name;
        statement->a->numb = -2; // $epilog

        statement->b = NULL;

        statement->c = NULL;

        list_insert_last(list, statement, 's');
    }
    //fprintf(stdout, "JUMP $%s$epilog\n", function->name);

    return E_OK;
}

error_t code_input(ast_node_t *input_node, char *frame, list_t *list){

    variable_t *var = ((ast_leaf_t *)(input_node->elems[0]->data))->value;

    error_t err;

    char *type_str;
    switch (var->data_type) {
        case 'i':
            type_str = "int";
            break;
        case 'd':
            type_str = "float";
            break;
        case 's':
            type_str = "string";
            break;
        default:
            tuttifruttimakro(E_INTERNAL);
    }

    {
        code_statement_t *statement = trash_add(sizeof(code_statement_t), &err);
        if(err != E_OK)
            return E_INTERNAL;
        statement->name = "WRITE";

        statement->a = trash_add(sizeof(code_operand_t), &err);
        if(err != E_OK)
            return E_INTERNAL;
        statement->a->frame = NULL;
        statement->a->name = "string@?\\032";
        statement->a->numb = -3;

        statement->b = NULL;
        statement->c = NULL;

        list_insert_last(list, statement, 's');
    }

    {
        code_statement_t *statement = trash_add(sizeof(code_statement_t), &err);
        if(err != E_OK)
            return E_INTERNAL;
        statement->name = "READ";

        statement->a = trash_add(sizeof(code_operand_t), &err);
        if(err != E_OK)
            return E_INTERNAL;
        statement->a->frame = frame;
        statement->a->name = var->name;
        statement->a->numb = -1;

        statement->b = trash_add(sizeof(code_operand_t), &err);
        if(err != E_OK)
            return E_INTERNAL;
        statement->b->frame = NULL;
        statement->b->name = type_str;
        statement->b->numb = -3;

        statement->c = NULL;

        list_insert_last(list, statement, 's');
    }

    //fprintf(stdout, "READ %s@%s %s\n", frame, var->name, type_str);

	return E_OK;
}

// --------------------------------------------------------------------------------

error_t code_cond(ast_node_t *cond_node, char *frame, list_t *list, int label_numb, char *label_name){
	error_t err;

	if (cond_node->cap != 2)
	{
		tuttifruttimakro(E_INTERNAL);
	}

	ast_elem_t *elem_a = cond_node->elems[0];	// vyraz 1
	ast_elem_t *elem_b = cond_node->elems[1];	// vyraz 2
	int expr_var_name1;			// cislo nazvu pomocne promenne v ktere je uchovana hodnota vyrazu 1
	int expr_var_name2;			// cislo nazvu pomocne promenne v ktere je uchovana hodnota vyrazu 2
    char expr_type1;
    char expr_type2;

	// vypocet hodnoty vyrazu 1 a predani cisla nazvu promenne 
	err = code_expr(elem_a, frame, &expr_var_name1, &expr_type1, list);
	if (err != E_OK)
		return err;

	// vypocet hodnoty vyrazu 1 a predani cisla nazvu promenne 
	err = code_expr(elem_b, frame, &expr_var_name2, &expr_type2, list);
	if(err != E_OK)
		return err;

    if ( (expr_type1 == 's' && expr_type2 == 's') || (expr_type1 == 'i' && expr_type2 == 'i') ) {
        {
            code_statement_t *statement = trash_add(sizeof(code_statement_t), &err);
            if(err != E_OK)
                return E_INTERNAL;
            statement->name = "MOVE";

            statement->a = trash_add(sizeof(code_operand_t), &err);
            if(err != E_OK)
                return E_INTERNAL;
            statement->a->frame = "GF";
            statement->a->name = "&l";
            statement->a->numb = -1;

            statement->b = trash_add(sizeof(code_operand_t), &err);
            if(err != E_OK)
                return E_INTERNAL;
            statement->b->frame = frame;
            statement->b->name = NULL;
            statement->b->numb = expr_var_name1;

            statement->c = NULL;

            list_insert_last(list, statement, 's');
        }
        {
            code_statement_t *statement = trash_add(sizeof(code_statement_t), &err);
            if(err != E_OK)
                return E_INTERNAL;
            statement->name = "MOVE";

            statement->a = trash_add(sizeof(code_operand_t), &err);
            if(err != E_OK)
                return E_INTERNAL;
            statement->a->frame = "GF";
            statement->a->name = "&r";
            statement->a->numb = -1;

            statement->b = trash_add(sizeof(code_operand_t), &err);
            if(err != E_OK)
                return E_INTERNAL;
            statement->b->frame = frame;
            statement->b->name = NULL;
            statement->b->numb = expr_var_name2;

            statement->c = NULL;

            list_insert_last(list, statement, 's');
        }
        //fprintf(stdout, "MOVE GF@&l %s@&ExprTmp%d\n", frame, expr_var_name1);
        //fprintf(stdout, "MOVE GF@&r %s@&ExprTmp%d\n", frame, expr_var_name2);
    } else if (expr_type1 == 'd' || expr_type2 == 'd') {
        code_cast(frame, "GF", 'd', "&l", expr_type1, expr_var_name1, list);
        code_cast(frame, "GF", 'd', "&r", expr_type2, expr_var_name2, list);
    } else {
        tuttifruttimakro(E_INTERNAL);
    }

	int bool_var = code_varname_gen(frame);		// cislo nazvu pomocne promenne v ktere bude uchovana hodnota (bool) porovnania vyrazov
	char *true_str = "bool@true";			// pomocna promenna, pro porovnani s true


    code_statement_t *statement = trash_add(sizeof(code_statement_t), &err);
    if(err != E_OK)
        return E_INTERNAL;

    statement->b = trash_add(sizeof(code_operand_t), &err);
    if(err != E_OK)
        return E_INTERNAL;
    statement->b->frame = "GF";
    statement->b->name = "&l";
    statement->b->numb = -1;

    statement->c = trash_add(sizeof(code_operand_t), &err);
    if(err != E_OK)
        return E_INTERNAL;
    statement->c->frame = "GF";
    statement->c->name = "&r";
    statement->c->numb = -1;

    list_insert_last(list, statement, 's');

	// spracovani podminky na zaklade typu
	switch(cond_node->type){
		//	>
		case AT_COND_GT:
            {
                statement->name = "GT";

                statement->a = trash_add(sizeof(code_operand_t), &err);
                if(err != E_OK)
                    return E_INTERNAL;
                statement->a->frame = frame;
                statement->a->name = NULL;
                statement->a->numb = bool_var;

            }
            {
                code_statement_t *statement = trash_add(sizeof(code_statement_t), &err);
                if(err != E_OK)
                    return E_INTERNAL;
                statement->name = "JUMPIFNEQ";

                statement->a = trash_add(sizeof(code_operand_t), &err);
                if(err != E_OK)
                    return E_INTERNAL;
                statement->a->frame = NULL;
                statement->a->name = label_name;
                statement->a->numb = label_numb;

                statement->b = trash_add(sizeof(code_operand_t), &err);
                if(err != E_OK)
                    return E_INTERNAL;
                statement->b->frame = frame;
                statement->b->name = NULL;
                statement->b->numb = bool_var;

                statement->c = trash_add(sizeof(code_operand_t), &err);
                if(err != E_OK)
                    return E_INTERNAL;
                statement->c->frame = NULL;
                statement->c->name = true_str;
                statement->c->numb = -3;

                list_insert_last(list, statement, 's');
            }
            //fprintf(stdout, "GT %s@&ExprTmp%d GF@&l GF@&r\n", frame, bool_var);
			//fprintf(stdout, "JUMPIFNEQ _$%s%d %s@&ExprTmp%d %s\n", label_name, label_numb, frame, bool_var, true_str);
			break;
		//	<
		case AT_COND_LT:
            {
                statement->name = "LT";

                statement->a = trash_add(sizeof(code_operand_t), &err);
                if(err != E_OK)
                    return E_INTERNAL;
                statement->a->frame = frame;
                statement->a->name = NULL;
                statement->a->numb = bool_var;

            }
            {
                code_statement_t *statement = trash_add(sizeof(code_statement_t), &err);
                if(err != E_OK)
                    return E_INTERNAL;
                statement->name = "JUMPIFNEQ";

                statement->a = trash_add(sizeof(code_operand_t), &err);
                if(err != E_OK)
                    return E_INTERNAL;
                statement->a->frame = NULL;
                statement->a->name = label_name;
                statement->a->numb = label_numb;

                statement->b = trash_add(sizeof(code_operand_t), &err);
                if(err != E_OK)
                    return E_INTERNAL;
                statement->b->frame = frame;
                statement->b->name = NULL;
                statement->b->numb = bool_var;

                statement->c = trash_add(sizeof(code_operand_t), &err);
                if(err != E_OK)
                    return E_INTERNAL;
                statement->c->frame = NULL;
                statement->c->name = true_str;
                statement->c->numb = -3;

                list_insert_last(list, statement, 's');
            }
            //fprintf(stdout, "LT %s@&ExprTmp%d GF@&l GF@&r\n", frame, bool_var);
			//fprintf(stdout, "JUMPIFNEQ _$%s%d %s@&ExprTmp%d %s\n", label_name, label_numb, frame, bool_var, true_str);
			break;
		// <>
		case AT_COND_IE:
            {
                statement->name = "JUMPIFEQ";

                statement->a = trash_add(sizeof(code_operand_t), &err);
                if(err != E_OK)
                    return E_INTERNAL;
                statement->a->frame = NULL;
                statement->a->name = label_name;
                statement->a->numb = label_numb;

            }
            //fprintf(stdout, "JUMPIFEQ _$%s%d GF@&l GF@&r\n", label_name, label_numb);
			break;
		// >=
		case AT_COND_GE:
            {
                statement->name = "LT";

                statement->a = trash_add(sizeof(code_operand_t), &err);
                if(err != E_OK)
                    return E_INTERNAL;
                statement->a->frame = frame;
                statement->a->name = NULL;
                statement->a->numb = bool_var;

            }
            {
                code_statement_t *statement = trash_add(sizeof(code_statement_t), &err);
                if(err != E_OK)
                    return E_INTERNAL;
                statement->name = "JUMPIFEQ";

                statement->a = trash_add(sizeof(code_operand_t), &err);
                if(err != E_OK)
                    return E_INTERNAL;
                statement->a->frame = NULL;
                statement->a->name = label_name;
                statement->a->numb = label_numb;

                statement->b = trash_add(sizeof(code_operand_t), &err);
                if(err != E_OK)
                    return E_INTERNAL;
                statement->b->frame = frame;
                statement->b->name = NULL;
                statement->b->numb = bool_var;

                statement->c = trash_add(sizeof(code_operand_t), &err);
                if(err != E_OK)
                    return E_INTERNAL;
                statement->c->frame = NULL;
                statement->c->name = true_str;
                statement->c->numb = -3;

                list_insert_last(list, statement, 's');
            }
			//fprintf(stdout, "LT %s@&ExprTmp%d GF@&l GF@&r\n", frame, bool_var);
            //fprintf(stdout, "JUMPIFEQ _$%s%d %s@&ExprTmp%d %s\n", label_name, label_numb, frame, bool_var, true_str);
			break;
		// <=
		case AT_COND_LE:
            {
                statement->name = "GT";

                statement->a = trash_add(sizeof(code_operand_t), &err);
                if(err != E_OK)
                    return E_INTERNAL;
                statement->a->frame = frame;
                statement->a->name = NULL;
                statement->a->numb = bool_var;

            }
            {
                code_statement_t *statement = trash_add(sizeof(code_statement_t), &err);
                if(err != E_OK)
                    return E_INTERNAL;
                statement->name = "JUMPIFEQ";

                statement->a = trash_add(sizeof(code_operand_t), &err);
                if(err != E_OK)
                    return E_INTERNAL;
                statement->a->frame = NULL;
                statement->a->name = label_name;
                statement->a->numb = label_numb;

                statement->b = trash_add(sizeof(code_operand_t), &err);
                if(err != E_OK)
                    return E_INTERNAL;
                statement->b->frame = frame;
                statement->b->name = NULL;
                statement->b->numb = bool_var;

                statement->c = trash_add(sizeof(code_operand_t), &err);
                if(err != E_OK)
                    return E_INTERNAL;
                statement->c->frame = NULL;
                statement->c->name = true_str;
                statement->c->numb = -3;

                list_insert_last(list, statement, 's');
            }
			//fprintf(stdout, "GT %s@&ExprTmp%d GF@&l GF@&r\n", frame, bool_var);
            //fprintf(stdout, "JUMPIFEQ _$%s%d %s@&ExprTmp%d %s\n", label_name, label_numb, frame, bool_var, true_str);
			break;
		// ==
		case AT_COND_EQ:
            {
                statement->name = "JUMPIFNEQ";

                statement->a = trash_add(sizeof(code_operand_t), &err);
                if(err != E_OK)
                    return E_INTERNAL;
                statement->a->frame = NULL;
                statement->a->name = label_name;
                statement->a->numb = label_numb;

            }
            //fprintf(stdout, "JUMPIFNEQ _$%s%d GF@&l GF@&r\n", label_name, label_numb);
			break;
		default:
		   	tuttifruttimakro(E_INTERNAL);
	}

	return err;
}

error_t code_term(ast_leaf_t *leaf, char *frame, list_t *list, int numb, char *data_type) {
    variable_t *vt_var;
    error_t err;

    code_statement_t *statement = trash_add(sizeof(code_statement_t), &err);
    if(err != E_OK)
        return E_INTERNAL;
    statement->name = "MOVE";

    statement->a = trash_add(sizeof(code_operand_t), &err);
    if(err != E_OK)
        return E_INTERNAL;
    statement->a->frame = frame;
    statement->a->name = NULL;
    statement->a->numb = numb;

    statement->b = trash_add(sizeof(code_operand_t), &err);
    if(err != E_OK)
        return E_INTERNAL;

    statement->c = NULL;

    list_insert_last(list, statement, 's');

    // presun hodnoty TERMU do pomocnej premennej a zjisteni typu
    switch(leaf->type) {
        case 'v':

            vt_var = (variable_t *)(leaf->value);

            statement->b->frame = frame;
            statement->b->name = vt_var->name;
            statement->b->numb = -1;

            //fprintf(stdout, "MOVE %s@&ExprTmp%d %s@%s\n", frame, *var_name, frame, vt_var->name);
            *data_type = vt_var->data_type;

            break;

        case 'd':

            //doub_val = (double *)(leaf->value);

            statement->b->frame = NULL;
            statement->b->name = leaf->value;
            statement->b->numb = -4;

            //fprintf(stdout, "MOVE %s@&ExprTmp%d float@%g\n", frame, *var_name, *doub_val);
            *data_type = 'd';
            break;
        case 'i':
            //int_val = (int *)(leaf->value);
            //fprintf(stdout, "MOVE %s@&ExprTmp%d int@%d\n", frame, *var_name, *int_val);
            statement->b->frame = NULL;
            statement->b->name = leaf->value;
            statement->b->numb = -5;
            *data_type = 'i';
            break;
        case 's':
            statement->b->frame = NULL;
            statement->b->name = leaf->value;
            statement->b->numb = -6;
            //str_val = (char *)(leaf->value);
            //fprintf(stdout, "MOVE %s@&ExprTmp%d string@%s\n", frame, *var_name, code_convert_string(str_val));
            *data_type = 's';
            break;
        default:
            tuttifruttimakro(E_INTERNAL);
    }
    return E_OK;
}

/**
 * Vrati cislo promenne pozadovaneho typu.
 * @param frame "GF" | "LF"
 * @param list Seznam prikazu
 * @param input_var Cislo vstupni promenne
 * @param input_type Typ vstupni promenne
 * @param output_type Pozadovany typ
 * @param round 'true' => zaokrouhlit, 'false' => orezat pri konverzich int
 * @return Cislo promenne pozadovaneho typu, nebo -1 pokud je prevod nepodaril
 */
int code_cast_or_not(char *frame, list_t *list, int input_var, char input_type, char output_type, bool round) {
    error_t err;
    if (input_type == output_type)
        return input_var;
    if (input_type == 's' || output_type == 's')
        return -1;

    int output_var = code_varname_gen(frame);

    code_statement_t *s = trash_add(sizeof(code_statement_t), &err);
    if(err != E_OK)
        return E_INTERNAL;
    s->name = input_type == 'i' ? "INT2FLOAT" : (round ? "FLOAT2R2EINT" : "FLOAT2INT");

    s->a = trash_add(sizeof(code_operand_t), &err);
    if(err != E_OK)
        return E_INTERNAL;
    s->a->frame = frame;
    s->a->name = NULL;
    s->a->numb = output_var;

    s->b = trash_add(sizeof(code_operand_t), &err);
    if(err != E_OK)
        return E_INTERNAL;
    s->b->frame = frame;
    s->b->name = NULL;
    s->b->numb = input_var;

    s->c = NULL;
    list_insert_last(list, s, 's'); // s jako statement

    return output_var;
}

// nepouzivat pro podminkove vyrazy!!
error_t code_expr(ast_elem_t *expr_elem, char *frame, int *var_name, char *data_type, list_t *list){
	error_t err = E_OK;			// vystupni error

	*var_name = code_varname_gen(frame);		// vytvoreni cisla pro nazev vystupni pomocni promenne

	// spracovani listu ve vyrazu (TERM)
	if (expr_elem->type == 'l')		
	{
		ast_leaf_t *leaf = (ast_leaf_t *)(expr_elem->data);

        code_term(leaf, frame, list, *var_name, data_type);
	}
	// spracovani uzlu vyrazu.. urcuje binarni operaci
	else
	{
        // uzel binarni operace
        ast_node_t *node = (ast_node_t *)(expr_elem->data);
        // hodnota cisla nazvu pomocni promenne prvniho operandu
		int var_1;
        char type_1;
        err = code_expr(node->elems[0], frame, &var_1, &type_1, list);
        if (err != E_OK)
            tuttifruttimakro(E_INTERNAL);

        // hodnota cisla nazvu pomocni promenne druheho operandu
		int var_2;
        char type_2;
        err = code_expr(node->elems[1], frame, &var_2, &type_2, list);
        if (err != E_OK)
            tuttifruttimakro(E_INTERNAL);

        // prevod na stejne formaty
        switch (node->type) {
            case AT_SLASH:
                var_1 = code_cast_or_not(frame, list, var_1, type_1, 'd', true);
                var_2 = code_cast_or_not(frame, list, var_2, type_2, 'd', true);
                *data_type = 'd'; // vysledny datovy typ
                break;
            case AT_BACKSLASH:
                // prevod na integer
                var_1 = code_cast_or_not(frame, list, var_1, type_1, 'i', true);
                var_2 = code_cast_or_not(frame, list, var_2, type_2, 'i', true);

                // prevod na double
                var_1 = code_cast_or_not(frame, list, var_1, 'i', 'd', true);
                var_2 = code_cast_or_not(frame, list, var_2, 'i', 'd', true);
                *data_type = 'i'; // vysledny datovy typ
                break;

            default:
                if (type_1 == 's' && type_2 == 's') {
                    *data_type = 's';
                } else {
                    char type = (type_1 == 'd' || type_2 == 'd') ? 'd' : 'i';
                    var_1 = code_cast_or_not(frame, list, var_1, type_1, type, true);
                    var_2 = code_cast_or_not(frame, list, var_2, type_2, type, true);
                    *data_type = type; // vysledny datovy typ
                }
        }

        code_operation_t *op_el = trash_add(sizeof(code_operation_t), &err);
        if(err != E_OK)
            return E_INTERNAL;
        if (op_el == NULL) {
            tuttifruttimakro(E_INTERNAL);
        }

        op_el->op1 = var_1;
        op_el->op2 = var_2;

        list_insert_last(list, op_el, 'n');

        // nastaveni nazvu operace
        switch (node->type) {
            case AT_PLUS:
                op_el->operation = *data_type == 's' ? "CONCAT" : "ADD";
                op_el->dst = *var_name;
                break;
            case AT_MINUS:
                op_el->operation = "SUB";
                op_el->dst = *var_name;
                break;
            case AT_MULTIPLY:
                op_el->operation = "MUL";
                op_el->dst = *var_name;
                break;
            case AT_SLASH:
                op_el->operation = "DIV";
                op_el->dst = *var_name;
                break;

            case AT_BACKSLASH: {
                op_el->operation = "DIV";
                op_el->dst = code_varname_gen(frame);
                *var_name = code_cast_or_not(frame, list, op_el->dst, 'd', 'i', false);
                break;
            }

            default:
                tuttifruttimakro(E_INTERNAL);
        }
	}

	return err;
}

error_t code_print_operand(code_operand_t *op) {
    if (op == NULL)
        return E_OK;

    fprintf(stdout, " ");
    /* numb:
    * -1 : $<name> | <frame>@<name>    (navesti funkci nebo promenna bez cisla)
    * -2 : $<name>$epilog              (u returnu)
    * -3 : <name>                      (datovy typ, nebo konstanta bool@true)
    * -4 : double literal
    * -5 : int literal
    * -6 : string literal // je treba prevest pomoci code_convert_string(str_val)
    */
    switch (op->numb) {
        case -1:
            if (op->frame) {
                fprintf(stdout, "%s@%s", op->frame, (char*)op->name);
            } else {
                fprintf(stdout, "$%s", (char*)op->name);
            }
            break;

        case -2:
            fprintf(stdout, "$%s$epilog\n", (char*)op->name);
            break;

        case -3:
            fprintf(stdout, "%s", (char*)op->name);
            break;

        case -4:
            fprintf(stdout, "float@%g", *(double*)op->name);
            break;

        case -5:
            fprintf(stdout, "int@%d", *(int*)op->name);
            break;

        case -6:
            fprintf(stdout, "string@%s", code_convert_string(op->name));
            break;

        default:
            if (op->frame && !op->name) // docasna promenna
                fprintf(stdout, "%s@&ExprTmp%d", op->frame, op->numb);
            else if (!op->frame && op->name) // navesti na while nebo if
                fprintf(stdout, "%s%d", (char*)op->name, op->numb);
            else
                tuttifruttimakro(E_INTERNAL);
    }

    return E_OK;
}

error_t code_stats_gen(char *frame, list_t *list) {

    error_t err = E_OK;

    /*if (label_name != NULL) {
        fprintf(stdout, "LABEL %s%d\n", label_name, label_numb);
    }*/

    list_el_ptr el = list->first;
    while (el != NULL) {
        if (el->type == 'n') {

            code_operation_t *op = el->data;

            if (op->op2 == -1) {
                // bez druheho operandu
                fprintf(stdout, "%s %s@&ExprTmp%d %s@&ExprTmp%d\n", op->operation, frame, op->dst, frame, op->op1);
            } else {
                // s druhym operandem
                fprintf(stdout, "%s %s@&ExprTmp%d %s@&ExprTmp%d %s@&ExprTmp%d\n", op->operation, frame, op->dst, frame, op->op1, frame, op->op2);
            }

        } else if (el->type == 's') {
            code_statement_t *statement = el->data;
            fprintf(stdout, "%s", statement->name);

            err = code_print_operand(statement->a);
            if (err != E_OK)
                return err;

            err = code_print_operand(statement->b);
            if (err != E_OK)
                return err;

            err = code_print_operand(statement->c);
            if (err != E_OK)
                return err;

            fprintf(stdout, "\n");

        } else {
            tuttifruttimakro(E_INTERNAL);
        }

        el = el->ptr_next;
    }

    return err;

}

error_t code_params(ast_node_t *params_node, char *frame, list_t *list, list_t *param_list) {
    list_el_ptr formal_el = param_list->first;
    error_t err;

    for (unsigned i = 0; i < params_node->cap; i++) {
        variable_t *formal_par = formal_el->data;
        ast_leaf_t *real_leaf = params_node->elems[i]->data;

        if (formal_el == NULL)
            tuttifruttimakro(E_INTERNAL);

        int tmp_var = code_varname_gen(frame);
        char data_type;
        code_term(real_leaf, frame, list, tmp_var, &data_type);

        {
            code_statement_t *statement = trash_add(sizeof(code_statement_t), &err);
            if(err != E_OK)
                return E_INTERNAL;
            statement->name = "DEFVAR";

            statement->a = trash_add(sizeof(code_operand_t), &err);
            if(err != E_OK)
                return E_INTERNAL;
            statement->a->frame = "TF";
            statement->a->name = formal_par->name;
            statement->a->numb = -1;

            statement->b = NULL;
            statement->c = NULL;

            list_insert_last(list, statement, 's');
        }

        //fprintf(stdout, "DEFVAR TF@%s\n", formal_par->name);

        code_param_cast(frame, list, formal_par, data_type, tmp_var);

        formal_el = formal_el->ptr_next;
    }

    return E_OK;
}

int code_varname_gen(char *frame) {
	static int i = 0;
    i++;
    fprintf(stdout, "DEFVAR %s@&ExprTmp%d\n", frame, i);
	return i;
}

int code_label_gen(){
	static int i = 0;
	return i++;
}

void code_param_cast(char *frame, list_t *list, variable_t *var, char real_type, int tmp_var) {
    code_cast(frame, "TF", var->data_type, var->name, real_type, tmp_var, list);
}

void code_cast(char *src_frame, char *dst_frame, char dst_type, char *dst_name, char real_type, int tmp_var, list_t *list) {
    error_t err;
    code_statement_t *statement = trash_add(sizeof(code_statement_t), &err);
    statement->name = "MOVE";

    statement->a = trash_add(sizeof(code_operand_t), &err);
    statement->a->frame = dst_frame;
    statement->a->name = dst_name;
    statement->a->numb = -1;

    statement->b = trash_add(sizeof(code_operand_t), &err);
    statement->b->frame = src_frame;
    statement->b->name = NULL;
    statement->b->numb = tmp_var;

    statement->c = NULL;

    list_insert_last(list, statement, 's'); // s jako statement

    if (dst_type == 'i' && real_type == 'd')
        statement->name = "FLOAT2R2EINT";
    else if (dst_type == 'd' && real_type == 'i')
        statement->name = "INT2FLOAT";
}

int code_round_float(char *frame, int input, list_t *list) {

    int tmp = code_varname_gen(frame);
    int out = code_varname_gen(frame);
    error_t err;

    code_operation_t *op = trash_add(sizeof(code_operation_t), &err); // TODO neosetreny malloc
    if(err != E_OK)
        return E_INTERNAL;
    op->operation = "FLOAT2R2EINT";
    op->dst = tmp;
    op->op1 = input;
    op->op2 = -1;
    list_insert_last(list, op, 'n');

    op = trash_add(sizeof(code_operation_t), &err);
    op->operation = "INT2FLOAT";
    op->dst = out;
    op->op1 = tmp;
    op->op2 = -1;
    list_insert_last(list, op, 'n');

    return out;
}

char *code_convert_string(char *src) {
    code_buffer->size = 0;
    char last_char = 0;

    for (int i = 0; src[i] != '\0'; i++) {

        bool backslash_eaten = false;

        if (src[i] == ' ') {
            code_buffer_append("\\032");
        } else if (src[i] == '#') {
            code_buffer_append("\\035");
        } else if (last_char == '\\') {
            backslash_eaten = true;
            switch (src[i]) {
                case '\\':
                    code_buffer_append("\\092");
                    break;
                case 'n':
                    code_buffer_append("\\010");
                    break;
                case 't':
                    code_buffer_append("\\009");
                    break;
                case '"':
                    code_buffer_append("\\034");
                    break;
                default:
                    buffer_append(code_buffer, '\\');
                    buffer_append(code_buffer, src[i]);
                    backslash_eaten = false;
            }
        } else if (src[i] != '\\') {
            buffer_append(code_buffer, src[i]);
        }
        if (backslash_eaten)
            last_char = 0;
        else
            last_char = src[i];
    }
    buffer_append(code_buffer, 0);

    return code_buffer->string;
}

void code_buffer_append(char *string) {
    for (int i = 0; string[i] != 0; i++)
        buffer_append(code_buffer, string[i]);
}

// ----------------------------- NASLEDUJI VESTAVENE FUNKCE -------------------------------------

// TODO pozor na deklaracie vo whiloch

void code_builtin_length(){
	// prolog
	fprintf(stdout, "LABEL $length # start of built in function length\n");
	fprintf(stdout, "PUSHFRAME\n");
	fprintf(stdout, "DEFVAR LF@%%retval\n");
	//fprintf(stdout, "MOVE LF@%%retval int@0\n");
	
	// telo funkce
	fprintf(stdout, "STRLEN LF@%%retval LF@s\n");	//%retval nyni obsahuje delku retezce s
	
	// epilog
	fprintf(stdout, "POPFRAME\n");
	fprintf(stdout, "RETURN\n");
}

/*
 * SubStr( s As String, i As Integer, n As Integer) As String
*/
void code_builtin_substr() {
    fprintf(stdout, "# ------------------------------------------------------------------\n");
    fprintf(stdout, "LABEL $substr\n");

    fprintf(stdout, "PUSHFRAME\n");
    fprintf(stdout, "# zistenie dlzky vstupneho retazca\n");
    fprintf(stdout, "DEFVAR LF@len_s\n");
    fprintf(stdout, "STRLEN LF@len_s LF@s\n");

    fprintf(stdout, "# inicializacia vyslednej hodnoty\n");
    fprintf(stdout, "DEFVAR LF@%%retval\n");
    fprintf(stdout, "MOVE LF@%%retval string@\n");

    fprintf(stdout, "# pomocne premenne\n");
    fprintf(stdout, "DEFVAR LF@tmp1\n");
    fprintf(stdout, "DEFVAR LF@tmp2\n");

    fprintf(stdout, "# if len_s == 0 || i <= 0\n");
    fprintf(stdout, "JUMPIFEQ $substr$epilog LF@len_s int@0\n");

    fprintf(stdout, "GT LF@tmp1 LF@i int@0\n");
    fprintf(stdout, "JUMPIFNEQ $substr$epilog LF@tmp1 bool@true\n");

    fprintf(stdout, "# if n < 0 || n > len_s - i\n");
    fprintf(stdout, "LT LF@tmp1 LF@n int@0\n");
    fprintf(stdout, "JUMPIFEQ $substr$if LF@tmp1 bool@true\n");

    fprintf(stdout, "SUB LF@tmp1 LF@len_s LF@i\n");
    fprintf(stdout, "GT LF@tmp2 LF@n LF@tmp1\n");
    fprintf(stdout, "JUMPIFEQ $substr$if LF@tmp2 bool@true\n");

    fprintf(stdout, "JUMP $substr$init\n");
    fprintf(stdout, "LABEL $substr$if\n");

    fprintf(stdout, "# n = len_s - i + 1\n");
    fprintf(stdout, "ADD LF@n LF@len_s int@1\n");
    fprintf(stdout, "SUB LF@n LF@n LF@i\n");

    fprintf(stdout, "LABEL $substr$init\n");

    fprintf(stdout, "# k = i - 1\n");
    fprintf(stdout, "DEFVAR LF@k\n");
    fprintf(stdout, "SUB LF@k LF@i int@1\n");

    fprintf(stdout, "# end = i - 1 + n\n");
    fprintf(stdout, "DEFVAR LF@end\n");
    fprintf(stdout, "ADD LF@end LF@k LF@n\n");

    fprintf(stdout, "LABEL $substr$loop\n");

    fprintf(stdout, "# while : k < end\n");
    fprintf(stdout, "LT LF@tmp1 LF@k LF@end\n");
    fprintf(stdout, "JUMPIFNEQ $substr$epilog LF@tmp1 bool@true\n");

    fprintf(stdout, "# tmp1 = s[k]\n");
    fprintf(stdout, "GETCHAR LF@tmp1 LF@s LF@k\n");

    fprintf(stdout, "# %%retval += tmp1\n");
    fprintf(stdout, "CONCAT LF@%%retval LF@%%retval LF@tmp1\n");

    fprintf(stdout, "# k++\n");
    fprintf(stdout, "ADD LF@k LF@k int@1\n");

    fprintf(stdout, "JUMP $substr$loop\n");

    fprintf(stdout, "LABEL $substr$epilog\n");

    fprintf(stdout, "POPFRAME\n");
    fprintf(stdout, "RETURN\n");
    fprintf(stdout, "# ------------------------------------------------------------------\n");
}

void code_builtin_asc(){





// prolog
	fprintf(stdout, "LABEL $asc # start of built in function asc\n");
	fprintf(stdout, "PUSHFRAME\n");
	fprintf(stdout, "DEFVAR LF@%%retval\n");
	fprintf(stdout, "MOVE LF@%%retval int@0\n");
	
// ---------------------------------------------------------------------------------------------------------------------
// telo funkce

    fprintf(stdout, "# zjisteni delky retezce\n");
    fprintf(stdout, "DEFVAR LF@len_s\n");
    fprintf(stdout, "STRLEN LF@len_s LF@s\n");
    fprintf(stdout, "\n");
    fprintf(stdout, "# implicitna hodnota\n");
    //fprintf(stdout, "DEF LF@%%retval\n");
    //fprintf(stdout, "MOVE LF@%%retval int@0\n");
    fprintf(stdout, "\n");
    fprintf(stdout, "# if i <= 0\n");
    fprintf(stdout, "DEFVAR LF@tmp\n");
    fprintf(stdout, "GT LF@tmp LF@i int@0\n");
    fprintf(stdout, "JUMPIFNEQ $asc$epilog LF@tmp bool@true\n");
    fprintf(stdout, "\n");
    fprintf(stdout, "# if i > len_s\n");
    fprintf(stdout, "GT LF@tmp LF@i LF@len_s\n");
    fprintf(stdout, "JUMPIFEQ $asc$epilog LF@tmp bool@true\n");
    fprintf(stdout, "\n");
    fprintf(stdout, "# tmp = i - 1\n");
    fprintf(stdout, "SUB LF@tmp LF@i int@1\n");
    fprintf(stdout, "\n");
    fprintf(stdout, "# %%retval = s[tmp]\n");
    fprintf(stdout, "STRI2INT LF@%%retval LF@s LF@tmp\n");
    fprintf(stdout, "\n");
    //fprintf(stdout, "LABEL $asc$epilog\n");


/*	fprintf(stdout, "DEFVAR LF@string_length\n");
	fprintf(stdout, "MOVE LF@string_length int@0\n");

	fprintf(stdout, "DEFVAR LF@bool\n");
	fprintf(stdout, "MOVE LF@bool bool@false\n");

    fprintf(stdout, "DEFVAR LF@i_sub_1\n");
    fprintf(stdout, "SUB LF@i_sub_1 LF@i int@1\n");

    //call strlength abychom overili zda neni pozice "i" mimo retezec
	fprintf(stdout, "STRLEN LF@string_length LF@s\n");

	fprintf(stdout, "GT LF@bool LF@i LF@string_length\n");	//zjisti, jestli je parametr "i" vetsi nez delka retezce
	fprintf(stdout, "JUMPIFEQ $asc$epilog LF@bool bool@true\n");	//pokud "i" je vetsi nez delka retezce, vrati se 0

	
	fprintf(stdout, "STRI2INT LF@%%retval LF@s LF@i_sub_1\n");*/

// ---------------------------------------------------------------------------------------------------------------------
// epilog
	fprintf(stdout, "LABEL $asc$epilog # end of function asc\n");
	fprintf(stdout, "POPFRAME\n");
	fprintf(stdout, "RETURN\n");
}


void code_builtin_chr(){
	// prolog
	fprintf(stdout, "LABEL $chr # start of built in function chr\n");
	fprintf(stdout, "PUSHFRAME\n");
	fprintf(stdout, "DEFVAR LF@%%retval\n");
	
	// telo funkce
	fprintf(stdout, "INT2CHAR LF@%%retval LF@i\n");	//je-li "i" mimo interval [0,255] ma mit funkce nedefinovane chovani, tzn.instrukce INT2CHAR vygeneruje chybu 58

	// epilog
	fprintf(stdout, "POPFRAME\n");
	fprintf(stdout, "RETURN ###\n\n\n\n");
}