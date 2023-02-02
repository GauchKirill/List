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
		size_t num = 0;
		if (list_insert_tail(&ls, val, &num))
			abort();
		printf("in %zu\n", num);
	}
	while(true)
	{
		char s[100] = {};
		scanf("%s", s);
		if (s[0] == 'q')
			break;
		if (list_pop(ls, &val))
			abort();
		printf("pop: %d\n", val);
	}
	if (list_sort(ls))
		abort();
	if (list_dump(ls, PRINT_LIST))
		abort()	;
}