#include "list_dump.h"
#include "errors.h"

#include <stdio.h>

const char* Elem_out = "%d";

#define finish_dump                                             \
	fprintf (graph_file, "}\n");                                \
	fclose  (graph_file);                                       \
	sprintf (cmd, "dot -Tpng %s -o graph.png", name_graph_file);\
	system  (cmd);                                              \
	printf  ("Runtime error. Open \"graph.png\"\n");            \
	abort();                                                    \
	return;

void list_dump (const list* ls, unsigned err)
{
	const size_t LENGHT_CMD = 50;
	char cmd[LENGHT_CMD] = {};

	const char* name_graph_file = "graph.dot";
	FILE* graph_file = fopen(name_graph_file, "w");

	err |= list_verify (ls);

	if (err == LIST_OK)
		return;

	fprintf (graph_file,
			"digraph G{\n"
			"	rankdir=LR;\n"
			"	node[shape=\"rectangle\",fontsize=14];\n"
			"	edge[arrowhead=\"open\"];\n");

	if (err & HAS_NOT_MEMORY)
	{
		fprintf (graph_file,
				"    HAS_NOT_MEMORY[];\n");
		finish_dump
	}

	if (err & LIST_NULL_PTR)
	{
		fprintf (graph_file,
				"	list[shape=record,label= \"NULL_PTR | %p\"];\n", ls);
		finish_dump	
	}

	if (err & LIST_NULL_PTR_DATA)
	{
		fprintf (graph_file,
				" list[shape=record, label=\"{ ptr = %p | data = %p }\"];\n", ls, ls->data);
		finish_dump
	}

	if (err & INVALID_SIZE)
	{
		fprintf (graph_file,
				" list[shape=record, label=\"{ ptr = %p | data = %p | size = %zu }\"];\n", ls, ls->data, ls->size);

		finish_dump	
	}

	if (err & WRONG_DATA)
	{
		for(size_t i_elem = 0; i_elem <= SIZE; i_elem++)
		{
			fprintf (graph_file,
				" list[shape=record, label=\"{ ptr = %p | data = %p | size = %zu }\"];\n", ls, ls->data, ls->size);

			fprintf(graph_file,
					"    V%zu[shape=record, label=\"{ num = %zu | ptr = %p }\"];\n", i_elem, i_elem, ls->data + i_elem);
		}
		finish_dump
	}

	if (err & LIST_IS_EMPTY)
	{
		fprintf(graph_file,
				"    INFO[label=\"List is empty\"];\n");
		finish_dump
	}
	fprintf (graph_file,
				"    list[shape=record, label=\"{ ptr = %p | data = %p | size = %zu }\"];\n", ls, ls->data, ls->size);

	for (size_t i_elem = 0; i_elem <= SIZE; ++i_elem)
	{
		fprintf (graph_file,
				"	V%zu[shape=record, label=\" num: %zu | val: ", i_elem, i_elem);
		fprintf (graph_file, Elem_out, data_of(i_elem));
		fprintf (graph_file,
				" | <next> next: %zu | <prev> prev: %zu \","
				"fillcolor=\"lightgrey\"];\n",
				next_of(i_elem), prev_of(i_elem));

		fprintf (graph_file,
				"	V%zu:<next>->V%zu[color=\"red\"];\n"
				"	V%zu:<prev>->V%zu[color=\"blue\"];\n",
				i_elem, next_of(i_elem),
				i_elem, prev_of(i_elem));
	}					

	fprintf (graph_file, "}\n");
	fclose  (graph_file);   
	sprintf (cmd, "dot -Tpng %s -o graph.png", name_graph_file);
	system  (cmd);                                       
}

#undef finish_dump

unsigned list_verify (const list* ls)
{
	unsigned err = LIST_OK;

	if (!ls)
		return LIST_NULL_PTR;

	if (!ls->data || ls->data == (elem*) POISON_DATA)
		return LIST_NULL_PTR_DATA;
	
	if (ls->size == (unsigned) POISON_SIZE)
		return INVALID_SIZE;

	size_t 	busy_elem = 1,
	        free_elem = 1,
	        ind       = 0,
			next_ind  = 0;

	for (size_t i_elem = 0; i_elem <= SIZE; ++i_elem)
		if (!(ls->data + i_elem))
			return WRONG_DATA;

	while(next_of(ind) != 0 && busy_elem <= SIZE)
	{
		next_ind = next_of(ind);
		if (ind      > SIZE || next_ind > SIZE)
			return WRONG_IND;

		if (ind != prev_of(next_ind))
			err |= WRONG_IND;

		ind = next_ind;
		busy_elem++;
	}
	ind = SIZE;
	while(next_of(ind) != SIZE && free_elem <= SIZE)
	{
		next_ind = next_of(ind);
		if (ind      > SIZE || next_ind > SIZE)
			return WRONG_IND;

		if (ind != prev_of(next_ind))
			err |= WRONG_IND;

		ind = next_ind;
		free_elem++;
	}

	if (free_elem + busy_elem != ls->size) err |= WRONG_IND;

	return err;
}
