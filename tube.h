#ifndef TUBE_H_
#define TUBE_H_

#include <string.h>
#include "common-defs.h"
#include "bytes.h"
#include "string.h"

#include <stdbool.h>
#include <stdint.h>
#include <sys/types.h>

#define CPWN_TUBE_RECV_DEFAULT (4096)
#define CPWN_TUBE_ALLOC_FUNC malloc
#define CPWN_TUBE_REALLOC_FUNC realloc

/* CPWN_TUBE_TYPE_GENERIC      -  tube which has some seperate fd for read operations
                                and an another one fd for write operations.
   CPWN_TUBE_TYPE_SOCKET_TCP   -  tube having ONE underlying fd referring to a tcp socket.
   CPWN_TUBE_TYPE_SOCKET_UDP   -  tube having ONE underlying fd referring to a udp socket.

 */
typedef enum
{
	CPWN_TUBE_TYPE_GENERIC,
	CPWN_TUBE_TYPE_SOCKET_TCP,
	CPWN_TUBE_TYPE_SOCKET_UDP,
	CPWN_TUBE_TYPE__COUNT
} cpwn_tube_type;

typedef struct
{
	int fd_r, fd_w;
	bool read_closed, write_closed;
	bool newline_crlf;
	cpwn_tube_type type;
} cpwn_tube;

typedef cpwn_tube* cpwn_tube_p;

typedef enum
{
	CPWN_IODIR_IN,
	CPWN_IODIR_OUT,
	CPWN_IODIR_ANY,
	CPWN_IODIR_INVALID
} cpwn_io_direction;

/* -------------->  MISC FUNCS <--------------  */

/* Create tube with FD_R for read operations and FD_W for write operations.
   TYPE specifies type of a tube to be created: see enum cpwn_tube_type for available
   types. FLAGS specify auxiliary tube creations flags.

   Available flags:
       CPWN_FLAG_NEWLINE_CRLF    -   treat CRLF as a newline by default
	                               in funcs like cpwn_tube_readline
 */
extern cpwn_tube_p cpwn_tube_create(int fd_r, int fd_w, cpwn_tube_type type, cpwn_flags flags) __wur;

/* Closes underlying file descriptors, rendering a tube unusable anymore.
*/
extern void cpwn_tube_close(cpwn_tube_p *const this);

/* Checks if tube is connected in the given DIRECTION. Look at cpwn_tube_get_direction
   to see available directions.
 */
extern bool cpwn_tube_connected(const cpwn_tube_p this, CSTR restrict direction) __wur;

/* Shutdown communications in the given DIRECTION. Look at cpwn_tube_get_direction
   to see available directions.
 */
extern void cpwn_tube_shutdown(cpwn_tube_p this, CSTR restrict direction);

/* Creates a tube connected to a remote system given its HOSTNAME and PORT(can
   be specified just as a service name).

   FLAGS specify connection details like preferred transport layer protocol.
   Default is CPWN_FLAG_WANT_TCP.

   Available flags:
       CPWN_FLAG_WANT_TCP     -        user wants tcp transport layer protocol
	   CPWN_FLAG_WANT_UDP     -        user wants udp transport layer protocol
	   CPWN_FLAG_WANT_IPv4    -        user wants IPv4 network layer protocol
	   CPWN_FLAG_WANT_IPv6    -        user wants IPv6 network layer protocol
   Anyway, flags is then passed to cpwn_tube_create, so flags applicable for it
   also applicable for this function.
*/
extern cpwn_tube_p cpwn_tube_connect(CSTR restrict hostname, CSTR restrict port, cpwn_flags flags) __wur;

/* Maps string representation DIRECTION to a numeric enum cpwn_io_direction

   Available directions:
       read,recv,in        -        maps to CPWN_IODIR_IN
	   write,send,out      -        maps to CPWN_IODIR_OUT
	   any                 -        maps to CPWN_IODIR_ANY
*/
extern cpwn_io_direction cpwn_tube_get_direction(CSTR restrict direction) __wur;

/* Returns string which is current newline for a tube */
extern CSTR cpwn_tube_newline(const cpwn_tube_p this) __wur;

/* -------------->  RECV FUNCS <--------------  */

/* NOTE: all RECV* and READ* functions accept CPWN_FLAG_IGNORE_RESULT flag
   presence of which says that user don't want to get bytes as a result of funcall.
*/

/* Internal function which is used by cpwn_tube_recv. 
   Receives up to NUMB bytes of data, and returns as soon as any quantity of 
   data is available.
*/
extern cpwn_bytes_p cpwn_tube_recv__numb(cpwn_tube_p this, size_t numb, cpwn_flags flags);
extern cpwn_bytes_p cpwn_tube_read__numb(cpwn_tube_p this, size_t numb, cpwn_flags flags);

/* Read maximum of CPWN_TUBE_RECV_DEFAULT bytes from tube.
   Returns as soon as any quantity of data is available.   */
extern cpwn_bytes_p cpwn_tube_recv(cpwn_tube_p this, cpwn_flags flags);
extern cpwn_bytes_p cpwn_tube_read(cpwn_tube_p this, cpwn_flags flags);

/* Receive exactly _numb_ bytes from tube.  */
extern cpwn_bytes_p cpwn_tube_recvn(cpwn_tube_p this, size_t numb, cpwn_flags flags);
/* Same as cpwn_tube_recvn */
static __always_inline cpwn_bytes_p cpwn_tube_readn(cpwn_tube_p this, size_t numb, cpwn_flags flags)
{
	return cpwn_tube_recvn(this, numb, flags);
}

/* recv bytes from fd until s is found */
extern cpwn_bytes_p cpwn_tube_recvuntil(cpwn_tube_p this, CSTR s, cpwn_flags flags);
/* Same as cpwn_tube_recvuntil */
static __always_inline cpwn_bytes_p cpwn_tube_readuntil(cpwn_tube_p this, CSTR s, cpwn_flags flags)
{
	return cpwn_tube_recvuntil(this, s, flags);
}

/* Receive exactly one line from tube */
extern cpwn_bytes_p cpwn_tube_recvline(cpwn_tube_p this, cpwn_flags flags);
/* Same as cpwn_tube_recvline. */
static __always_inline cpwn_bytes_p cpwn_tube_readline(cpwn_tube_p this, cpwn_flags flags)
{
	return cpwn_tube_recvline(this, flags);
}

/* Receive NUM lines from tube. */
extern cpwn_bytes_p cpwn_tube_recvlines(cpwn_tube_p this, int num, cpwn_flags flags);
static __always_inline cpwn_bytes_p cpwn_tube_readlines(cpwn_tube_p this, int num, cpwn_flags flags)
{
	return cpwn_tube_recvlines(this, num, flags);
}

/* -------------->  SEND FUNCS <--------------  */

/* Send buf to the tube */
extern void cpwn_tube_send_buf(cpwn_tube_p this, const void *buf, size_t n);
/* Same as cpwn_tube_send_buf */
static __always_inline void cpwn_tube_write_buf(cpwn_tube_p this, const void *buf, size_t n)
{
	return cpwn_tube_send_buf(this, buf, n);
}
/* Send data to the tube */
static __always_inline void cpwn_tube_send(cpwn_tube_p this, const cpwn_bytes_p bytes)
{
	return cpwn_tube_send_buf(this, bytes->mem, bytes->length);
}
/* Same as cpwn_tube_send */
static __always_inline void cpwn_tube_write(cpwn_tube_p this, const cpwn_bytes_p bytes)
{
	return cpwn_tube_send(this, bytes);
}
/* Send data + newline(maybe CRLF) to the tube */
static __always_inline void cpwn_tube_sendline(cpwn_tube_p this, const cpwn_bytes_p bytes)
{
	const char *newline = cpwn_tube_newline(this);
	cpwn_tube_send(this, bytes);
	cpwn_tube_send_buf(this, newline, strlen(newline));
}
/* Same as cpwn_tube_sendline */
static __always_inline void cpwn_tube_writeline(cpwn_tube_p this, const cpwn_bytes_p bytes)
{
	return cpwn_tube_sendline(this, bytes);
}
/* Send cstr to the tube */
static __always_inline void cpwn_tube_send_cstr(cpwn_tube_p this, CSTR restrict line)
{
	return cpwn_tube_send_buf(this, line, strlen(line));
}
/* Same as cpwn_tube_send_cstr */
static __always_inline void cpwn_tube_write_cstr(cpwn_tube_p this, CSTR restrict line)
{
	return cpwn_tube_send_cstr(this, line);
}

#endif
