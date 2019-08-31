#ifndef __PCB_H__
#define __PCB_H__

#include "x86.h"

#define MAX_PCB 32
#define PROCESS_STACK_SIZE 4096
#define TIME_SLICE 8
enum PCB_STATE { NEW, RUNNABLE, RUNNING, BLOCKED, DEAD };

typedef struct PCB PCB;
typedef struct semaphore semaphore;

typedef void (*fun)(semaphore *sem);

struct semaphore{
	int value;
	PCB* list[8];
	int list_head, list_tail;
	fun P, V;
};

extern semaphore sem;
void init_sem(int);

struct PCB{
	struct TrapFrame tf;
	unsigned char stack[PROCESS_STACK_SIZE];
	unsigned int pid;
	char name[32];
	int state;
	int time_count;
	int sleep_time;
	semaphore *sem;
};

extern PCB *current_process;
extern unsigned int nr_process;
void init_pcb_list();
void add_pcb(PCB *pcb);
void del_pcb(unsigned int pid);
unsigned int alloc_pid();
PCB* alloc_pcb();
void schedule(int);
void show_all_process();
void blocked_process_handler();

#endif
