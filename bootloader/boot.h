/* ELF32二进制文件头 */
struct ELFHeader {
	unsigned int   magic;
	unsigned char  elf[12];
	unsigned short type;
	unsigned short machine;
	unsigned int   version;
	unsigned int   entry;
	unsigned int   phoff;
	unsigned int   shoff;
	unsigned int   flags;
	unsigned short ehsize;
	unsigned short phentsize;
	unsigned short phnum;
	unsigned short shentsize;
	unsigned short shnum;
	unsigned short shstrndx;
};

/* ELF32 Program header */
struct ProgramHeader {
	unsigned int type;
	unsigned int off;
	unsigned int vaddr;
	unsigned int paddr;
	unsigned int filesz;
	unsigned int memsz;
	unsigned int flags;
	unsigned int align;
};

/* I/O处理函数 */
static inline char
in_byte(short port) {
	char data;
	asm volatile("in %1,%0" : "=a" (data) : "d" (port));
	return data;
}
static inline int 
in_long(short port) {
	int data;
	asm volatile("in %1, %0" : "=a" (data) : "d" (port));
	return data;
}
static inline void
out_byte(short port, char data) {
	asm volatile("out %0,%1" : : "a" (data), "d" (port));
}

static inline void
stosb(void *addr, int data, int cnt)
{
	asm volatile("cld; rep stosb" :
				 "=D" (addr), "=c" (cnt) :
				 "0" (addr), "1" (cnt), "a" (data) :
				 "memory", "cc");
}

static inline void
printMsg(char* str, unsigned len)
{
	asm volatile("movl	$((80 * 5 + 2) * 2), %%edi;"
				 "movl	%0, %%ebx;"
				 "movl	%1, %%ecx;"
				 "movb	$0x0c, %%ah;"
				"print:"
				  "movb	(%%ebx), %%al;"
				  "movw	%%ax, %%gs:(%%edi);"
				  "inc	%%ebx;"
				  "add	$2, %%edi;"
				  "loop	print"
				  :
				  :"g"(str), "g"(len)
				  );
}

