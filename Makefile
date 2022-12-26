CFLAGS = -std=gnu99 -Og -ggdb -Wall -Wextra -Werror -Wno-unused-parameter -fPIC
CC = cc

SRC = bytes.c string.c util.c tube.c
OBJ = ${SRC:.c=.o}
RESULT = libcpwn.so

all: options ${RESULT}


.c.o:
	${CC} -c ${CFLAGS} $<

${RESULT}: ${OBJ}				
	${CC} ${CFLAGS} -shared -o $@ ${OBJ}

options:
	@echo myc build options:
	@echo "CFLAGS   = ${CFLAGS}"
	@echo "CC       = ${CC}"

clean:
	rm -f ${OBJ} ${RESULT}

.PHONY: options all install clean
