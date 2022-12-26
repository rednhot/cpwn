#ifndef COMMON_DEFS_H_
#define COMMON_DEFS_H_

#include <stdint.h>

#ifndef __wur
# define __wur __attribute__((warn_unused_result))
#endif

#ifndef __inline
# define __inline inline
#endif

#ifndef __always_inline
# define __always_inline __inline  __attribute__((always_inline))
#endif

#ifndef eprintf
# define eprintf(format,...) fprintf(stderr, format, ##__VA_ARGS__)
#endif

#define TRUE 1
#define FALSE 0

#define CSTR const char *


#include "context.h"

#define	CPWN_FLAG_IGNORE_RESULT      (1u << 0)
#define	CPWN_FLAG_WANT_TCP           (1u << 1)
#define	CPWN_FLAG_WANT_UDP           (1u << 2)
#define CPWN_FLAG_NOKEEPLEND          (1u << 3)
#define CPWN_FLAG_NEWLINE_CRLF       (1u << 4)
#define CPWN_FLAG_WANT_IPV4          (1u << 5)
#define CPWN_FLAG_WANT_IPV6          (1u << 6)
#define CPWN_FLAG_TCP_NODELAY        (1u << 7)
#define CPWN_FLAG_DROP               (1u << 8)

typedef uint32_t cpwn_flags;

extern cpwn_context context;

#endif
