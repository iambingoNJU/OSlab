# 系统启动时，BIOS会完成初始化，然后将磁盘第一个512字节保存到物理内存0x00007C00
# 位置，然后设置%CS=0000h, %IP=7C00h

#include "asm.h"

.code16
.globl start
start:
	#disable interupt
	cli
	
	#zero data segment registers
	xorw	%ax, %ax
	movw	%ax, %ds
	movw	%ax, %es
	movw	%ax, %ss

	#open A20 address line
A201:
	inb		$0x64, %al
	testb	$0x2, %al
	jnz		A201
	movb	$0xd1, %al
	outb	%al, $0x64
A202:
	inb		$0x64, %al
	testb	$0x2, %al
	jnz		A202
	movb	$0xdf, %al
	outb	%al, $0x60

	#load gdt and set PE to 1
	lgdt	gdtdesc
	mov		%cr0, %eax
	or		$0x1, %eax
	mov		%eax, %cr0

	#go to protection mode
	ljmp	$GDT_ENTRY(1), $start32

.code32
start32:
	#load segment registers
	movw	$GDT_ENTRY(2), %ax
	movw	%ax, %ds
	movw	%ax, %es
	movw	%ax, %ss
	movw	$GDT_ENTRY(3), %ax
	movw	%ax, %gs

	#set stack for c code and go to c code
	movl	$(128 << 20), %esp
	jmp		bootmain

# GDT
.p2align 2                         # 对齐
gdt:    
	SEG_NULLASM							#NULL
	SEG_ASM(0xA, 0x0, 0xffffffff)		#code
	SEG_ASM(0x2, 0x0, 0xffffffff)		#data
	SEG_ASM(0x2, 0xB8000, 0xffffffff)	#video

gdtdesc:                           # GDT描述符
	.word   (gdtdesc - gdt - 1)    # GDT长度，留意地址运算
	.long   gdt                    # GDT地址
