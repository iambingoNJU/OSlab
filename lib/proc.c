#include "syscall.h"

int fork() {
	return sys_fork();
}

void exit(int status) {
	sys_exit(status);
}

void sleep(unsigned int clocks) {
	sys_sleep(clocks);
}

void pcb_table() {
	sys_pcb_table();
}
