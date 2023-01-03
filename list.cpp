#include "list.h"

const char* Elem_out = "%d";
const char* name_graph_file = "graph.dot";

Elem_t List_pop(list* ls) //pop, delete, erase
{
	unsigned err = List_Verify (ls);
	if (err != LIST_OK) return err;
	if (ls->data[0].next == 0) return LIST_IS_EMPTY;

	size_t num_out = ls->data[0].prev;
	*val = ls->data[num_out].elem;

	size_t prev = ls->data[num_out].prev;
	size_t next = ls->data[num_out].next;

	ls->data[prev].next = next;
	ls->data[next].prev = prev;

	ls->data[num_out].prev = ls->size - 1;
	ls->data[num_out].next = ls->data[ls->size - 1].next;
	ls->data[num_out].elem = POISON; //def
	ls->data[ls->size - 1].next = num_out;
	ls->data[ls->data[num_out].next].prev = num_out;

	err = List_Verify(ls);
	return err;
}


void ListInsert_head(list* ls, Elem_t x, size_t ip, size_t* index)
{
	unsigned err = List_Verify (ls);

	if (err != LIST_OK) return err;
	if (Found_index(ls, ip) != ip) return INDEX_IS_FREE;
	
	size_t num_free = ls->data[ls->size - 1].next;
	size_t new_num_free = ls->data[num_free].next;
	size_t next_for_elem = ls->data[ip].next;
	ls->data[ls->size - 1].next = new_num_free;
	ls->data[new_num_free].prev = ls->size - 1;

	ls->data[num_free].elem = x;
	ls->data[num_free].prev = ip;
	ls->data[num_free].next = next_for_elem;

	ls->data[ip].next = num_free;
	ls->data[next_for_elem].prev = num_free;
	*index = num_free;

	err = List_Verify (ls);
	return err;
}


void ListInsert_tail(list* ls, Elem_t x, size_t ip, size_t* index) //val ind, change
{
	unsigned err = List_Verify (ls);

	if (err != LIST_OK) return err;
	if (Found_index(ls, ip) != ip) return INDEX_IS_FREE;
	if (ip == 0) ip = ls->data[0].next;

	size_t num_free = ls->data[ls->size - 1].next;
	size_t new_num_free = ls->data[num_free].next;
	size_t prev = ls->data[ip].prev;
	ls->data[ls->size - 1].next = new_num_free; //LAST_ELEM_OF (ls) = new_num_free.
	ls->data[new_num_free].prev = ls->size - 1; //PREV_ELEM_OF (ls, new_num_free) = LIST_SIZE ()

	ls->data[num_free].elem = x;
	ls->data[num_free].next = ip;
	ls->data[num_free].prev = prev;

	ls->data[ip].prev   = num_free;
	ls->data[prev].next = num_free;
	*index = num_free;

	err = List_Verify (ls);
	return err;
}

list* ListCtor (size_t n)
{
	if (n < min_size_data) n = min_size_data;

	list* ls = (list*) calloc(1, sizeof(list));

	/*if (ls == NULL)
		return HAS_NOT_MEMORY;
	*/
	ls->data = (elem*) calloc(n, sizeof(elem));
	/*if((*ls)->data == NULL)
		return HAS_NOT_MEMORY;
	*/
	ls->size = n;
	ls->data[0].next = ls->data[0].prev = (*ls)->data[0].elem = 0;
	for (size_t i = 1; i < ls->size; i++)
	{
		ls->data[i].elem = POISON;
		ls->data[i].next = (i == ls->size - 1) ? 1 : (i + 1);
		ls->data[i].prev = (i == 1) ? ls->size - 1 : (i - 1);
	}

	return ls;
}

void List_Dtor (list* ls)
{
	//todo check list
	free (ls->data);
	ls->data = (elem*) POISON_DATA;
	ls->size = (unsigned) POISON_SIZE;

	#ifdef DUMP
		ls->info.name = ls->info.func = ls->info.file = (const char*) POISON_NAME;
		ls->info.line = (unsigned) POISON_LINE;
	#endif
}

unsigned List_Verify (list* ls)
{
	unsigned err = LIST_OK;

	if (ls == nullptr) err |= LIST_NULL_PTR;
	else
	{
		if (ls->data == nullptr || ls->data == (elem*) POISON_DATA)
			err |= LIST_NULL_PTR_DATA;
		else
		{
			if (ls->size < 0 || ls->size == (unsigned) POISON_SIZE)
			{
				err |= INVALID_SIZE;
				return err;
			}
			size_t 	occupied_zone = 0;
			size_t 	free_zone 	  = 0;
			size_t 	index 		  = 0,
					next_index	  = 0;

			do
			{
				next_index = ls->data[index].next; //todo check???
				if (index != ls->data[next_index].prev)
				{
					err |= WRONG_DATA;
					break;
				}
				index = next_index;
				occupied_zone++;
			} while (index != 0);
			index = ls->size - 1;
			do //define
			{
				next_index = ls->data[index].next;
				if (index != ls->data[next_index].prev)
				{
					err |= WRONG_DATA;
					break;
				}
				index = next_index;
				free_zone++;
			} while (index != ls->size - 1);

			if (free_zone + occupied_zone < ls->size) err |= LIST_SKIP_ELEM;
			else if (free_zone + occupied_zone > ls->size) err |= INVALID_SIZE;
		}
	}
	return err;
}

unsigned PrintfList (list* ls)
{
	unsigned err = List_Verify (ls);
	if (err != LIST_OK) return err;

	printf ("ls[%p](%s) \"%s\" at %s at %s(%d)\n"
			"{ \n"
			"    size =     %lu\n"
			"    ls->data[%p]\n"
			"    {\n",
			ls, (err == LIST_OK) ? "Ok" : "ERROR", ls->info.name, ls->info.func, ls->info.file, ls->info.line,
			ls->size, ls->data);

	printf ("index: ");
	for (int i = 0; i < ls->size; ++i)
		printf ("%4d | ", i);

	printf ("\neltms: ");
	for (int i = 0; i < ls->size; ++i)
		printf ("%4d | ", ls->data[i].elem); //define

	printf ("\nnext : ");
	for (int i = 0; i < ls->size; ++i)
		printf ("%4d | ", ls->data[i].next);

	printf ("\nprev : ");
	for (int i = 0; i < ls->size; ++i)
		printf ("%4d | ", ls->data[i].prev);

	printf ("\n	}\n"
			"}\n");

	return err;
}

void ListSort (list* ls)
{
	size_t index = 0;
	size_t i = 0;
	do
	{
		ls->data[index].prev = i++;
		index = ls->data[index].next;
	} while (index != 0);

	index = ls->data[ls->size - 1].next;
	do
	{
		ls->data[index].prev = i++;
		index = ls->data[index].next;
	} while (index < ls->size - 1);
	ls->data[index].prev = i++;
	ListDump(ls, List_Verify(ls));
	qsort (ls->data, ls->size - 1, sizeof(elem), List_Compare);

	i = 0;
	while(ls->data[i].next != 0)
	{
		ls->data[i].next = i+1;
		i++;
		ls->data[i].prev = i-1;
	}
	ls->data[0].prev = i;
	size_t j = i+1;
	i++;
	while (i < ls->size - 1)
	{
		ls->data[i].next = i+1;
		i++;
		ls->data[i].prev = i-1;
	}
	ls->data[i].next = j;
	ls->data[j].prev = i;
}

int List_Compare (const void* a, const void* b)
{
	assert(a != NULL);
	assert(b != NULL);

	const elem* a_ptr = (const elem*) a;
	const elem* b_ptr = (const elem*) b;

	return (a_ptr->prev < b_ptr->prev) ? -1 : 1;
}

size_t Find_index_by_num (const list* ls, Elem_t x)
{
	size_t index = 0;
	do
	{
		index = ls->data[index].next;
		if (ls->data[index].elem == x) break;
	} while (index != 0);

	return index;
}

size_t At_list (const list* ls, size_t index) //todo
{
	size_t ip = 0;
	do
	{
		if (ip == index) break;
		ip = ls->data[ip].next;
	} while (ip != 0 && ip != index);

	return ip;
}