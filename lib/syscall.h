#ifndef __SYSCALL_H__
#define __SYSCALL_H__

#include "types.h"

#define SYS_puts	0x4

#define SYS_fork	0x5
#define SYS_exit	0x6
#define SYS_sleep	0x7
#define SYS_pcb_table 0x8

#define SYS_createSem 0x10
#define SYS_lockSem 0x11
#define SYS_unlockSem 0x12
#define SYS_destroySem 0x13

void sys_puts(const char* s, size_t len);

int sys_fork();
void sys_exit(int status);
void sys_sleep(unsigned int sec);
void sys_pcb_table();

void sys_createSem();
void sys_lockSem();
void sys_unlockSem();
void sys_destroySem();

#endif
