#include "util.h"
#include "bytes.h"
#include <string.h>
#include <stdlib.h>

cpwn_bytes_p cpwn_bytes_create(void)
{
	return cpwn_bytes_create_n(CPWN_BYTES_DEFAULT_CAPACITY);
}

cpwn_bytes_p cpwn_bytes_create_n(size_t n)
{
	cpwn_bytes_p res;

	res = CPWN_BYTES_ALLOC_FUNC(sizeof(cpwn_bytes));
	
	
	res->mem = CPWN_BYTES_ALLOC_FUNC(n);
	res->capacity = n;
	res->length = 0;

	return res;
}

cpwn_bytes_p cpwn_bytes_create_buf(const void *what, size_t n)
{
	cpwn_bytes_p res;
	size_t new_capacity;

	res = CPWN_STRING_ALLOC_FUNC(sizeof(cpwn_bytes));
	
	
	new_capacity = max((size_t) CPWN_BYTES_DEFAULT_CAPACITY, n);
	res->mem = CPWN_BYTES_ALLOC_FUNC(new_capacity);
	res->length = n;
	res->capacity = new_capacity;
	
	memcpy(res->mem, what, n);
	
	return res;
}

cpwn_bytes_p cpwn_bytes_create_cstr(const char *what)
{
	return cpwn_bytes_create_buf(what, strlen(what));
}

cpwn_bytes_p cpwn_bytes_create_cpwn_bytes(const cpwn_bytes_p what)
{
	return cpwn_bytes_create_buf(what->mem, what->length);
}

void cpwn_bytes_free(cpwn_bytes_p *this)
{
	if (*this == NULL)
		return;
	free((*this)->mem);
	free(*this);
	*this = NULL;
}

void cpwn_bytes_append_buf(cpwn_bytes_p this, const void *what, size_t n)
{
	size_t new_len, new_capacity, old_len;

	if (this->length + n <= this->capacity)
	{
		memcpy(this->mem + this->length, what, n);
		this->length += n;
	} else
	{
		new_len = this->length + n;
		new_capacity = new_len + CPWN_BYTES_DEFAULT_CAPACITY;
		old_len = this->length;
		
		this->mem = CPWN_BYTES_REALLOC_FUNC(this->mem, new_capacity);
		this->length = new_len;
		this->capacity = new_capacity;

		memcpy(this->mem+old_len, what, n);
	}
}

void cpwn_bytes_concat_buf(cpwn_bytes_p this, const void *what, size_t n)
{
	cpwn_bytes_append_buf(this, what, n);
}

void cpwn_bytes_append_cstr(cpwn_bytes_p this, const char *what)
{
	cpwn_bytes_append_buf(this, what, strlen(what));
}

void cpwn_bytes_concat_cstr(cpwn_bytes_p this, const char *what)
{
	cpwn_bytes_append_cstr(this, what);
}

void cpwn_bytes_append_cpwn_bytes(cpwn_bytes_p this, const cpwn_bytes_p what)
{
	cpwn_bytes_append_buf(this, what->mem, what->length);
}

void cpwn_bytes_concat_cpwn_bytes(cpwn_bytes_p this, const cpwn_bytes_p what)
{
	cpwn_bytes_append_cpwn_bytes(this, what);
}

void cpwn_bytes_append_cpwn_string(cpwn_bytes_p this, const cpwn_string_p what)
{
	cpwn_bytes_append_buf(this, what->str, what->length);
}

void cpwn_bytes_concat_cpwn_string(cpwn_bytes_p this, const cpwn_string_p what)
{
	cpwn_bytes_append_cpwn_string(this, what);
}

void cpwn_bytes_append_char(cpwn_bytes_p this, char c)
{
	cpwn_bytes_append_buf(this, &c, 1);
}

void cpwn_bytes_concat_char(cpwn_bytes_p this, char c)
{
	cpwn_bytes_append_char(this, c);
}

