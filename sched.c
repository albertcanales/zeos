/*
 * sched.c - initializes struct for task 0 anda task 1
 */

#include "list.h"
#include <sched.h>
#include <mm.h>
#include <io.h>

struct list_head freequeue;
struct list_head readyqueue;

struct task_struct *idle_task;

void writeMSR(int msr_addr, int msr_topval, int msr_lowval);
void inner_task_switch(union task_union*t);
void inner_task_switch_asm();


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
	struct list_head *list_head_idle = freequeue.next;
	idle_task = list_head_to_task_struct(list_head_idle);
	list_del(list_head_idle);

	idle_task->PID = 0;
	allocate_DIR(idle_task);

	DWord *stack = (DWord*)KERNEL_ESP((union task_union*)idle_task);
	stack[-1] = (DWord)cpu_idle;
	stack[-2] = 0;

	// El punt 6 ja està implementat a la línea 2 de la funció
}

void init_task1(void)
{
	struct list_head *list_head_init = freequeue.next;
	struct task_struct *task_init = list_head_to_task_struct(list_head_init);
	list_del(list_head_init);
	
	task_init->PID = 1;
	allocate_DIR(task_init);
	set_user_pages(task_init);
	tss.esp0 = KERNEL_ESP((union task_union*)task_init);
	writeMSR(0x175, 0, task_init->kernel_esp);
	set_cr3(get_DIR(task_init));
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

void inner_task_switch(union task_union*t)
{
	tss.esp0 = KERNEL_ESP(t);
  	writeMSR(0x175, 0, t->task.kernel_esp);
	set_cr3(get_DIR(&t->task));

	inner_task_switch_asm(&current()->kernel_esp, &t->task.kernel_esp);
}