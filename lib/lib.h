#ifndef __lib_h__
#define __lib_h__

void printf(const char *format,...);

int fork();
void exit(int status);
void sleep(unsigned int sec);
void pcb_table();

void createSem();
void lockSem();
void unlockSem();
void destroySem();

#endif
