#include "syscall.h"

static inline int32_t syscall(int num, uint32_t a1, uint32_t a2, uint32_t a3)
{
	int32_t ret = 0;

	asm volatile("int $0x80"
			:	"=a" (ret)
			:	"a"	(num),
				"d" (a1),
				"c" (a2),
				"b" (a3));

	return ret;
}

void sys_puts(const char* s, size_t len) {
	syscall(SYS_puts, (uint32_t)s, len, 0);
}

int sys_fork() {
	return syscall(SYS_fork, 0, 0, 0);
}

void sys_exit(int status) {
	syscall(SYS_exit, status, 0, 0);
}

void sys_sleep(unsigned int clocks) {
	syscall(SYS_sleep, clocks, 0, 0);
}

void sys_pcb_table() {
	syscall(SYS_pcb_table, 0, 0, 0);
}

void sys_createSem() {
	syscall(SYS_createSem, 0, 0, 0);
}

void sys_lockSem() {
	syscall(SYS_lockSem, 0, 0, 0);
}

void sys_unlockSem() {
	syscall(SYS_unlockSem, 0, 0, 0);
}

void sys_destroySem() {
	syscall(SYS_destroySem, 0, 0, 0);
}

