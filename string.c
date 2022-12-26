#include <string.h>
#include "string.h"
#include "util.h"
#include <stdlib.h>

cpwn_string_p cpwn_string_create(void)
{
	return cpwn_string_create_n(CPWN_STRING_DEFAULT_CAPACITY);
}

cpwn_string_p cpwn_string_create_cstr(CSTR restrict what)
{
	cpwn_string_p res;
	size_t what_len, new_capacity;

	res = CPWN_STRING_ALLOC_FUNC(sizeof(cpwn_string));
	what_len = strlen(what);
	new_capacity = max(what_len+1, (size_t) CPWN_STRING_DEFAULT_CAPACITY);
	res->capacity = new_capacity;
	res->str = CPWN_STRING_ALLOC_FUNC(new_capacity);
	res->length = what_len;
	
	strcpy(res->str, what);

	return res;
}

cpwn_string_p cpwn_string_create_cpwn_string(const cpwn_string_p restrict what)
{
	cpwn_string_p res;

	res = CPWN_STRING_ALLOC_FUNC(sizeof(cpwn_string));
	
	res->capacity = what->capacity;
	res->str = CPWN_STRING_ALLOC_FUNC(what->capacity);
	res->length = what->length;

	strcpy(res->str, what->str);

	return res;
}

cpwn_string_p cpwn_string_create_n(size_t n)
{
	cpwn_string_p res;

	res = CPWN_STRING_ALLOC_FUNC(sizeof(cpwn_string));
	
	res->capacity = n;
	res->length = 0;
	res->str = CPWN_STRING_ALLOC_FUNC(n);

	return res;
}

void cpwn_string_append_cstr(cpwn_string_p restrict this, CSTR restrict what)
{
	size_t new_len, new_capacity;
	size_t what_len = strlen(what);
	
	if (what_len + this->length < this->capacity)
	{
		this->length += what_len;
		strcat(this->str, what);
	} else
	{
		new_len = this->length + what_len;
		new_capacity = new_len + CPWN_STRING_DEFAULT_CAPACITY;

		this->str = CPWN_STRING_REALLOC_FUNC(this->str, new_capacity);
		this->length = new_len;
		this->capacity = new_capacity;

		strcat(this->str, what);
	}
}

void cpwn_string_concat_cstr(cpwn_string_p restrict this, CSTR restrict what)
{
	cpwn_string_append_cstr(this, what);
}

void cpwn_string_append_cpwn_string(cpwn_string_p restrict this, const cpwn_string_p restrict what)
{
	size_t new_len, new_capacity;

	if (this->length + what->length < this->capacity)
	{
		this->length += what->length;
		strcat(this->str, what->str);
	} else
	{
		new_len = this->length + what->length;
		new_capacity = new_len + CPWN_STRING_DEFAULT_CAPACITY;

		this->str = CPWN_STRING_REALLOC_FUNC(this->str, new_capacity);
		this->length = new_len;
		this->capacity = new_capacity;

		strcat(this->str, what->str);
	}
}

void cpwn_string_concat_cpwn_string(cpwn_string_p restrict this, const cpwn_string_p restrict what)
{
	cpwn_string_append_cpwn_string(this, what);
}

void cpwn_string_free(cpwn_string_p *const this)
{
	if (*this == NULL)
		return;
	free((*this)->str);
	free(*this);
	*this = NULL;
}
