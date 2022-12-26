#ifndef UTIL_H_
#define UTIL_H_

#include "common-defs.h"
#include "bytes.h"
#include "string.h"
#include <stdio.h>
#include <string.h>

#define max(a, b) \
	({ typeof(a) _a = (a); \
		typeof(b) _b = (b); \
		_a > _b ? _a : _b;})

#define min(a, b) \
	({ typeof(a) _a = (a); \
		typeof(b) _b = (b); \
		_a < _b ? _a : _b;})


static inline void hexdump(const void *buf, size_t n);
static inline void hexdump_buf(const void *buf, size_t n);
static inline void hexdump_cstr(CSTR const s);
static inline void hexdump_cpwn_string(const cpwn_string *ms);
static inline void hexdump_cpwn_bytes(const cpwn_bytes *mb);
static inline void fhexdump_cpwn_string(FILE *f, const cpwn_string *ms);
static inline void fhexdump_cpwn_bytes(FILE *f, const cpwn_bytes *mb);
static inline void fhexdump_cstr(FILE *F, CSTR const s);

extern void fhexdump(FILE *f, const char *buf, size_t n);

static inline void hexdump(const void *buf, size_t n)
{
	fhexdump(stderr, buf, n);
}

static inline void hexdump_buf(const void *buf, size_t n)
{
	fhexdump(stderr, buf, n);
}

static inline void hexdump_cpwn_string(const cpwn_string *ms)
{
	fhexdump_cpwn_string(stderr, ms);
}

static inline void hexdump_cpwn_bytes(const cpwn_bytes *mb)
{
	fhexdump_cpwn_bytes(stderr, mb);
}

static inline void fhexdump_cpwn_string(FILE *f, const cpwn_string *ms)
{
	fhexdump(f, ms->str, ms->length);
}

static inline void fhexdump_cpwn_bytes(FILE *f, const cpwn_bytes *mb)
{
	fhexdump(f, mb->mem, mb->length);
}

static inline void fhexdump_cstr(FILE *f, CSTR const s)
{
	hexdump_buf(s, strlen(s));
}

static inline void hexdump_cstr(CSTR const s)
{
	fhexdump_cstr(stderr, s);
}


#endif
