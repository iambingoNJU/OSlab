#include "common.h"
#include "pcb.h"

/* PCB table is implemented with circle lists. */
struct Node {
	PCB *p;
	struct Node *next;
};
typedef struct Node Node;

PCB pcb[MAX_PCB];
Node pcb_list[MAX_PCB];
Node *pcb_table, *pcb_table_tail, *free_;

PCB *current_process = NULL;
unsigned int nr_process = 0;
unsigned int PID = 0;
unsigned int alloc_pid() {
	return PID ++;
}
PCB* alloc_pcb() {
	return &pcb[PID];
}

void init_pcb_list()
{
	int i;
	for(i = 0; i < MAX_PCB - 1; i++)
		pcb_list[i].next = &pcb_list[i + 1];
	pcb_list[MAX_PCB - 1].next = NULL;

	free_ = pcb_list;
	pcb_table = NULL;
	pcb_table_tail = NULL;
}

//insert a pcb into the list. the new pcb will be put after the current process.
void add_pcb(PCB *proc)
{
	Node *newp = free_;
	free_ = free_->next;
	assert(newp != NULL);
	newp->p = proc;

	if(pcb_table == NULL)
	{
		pcb_table = newp;
		pcb_table_tail = newp;
		newp->next = pcb_table;
	}
	else
	{
		Node *temp = pcb_table;
		assert(current_process != NULL);
		while(temp->p != current_process)
			temp = temp->next;
		assert(temp->p == current_process);

		newp->next = temp->next;
		temp->next = newp;
		if(newp->next == pcb_table)
			pcb_table_tail = newp;	
	}
	nr_process++;
}

void del_pcb(unsigned int pid)
{
	assert(pcb_table != NULL);
	Node *temp = pcb_table, *pre = NULL;
	while(temp != pcb_table_tail) {
		if(temp->p->pid == pid)	break;
		else {
			pre = temp;
			temp = temp->next;
		}
	}
	
	if(temp->p->pid == pid)
	{
		if(pre == NULL)	//Only one process
		{
			pcb_table = NULL;
			pcb_table_tail = NULL;
		}
		else
			pre->next = temp->next;

		temp->next = free_;
		free_ = temp;
		nr_process--;
	}
	else
		printk("No such process(#%d) to be deleted!\n", pid);
}

void dec_sleep_time(PCB *pcb) {
	if((pcb->state == BLOCKED)&&(pcb->pid != 0)) {
		if(pcb->sleep_time > 0) {
			pcb->sleep_time--;
			//printk("pid: %d\tsleep time: %d\n", pcb->pid, pcb->sleep_time);
		}
		if(pcb->sleep_time == 0)
			pcb->state = RUNNABLE;
	}
}

void print_pcb(PCB *p) {
	printk("%d\t%s\t\t%u\t%u\t%u\n", p->pid, p->name, 
		p->state, p->time_count, p->sleep_time);
}

void traverse_process(char *printMsg, void (*f)(PCB *pcb)) {
	Node *temp = pcb_table;
	if(printMsg)	printk("%s\n", printMsg);
	while(temp != pcb_table_tail) {
		f(temp->p);
		temp = temp->next;
	}
	f(pcb_table_tail->p);
}

void show_all_process() {
	traverse_process("ID		name                state     time_count  sleep_time", print_pcb);
}

void blocked_process_handler() {
	//printk("before blocked_process_handler.\n");
	traverse_process(NULL, dec_sleep_time);
	//printk("after blocked_process_handler.\n");
}

void schedule(int current_state) {
	Node *temp = pcb_table, *temp2;
	//printk("before finding current process\n");
	//test_pcb_table();
	while(temp->p != current_process)
		temp = temp->next;
	//printk("after finding current process\n");
	assert(temp->p == current_process);

	temp2 = temp;
	temp = temp->next;
	while((temp->p->state != RUNNABLE)&&(temp != temp2))
		temp = temp->next;
	
	if(temp->p->state == RUNNABLE) {
		//printk("schedule: from pid:%d to pid:%d\n", current_process->pid, temp->p->pid);
		current_process->state = current_state;
		current_process = temp->p;
		current_process->state = RUNNING;
	} else {
		//printk("No process to be scheduled. Let's run idle!\n");
		printk("");
		assert(pcb_table->p->pid == 0);
		current_process->state = current_state;
		current_process = pcb_table->p;
		current_process->state = RUNNING;
		current_process->tf.eflags |= 0x200;
	}
}
