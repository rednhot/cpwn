#define _GNU_SOURCE

#include "util.h"
#include "common-defs.h"
#include "tube.h"

#include <stdio.h>
#include <netinet/tcp.h>
#include <errno.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <assert.h>
#include <stdlib.h>
#include <err.h>
#include <string.h>
#include <unistd.h>

#define error(format,...)						\
	do {										\
		eprintf(format, ##__VA_ARGS__);			\
		exit(EXIT_FAILURE);						\
    } while (0)

cpwn_tube_p cpwn_tube_create(int fd_r, int fd_w, cpwn_tube_type type, cpwn_flags flags)
{
	cpwn_tube_p tube;

	tube = CPWN_TUBE_ALLOC_FUNC(sizeof(cpwn_tube));
	tube->fd_r = fd_r;
	tube->fd_w = fd_w;
	tube->newline_crlf = flags&CPWN_FLAG_NEWLINE_CRLF;
	tube->type = type;
	tube->read_closed = fd_r == -1;
	tube->write_closed = fd_w == -1;
	
	return tube;
}

void cpwn_tube_close(cpwn_tube_p *const this)
{
	if (*this == NULL)
		return;
	cpwn_tube_shutdown(*this, "any");
	if ((*this)->type == CPWN_TUBE_TYPE_SOCKET_TCP ||
		(*this)->type == CPWN_TUBE_TYPE_SOCKET_UDP)
		close((*this)->fd_r); 		/* fd_r and fd_w are the same fd */
   	free(*this);
	*this = NULL;
}

cpwn_io_direction cpwn_tube_get_direction(CSTR restrict direction)
{
	if (!strcasecmp(direction, "in") ||
		!strcasecmp(direction, "read") ||
		!strcasecmp(direction, "recv"))
		return CPWN_IODIR_IN;
	if (!strcasecmp(direction, "out") ||
		!strcasecmp(direction, "write") ||
		!strcasecmp(direction, "send"))
		return CPWN_IODIR_OUT;
	if (!strcasecmp(direction, "any"))
		return CPWN_IODIR_ANY;
	return CPWN_IODIR_INVALID;
}

CSTR cpwn_tube_newline(const cpwn_tube_p this)
{
	static const char* newlines[] = {"\n", "\r\n"};

	return newlines[this->newline_crlf];
}

bool cpwn_tube_connected(const cpwn_tube_p this, CSTR restrict direction)
{
	switch (cpwn_tube_get_direction(direction))
	{
	case CPWN_IODIR_IN:
		return !this->read_closed;
	case CPWN_IODIR_OUT:
		return !this->write_closed;
	case CPWN_IODIR_ANY:
		return !(this->read_closed && this->write_closed);
	case CPWN_IODIR_INVALID:
		error("Invalid direction specified in call to %s", __FUNCTION__);
		break;
	default:
		/* should never get here */
		assert(0);
	}

	return false;
}

void cpwn_tube_shutdown(cpwn_tube_p this, CSTR restrict direction)
{
	switch (cpwn_tube_get_direction(direction))
	{
	case CPWN_IODIR_IN:
		if (this->read_closed)
		{
			warnx("Tube read direction is already closed!");
			break;
		}
		
		switch (this->type)
		{
		case CPWN_TUBE_TYPE_GENERIC:
			close(this->fd_r);
			break;
		case CPWN_TUBE_TYPE_SOCKET_TCP:
		case CPWN_TUBE_TYPE_SOCKET_UDP:
			shutdown(this->fd_r, SHUT_RD);
			break;
		default:
			assert(0);
		}
		
		/* mark read direction unusable */
		this->read_closed = true;
		break;
	case CPWN_IODIR_OUT:
		if (this->write_closed)
		{
			warnx("Tube write direction is already closed!");
			break;
		}

		switch (this->type)
		{
		case CPWN_TUBE_TYPE_GENERIC:
			close(this->fd_w);
			break;
		case CPWN_TUBE_TYPE_SOCKET_TCP:
		case CPWN_TUBE_TYPE_SOCKET_UDP:
			shutdown(this->fd_w, SHUT_WR);
			break;
		default:
			assert("Wrong tube type" && 0);
		}

		/* mark write direction unusable */
		this->write_closed = true;
		break;
	case CPWN_IODIR_ANY:
		if (this->read_closed)
		{
			warnx("Tube read direction is already closed!");
			break;
		}

		if (this->write_closed)
		{
			warnx("Tube write direction is already closed!");
			break;
		}

		switch (this->type)
		{
		case CPWN_TUBE_TYPE_GENERIC:
			close(this->fd_r);
			close(this->fd_w);
			break;
		case CPWN_TUBE_TYPE_SOCKET_TCP:
		case CPWN_TUBE_TYPE_SOCKET_UDP:
			shutdown(this->fd_r, SHUT_RD);
			shutdown(this->fd_w, SHUT_WR);
			break;
		default:
			assert("Wrong tube type" && 0);
		}
		this->read_closed = this->write_closed = true;
		break;
	case CPWN_IODIR_INVALID:
		error("Invalid direction specified in call to %s", __FUNCTION__);
		break;
	default:
		assert(0);
	}
}

cpwn_tube_p cpwn_tube_connect(CSTR restrict hostname, CSTR restrict port, cpwn_flags flags)
{
	cpwn_tube_p res;
	struct addrinfo *ai, hint;
	
	int ret_fd;
	{
		memset(&hint, 0, sizeof(struct addrinfo));
		hint.ai_family = flags & CPWN_FLAG_WANT_IPV4 ? AF_INET :
			flags & CPWN_FLAG_WANT_IPV6 ? AF_INET6 : 0;
		hint.ai_socktype = flags & CPWN_FLAG_WANT_UDP ? SOCK_DGRAM : SOCK_STREAM;
		hint.ai_protocol = 0;
		hint.ai_flags = AI_V4MAPPED | AI_ADDRCONFIG; /* default hint flags */
		int gai_errno;
		if ((gai_errno = getaddrinfo(hostname, port, &hint, &ai)))
			errx(1,
				 "Can't getaddrinfo in %s on line %d: %s",
				 __FUNCTION__,
				 __LINE__,
				 gai_strerror(gai_errno));
	}

	if ((ret_fd = socket(ai->ai_family, ai->ai_socktype, 0)) == -1)
		err(errno, "Can't create a socket");

	if (ai->ai_socktype == SOCK_STREAM && (flags & CPWN_FLAG_TCP_NODELAY))
	{
		int tcp_nodelay_opt = 1;
		if (setsockopt(ret_fd, IPPROTO_TCP, TCP_NODELAY, &tcp_nodelay_opt, 4))
			err(errno, "Can't set TCP_NODELAY");
	}

	if (connect(ret_fd, ai->ai_addr, ai->ai_addrlen) == -1)
		err(errno, "Can't connect to remote server");

	freeaddrinfo(ai);

	if (flags&CPWN_FLAG_WANT_UDP)
	{
		/* send an empty datagram 2 times to a remote host, so that
		   user can call recv functions right after the tube creation,
		   and to make sure that the remote socket do really exist
		 */
		if (send(ret_fd, &res, 0, 0) == -1 ||
			send(ret_fd, &res, 0, 0) == -1)
		{
			err(errno, "send");
		}
	}

	/* wrap fd in a tube */
	res = cpwn_tube_create(ret_fd, /* fd_r */
						 ret_fd, /* fd_w */
						 flags&CPWN_FLAG_WANT_UDP ? CPWN_TUBE_TYPE_SOCKET_UDP :
						 CPWN_TUBE_TYPE_SOCKET_TCP,
						 flags);
	return res;
}


/* -------------->  RECV FUNCS <--------------  */

cpwn_bytes_p cpwn_tube_recv__numb(cpwn_tube_p this, size_t numb, cpwn_flags flags)
{
	cpwn_bytes_p res;
	ssize_t br;
	
	res = cpwn_bytes_create_n(numb);

	br = read(this->fd_r, res->mem, numb);
	if (br == -1)
	{
		error("Can't recv in %s", __FUNCTION__);
	}
	
	res->length = br;

	if (flags & CPWN_FLAG_IGNORE_RESULT)
	{
		cpwn_bytes_free(&res);
	}
	
	return res;
}

/* Same as cpwn_tube_recv__numb */
cpwn_bytes_p cpwn_tube_read__numb(cpwn_tube_p this, size_t numb, cpwn_flags flags)
{
	return cpwn_tube_recv__numb(this, numb, flags);
}

/* Read maximum of CPWN_TUBE_RECV_DEFAULT bytes from tube.
   Returns as soon as any quantity of data is available.   */
cpwn_bytes_p cpwn_tube_recv(cpwn_tube_p this, cpwn_flags flags)
{
	return cpwn_tube_recv__numb(this, CPWN_TUBE_RECV_DEFAULT, flags);
}

/* Same as cpwn_tube_recv. */
cpwn_bytes_p cpwn_tube_read(cpwn_tube_p this, cpwn_flags flags)
{
	return cpwn_tube_recv__numb(this, CPWN_TUBE_RECV_DEFAULT, flags);
}

/* Receive exactly _numb_ bytes from tube.  */
cpwn_bytes_p cpwn_tube_recvn(cpwn_tube_p this, size_t numb, cpwn_flags flags)
{
	cpwn_bytes_p res;
	ssize_t br;
	char *p;
	
	res = cpwn_bytes_create_n(numb);
	p = res->mem;
	while (numb)
	{
		switch (br = read(this->fd_r, p, numb))
		{
		case -1:
			/* some i/o error happened */
			error("Can't recv");
			break;
		case 0:
			/* No more bytes will come.
			   Throw an error since user wanted exactly NUMB chars to be read in 
			*/
			error("No more chars available");
			break;
		default:
			res->length += br;
			numb -= br;
			p += br;
		}
	}

	if (flags & CPWN_FLAG_IGNORE_RESULT)
	{
		cpwn_bytes_free(&res);
	}
	
	return res;
}

/* recv bytes from tube until s is found 
   Current implementetion is VERY VERY slow, and don't work for UDP sockets.

   TODO: make tubes buffered to be faster and to support UDP sockets
*/
cpwn_bytes_p cpwn_tube_recvuntil(cpwn_tube_p this, CSTR s, cpwn_flags flags)
{
	char c;
	ssize_t br;
	cpwn_bytes_p res;
	const char *p;
	size_t s_len;
	
	if (this->read_closed)
		return NULL;

	if (this->type == CPWN_TUBE_TYPE_SOCKET_UDP)
	{
		errx(EXIT_FAILURE, "%s doesn't support UDP sockets", __FUNCTION__);
	}

	res = cpwn_bytes_create();
	s_len = strlen(s);
	while (true)
	{
		br = read(this->fd_r, &c, 1);

		if (br == -1)
		{
			switch (errno)
			{
			case EINTR:
				break;
			default:
				err(errno, "Can't read in %s", __FUNCTION__);
			}
		} else if (br == 0)
		{
			/* s is not found */
			cpwn_bytes_free(&res);
			break;
		} else {
			cpwn_bytes_append_char(res, c);
			/* one byte is read in */
			if ((p = memmem(res->mem, res->length, s, s_len)))
			{
				if (flags & CPWN_FLAG_DROP)
				{
					res->length -= strlen(s);
					((char*)res->mem)[res->length] = '\0';
				}
				break;
			}
		}
	}
	
	if (flags & CPWN_FLAG_IGNORE_RESULT)
		cpwn_bytes_free(&res);
	return res;
}

/* Receive _num_ lines from tube. */
cpwn_bytes_p cpwn_tube_recvlines(cpwn_tube_p this, int num, cpwn_flags flags)
{
	cpwn_bytes_p res;
	char c;
	ssize_t br;
	const char *nl;
	size_t nl_len;
	
	if (this->read_closed || num == 0)
		return NULL;

	if (this->type == CPWN_TUBE_TYPE_SOCKET_UDP)
		errx(EXIT_FAILURE, "%s doesn't support UDP sockets", __FUNCTION__);
	
	res = cpwn_bytes_create();
	nl = cpwn_tube_newline(this);
	nl_len = strlen(nl);
	
	while (num)
	{
		br = read(this->fd_r, &c, 1);

		if (br == -1)
		{
			switch (errno)
			{
			case EINTR:
				break;
			default:
				err(errno, "Can't read in %s", __FUNCTION__);
			}
		} else if (br == 0)
		{
			errx(EXIT_FAILURE,
				 "EOF occurreed before request is satisfied in %s",
				 __FUNCTION__);
		} else
		{
			cpwn_bytes_append_char(res, c);
			if (res->length >= nl_len &&
				!memcmp(res->mem + res->length - nl_len, nl, nl_len))
			{
				/* newline is found */
				--num;
 			}
		}
	}

	if (flags & CPWN_FLAG_NOKEEPLEND)
	{
		warnx("CPWN_FLAG_NOKEEPLEND is not implemented for %s", __FUNCTION__);
	}

	if (flags & CPWN_FLAG_IGNORE_RESULT)
	{
		cpwn_bytes_free(&res);
	}
	
	return res;
}

/* -------------->  SEND FUNCS <--------------  */

void cpwn_tube_send_buf(cpwn_tube_p this, const void *buf, size_t n)
{
	ssize_t s;
	const void *p = buf;

	if (this->write_closed)
	{
		errx(EXIT_FAILURE, "Write end is closed for a tube");
		return;
	}
	
	/* try to send all of the user-provided buffer */
	while (n)
	{
		s = write(this->fd_w, p, n);
		if (s == -1)
		{
			/* some error happened */
			switch (errno)
			{
			case EINTR:
				/* Interrupted by some signal. Nothing bad in it, so continue */
				break;
			default:
				err(errno, "Can't write in %s", __FUNCTION__);
			}
		} else
		{
			p += s;
			n -= s;
		}
	}
}
