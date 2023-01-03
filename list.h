#ifndef LIST_H
#define LIST_H

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#define DUMP

typedef int Elem_t;
extern const char* Elem_out;
const int min_size_data = 8; 

enum poisons
{
	POISON      = -1,
	POISON_DATA =  0,
	POISON_NAME =  0,
	POISON_SIZE = -1,
	POISON_LINE = -1,
};

enum errors
{
	LIST_OK        		= 0x0,
	HAS_NOT_MEMORY 		= 0x1, //todo 1 << 1
	LIST_NULL_PTR  		= 0x2,
	LIST_NULL_PTR_DATA 	= 0x4,
	WRONG_DATA			= 0x8,
	LIST_SKIP_ELEM		= 0x10,
	INVALID_SIZE		= 0x20,
	INDEX_IS_FREE		= 0x40,
	EXTRACT_FROM_FREE	= 0x80,
	LIST_IS_EMPTY		= 0x100,
};
/*
enum modes
{
	INSERT_IN_TAIL = 0,
	INSERT_IN_HEAD = 0,
};*/

typedef struct
{
	Elem_t elem;
	int next;
	int prev;
} elem;

#ifdef DUMP

#endif

typedef struct
{	
	elem* data;
	size_t  size;
} list;

list* ListCtor   		(size_t n);
Elem_t list_pop 		(list* ls);
void ListInsert_head(list* ls, Elem_t x, size_t ip, size_t* index);
void ListInsert_tail(list* ls, Elem_t x, size_t ip, size_t* index);
void PrintfList  	(list* ls);

void 	 ListSort 	 	(list* ls);
int 	 List_Compare 	(const void* a, const void* b);
void 	 List_Dtor 	 	(list* ls);
unsigned List_Verify 	(list* ls);

size_t Find_index_by_num(const list* ls, Elem_t x);
size_t Find_index 		(const list* ls, size_t index);

#endif