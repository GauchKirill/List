#include "list.h"
#include "list_dump.h"
#include "errors.h"

#include <stdio.h>
#include <stdlib.h>

Elem_t list_pop (list* ls)
{
	#ifdef DUMP

		unsigned err = list_verify (ls);
		if (next_of(0) == 0) err |= LIST_IS_EMPTY;
		list_dump(ls, err);

	#endif

	size_t num_out = prev_of(0);
	Elem_t val = data_of(num_out);

	size_t prev = prev_of(num_out);
	size_t next = next_of(num_out);

	next_of(prev) = next;
	prev_of(next) = prev;

	prev_of(num_out) = SIZE;
	next_of(num_out) = next_of(SIZE);

	data_of(num_out) = POISON;
	next_of(SIZE) = num_out;
	prev_of(SIZE) = num_out;

	#ifdef DUMP

		list_dump(ls, list_verify(ls));
	
	#endif
		
	return val;
}

size_t list_insert_head(list** ls_ptr, Elem_t val)
{
	return list_insert_after_ind(ls_ptr, 0, val);
}


size_t list_insert_tail(list** ls_ptr, Elem_t val)
{
	return list_insert_before_ind(ls_ptr, 0, val);
}

size_t list_insert_after_ind(list** ls_ptr, size_t ind, Elem_t val)
{
	list* ls = *ls_ptr;

	#ifdef DUMP

		list_dump(ls, list_verify(ls));

	#endif
	
	if(at_list(ls, ind) != ind)
		return 0;

	if (next_of(SIZE) == SIZE)
		ls = list_up_size(ls);

	size_t num_free      = next_of(SIZE);
	size_t new_num_free  = next_of(num_free);
	size_t next_for_elem = next_of(ind);

	next_of(SIZE)         = new_num_free;
	prev_of(new_num_free) = SIZE;

	data_of(num_free) = val;
	prev_of(num_free) = ind;
	next_of(num_free) = next_for_elem;

	next_of(ind) = num_free;
	prev_of(next_for_elem) = num_free;

	*ls_ptr = ls;
	return num_free;
}

size_t list_insert_before_ind(list** ls_ptr, size_t ind, Elem_t val)
{
	list* ls = *ls_ptr;
	#ifdef DUMP

		list_dump(ls, list_verify(ls));

	#endif

	if(at_list(ls, ind) != ind)
		return 0;

	if (next_of(SIZE) == SIZE)
		ls = list_up_size(ls);

	size_t num_free      = next_of(SIZE);
	size_t new_num_free  = next_of(num_free);
	size_t prev_for_elem = prev_of(ind);

	next_of(SIZE) = new_num_free;
	prev_of(new_num_free) = SIZE;
	
	data_of(num_free) = val;
	next_of(num_free) = ind;
	prev_of(num_free) = prev_for_elem;
	
	prev_of(ind) = num_free;
	next_of(prev_for_elem) = num_free;

	*ls_ptr = ls;
	return num_free;
}

list* list_ctor (size_t size)
{
	if (size < MIN_SIZE_DATA) size = MIN_SIZE_DATA;

	list* ls = (list*) calloc(1, sizeof(list));

	if (ls == nullptr)
	{
		list_dump(ls, HAS_NOT_MEMORY);
		return nullptr;
	}
	
	ls->data = (elem*) calloc(size + 1, sizeof(elem));

	if(ls->data == nullptr)
	{
		free(ls);
		list_dump(ls, HAS_NOT_MEMORY);
		return nullptr;
	}

	ls->size = size + 1;
	next_of(0) = prev_of(0) = data_of(0) = 0;
	for (size_t i_elem = 1; i_elem <= SIZE; ++i_elem)
	{
		data_of(i_elem) = POISON;
		next_of(i_elem) = (i_elem == SIZE) ? 1 : (i_elem + 1);
		prev_of(i_elem) = (i_elem == 1) ? SIZE : (i_elem - 1);
	}

	return ls;
}

void list_dtor (list* ls)
{
	if (ls == nullptr)
		return;
	
	free (ls->data);
	ls->data = (elem*) POISON_DATA;
	ls->size = (size_t) POISON_SIZE;
}

void list_sort (list* ls)
{
	#ifdef DUMP

		list_dump(ls, list_verify(ls));

	#endif

	size_t ind = 0;
	size_t i = 0;

	do
	{
		prev_of(ind) = i++;
		ind = next_of(ind);
	} while (ind != 0);

	ind = next_of(SIZE);
	do
	{
		prev_of(ind) = i++;
		ind = next_of(ind);
	} while (ind != SIZE);
	prev_of(ind) = i++;

	qsort (ls->data, ls->size, sizeof(elem), list_compare);

	for (i = 0; next_of(i) != 0; ++i)
	{
		next_of(i) = i+1;
		prev_of(i+1) = i;
	}
	prev_of(0) = i;

	size_t j = i+1;
	i++;
	for(; i < SIZE; ++i)
	{
		next_of(i) = i+1;
		prev_of(i+1) = i;
	}

	next_of(i) = j;
	prev_of(j) = i;
}

int list_compare (const void* a, const void* b)
{
	const elem* a_ptr = (const elem*) a;
	const elem* b_ptr = (const elem*) b;

	return (a_ptr->prev < b_ptr->prev) ? -1 : 1;
}

list* list_up_size(list* ls)
{
	#ifdef DUMP

		list_dump(ls, list_verify(ls));

	#endif

	size_t size = ls->size;
	size_t new_size = 2*size;
	list* new_ls = (list*) calloc(1, sizeof(list));
	if (!new_ls)
	{
		list_dump(ls, HAS_NOT_MEMORY);
		return ls;
	}

	new_ls->size = new_size;
	new_ls->data = (elem*) realloc(ls->data, new_size*sizeof(elem));
	if (!new_ls->data)
	{
		list_dump(ls, HAS_NOT_MEMORY);
		return ls;
	}

	free(ls);

	prev_of_new(next_of_new(size - 1)) = NEW_SIZE;
	
	for (size_t i_elem = size - 1; i_elem <= NEW_SIZE; ++i_elem)
	{
		data_of_new(i_elem) = POISON;
		next_of_new(i_elem) = (i_elem == NEW_SIZE) ? size - 1              : i_elem + 1;
		prev_of_new(i_elem) = (i_elem == size - 1) ? prev_of_new(size - 1) : i_elem - 1;
	}

	return new_ls;
}

size_t find_ind_by_num (const list* ls, Elem_t x)
{
	#ifdef DUMP

		list_dump(ls, list_verify(ls));

	#endif

	size_t ind = 0;
	do
	{
		ind = next_of(ind);
		if (data_of(ind) == x) break;
	} while (ind != 0);

	return ind;
}

size_t at_list (const list* ls, size_t ind)
{
	#ifdef DUMP

		list_dump(ls, list_verify(ls));

	#endif

	size_t ip = 0;
	do
	{
		if (ip == ind) break;
		ip = next_of(ip);
	} while (ip != 0);

	return ip;
}

size_t find_logical_ind(const list* ls, size_t ind)
{
	#ifdef DUMP

		list_dump(ls, list_verify(ls));

	#endif

	size_t serial_num = 0;
	size_t i = next_of(0);
	while (true)
	{
		serial_num++;
		if (i == ind) break;
		if (next_of(i) == 0)
		{
			serial_num = 0;
			break;
		}
	}

	return serial_num;
}