#include "boot.h"

#define SECTSIZE 512

void readseg(unsigned char*, unsigned, unsigned);

void bootmain(void)
{
	struct ELFHeader *elf;
	struct ProgramHeader *ph;
	int i;

	char s1[] = { "Boot loading..." };
	printMsg(s1, 15);

	/* 这里是加载磁盘程序的代码 */
	elf = (struct ELFHeader*)0x8000;
	readseg((unsigned char*)elf, 4096, 0);

	if(elf->magic != 0x464c457fU)
		return;

	ph = (struct ProgramHeader*)((unsigned char*)elf + elf->phoff);
	for(i = 0; i < elf->phnum; i++, ph++)
	{
		readseg((unsigned char*)ph->paddr, ph->filesz, ph->off);
		if(ph->memsz > ph->filesz)
			stosb((unsigned char*)(ph->paddr + ph->filesz), 0, ph->memsz - ph->filesz);
	}

	((void(*)(void))elf->entry)();

	while(1);
}

void
waitdisk(void) {
	while((in_byte(0x1F7) & 0xC0) != 0x40); /* 等待磁盘完毕 */
}

/* 读磁盘的一个扇区 */
void
readsect(void *dst, int offset) {
	int i;
	waitdisk();
	out_byte(0x1F2, 1);
	out_byte(0x1F3, offset);
	out_byte(0x1F4, offset >> 8);
	out_byte(0x1F5, offset >> 16);
	out_byte(0x1F6, (offset >> 24) | 0xE0);
	out_byte(0x1F7, 0x20);

	waitdisk();
	for (i = 0; i < SECTSIZE / 4; i ++) {
		((int *)dst)[i] = in_long(0x1F0);
	}
}

void readseg(unsigned char* pa, unsigned cnt, unsigned offset)
{
	unsigned char *epa = pa + cnt;
	pa -= offset % SECTSIZE;
	offset = (offset / SECTSIZE) + 1;
	for(; pa < epa; pa += SECTSIZE, offset++)
		readsect(pa, offset);
}
