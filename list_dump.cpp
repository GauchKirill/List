#include "list_dump.h"

unsigned ListDump (list* ls, unsigned err)
{
	FILE* graph_file = fopen(name_graph_file, "w");
	
	fprintf (graph_file,
			"digraph G{\n"
			"	rankdir=LR;\n"
			"	node[fontsize=14];\n"
			"	edge[arrowhead=\"open\"];\n");
	err |= List_Verify (ls);
	if (err & HAS_NOT_MEMORY)
		fprintf (graph_file,
				" HAS_NOT_MEMORY[];\n");
	else
	{
		if (err & LIST_NULL_PTR)
			fprintf (graph_file,
					"	INFO[shape=\"rectangle\",style=\"rounded,filled\",shape=record,]"
					"label= \"NULL_PTR | %p\"];", ls);	
		else
		{
			#ifdef DUMP

				fprintf (graph_file,
						"	INFO[shape=\"rectangle\",style=\"rounded,filled\",shape=record,"
						"label= \"name: %s| state: %s | ptr: %p |file: %s | function: %s | line: %u | data_ptr: %p | size: %lu | "
						"insert from free: %s | extract from free: %s | list's empty: %s\","
						"fillcolor=\"white\"];\n",
						ls->info.name, (err == LIST_OK) ? "Ok" : "ERROR", ls, ls->info.file, ls->info.func, ls->info.line, ls->data,
						ls->size, (err & INDEX_IS_FREE) ? "yes" : "no", (err & EXTRACT_FROM_FREE) ? "yes" : "no",
						(err & LIST_IS_EMPTY) ? "yes" : "no"); //func

			#endif

			if (!(err & LIST_NULL_PTR_DATA))
			{
				for (int i = 0; i < ls->size; i++)
				{
					fprintf (graph_file,
							"	V%d[shape=\"rectangle\",style=\"rounded,filled\",shape=record,"
							"label=\"%d | data: ", i, i);
					fprintf (graph_file, Elem_out, ls->data[i].elem);
					fprintf (graph_file,
							" | <next> next: %u | <prev> prev: %u\","
							"fillcolor=\"lightgrey\"];\n",
							ls->data[i].next, ls->data[i].prev);
				}
				for (int i = 0; i < ls->size; i++)
					fprintf (graph_file,
							"	V%d:<next>->V%u[color=\"red\"];\n"
							"	V%d:<prev>->V%u[color=\"blue\"];\n",
							i, ls->data[i].next,
							i, ls->data[i].prev);
			}
		}
	}
	fprintf (graph_file, "}\n");

	fclose(graph_file);	
	system ("dot -Tpng graph.dot -o graph.png");
	return err;
}