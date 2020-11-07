/**
 * @file garbage.c
 * @author Zdenek Brhel
 * @date 06-12 2017
 * @brief Garbage collector
 */

#include <stdlib.h>
#include <stdio.h>
#include "errors.h"


#ifndef IFJ_GARBAGE_H
#define IFJ_GARBAGE_H


typedef struct garbage{
	void *ptr;
	struct garbage *next;
} garbage_t;

typedef struct {
	garbage_t *first;
} trashbin_t;

trashbin_t *trash;

error_t trash_init();

garbage_t *garbage_init(void *ptr);

void* trash_add(size_t size, error_t* err);

void * trash_add_calloc(unsigned N, size_t size, error_t *err);

void trash_empty();

void * trash_add_realloc(void *ptr, size_t size, error_t *err);

#endif