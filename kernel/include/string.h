#ifndef __STRING_H__
#define __STRING_H__

void memcpy(void *dest, const void *src, size_t size);
void memset(void *dest, uint8_t data, size_t size);
size_t strlen(const char *str);
void strcpy(char *d, const char *s);

#endif
