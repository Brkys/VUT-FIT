/**
 * @file garbage.c
 * @author Zdenek Brhel
 * @date 06-12 2017
 * @brief Garbage collector
 */

#include "garbage.h"

error_t trash_init()
{
	trash = malloc(sizeof(trashbin_t));
	if(trash == NULL)
		return E_INTERNAL;
	trash->first = NULL;
	return E_OK;
}

garbage_t *garbage_init(void *ptr)
{
	garbage_t *garbage = malloc(sizeof(garbage_t));
	if(garbage == NULL)
		return NULL;
	garbage->ptr = ptr;
	garbage->next = NULL;
	return garbage;
}

void * trash_add(size_t size, error_t* err)
{
	void *ptr = malloc(size);
	if(ptr == NULL)
	{
		*err = E_INTERNAL;
		return NULL;
	}
	garbage_t *garbage = garbage_init(ptr);
	if(garbage == NULL)
	{
		*err = E_INTERNAL;
		return NULL;;
	}
	garbage->next = trash->first;
	trash->first = garbage;
	*err = E_OK;
	return ptr;
}

void * trash_add_calloc(unsigned N, size_t size, error_t *err)
{
	void *ptr = calloc(N, size);
	if(ptr == NULL)
	{
		*err = E_INTERNAL;
		return NULL;
	}
	garbage_t *garbage = garbage_init(ptr);
	if(garbage == NULL)
	{
		*err = E_INTERNAL;
		return NULL;;
	}
	garbage->next = trash->first;
	trash->first = garbage;
	*err = E_OK;
	return ptr;
}

void * trash_add_realloc(void *ptr, size_t size, error_t *err)
{
	void *tmp = realloc(ptr, size);
	if(tmp == NULL)
	{
		*err = E_INTERNAL;
		return NULL;
	}

	if(tmp != ptr)
	{
		garbage_t *aux = trash->first;
		while (aux)
		{
			if(aux->ptr == ptr)
			{
				aux->ptr = tmp;
				break;
			}
			aux = aux->next;
		}
	}
	*err = E_OK;
	return tmp;
}

void trash_empty()
{
	garbage_t *garbage = trash->first;
	while(garbage != NULL)
	{
		garbage_t *tmp = garbage;
		garbage = garbage->next;
		free(tmp->ptr);
		free(tmp);
	}
	free(trash);
}