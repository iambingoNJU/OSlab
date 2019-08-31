#include "syscall.h"

void createSem() {
	sys_createSem();
}

void lockSem() {
	sys_lockSem();
}

void unlockSem() {
	sys_unlockSem();
}

void destroySem() {
	sys_destroySem();
}
