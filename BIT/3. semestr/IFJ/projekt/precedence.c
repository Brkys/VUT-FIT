/**
 * @file precedence.c
 * @author Filip Borcik, Simeon Borko
 * @date 11-12 2017
 * @brief Soubor obsahujici algoritmus precedencni syntakticke analyzy
 */

#include "precedence.h"

#define tuttifruttimakro(err) { fprintf(stderr, "Chyba : %s\t: %s\t: %d\n", __FILE__, __func__, __LINE__); return err; }


char precedence_table[15][15] = {
//	  *		 /		 \		 +		 -		 =		<>		 <		<=		 >		>=		 (		 )		ID		 $	 
	{'>',	'>',	'>',	'>',	'>',	'>',	'>',	'>',	'>',	'>',	'>',	'<',	'>',	'<',	'>'	},	// * 
	{'>',	'>',	'>',	'>',	'>',	'>',	'>',	'>',	'>',	'>',	'>',	'<',	'>',	'<',	'>'	},	// /
	{'<',	'<',	'>',	'>',	'>',	'>',	'>',	'>',	'>',	'>',	'>',	'<',	'>',	'<',	'>'	},	// \ (celociselne deleni)
	{'<',	'<',	'<',	'>',	'>',	'>',	'>',	'>',	'>',	'>',	'>',	'<',	'>',	'<',	'>'	},	// +
	{'<',	'<',	'<',	'>',	'>',	'>',	'>',	'>',	'>',	'>',	'>',	'<',	'>',	'<',	'>'	},	// -
	{'<',	'<',	'<',	'<',	'<',	'E',	'E',	'E',	'E',	'E',	'E',	'<',	'>',	'<',	'>'	},	// =
	{'<',	'<',	'<',	'<',	'<',	'E',	'E',	'E',	'E',	'E',	'E',	'<',	'>',	'<',	'>'	},	// <>
	{'<',	'<',	'<',	'<',	'<',	'E',	'E',	'E',	'E',	'E',	'E',	'<',	'>',	'<',	'>'	},	// <
	{'<',	'<',	'<',	'<',	'<',	'E',	'E',	'E',	'E',	'E',	'E',	'<',	'>',	'<',	'>'	},	// <=
	{'<',	'<',	'<',	'<',	'<',	'E',	'E',	'E',	'E',	'E',	'E',	'<',	'>',	'<',	'>'	},	// >
	{'<',	'<',	'<',	'<',	'<',	'E',	'E',	'E',	'E',	'E',	'E',	'<',	'>',	'<',	'>'	},	// >=
	{'<',	'<',	'<',	'<',	'<',	'<',	'<',	'<',	'<',	'<',	'<',	'<',	'=',	'<',	'E'	},	// (
	{'>',	'>',	'>',	'>',	'>',	'>',	'>',	'>',	'>',	'>',	'>',	'E',	'>',	'E',	'>'	},	// )
	{'>',	'>',	'>',	'>',	'>',	'>',	'>',	'>',	'>',	'>',	'>',	'E',	'>',	'E',	'>'	},	// ID
	{'<',	'<',	'<',	'<',	'<',	'<',	'<',	'<',	'<',	'<',	'<',	'<',	'E',	'<',	'E'	},	// $
};


/*char precedence_table[9][9] = {
//	  *		 /		 \		 +		 -		 (		 )		ID		 $	 
	{'>',	'>',	'>',	'>',	'>',	'<',	'>',	'<',	'>'	},	// * 
	{'>',	'>',	'>',	'>',	'>',	'<',	'>',	'<',	'>'	},	// / 
	{'<',	'<',	'>',	'>',	'>',	'<',	'>',	'<',	'>'	},	// \ (celociselne deleni)
	{'<',	'<',	'<',	'>',	'>',	'<',	'>',	'<',	'>'	},	// +
	{'<',	'<',	'<',	'>',	'>',	'<',	'>',	'<',	'>'	},	// -
	{'<',	'<',	'<',	'<',	'<',	'<',	'=',	'<',	'E'	},	// (
	{'>',	'>',	'>',	'>',	'>',	'E',	'>',	'E',	'>'	},	// )
	{'>',	'>',	'>',	'>',	'>',	'E',	'>',	'E',	'>'	},	// ID
	{'<',	'<',	'<',	'<',	'<',	'<',	'E',	'<',	'E'	},	// $
};*/

stack_el_ptr header_top(stack_t *stack) {
	return stack_find_from_top_by_type(stack, IN_HEADER_PT);
}

error_t precedence_algorithm(token_t **token_ptr, buffer_t *buffer,
                              hashtable_t *ht, bool condition,
                              ast_elem_t **elem_ptr, char *final_data_type) {
	
	stack_t *stack = stack_init();					// inicializace zasobniku pro precedencni SA

	if (stack == NULL)
	{
		tuttifruttimakro(E_INTERNAL);
	}

	int repetition = 0; 	// pocet cyklu

	// cyklus algoritmu precedencni syntakticke analyzy
	while (true) {

        // zjisteni typu vstupniho tokenu
        precedence_token_t input_token_type = precedence_token_type(*token_ptr);

        // v pripade prazdneho vyrazu vraci funkce E_SYNTACTIC
		if(input_token_type == PT_EMPTY && repetition == 0)
			return E_SYNTACTIC;
        
        precedence_header_t *new_el_header;
        error_t err;

        // ziskani nejvyssiho tokenu na zasobniku
        stack_el_ptr stack_token_el = header_top(stack);
        precedence_token_t stack_token_type = stack_token_el ? ((precedence_header_t *)(stack_token_el->data))->type : PT_EMPTY;
        
        // podminka ukonceni cyklu
        if (input_token_type == PT_EMPTY && stack_token_type == PT_EMPTY)
            break;

//-----------------------------------------------------------------------------------------

        char *green_in_table = &(precedence_table[stack_token_type][input_token_type]);

		switch (*green_in_table) {		// zjisteni hodnoty z precedencni tabulky
			case '<': 
																			// prepsani a za a<
				if (!stack_push_above(stack, stack_token_el, green_in_table, IN_TABLE_PT))
				{
					//if (token)
					//	token_destroy(token);
					tuttifruttimakro(E_INTERNAL);
				}

			case '=':

																			// vytvoreni zahlavi prvku b
				new_el_header = trash_add(sizeof(precedence_header_t), &err);
			    if(err != E_OK)
			    {
			        return E_INTERNAL;
			    }
				if (new_el_header == NULL)
				{
					//if(token)
					//	token_destroy(token);
					tuttifruttimakro(E_INTERNAL);
				}
																			// inicializace hodnot zahlavi
				new_el_header->type = input_token_type;
				if (input_token_type == PT_ID)
					new_el_header->token = *token_ptr;
				else
					new_el_header->token = NULL;
																			// vlozeni prvku b na zasobnik
				if(!stack_push(stack, new_el_header, IN_HEADER_PT))
				{
					//if (token)
					//	token_destroy(token);
					tuttifruttimakro(E_INTERNAL);
				}

				err = token_get(token_ptr, buffer);
				if (err != E_OK)
					tuttifruttimakro(err);
				if (*token_ptr == NULL)
					return E_SYNTACTIC;
								
				break;

			case '>':

				err = precedence_rule_control(stack, ht);
				if (err != E_OK) 	{									// v opacnem pripade doslo k syntakticke chybe
					//if (token)
					//	token_destroy(token);

					tuttifruttimakro(err);
				}

				break;

			default:														// v pripade nepovolene kombinace doslo k syntakticke chybe
				//if(token)
				//	token_destroy(token);
				tuttifruttimakro(E_SYNTACTIC);
				break;
		}

		repetition++;

	}

	stack_el_ptr final_top_el = stack_top(stack); 
	if(final_top_el){
		precedence_nonterminal_t *nonterm = final_top_el->data;

        if (condition != (nonterm->data_type == 'b')) {
            // pokud se nenaplni (ne)ocekavani podminkoveho vyrazu
            *elem_ptr = NULL;
            return E_SEM_TYPE_OR_PARAMS;
        }

        // vraceni uzlu a vysledneho typu
		*elem_ptr = nonterm->ast;
		if (final_data_type != NULL)
			*final_data_type = nonterm->data_type;
	}
	else{
        // pokud na zasobniku nic nezustalo
		*elem_ptr = NULL;
        return E_SYNTACTIC;
	}

    return E_OK;
}

precedence_token_t precedence_token_type (token_t *token){
	switch (token->type) {
		case T_ID:
		case T_INTEGER:
        case T_DOUBLE:
		case T_STRING:
			return PT_ID;
		case T_PLUS:
			return PT_PLUS;
		case T_MINUS:
			return PT_MINUS;
		case T_MULTIPLIER:
			return PT_MULTIPLIER;
		case T_SLASH:
			return PT_SLASH;
		case T_BACKSLASH:
			return PT_BACKSLASH;
		case T_BR_LEFT:
			return PT_BRACKET_LEFT;
		case T_BR_RIGHT:
			return PT_BRACKET_RIGHT;
        case T_EQUAL:
            return PT_EQUAL;
        case T_DIFF:
            return PT_DIFF;
        case T_LESS_THAN:
            return PT_LESS_THAN;
        case T_LESS_EQUAL:
            return PT_LESS_EQUAL;
        case T_GREATER_THAN:
            return PT_GREATER_THAN;
        case T_GREATER_EQUAL:
            return PT_GREATER_EQUAL;
        default:
            return PT_EMPTY;
	}
}

error_t precedence_rule_control (stack_t *stack, hashtable_t *ht) {
	stack_el_ptr first = NULL;
	stack_el_ptr second = NULL;
	stack_el_ptr third = NULL;
	stack_el_ptr fourth = NULL;

	first = stack_top(stack);
	if (first == NULL)
		tuttifruttimakro(E_SYNTACTIC);

	second = first->ptr_prev;
	if(second)
		third = second->ptr_prev;

	if(third)
		fourth = third->ptr_prev;


	// 7: E->ID
	if (first->type == IN_HEADER_PT && ((precedence_header_t *)(first->data))->type == PT_ID &&
		second && second->type == IN_TABLE_PT)
	{
		error_t err;
		precedence_nonterminal_t *nonterm = precedence_id_nonterm(((precedence_header_t *)(first->data))->token, ht, &err);
		if(nonterm == NULL)
			tuttifruttimakro(err);

		stack_pop_until_param(stack);

		if(!stack_push(stack, nonterm, NONTERMINAL_PT))
			tuttifruttimakro(E_INTERNAL);

		return E_OK;
	}

	// 6: E->(E)
	else if (first->type == IN_HEADER_PT && ((precedence_header_t *)(first->data))->type == PT_BRACKET_RIGHT &&
		second && third && fourth &&
		second->type == NONTERMINAL_PT &&
		third->type == IN_HEADER_PT && ((precedence_header_t *)(third->data))->type == PT_BRACKET_LEFT &&
		fourth->type == IN_TABLE_PT)
	{
        void *data = second->data; // TODO uvolnovani pamete?

		stack_pop_until_param(stack);

		if(!stack_push(stack, data, NONTERMINAL_PT))
			tuttifruttimakro(E_INTERNAL);

		return E_OK;	
	}

	else if (second && third && fourth && first->type == NONTERMINAL_PT &&
		third->type == NONTERMINAL_PT && fourth->type == IN_TABLE_PT && second->type == IN_HEADER_PT)
	{
		error_t err;
		precedence_nonterminal_t *nonterm = precedence_op_nonterm((precedence_nonterminal_t *)(third->data), (precedence_nonterminal_t *)(first->data),
                                                                  ((precedence_header_t *)(second->data))->type, &err);
		if(nonterm == NULL)
			tuttifruttimakro(err);

		stack_pop_until_param(stack);

		if(!stack_push(stack, nonterm, NONTERMINAL_PT))
			tuttifruttimakro(E_INTERNAL);
		return E_OK;
	}

	else
		tuttifruttimakro(E_SYNTACTIC);

}

precedence_nonterminal_t *precedence_id_nonterm(token_t *token, hashtable_t *ht, error_t *err) {
	
	char type;
	void *ptr;
	char nonterm_type;
	error_t erro;
	switch(token->type){
		case T_ID:
			type = 'v';

			variable_t *pointer = ht_read(ht, token->value);
			if (!pointer)
			{
				*err = E_SEM_DEF;
				return NULL;
			}

			nonterm_type = pointer->data_type;
			ptr = pointer;

			break;
    	case T_INTEGER:
    		type = 'i';
    		nonterm_type = 'i';
            if (!token_to_int(token, (int **)(&ptr))) {
                *err = E_INTERNAL;
                return NULL;
            }
    		break;
		case T_DOUBLE:
			type = 'd';
			nonterm_type = 'd';
            if (!token_to_double(token, (double **)(&ptr))) {
                *err = E_INTERNAL;
                return NULL;
            }
    		break;
    	case T_STRING:
    		type = 's';
    		nonterm_type = 's';
            if (!token_to_string(token, (char **)(&ptr))) {
                *err = E_INTERNAL;
                return NULL;
            }
    		break;
    	default:
   			*err = E_INTERNAL;
			return NULL;

	}

	ast_elem_t *new_ast_elem;
	*err = ast_create_leaf_elem(type, ptr, &new_ast_elem);
	if (*err != E_OK){
        return NULL;		// Interni chyba (E_INTERNAL)
	}

	precedence_nonterminal_t *nonterm = trash_add(sizeof(precedence_nonterminal_t), &erro);
    if(erro != E_OK)
    {
        *err = E_INTERNAL;
        return NULL;
    }
	if (!nonterm){
		*err = E_INTERNAL;
		return NULL;
	}


	nonterm->data_type = nonterm_type;
	nonterm->ast = new_ast_elem;

	return nonterm;
}

error_t precedence_ast_type(precedence_token_t op, ast_node_type_t *ast_type) {
    switch (op) {
        case PT_MULTIPLIER:
            *ast_type = AT_MULTIPLY;
			break;
        case PT_SLASH:
            *ast_type = AT_SLASH;
			break;
        case PT_BACKSLASH:
            *ast_type = AT_BACKSLASH;
			break;
        case PT_PLUS:
            *ast_type = AT_PLUS;
			break;
        case PT_MINUS:
            *ast_type = AT_MINUS;
			break;

        case PT_EQUAL:
            *ast_type = AT_COND_EQ;
			break;
        case PT_DIFF:
            *ast_type = AT_COND_IE;
			break;
        case PT_LESS_THAN:
            *ast_type = AT_COND_LT;
			break;
        case PT_LESS_EQUAL:
            *ast_type = AT_COND_LE;
			break;
        case PT_GREATER_THAN:
            *ast_type = AT_COND_GT;
			break;
        case PT_GREATER_EQUAL:
            *ast_type = AT_COND_GE;
			break;

        default:
            return E_INTERNAL;
    }
    return E_OK;
}

bool precedence_is_compare(precedence_token_t op) {
    switch (op) {
        case PT_EQUAL:
        case PT_DIFF:
        case PT_LESS_THAN:
        case PT_LESS_EQUAL:
        case PT_GREATER_THAN:
        case PT_GREATER_EQUAL:
            return true;
        default:
            return false;
    }
}

precedence_nonterminal_t *precedence_op_nonterm(precedence_nonterminal_t *e1, precedence_nonterminal_t *e2, precedence_token_t op, error_t *err) {

	char e1t = e1->data_type;
	char e2t = e2->data_type;
	char ntt = 0;					// result nonterminal type
	error_t erro;
    // get ast node type
	ast_node_type_t astt;
    *err = precedence_ast_type(op, &astt); // pokud op je nepovoleny operator, vrati E_INTERNAL
    if (*err != E_OK)
        return NULL;

    if ((e1t == 'i' || e1t == 'd') && (e2t == 'i' || e2t == 'd')) {
        // pokud jsou oba operandy cislem, nastavi se vysledny typ
        if (precedence_is_compare(op))
            ntt = 'b';
        else if (op == PT_BACKSLASH)
            ntt = 'i';
        else if (op == PT_SLASH)
            ntt = 'd';
        else if (e1t == 'i' && e2t == 'i')
            ntt = 'i';
        else
            ntt = 'd';
    } else if (e1t == 's' && e2t == 's') {
        // pokud jsou oba operandy retezcem, nastavi se typ, nebo se skonci s chybou
        if (precedence_is_compare(op))
            ntt = 'b';
        else if (op == PT_PLUS)
            ntt = 's';
        else {
            // nepovoleny operator retezcu
            *err = E_SEM_TYPE_OR_PARAMS;
            return NULL;
        }
    } else {
        // nekompatibilne typy (nebo booleany)
        *err = E_SEM_TYPE_OR_PARAMS;
        return NULL;
    }

	ast_elem_t *new_ast_elem;
	*err = ast_create_node_elem(2, astt, &new_ast_elem);
	if(*err != E_OK)
		return NULL;

	ast_node_t *node = new_ast_elem->data;
	node->elems[0] = e1->ast;
	node->elems[1] = e2->ast;

	precedence_nonterminal_t *nonterm = trash_add(sizeof(precedence_nonterminal_t), &erro);
    if(erro != E_OK) {
		*err = E_INTERNAL;
		return NULL;
	}
	if (!nonterm){
		*err = E_INTERNAL;
		return NULL;
	}

	nonterm->data_type = ntt;
	nonterm->ast = new_ast_elem;

	return nonterm;
}