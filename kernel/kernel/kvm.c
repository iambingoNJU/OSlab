#include "x86.h"
#include "device.h"
#include "pcb.h"

#define SECTSIZE 512

SegDesc gdt[NR_SEGMENTS];       // the new GDT
TSS tss;

void
init_seg() { // setup kernel segements
	gdt[SEG_KCODE] = SEG(STA_X | STA_R, 0,       0xffffffff, DPL_KERN);
	gdt[SEG_KDATA] = SEG(STA_W,         0,       0xffffffff, DPL_KERN);
	gdt[SEG_UCODE] = SEG(STA_X | STA_R, 0,       0xffffffff, DPL_USER);
	gdt[SEG_UDATA] = SEG(STA_W,         0,       0xffffffff, DPL_USER);
	gdt[SEG_TSS] = SEG16(STS_T32A,      &tss, sizeof(TSS)-1, DPL_KERN);
    gdt[SEG_TSS].s = 0;
	set_gdt(gdt, sizeof(gdt));

    /*
	 * 初始化TSS
	 */
	tss.ss0 = KSEL(SEG_KDATA);
	tss.esp0 = 0x400000;
	asm volatile("ltr %%ax":: "a" (KSEL(SEG_TSS)));
	
	/*设置正确的段寄存器*/
	asm volatile("movw %%ax,%%es":: "a" (KSEL(SEG_KDATA)));
	asm volatile("movw %%ax,%%ds":: "a" (KSEL(SEG_KDATA)));
	asm volatile("movw %%ax,%%ss":: "a" (KSEL(SEG_KDATA)));

	lldt(0);
}

void
enter_user_space(void) {
    /*
     * Before enter user space 
     * you should set the right segment registers here
     * and use 'iret' to jump to ring3
     * 进入用户空间
     */

	struct ELFHeader *elf = (struct ELFHeader*)0x8000;

	asm volatile("movw %%ax,%%es":: "a" (USEL(SEG_UDATA)));
	asm volatile("movw %%ax,%%ds":: "a" (USEL(SEG_UDATA)));
	asm volatile("pushl %0":: "i" (USEL(SEG_UDATA)));	//ss
	asm volatile("pushl %0":: "i" (0x300000));			//esp
	asm volatile("pushl %0":: "i" (0x2));				//eflags
	asm volatile("pushl %0":: "i" (USEL(SEG_UCODE)));	//cs
	asm volatile("pushl %0":: "g" (elf->entry));		//eip
	asm volatile("iret");
	//((void(*)(void))elf->entry)();
}

void readseg(unsigned char*, unsigned, unsigned);

void
load_umain(void) {
    /*
     * Load your app here
     * 加载用户程序
     */
	struct ELFHeader *elf;
	struct ProgramHeader *ph;
	int i;

	char s1[] = { "OS loading..." };
	printMsg(s1, 13);

	/* 这里是加载磁盘程序的代码 */
	elf = (struct ELFHeader*)0x8000;
	readseg((unsigned char*)elf, 4096, 0);

	assert(elf->magic == 0x464c457fU);

	ph = (struct ProgramHeader*)((unsigned char*)elf + elf->phoff);
	for(i = 0; i < elf->phnum; i++, ph++)	
		if(ph->type == 1) {
			readseg((unsigned char*)ph->paddr, ph->filesz, ph->off);
			if(ph->memsz > ph->filesz)
				stosb((unsigned char*)(ph->paddr + ph->filesz), 0, ph->memsz - ph->filesz);
		}

}

void load_first_process(PCB* pcb) {
	load_umain();

	struct ELFHeader *elf = (struct ELFHeader*)0x8000;
	pcb->tf.es = USEL(SEG_UDATA);
	pcb->tf.ds = USEL(SEG_UDATA);
	pcb->tf.ss = USEL(SEG_UDATA);
	pcb->tf.ebp = (unsigned int)(pcb->stack + PROCESS_STACK_SIZE);
	pcb->tf.esp = pcb->tf.ebp;
	pcb->tf.eflags = 0x2;
	pcb->tf.cs = USEL(SEG_UCODE);
	pcb->tf.eip = elf->entry;
	pcb->pid = alloc_pid();
	strcpy(pcb->name, "first user process");
	pcb->state = RUNNABLE;
	pcb->time_count = TIME_SLICE;
	pcb->sleep_time = 0;
	pcb->sem = NULL;
}

void
waitdisk(void) {
	while((in_byte(0x1F7) & 0xC0) != 0x40); 
}

void
readsect(void *dst, int offset) {
	int i;
	waitdisk();
	out_byte2(0x1F2, 1);
	out_byte2(0x1F3, offset);
	out_byte2(0x1F4, offset >> 8);
	out_byte2(0x1F5, offset >> 16);
	out_byte2(0x1F6, (offset >> 24) | 0xE0);
	out_byte2(0x1F7, 0x20);

	waitdisk();
	for (i = 0; i < SECTSIZE / 4; i ++) {
		((int *)dst)[i] = in_long(0x1F0);
	}
}

void readseg(unsigned char* pa, unsigned cnt, unsigned offset)
{
	unsigned char *epa = pa + cnt;
	pa -= offset % SECTSIZE;
	offset = (offset / SECTSIZE) + 201;
	for(; pa < epa; pa += SECTSIZE, offset++)
		readsect(pa, offset);
}
