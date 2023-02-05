#include "list.h"
#include "list_dump.h"
#include "errors.h"

#include <stdio.h>
#include <stdlib.h>

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
	ls->data = (elem*)  POISON_DATA;
	ls->size = (size_t) POISON_SIZE;
}

size_t list_insert_head(list** ls_ptr, Elem_t val, size_t* returned_num)
{
	return list_insert_after_ind(ls_ptr, 0, val, returned_num);
}


size_t list_insert_tail(list** ls_ptr, Elem_t val, size_t* returned_num)
{
	return list_insert_before_ind(ls_ptr, 0, val, returned_num);
}

size_t list_insert_after_ind(list** ls_ptr, size_t ind, Elem_t val, size_t* returned_num)
{
	size_t err = LIST_OK;
	list* ls = *ls_ptr;

	CHECK_ERR(err = list_dump(ls, list_verify(ls)))
	
	size_t returned_ind = 0;
	if ((err = at_list(ls, ind, &returned_ind)))
		return err;

	if(returned_ind != ind)
	{
		*returned_num = 0;
		return LIST_OK;
	}

	if (next_of(SIZE) == SIZE)
	{
		if ((err = list_up_size(ls, &ls)))
			return err;
	}

	size_t num_free      = next_of(SIZE),
	       new_num_free  = next_of(num_free),
	       next_for_elem = next_of(ind);

	next_of(SIZE)         = new_num_free;
	prev_of(new_num_free) = SIZE;

	data_of(num_free) = val;
	prev_of(num_free) = ind;
	next_of(num_free) = next_for_elem;

	next_of(ind) = num_free;
	prev_of(next_for_elem) = num_free;

	*ls_ptr = ls;
	*returned_num = num_free;

	return err;
}

size_t list_insert_before_ind(list** ls_ptr, size_t ind, Elem_t val, size_t* returned_num)
{
	size_t err = LIST_OK;
	list* ls = *ls_ptr;

	CHECK_ERR(err = list_dump(ls, list_verify(ls)))

	size_t returned_ind = 0;
	if ((err = at_list(ls, ind, &returned_ind)))
		return err;

	if(returned_ind != ind)
	{
		*returned_num = 0;
		return LIST_OK;
	}

	if (next_of(SIZE) == SIZE)
	{
		if ((err = list_up_size(ls, &ls)))
			return err;
	}

	size_t num_free      = next_of(SIZE),
	       new_num_free  = next_of(num_free),
	       prev_for_elem = prev_of(ind);

	next_of(SIZE) = new_num_free;
	prev_of(new_num_free) = SIZE;
	
	data_of(num_free) = val;
	next_of(num_free) = ind;
	prev_of(num_free) = prev_for_elem;
	
	prev_of(ind) = num_free;
	next_of(prev_for_elem) = num_free;

	*ls_ptr = ls;
	*returned_num = num_free;

	return err;
}

size_t list_pop (list* ls, Elem_t* returned_val)
{
	size_t err = LIST_OK;

	err = list_verify (ls);
	if (next_of(0) == 0) err |= LIST_IS_EMPTY;
	CHECK_ERR(err = list_dump(ls, err))

	size_t num_out = prev_of(0);
	Elem_t val = data_of(num_out);

	size_t prev = prev_of(num_out),
	       next = next_of(num_out);

	next_of(prev) = next;
	prev_of(next) = prev;

	prev_of(num_out) = SIZE;
	next_of(num_out) = next_of(SIZE);

	data_of(num_out) = POISON;
	next_of(SIZE) = num_out;
	prev_of(SIZE) = num_out;

	#ifdef DUMP

		if ((err = list_dump(ls, list_verify(ls)))) return err;
	
	#endif
		
	*returned_val = val;
	return err;
}

size_t list_sort (list* ls)
{
	size_t err = LIST_OK;

	CHECK_ERR(err = list_dump(ls, list_verify(ls)))
	
	size_t ind = 0,
	       i = 0;

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
	} while (ind != next_of(SIZE));

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

	return err;
}

size_t list_linear_sort (list** ls_ptr)
{
	size_t err = LIST_OK;
	list* ls = *ls_ptr;

	CHECK_ERR(err = list_dump(ls, list_verify(ls)))
	
	list* new_ls = nullptr;

	new_ls = (list*) calloc(1, sizeof(list));
	if (!new_ls)
		return HAS_NOT_MEMORY;

	new_ls->size = ls->size;
	new_ls->data = (elem*) calloc(new_ls->size, sizeof(elem));
	if (!new_ls->data)
		return HAS_NOT_MEMORY;

	size_t ind_new_ls = 0,
           ind = 0;

	for (; next_of(ind) != 0; ind = next_of(ind), ind_new_ls++)
	{
		data_of_new(ind_new_ls) = data_of(ind);
		next_of_new(ind_new_ls) = (ind_new_ls) + 1;
		prev_of_new(ind_new_ls) = (ind_new_ls) - 1;
	}
	data_of_new(ind_new_ls) = data_of(ind);
	next_of_new(ind_new_ls) = 0;
	prev_of_new(ind_new_ls) = ind_new_ls - 1;
	prev_of_new(0) = ind_new_ls++;

	size_t first_free_place = ind_new_ls;

	for (; ind_new_ls <= NEW_SIZE; ++ind_new_ls)
	{
		data_of_new(ind_new_ls) = POISON;
		next_of_new(ind_new_ls) = ind_new_ls + 1;
		prev_of_new(ind_new_ls) = ind_new_ls - 1;
	}

	prev_of_new(first_free_place) = NEW_SIZE;
	next_of_new(NEW_SIZE) = first_free_place;
	
	free(ls->data);
	free(ls);

	*ls_ptr = new_ls;
	return LIST_OK;	
}

int list_compare (const void* a, const void* b)
{
	const elem* a_ptr = (const elem*) a;
	const elem* b_ptr = (const elem*) b;

	return (a_ptr->prev < b_ptr->prev) ? -1 : 1;
}

size_t list_up_size(list* ls, list** returned_ls)
{
	size_t err = LIST_OK;

	CHECK_ERR(err = list_dump(ls, list_verify(ls)))
	
	size_t size = ls->size,
	       new_size = 2*size;
	list* new_ls = (list*) calloc(1, sizeof(list));
	if (!new_ls)
		return list_dump(ls, HAS_NOT_MEMORY);

	new_ls->size = new_size;
	new_ls->data = (elem*) realloc(ls->data, new_size*sizeof(elem));
	if (!new_ls->data)
		return list_dump(ls, HAS_NOT_MEMORY);

	free(ls);

	prev_of_new(next_of_new(size - 1)) = NEW_SIZE;
	
	for (size_t i_elem = size - 1; i_elem <= NEW_SIZE; ++i_elem)
	{
		data_of_new(i_elem) = POISON;
		next_of_new(i_elem) = (i_elem == NEW_SIZE) ? size - 1              : i_elem + 1;
		prev_of_new(i_elem) = (i_elem == size - 1) ? prev_of_new(size - 1) : i_elem - 1;
	}

	*returned_ls = new_ls;
	return err;
}

size_t find_ind_by_num (const list* ls, Elem_t val, size_t* returned_ind)
{
	size_t err = LIST_OK;

	CHECK_ERR(err = list_dump(ls, list_verify(ls)))
	
	size_t ind = 0;
	do
	{
		ind = next_of(ind);
		if (data_of(ind) == val) break;
	} while (ind != 0);

	*returned_ind = ind;
	return err;
}

size_t at_list (const list* ls, size_t ind, size_t* returned_ip)
{
	size_t err = LIST_OK;

	CHECK_ERR(err = list_dump(ls, list_verify(ls)))
	
	size_t ip = 0;
	do
	{
		if (ip == ind) break;
		ip = next_of(ip);
	} while (ip != 0);

	*returned_ip = ip;
	return err;
}

size_t find_logical_ind(const list* ls, size_t ind, size_t* returned_serial_num)
{
	size_t err = LIST_OK;

	CHECK_ERR(err = list_dump(ls, list_verify(ls)))
	
	size_t serial_num = 0;
	for (size_t i = 0; true; serial_num++)
	{
		if (i == ind) break;
		if (next_of(i) == 0)
		{
			serial_num = 0;
			break;
		}
	}

	*returned_serial_num = serial_num;
	return err;
}