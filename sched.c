/*
 * sched.c - initializes struct for task 0 anda task 1
 */

#include "list.h"
#include <sched.h>
#include <mm.h>
#include <io.h>

struct list_head freequeue;
struct list_head readyqueue;

int next_pid = 3;
int current_time_left;

void writeMSR(int msr_addr, int msr_topval, int msr_lowval);
void inner_task_switch(union task_union*t);
void inner_task_switch_asm();

struct task_struct *idle_task;
struct task_struct *init_task;


union task_union task[NR_TASKS]
  __attribute__((__section__(".data.task")));

struct task_struct *list_head_to_task_struct(struct list_head *l)
{
  return list_entry( l, struct task_struct, list);
}

extern struct list_head blocked;


/* get_DIR - Returns the Page Directory address for task 't' */
page_table_entry * get_DIR (struct task_struct *t) 
{
	return t->dir_pages_baseAddr;
}

/* get_PT - Returns the Page Table address for task 't' */
page_table_entry * get_PT (struct task_struct *t) 
{
	return (page_table_entry *)(((unsigned int)(t->dir_pages_baseAddr->bits.pbase_addr))<<12);
}


int allocate_DIR(struct task_struct *t) 
{
	int pos;

	pos = ((int)t-(int)task)/sizeof(union task_union);

	t->dir_pages_baseAddr = (page_table_entry*) &dir_pages[pos]; 

	return 1;
}

void cpu_idle(void)
{
	__asm__ __volatile__("sti": : :"memory");

	while(1)
	{
	;
	}
}

void init_idle (void)
{
	struct list_head *list_head_idle = list_first(&freequeue);
	idle_task = list_head_to_task_struct(list_head_idle);
	list_del(list_head_idle);

	idle_task->PID = 0;
	idle_task->quantum = QUANTUM_INIT;
	allocate_DIR(idle_task);

	DWord *stack = (DWord*)KERNEL_ESP((union task_union*)idle_task);
	stack[-1] = (DWord)cpu_idle;
	stack[-2] = 0;
	idle_task->kernel_esp = (DWord)&stack[-2];

	// El punt 6 ja està implementat a la línea 2 de la funció
}

void init_task1(void)
{
	struct list_head *list_head_init = list_first(&freequeue);
	struct task_struct *task_init = list_head_to_task_struct(list_head_init);
	list_del(list_head_init);
	
	task_init->PID = 1;
	task_init->quantum = QUANTUM_INIT;
	current_time_left = task_init->quantum;

	allocate_DIR(task_init);
	set_user_pages(task_init);
	DWord* task_init_stack = (DWord*)KERNEL_ESP((union task_union*)task_init);
	tss.esp0 = (DWord)task_init_stack;
	writeMSR(0x175, 0, (DWord)task_init_stack);
	task_init->kernel_esp = (DWord)&task_init_stack[-19];
	set_cr3(get_DIR(task_init));

	init_task = task_init;
}


void init_sched()
{
	INIT_LIST_HEAD(&freequeue);
	for(int i = 0; i < NR_TASKS; i++)
		list_add_tail(&task[i].task.list, &freequeue);

	INIT_LIST_HEAD(&readyqueue);
}

struct task_struct* current()
{
  int ret_value;
  
  __asm__ __volatile__(
  	"movl %%esp, %0"
	: "=g" (ret_value)
  );
  return (struct task_struct*)(ret_value&0xfffff000);
}

void inner_task_switch(union task_union *t)
{
	tss.esp0 = KERNEL_ESP((union task_union*) t);
  	writeMSR(0x175, 0, KERNEL_ESP((union task_union*) t));
	set_cr3(get_DIR(&t->task));

	inner_task_switch_asm(current()->kernel_esp, t->task.kernel_esp);
}

// Scheduling Policy - Round Robin

int get_quantum(struct task_struct *t) {
	return t->quantum;
}

void set_quantum(struct task_struct *t, int new_quantum) {
	t->quantum = new_quantum;
}

void update_sched_data_rr() {
	current_time_left--;
}

int needs_sched_rr() {
	if(current_time_left == 0) {
		if(!list_empty(&readyqueue))
			return 1;
		current_time_left = get_quantum(current());
	}
	return 0;
}

void update_process_state_rr(struct task_struct *t, struct list_head *dest) {
	// Remove it from the actual queue
	if(t->list.prev && t->list.next)
		list_del(&t->list);

	// Add it to the new queue
	if(dest)
		list_add_tail(&t->list, dest);
}

void sched_next_rr() {
	struct task_struct * next_task;

	if(list_empty(&readyqueue))
		next_task = idle_task;
	else {
		struct list_head * next_list_head = readyqueue.next;
		next_task = list_head_to_task_struct(next_list_head);
		list_del(next_list_head);
	}
	
	current_time_left = next_task->quantum;
	task_switch((union task_union *)next_task);
}

void schedule() {
	update_sched_data_rr();
	if(needs_sched_rr()) {
		update_process_state_rr(current(), &readyqueue);
		sched_next_rr();
	}
}