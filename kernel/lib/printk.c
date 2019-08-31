#include <stdarg.h>

void putchar(char);
//change an unsigned number to chars
static void printnum(void (*putch)(char), unsigned num, unsigned base)
{
	if(num >=base)
		printnum(putch, num / base, base);

	putch("0123456789abcdef"[num % base]);
}

void vprintk(const char *fmt, va_list ap)
{
	char *p;
	int ch;
	unsigned num, base;

	while(1)
	{
		while((ch = *(unsigned char*)fmt++) != '%')
		{
			if(ch == '\0')	return;
			putchar(ch);
		}
		
		switch(ch = *(unsigned char*)fmt++)
		{
			case 'c':
				putchar(va_arg(ap, int));
				break;

			case 's':
				if((p = va_arg(ap, char*)) == 0)
					p = "(nullptr)";
				while((ch = *p++) != '\0')	putchar(ch);
				break;

			case 'd':
				num = va_arg(ap, int);
				if((int)num < 0)
				{
					putchar('-');
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
				putchar('0');
				putchar('x');
				base = 16;
			number:
				printnum(putchar, num, base);
				break;

			case '%':
				putchar(ch);
				break;
		}
	}

}

void printk(const char *format, ...){
	va_list ap;

	va_start(ap, format);
	vprintk(format, ap);
	va_end(ap);
} 
