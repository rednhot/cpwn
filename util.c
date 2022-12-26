#include <string.h>
#include <ctype.h>
#include <stdint.h>
#include "util.h"

void fhexdump(FILE *f, const char *buf, size_t n)
{
	size_t o;
	int i;
	
	for (o = 0; o < n; ++o)
	{
		// possibly print current offset
		if (o % 16 == 0)
			fprintf(f, "%08x:", (int) o);

		// print hex of current char
		if (o % 2 == 0)
			fprintf(f, " %02hhx", buf[o]);
		else
			fprintf(f, "%02hhx", buf[o]);

		// possibly print printable ascii of current line
		if (o % 16 == 15 || o == n-1)
		{
			fprintf(f, "%*c", (int) (2 + ((15 - o%16)*2 + (15-o%16)/2)), ' ');
			int d;
			for (i = o%16, d=0; ~i; --i, ++d)
			{
				if (d % 4 == 0)
					fprintf(f, "|");
				if (isprint(buf[o-i]))
					fprintf(f, "%c", buf[o-i]);
				else
					fprintf(f, ".");
			}
			fprintf(f, "\n");
		}
	}
}

