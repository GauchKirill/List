#include "list.h"
#include "list_dump.h"
#include "errors.h"
#include <stdio.h>

int main()
{
	list* ls = list_ctor(4);
	Elem_t val = 0;
	while(scanf("%d", &val) == 1)
	{
		printf("in %zu\n", list_insert_head(&ls, val));
	}
	while(true)
	{
		char s[100] = {};
		scanf("%s", s);
		if (s[0] == 'q')
			break;
		printf("pop: %d\n", list_pop(ls));
	}
	list_sort(ls);
	list_dump(ls, PRINT_LIST);
}