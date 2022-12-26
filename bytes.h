#ifndef BYTES_H_
#define BYTES_H_

#include "common-defs.h"
#include "string.h"
#include <stddef.h>

#define CPWN_BYTES_DEFAULT_CAPACITY (1024)
#define CPWN_BYTES_ALLOC_FUNC malloc
#define CPWN_BYTES_REALLOC_FUNC realloc

typedef struct
{
	void *mem;
	size_t length;
	size_t capacity;
} cpwn_bytes;

typedef cpwn_bytes* cpwn_bytes_p;

cpwn_bytes_p cpwn_bytes_create(void) __wur;
cpwn_bytes_p cpwn_bytes_create_n(size_t n) __wur;
cpwn_bytes_p cpwn_bytes_create_buf(const void *what, size_t n) __wur;
cpwn_bytes_p cpwn_bytes_create_cstr(const char *what) __wur;
cpwn_bytes_p cpwn_bytes_create_cpwn_bytes(const cpwn_bytes_p what) __wur;

void cpwn_bytes_free(cpwn_bytes_p *this);

void cpwn_bytes_append_buf(cpwn_bytes_p this, const void *what, size_t n);
void cpwn_bytes_concat_buf(cpwn_bytes_p this, const void *what, size_t n);
void cpwn_bytes_append_cstr(cpwn_bytes_p this, const char *what);
void cpwn_bytes_concat_cstr(cpwn_bytes_p this, const char *what);
void cpwn_bytes_append_cpwn_bytes(cpwn_bytes_p this, const cpwn_bytes_p what);
void cpwn_bytes_concat_cpwn_bytes(cpwn_bytes_p this, const cpwn_bytes_p what);
void cpwn_bytes_append_cpwn_string(cpwn_bytes_p this, const cpwn_string_p what);
void cpwn_bytes_concat_cpwn_string(cpwn_bytes_p this, const cpwn_string_p what);

void cpwn_bytes_append_char(cpwn_bytes_p this, char c);
void cpwn_bytes_concat_char(cpwn_bytes_p this, char c);



#endif
