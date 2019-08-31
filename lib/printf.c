#include <stdarg.h>
#include "syscall.h"

#define PRINT_BUFSZ 256

//print buffer, accumulate a certain number of chars, then do a syscall togother.
struct printbuf {
	int idx;
	char buf[PRINT_BUFSZ];
};

static void putch(int ch, struct printbuf *b)
{
	b->buf[b->idx++] = ch;
	if(b->idx == PRINT_BUFSZ)
	{
		sys_puts(b->buf, b->idx);
		b->idx = 0;
	}
}

//change an unsigned number to chars
static void printnum(void (*putch)(int, struct printbuf*), 
	struct printbuf* putdat, unsigned num, unsigned base)
{
	if(num >=base)
		printnum(putch, putdat, num / base, base);

	putch("0123456789abcdef"[num % base], putdat);
}

void vprintf(const char *fmt, va_list ap)
{
	char *p;
	int ch;
	unsigned num, base;

	struct printbuf b;
	b.idx = 0;

	while(1)
	{
		while((ch = *(unsigned char*)fmt++) != '%')
		{
			if(ch == '\0')	goto end;
			putch(ch, &b);
		}
		
		switch(ch = *(unsigned char*)fmt++)
		{
			case 'c':
				putch(va_arg(ap, int), &b);
				break;

			case 's':
				if((p = va_arg(ap, char*)) == 0)
					p = "(nullptr)";
				while((ch = *p++) != '\0')	putch(ch, &b);
				break;

			case 'd':
				num = va_arg(ap, int);
				if((int)num < 0)
				{
					putch('-', &b);
					num = -(int) num;
				}
				base = 10;
				goto number;

			case 'u':
				num = va_arg(ap, unsigned);
				base = 10;
				goto number;

			case 'x':
				num = va_arg(ap, unsigned);
				base = 16;
			number:
				printnum(putch, &b, num, base);
				break;

			case '%':
				putch(ch, &b);
				break;
		}
	}

end:
	sys_puts(b.buf, b.idx);
}

void printf(const char *format, ...){
	va_list ap;

	va_start(ap, format);
	vprintf(format, ap);
	va_end(ap);
} 
