#ifndef STRING_H_
#define STRING_H_

#include "common-defs.h"
#include <stddef.h>

#define CPWN_STRING_DEFAULT_CAPACITY (1024)
#define CPWN_STRING_ALLOC_FUNC malloc
#define CPWN_STRING_REALLOC_FUNC realloc

typedef struct
{
	char *str;
	size_t length;
	size_t capacity;
} cpwn_string;

typedef cpwn_string* cpwn_string_p;

cpwn_string_p cpwn_string_create(void) __wur;
cpwn_string_p cpwn_string_create_cstr(CSTR restrict what) __wur;
cpwn_string_p cpwn_string_create_cpwn_string(const cpwn_string_p restrict what) __wur;
cpwn_string_p cpwn_string_create_n(size_t n) __wur;

void cpwn_string_free(cpwn_string_p *const this);

void cpwn_string_append_cstr(cpwn_string_p restrict this, CSTR restrict what);
void cpwn_string_concat_cstr(cpwn_string_p restrict this, CSTR restrict what);
void cpwn_string_append_cpwn_string(cpwn_string_p restrict this, const cpwn_string_p restrict what);
void cpwn_string_concat_cpwn_string(cpwn_string_p restrict this, const cpwn_string_p restrict what);

#endif
