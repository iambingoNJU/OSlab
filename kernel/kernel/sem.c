#include "common.h"
#include "pcb.h"

semaphore sem;

void proberen(semaphore *s) {
	s->value--;
	if(s->value < 0) {
		s->list[s->list_tail] = current_process;
		s->list_tail++;
		s->list_tail %= 8;
		current_process->sleep_time = 100000;
		schedule(BLOCKED);
	}
}

void verhogen(semaphore *s) {
	if(s->list[s->list_head] != NULL) {
		s->value++;
		if(s->value <= 0) {
			s->list[s->list_head]->sleep_time = 0;
			s->list[s->list_head]->state = RUNNABLE;
			s->list[s->list_head] = NULL;
			s->list_head++;
			s->list_head %= 8;
		}
	} else {
		printk("Unusable unlockSem operation!\n");
	}
}

void init_sem(int val) {
	sem.value = val;
	int i = 0;
	for(i = 0; i < 8; i++) {
		sem.list[i] = NULL;
	}
	sem.list_head = 0;
	sem.list_tail = 0;
	sem.P = proberen;
	sem.V = verhogen;
}
