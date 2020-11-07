/**
 * @file stack.c
 * @author Filip Borcik
 * @date 11-12 2017
 * @brief Soubor obsahujujici vytvareni a pracu se zasobnikem
 */


#include "stack.h"

stack_t *stack_init (){ //inicializace stacku
	return list_init();
}

bool stack_empty (stack_t *stack){
	return stack->last == NULL; //pokud je nejvyssi prvek ve stacku NULL
}

stack_el_ptr stack_top (stack_t *stack){
	return stack->last; //vrti nejvyssi prvek
}

void stack_pop (stack_t *stack){
	list_delete_last(stack); //smaze nejvyssi prvek
}

void stack_pop_until_param(stack_t *stack){
	stack_el_ptr tmp = stack_top(stack);
	while (tmp != NULL && *((char*)(tmp->data)) != '<' ){
		stack_pop(stack); //popuje vse az do '<'
		tmp = stack_top(stack);
	}
	if (tmp != NULL) // na vrcholu zasobniku je '<'
		stack_pop(stack);
}

bool stack_push (stack_t *stack, void *data, int type){
	return list_insert_last(stack, data, type); //prida prvek na vrchol stacku
}
bool stack_push_above(stack_t *stack, stack_el_ptr elem, void *data, int type){
	if (elem)
		return list_post_insert(stack, elem, data, type); //prida prvek nad prvek
	else
		return list_insert_first(stack, data, type); //pokud je prvek prazdny, prida na zacatek
}

stack_el_ptr stack_find_from_top_by_type(stack_t *stack, int type) {
	return list_find_from_end_by_type(stack, type); 
}