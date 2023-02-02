#ifndef ERRORS_H
#define ERRORS_H

enum errors
{
	LIST_OK             = 0,
	HAS_NOT_MEMORY      = 1<<1,
	LIST_NULL_PTR       = 1<<2,
	LIST_NULL_PTR_DATA  = 1<<3,
	WRONG_DATA          = 1<<4,
	WRONG_IND           = 1<<5,
	INVALID_SIZE        = 1<<7,
	PRINT_LIST          = 1<<8,
	LIST_IS_EMPTY       = 1<<9,
};

#endif