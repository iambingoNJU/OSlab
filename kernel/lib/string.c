#include "common.h"

void memcpy(void *dest, const void *src, size_t size) {
	asm volatile ("cld; rep movsb" : : "c"(size), "S"(src), "D"(dest));
}

void memset(void *dest, uint8_t data, size_t size) {
	asm volatile ("cld; rep stosb" : : "c"(size), "a"(data), "D"(dest));
}

size_t strlen(const char *str) {
	int len = 0;
	while (*str ++) len ++;
	return len;
}

void strcpy(char *d, const char *s) {
	memcpy(d, s, strlen(s) + 1);
}
