#ifndef DUMP_H
#define DUMP_H

#include "list.h"

void     list_dump	(const list* ls, unsigned err);
unsigned list_verify(const list* ls);

#endif